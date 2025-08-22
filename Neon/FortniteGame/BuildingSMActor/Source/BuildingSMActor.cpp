#include "pch.h"
#include "../Header/BuildingSMActor.h"

#include "Engine/DataTable/Header/DataTable.h"
#include "Engine/Kismet/Header/Kismet.h"
#include "Engine/NetDriver/Header/NetDriver.h"
#include "FortniteGame/FortGameState/Header/FortGameState.h"
#include "FortniteGame/FortQuestManager/Header/FortQuestManager.h"

void ABuildingSMActor::OnDamageServer(ABuildingSMActor* BuildingActor,
                                      float Damage,
                                      FGameplayTagContainer DamageTags,
                                      FVector Momentum,
                                      FHitResult HitInfo,
                                      AFortPlayerControllerAthena* Controller,
                                      AActor* DamageCauser,
                                      FGameplayEffectContextHandle Context)
{
    AFortGameStateAthena* GameState = UWorld::GetWorld()->GetGameState();
    
    if (!BuildingActor || !Controller || !GameState)
    {
        return OnDamageServerOG(BuildingActor, Damage, DamageTags, Momentum, HitInfo, Controller, DamageCauser, Context);
    }

    if (BuildingActor->GetHealth() <= 1.0f)
    {
        return OnDamageServerOG(BuildingActor, Damage, DamageTags, Momentum, HitInfo, Controller, DamageCauser, Context);
    }

    static UCurveTable* ResourceCurveTable = nullptr;
    int ResourceAmount = 0;

    if (BuildingActor->GetbPlayerPlaced() == true)
    {
        FGameplayTagContainer SourceTags;
        FGameplayTagContainer TargetTags;
        FGameplayTagContainer ContextTags;
        UFortQuestManager* QuestManager = Controller->GetQuestManager(ESubGame::Athena);

        if (!QuestManager) {
            UE_LOG(LogNeon, Warning, "QuestManager is null for controller");
        } else
        {
            if (BuildingActor->GetBuildingResourceAmountOverride().RowName.GetComparisonIndex() > 0) {
                if (!ResourceCurveTable)
                {
                    if (BuildingActor->GetBuildingResourceAmountOverride().CurveTable) {
                        ResourceCurveTable = BuildingActor->GetBuildingResourceAmountOverride().CurveTable;
                    }

                    if (!ResourceCurveTable)
                    {
                        ResourceCurveTable = Runtime::StaticFindObject<UCurveTable>(
                            "/Game/Athena/Balance/DataTables/AthenaResourceRates.AthenaResourceRates");
                    }
                }
        
                float Out = UDataTableFunctionLibrary::EvaluateCurveTableRow(
                    ResourceCurveTable,
                    BuildingActor->GetBuildingResourceAmountOverride().RowName,
                    0.0f
                );
        
                float RC = Out / (BuildingActor->GetMaxHealth() / Damage);

                ResourceAmount = round(RC);
            }
            
            SourceTags.GameplayTags.Add(FGameplayTag(UKismetStringLibrary::Conv_StringToName(L"DamageDone.Building")));
            ContextTags.GameplayTags.Add(FGameplayTag(UKismetStringLibrary::Conv_StringToName(L"Athena.Playlist")));
            
            UFortQuestManager::SendStatEvent(QuestManager, BuildingActor, SourceTags, TargetTags, nullptr, nullptr, ResourceAmount, EFortQuestObjectiveStatEvent::Damage, ContextTags);
        }
        
        return OnDamageServerOG(BuildingActor, Damage, DamageTags, Momentum, HitInfo, Controller, DamageCauser, Context);
    }
    
    if (!DamageCauser || 
    DamageCauser && !DamageCauser->IsA<AFortWeapon>() || 
    !static_cast<AFortWeapon*>(DamageCauser)->GetWeaponData()->IsA<UFortWeaponMeleeItemDefinition>()) {
        return OnDamageServerOG(BuildingActor, Damage, DamageTags, Momentum, HitInfo, Controller, DamageCauser, Context);
    }

    static FName PickaxeTag = UKismetStringLibrary::Conv_StringToName(L"Weapon.Melee.Impact.Pickaxe");
    FGameplayTag* DamageTagEntry = nullptr;
    for (FGameplayTag& entry : DamageTags.GameplayTags) {
        if (entry.TagName.GetComparisonIndex() == PickaxeTag.GetComparisonIndex()) {
            DamageTagEntry = &entry;
            break;
        }
    }

    if (!DamageTagEntry) {
        return OnDamageServerOG(BuildingActor, Damage, DamageTags, Momentum, HitInfo, Controller, DamageCauser, Context);
    }

    auto ResourceDefinition = UFortKismetLibrary::K2_GetResourceItemDefinition(BuildingActor->GetResourceType());
    if (!ResourceDefinition) {
        return OnDamageServerOG(BuildingActor, Damage, DamageTags, Momentum, HitInfo, Controller, DamageCauser, Context);
    }

    const float MaxStackSize = ResourceDefinition->GetMaxStackSize().Value;
    
    if (BuildingActor->GetBuildingResourceAmountOverride().RowName.GetComparisonIndex() > 0) {
        if (!ResourceCurveTable)
        {
            if (BuildingActor->GetBuildingResourceAmountOverride().CurveTable) {
                ResourceCurveTable = BuildingActor->GetBuildingResourceAmountOverride().CurveTable;
            }

            if (!ResourceCurveTable)
            {
                ResourceCurveTable = Runtime::StaticFindObject<UCurveTable>(
                    "/Game/Athena/Balance/DataTables/AthenaResourceRates.AthenaResourceRates");
            }
        }
        
        float Out = UDataTableFunctionLibrary::EvaluateCurveTableRow(
            ResourceCurveTable,
            BuildingActor->GetBuildingResourceAmountOverride().RowName,
            0.0f
        );
        
        float RC = Out / (BuildingActor->GetMaxHealth() / Damage);

        ResourceAmount = round(RC);
    }
    
    if (ResourceAmount <= 0)
    {
        return OnDamageServerOG(BuildingActor, Damage, DamageTags, Momentum, HitInfo, Controller, DamageCauser, Context);
    }
    
    const FVector PlayerLocation = Controller->GetPawn()->K2_GetActorLocation();
    
    AFortInventory* Inventory = nullptr;
    AFortAthenaAIBotController* AIController = nullptr;
    
    bool bIsAI = false;
    
    if (auto AI = Cast<AFortAthenaAIBotController>(Controller))
    {
        Inventory = AI->GetInventory();
        AIController = AI;
        bIsAI = true;
    }
    
    if (!Inventory)
        Inventory = Controller->GetWorldInventory();
    if (!Inventory)
        return OnDamageServerOG(BuildingActor, Damage, DamageTags, Momentum, HitInfo, Controller, DamageCauser, Context);

    FFortItemList& IInventory = Inventory->GetInventory();
    TArray<UFortWorldItem*>& ItemInstances = IInventory.GetItemInstances();
    UFortWorldItem* WorldItem = nullptr;
    FFortItemEntry* InventoryEntry = nullptr;

    for (int32 i = 0; i < ItemInstances.Num(); i++)
    {
        if (ItemInstances[i] && ItemInstances[i]->GetItemEntry().GetItemDefinition() == ResourceDefinition)
        {
            WorldItem = ItemInstances[i];
            InventoryEntry = &ItemInstances[i]->GetItemEntry();
            break;
        }
    }
    
    if (InventoryEntry != nullptr) {
        int32 NewCount = InventoryEntry->GetCount() + ResourceAmount;
        InventoryEntry->SetCount(NewCount);
    
        if (NewCount > MaxStackSize) {
            int32 OverflowAmount = NewCount - MaxStackSize;
            InventoryEntry->SetCount(MaxStackSize);
            
            AFortInventory::SpawnPickup(
                PlayerLocation, 
                ResourceDefinition, 
                OverflowAmount, 
                0,
                EFortPickupSourceTypeFlag::Tossed, 
                EFortPickupSpawnSource::Unset,
                bIsAI ? (AFortPlayerPawn*)AIController->GetPawn() : Controller->GetMyFortPawn()
            );
        }
        
        WorldItem->SetItemEntry(*InventoryEntry);
        if (bIsAI) {
            AFortInventory::ReplaceEntry(AIController, *InventoryEntry);
        } else {
            AFortInventory::ReplaceEntry(Controller, *InventoryEntry);
        }

        if (InventoryEntry->GetCount() <= 0)
        {
            if (bIsAI) {
                AFortInventory::Remove(AIController, InventoryEntry->GetItemGuid(), 0);
            } else {
                AFortInventory::Remove(Controller, InventoryEntry->GetItemGuid());
            }
        }
    } else if (ResourceAmount > 0) {
        if (ResourceAmount > MaxStackSize) {
            AFortInventory::SpawnPickup(
                PlayerLocation, 
                ResourceDefinition, 
                ResourceAmount - MaxStackSize, 
                0,
                EFortPickupSourceTypeFlag::Tossed, 
                EFortPickupSpawnSource::Unset,
                bIsAI ? (AFortPlayerPawn*)AIController->GetPawn() : Controller->GetMyFortPawn()
            );
        
            ResourceAmount = MaxStackSize;
        }
        if (bIsAI) {
            AFortInventory::GiveItem(AIController, ResourceDefinition, ResourceAmount, 0, 0);
        } else {
            AFortInventory::GiveItem(Controller, ResourceDefinition, ResourceAmount, 0, 0);
        }
    }

    if (!bIsAI)
    {
        Controller->ClientReportDamagedResourceBuilding(BuildingActor, BuildingActor->GetResourceType(), ResourceAmount, false, Damage == 100.f);
        if (Damage == 100.f) {
            static auto AccoladeDef = Runtime::StaticFindObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_066_WeakSpotsInARow.AccoladeId_066_WeakSpotsInARow");
            if (AccoladeDef)
            {
                UFortQuestManager::GiveAccolade(Controller, AccoladeDef);
            }
        }
    }

    if (BuildingActor->GetResourceType() == EFortResourceType::Wood ||
        BuildingActor->GetResourceType() == EFortResourceType::Stone ||
        BuildingActor->GetResourceType() == EFortResourceType::Metal)
    {
        if (!bIsAI)
        {
            FGameplayTagContainer SourceTags;
            FGameplayTagContainer TargetTags;
            FGameplayTagContainer ContextTags;
            UFortQuestManager* QuestManager = Controller->GetQuestManager(ESubGame::Athena);

            if (!QuestManager) {
                UE_LOG(LogNeon, Warning, "QuestManager is null for controller");
            } else {
                const WCHAR* ResourceTag = nullptr;
                switch (BuildingActor->GetResourceType())
                {
                case EFortResourceType::Wood:
                    ResourceTag = L"Building.Resource.Wood";
                    break;
                case EFortResourceType::Stone:
                    ResourceTag = L"Building.Resource.Stone";
                    break;
                case EFortResourceType::Metal:
                    ResourceTag = L"Building.Resource.Metal";
                    break;
                }
        
                TargetTags.GameplayTags.Add(FGameplayTag(UKismetStringLibrary::Conv_StringToName(ResourceTag)));
                SourceTags.GameplayTags.Add(FGameplayTag(UKismetStringLibrary::Conv_StringToName(L"Homebase.Class")));
                ContextTags.GameplayTags.Add(FGameplayTag(UKismetStringLibrary::Conv_StringToName(L"Athena.Playlist")));
                ContextTags.GameplayTags.Add(FGameplayTag(UKismetStringLibrary::Conv_StringToName(L"Athena.GameOn")));
            
                UFortQuestManager::SendStatEvent(QuestManager, BuildingActor, SourceTags, TargetTags, nullptr, nullptr, ResourceAmount, EFortQuestObjectiveStatEvent::Collect, ContextTags);
            }
        }
    }

    return OnDamageServerOG(BuildingActor, Damage, DamageTags, Momentum, HitInfo, Controller, DamageCauser, Context);
}

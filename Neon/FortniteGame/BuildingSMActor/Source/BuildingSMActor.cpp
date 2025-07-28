#include "pch.h"
#include "../Header/BuildingSMActor.h"

#include "Engine/DataTable/Header/DataTable.h"
#include "Engine/Kismet/Header/Kismet.h"
#include "Engine/NetDriver/Header/NetDriver.h"
#include "FortniteGame/FortGameState/Header/FortGameState.h"
#include "FortniteGame/FortKismetLibrary/Header/FortKismetLibrary.h"

void ABuildingSMActor::OnDamageServer(ABuildingSMActor* BuildingActor,
                                      float Damage,
                                      FGameplayTagContainer DamageTags,
                                      FVector Momentum,
                                      FHitResult HitInfo,
                                      AFortPlayerControllerAthena* Controller,
                                      AActor* DamageCauser,
                                      FGameplayEffectContextHandle Context)
{
    UE_LOG(LogNeon, Log, "Called");
    AFortGameStateAthena* GameState = UWorld::GetWorld()->GetGameState();
    
    if (!BuildingActor || !Controller || !GameState)
    {
        UE_LOG(LogNeon, Log, "Invalid Params in OnDamageServer!");
        return OnDamageServerOG(BuildingActor, Damage, DamageTags, Momentum, HitInfo, Controller, DamageCauser, Context);
    }

    if (BuildingActor->GetbPlayerPlaced() == true || BuildingActor->CallFunc<float>("BuildingActor", "GetHealth") <= 1.0f)
    {
        UE_LOG(LogNeon, Log, "BuildingActor is player placed or has low health, skipping resource collection.");
        return OnDamageServerOG(BuildingActor, Damage, DamageTags, Momentum, HitInfo, Controller, DamageCauser, Context);
    }
    
    if (!DamageCauser || 
    !DamageCauser->IsA<AFortWeapon>() || 
    !static_cast<AFortWeapon*>(DamageCauser)->GetWeaponData()->IsA<UFortWeaponMeleeItemDefinition>()) {
        UE_LOG(LogNeon, Log, "DamageCauser is not a melee weapon, skipping resource collection.");
        return OnDamageServerOG(BuildingActor, Damage, DamageTags, Momentum, HitInfo, Controller, DamageCauser, Context);
    }

    static FName PickaxeTag = UKismetStringLibrary::GetDefaultObj()->CallFunc<FName>("KismetStringLibrary","Conv_StringToName", FString(L"Weapon.Melee.Impact.Pickaxe"));
    FGameplayTag* DamageTagEntry = nullptr;
    for (FGameplayTag& entry : DamageTags.GameplayTags) {
        if (entry.TagName.GetComparisonIndex() == PickaxeTag.GetComparisonIndex()) {
            DamageTagEntry = &entry;
            break;
        }
    }

    if (!DamageTagEntry) {
        UE_LOG(LogNeon, Log, "DamageTags do not contain a pickaxe tag, skipping resource collection.");
        return OnDamageServerOG(BuildingActor, Damage, DamageTags, Momentum, HitInfo, Controller, DamageCauser, Context);
    }

    auto ResourceDefinition = UFortKismetLibrary::K2_GetResourceItemDefinition(BuildingActor->Get<EFortResourceType>("BuildingSMActor", "ResourceType"));
    if (!ResourceDefinition) {
        UE_LOG(LogNeon, Log, "KismetLibrary::K2_GetResourceItemDefinition() failed.");
        return OnDamageServerOG(BuildingActor, Damage, DamageTags, Momentum, HitInfo, Controller, DamageCauser, Context);
    }

    const float MaxStackSize = ResourceDefinition->GetMaxStackSize().Value;
    int ResourceAmount = 0;

    UCurveTable* ResourceCurveTable = nullptr;
    
    if (BuildingActor->GetBuildingResourceAmountOverride().RowName.GetComparisonIndex() > 0) {
        UE_LOG(LogNeon, Log, "GetBuildingResourceAmountOverride");
        if (BuildingActor->GetBuildingResourceAmountOverride().CurveTable) {
            ResourceCurveTable = BuildingActor->GetBuildingResourceAmountOverride().CurveTable;
        } else {
            ResourceCurveTable = Runtime::StaticFindObject<UCurveTable>(
                "/Game/Athena/Balance/DataTables/AthenaResourceRates.AthenaResourceRates");
        }
        
        EEvaluateCurveTableResult Result;
        float Out = UDataTableFunctionLibrary::EvaluateCurveTableRow(
            ResourceCurveTable,
            BuildingActor->GetBuildingResourceAmountOverride().RowName,
            0.0f,
            FString(),  
            &Result    
        );
        
        float RC = Out / (BuildingActor->CallFunc<float>("BuildingActor", "GetMaxHealth") / Damage);

        ResourceAmount = round(RC);
    }

    UE_LOG(LogNeon, Log, "ResourceAmount: %d", ResourceAmount);

    if (ResourceAmount <= 0)
    {
        return OnDamageServerOG(BuildingActor, Damage, DamageTags, Momentum, HitInfo, Controller, DamageCauser, Context);
    }
    
    const FVector PlayerLocation = Controller->GetPawn()->CallFunc<FVector>("Actor", "K2_GetActorLocation");
    
    AFortInventory* Inventory = nullptr;

    if (!Inventory) Inventory = Controller->GetWorldInventory();

    if (!Inventory)
    {
        return; 
    }

    FFortItemList& IInventory = Inventory->GetInventory();
    TArray<FFortItemEntry>& ReplicatedEntriesOffsetPtr = IInventory.GetReplicatedEntries();
    FFortItemEntry* InventoryEntry = nullptr;

    for (int32 i = 0; i < ReplicatedEntriesOffsetPtr.Num(); i++)
    {
        if (ReplicatedEntriesOffsetPtr[i].GetItemDefinition() == ResourceDefinition)
        {
            InventoryEntry = &ReplicatedEntriesOffsetPtr[i];
            break;
        }
    }
    
    if (InventoryEntry != nullptr) {
        InventoryEntry->SetCount(InventoryEntry->GetCount() + ResourceAmount);
    
        if (InventoryEntry->GetCount() > MaxStackSize) {
            AFortInventory::SpawnPickup(
                PlayerLocation, 
                ResourceDefinition, 
                InventoryEntry->GetCount() - MaxStackSize, 
                0,
                EFortPickupSourceTypeFlag::Tossed, 
                EFortPickupSpawnSource::Unset,
                Controller->GetMyFortPawn()
            );
        }
    
        AFortInventory::ReplaceEntry(Controller, *InventoryEntry);
    } else if (ResourceAmount > 0) {
        if (ResourceAmount > MaxStackSize) {
            AFortInventory::SpawnPickup(
                PlayerLocation, 
                ResourceDefinition, 
                ResourceAmount - MaxStackSize, 
                0,
                EFortPickupSourceTypeFlag::Tossed, 
                EFortPickupSpawnSource::Unset,
                Controller->GetMyFortPawn()
            );
        
            ResourceAmount = MaxStackSize;
        }
    
        AFortInventory::GiveItem(Controller, ResourceDefinition, ResourceAmount, 0, 0);
    }
    
    Controller->CallFunc<void>("FortPlayerController", "ClientReportDamagedResourceBuilding",
        BuildingActor, 
        BuildingActor, 
        ResourceAmount, 
        false, 
        false
    );
    
  //  return OnDamageServerOG(BuildingActor, Damage, DamageTags, Momentum, HitInfo, Controller, DamageCauser, Context);
}
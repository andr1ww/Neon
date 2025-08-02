#include "pch.h"
#include "../Header/BuildingSMActor.h"

#include "Engine/DataTable/Header/DataTable.h"
#include "Engine/Kismet/Header/Kismet.h"
#include "Engine/NetDriver/Header/NetDriver.h"
#include "FortniteGame/FortGameState/Header/FortGameState.h"

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
        UE_LOG(LogNeon, Log, "Invalid Params in OnDamageServer!");
        return OnDamageServerOG(BuildingActor, Damage, DamageTags, Momentum, HitInfo, Controller, DamageCauser, Context);
    }

    if (BuildingActor->GetbPlayerPlaced() == true || BuildingActor->CallFunc<float>("BuildingActor", "GetHealth") <= 1.0f)
    {
        return OnDamageServerOG(BuildingActor, Damage, DamageTags, Momentum, HitInfo, Controller, DamageCauser, Context);
    }
    
    if (!DamageCauser || 
    DamageCauser && !DamageCauser->IsA<AFortWeapon>() || 
    !static_cast<AFortWeapon*>(DamageCauser)->GetWeaponData()->IsA<UFortWeaponMeleeItemDefinition>()) {
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
        return OnDamageServerOG(BuildingActor, Damage, DamageTags, Momentum, HitInfo, Controller, DamageCauser, Context);
    }

    auto ResourceDefinition = UFortKismetLibrary::K2_GetResourceItemDefinition(BuildingActor->GetResourceType());
    if (!ResourceDefinition) {
        return OnDamageServerOG(BuildingActor, Damage, DamageTags, Momentum, HitInfo, Controller, DamageCauser, Context);
    }

    const float MaxStackSize = ResourceDefinition->GetMaxStackSize().Value;
    int ResourceAmount = 0;

    UCurveTable* ResourceCurveTable = nullptr;
    
    if (BuildingActor->GetBuildingResourceAmountOverride().RowName.GetComparisonIndex() > 0) {
        if (BuildingActor->GetBuildingResourceAmountOverride().CurveTable) {
            ResourceCurveTable = BuildingActor->GetBuildingResourceAmountOverride().CurveTable;
        } else {
            ResourceCurveTable = Runtime::StaticFindObject<UCurveTable>(
                "/Game/Athena/Balance/DataTables/AthenaResourceRates.AthenaResourceRates");
        }
        
        float Out = UDataTableFunctionLibrary::EvaluateCurveTableRow(
            ResourceCurveTable,
            BuildingActor->GetBuildingResourceAmountOverride().RowName,
            0.0f
        );
        
        float RC = Out / (BuildingActor->CallFunc<float>("BuildingActor", "GetMaxHealth") / Damage);

        ResourceAmount = round(RC);
    }
    
    if (ResourceAmount <= 0)
    {
        return OnDamageServerOG(BuildingActor, Damage, DamageTags, Momentum, HitInfo, Controller, DamageCauser, Context);
    }
    
    const FVector PlayerLocation = Controller->GetPawn()->CallFunc<FVector>("Actor", "K2_GetActorLocation");
    
    AFortInventory* Inventory = nullptr;

    if (!Inventory) Inventory = Controller->GetWorldInventory();

    if (!Inventory)
    {
        return OnDamageServerOG(BuildingActor, Damage, DamageTags, Momentum, HitInfo, Controller, DamageCauser, Context);
    }

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
                Controller->GetMyFortPawn()
            );
        }
        
        WorldItem->SetItemEntry(*InventoryEntry);
        AFortInventory::ReplaceEntry(Controller, *InventoryEntry);

        if (InventoryEntry->GetCount() <= 0)
        {
            AFortInventory::Remove(Controller, InventoryEntry->GetItemGuid());
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
                Controller->GetMyFortPawn()
            );
        
            ResourceAmount = MaxStackSize;
        }

        AFortInventory::GiveItem(Controller, ResourceDefinition, ResourceAmount, 0, 0);
    }
        
    Controller->CallFunc<void>("FortPlayerController", "ClientReportDamagedResourceBuilding",
        BuildingActor, 
        BuildingActor->GetResourceType(), 
        ResourceAmount, 
        false, 
        Damage == 100.f
    );

    return OnDamageServerOG(BuildingActor, Damage, DamageTags, Momentum, HitInfo, Controller, DamageCauser, Context);
}

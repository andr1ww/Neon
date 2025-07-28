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
        return OnDamageServerOG(BuildingActor, Damage, DamageTags, Momentum, HitInfo, Controller, DamageCauser, Context);

    if (BuildingActor->Get<bool>("BuildingActor", "bPlayerPlaced") || BuildingActor->CallFunc<float>("BuildingActor", "GetHealth") <= 1.0f)
        return OnDamageServerOG(BuildingActor, Damage, DamageTags, Momentum, HitInfo, Controller, DamageCauser, Context);

    if (!DamageCauser || 
    !DamageCauser->IsA<AFortWeapon>() || 
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

    auto ResourceDefinition = UFortKismetLibrary::K2_GetResourceItemDefinition(BuildingActor->Get<EFortResourceType>("BuildingSMActor", "ResourceType"));
    if (!ResourceDefinition) {
        return OnDamageServerOG(BuildingActor, Damage, DamageTags, Momentum, HitInfo, Controller, DamageCauser, Context);
    }

    const float MaxStackSize = ResourceDefinition->GetMaxStackSize().Value;
    int ResourceAmount = 0;

    UCurveTable* ResourceCurveTable = nullptr;
    
    if (BuildingActor->GetBuildingResourceAmountOverride().RowName.GetComparisonIndex() > 0) {
        ResourceCurveTable = Runtime::StaticFindObject<UCurveTable>(
                "/Game/Athena/Balance/DataTables/AthenaResourceRates.AthenaResourceRates");
    
        float BaseResourceValue = 0.0f;
        EEvaluateCurveTableResult Result;
        UDataTableFunctionLibrary::EvaluateCurveTableRow(
            ResourceCurveTable,
            BuildingActor->GetBuildingResourceAmountOverride().RowName,
            0.0f,
            FString(),  
            &Result    
        );
        
        ResourceAmount = static_cast<int>(round(BaseResourceValue * (Damage / BuildingActor->CallFunc<float>("BuildingActor", "GetMaxHealth"))));
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
        
            InventoryEntry->GetCount() = MaxStackSize;
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
    
    return OnDamageServerOG(BuildingActor, Damage, DamageTags, Momentum, HitInfo, Controller, DamageCauser, Context);
}
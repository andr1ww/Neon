#include "pch.h"
#include "../Header/FortPlayerPawn.h"

#include "FortniteGame/FortLoot/Header/FortLootPackage.h"

void AFortPlayerPawn::ServerHandlePickupInfo(AFortPlayerPawn* Pawn, FFrame& Stack)
{
    AFortPickup* Pickup;
    FFortPickupRequestInfo Params;
    Stack.StepCompiledIn(&Pickup);
    Stack.StepCompiledIn(&Params);
    Stack.IncrementCode();

    if (!Pawn || !Pickup || Pickup->GetbPickedUp())
        return;

    Pawn->GetIncomingPickups().Add(Pickup);

    Pickup->GetPickupLocationData().SetbPlayPickupSound(Params.bPlayPickupSound);
    Pickup->GetPickupLocationData().SetFlyTime(0.4f);
    Pickup->GetPickupLocationData().SetItemOwner(Pawn);
    Pickup->GetPickupLocationData().SetPickupGuid(Pickup->GetPrimaryPickupItemEntry().GetItemGuid());
    Pickup->GetPickupLocationData().SetPickupTarget(Pawn);
    Pickup->CallFunc<void>("FortPickup", "OnRep_PickupLocationData");

    Pickup->SetbPickedUp(true);
    Pickup->CallFunc<void>("FortPickup", "OnRep_bPickedUp");
}

void AFortPlayerPawn::CompletePickupAnimation(AFortPickup* Pickup)
{
    AFortPlayerPawn* Pawn = (AFortPlayerPawn*)Pickup->GetPickupLocationData().GetPickupTarget().Get();
    if (!Pawn) return CompletePickupAnimationOG(Pickup);

    AFortPlayerControllerAthena* PlayerController = (AFortPlayerControllerAthena*)Pawn->GetController();
    if (!PlayerController) return CompletePickupAnimationOG(Pickup);

    AFortInventory::GiveItem(PlayerController, Pickup->GetPrimaryPickupItemEntry().GetItemDefinition(), Pickup->GetPrimaryPickupItemEntry().GetCount(), 30, 0);
    return CompletePickupAnimationOG(Pickup);
}

void AFortPlayerPawn::ServerHandlePickup(AFortPlayerPawn* Pawn, FFrame& Stack)
{
    AFortPickup* Pickup;
    float InFlyTime;
    FVector InStartDirection;
    bool bPlayPickupSound;
    Stack.StepCompiledIn(&Pickup);
    Stack.StepCompiledIn(&InFlyTime);
    Stack.StepCompiledIn(&InStartDirection);
    Stack.StepCompiledIn(&bPlayPickupSound);
    Stack.IncrementCode();
    
    if (!Pawn || !Pickup || Pickup->GetbPickedUp())
        return;

    auto PlayerController = (AFortPlayerControllerAthena*)Pawn->GetController();
    
    if (!PlayerController || !Pickup->GetPrimaryPickupItemEntry().GetItemDefinition())
        return;

    auto WorldInventory = PlayerController->GetWorldInventory();
    if (!WorldInventory)
        return;

    auto MyFortPawn = PlayerController->GetMyFortPawn();
    if (!MyFortPawn)
        return;
    
    FFortItemList& Inventory = WorldInventory->GetInventory();
    TArray<UFortWorldItem*>& ItemInstances = Inventory.GetItemInstances();

    int ItemCount = 0;
    for (int32 i = 0; i < ItemInstances.Num(); i++) {
        if (FortLootPackage::GetQuickbar(ItemInstances[i]->GetItemEntry().GetItemDefinition()) == EFortQuickBars::Primary) 
            ++ItemCount;
    }

    int AmmoCount = 0;
    if (auto* WeaponDef = Cast<UFortWeaponItemDefinition>(Pickup->GetPrimaryPickupItemEntry().GetItemDefinition()))
    {
        if (auto Stats = AFortInventory::GetStats(WeaponDef))
        {
            AmmoCount = Stats->GetClipSize();
        }
    }

    if (ItemCount == 5 && FortLootPackage::GetQuickbar(Pickup->GetPrimaryPickupItemEntry().GetItemDefinition()) == EFortQuickBars::Primary) {
        if (MyFortPawn && MyFortPawn->GetCurrentWeapon() &&
            FortLootPackage::GetQuickbar(MyFortPawn->GetCurrentWeapon()->GetWeaponData()) == EFortQuickBars::Primary) {
        
            FGuid CurrentWeaponGuid = MyFortPawn->GetCurrentWeapon()->GetItemEntryGuid();
            FFortItemEntry* foundItemEntry = nullptr;
        
            for (int32 i = 0; i < ItemInstances.Num(); i++) {
                if (ItemInstances[i]->GetItemEntry().GetItemGuid() == CurrentWeaponGuid) {
                    foundItemEntry = &ItemInstances[i]->GetItemEntry();
                    break;
                }
            }
        
            if (foundItemEntry)
            {
                AFortInventory::SpawnPickup(PlayerController->GetPawn()->GetActorLocation() + PlayerController->GetPawn()->GetActorForwardVector() * 70.f + FVector(0, 0, 50), foundItemEntry->GetItemDefinition(), foundItemEntry->GetCount(), foundItemEntry->GetLoadedAmmo(), EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::Unset, PlayerController->GetMyFortPawn(), true);                AFortInventory::Remove(PlayerController, CurrentWeaponGuid, 1);
                AFortInventory::Remove(PlayerController, CurrentWeaponGuid, 1);
                AFortInventory::GiveItem(PlayerController, Pickup->GetPrimaryPickupItemEntry().GetItemDefinition(), Pickup->GetPrimaryPickupItemEntry().GetCount(), AmmoCount, Pickup->GetPrimaryPickupItemEntry().GetLevel());
            } else {
                AFortInventory::SpawnPickupDirect(PlayerController->GetViewTarget()->GetActorLocation(),
                    Pickup->GetPrimaryPickupItemEntry().GetItemDefinition(), Pickup->GetPrimaryPickupItemEntry().GetCount(), Pickup->GetPrimaryPickupItemEntry().GetLoadedAmmo(), EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::Unset, MyFortPawn, true);
            }
        } else {
            AFortInventory::SpawnPickupDirect(PlayerController->GetViewTarget()->GetActorLocation(),
                Pickup->GetPrimaryPickupItemEntry().GetItemDefinition(), Pickup->GetPrimaryPickupItemEntry().GetCount(), Pickup->GetPrimaryPickupItemEntry().GetLoadedAmmo(), EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::Unset, MyFortPawn, true);
        }
    } 
    else if (FortLootPackage::GetQuickbar(Pickup->GetPrimaryPickupItemEntry().GetItemDefinition()) != EFortQuickBars::Primary) {
        FFortItemEntry* existingItemEntry = nullptr;
        FGuid existingItemGuid;
        
        for (int32 i = 0; i < ItemInstances.Num(); i++) {
            if (ItemInstances[i] && ItemInstances[i]->GetItemEntry().GetItemDefinition() && 
                ItemInstances[i]->GetItemEntry().GetItemDefinition() == Pickup->GetPrimaryPickupItemEntry().GetItemDefinition()) {
                existingItemEntry = &ItemInstances[i]->GetItemEntry();
                existingItemGuid = existingItemEntry->GetItemGuid();
                break;
            }
        }
        
        if (existingItemEntry && existingItemEntry->GetItemDefinition()) {
            FFortItemEntry newEntry = *existingItemEntry;
            newEntry.SetCount(existingItemEntry->GetCount() + Pickup->GetPrimaryPickupItemEntry().GetCount());
            newEntry.SetItemGuid(existingItemGuid);
            newEntry.SetItemDefinition(existingItemEntry->GetItemDefinition()); 
            
            AFortInventory::ReplaceEntry(PlayerController, newEntry);
        } else {
            AFortInventory::GiveItem(PlayerController, Pickup->GetPrimaryPickupItemEntry().GetItemDefinition(), Pickup->GetPrimaryPickupItemEntry().GetCount(), AmmoCount, Pickup->GetPrimaryPickupItemEntry().GetLevel());
        }
    }
    else {
        AFortInventory::GiveItem(PlayerController, Pickup->GetPrimaryPickupItemEntry().GetItemDefinition(), Pickup->GetPrimaryPickupItemEntry().GetCount(), AmmoCount, Pickup->GetPrimaryPickupItemEntry().GetLevel());
    }
    
    Pawn->GetIncomingPickups().Add(Pickup);

    int32 FortPickupLocationDataSize = StaticClassImpl("FortPickupLocationData")->GetSize();
    FFortPickupLocationData* FortPickupLocationData = (FFortPickupLocationData*)VirtualAlloc(0, FortPickupLocationDataSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    
    FortPickupLocationData->SetbPlayPickupSound(bPlayPickupSound);
    FortPickupLocationData->SetFlyTime(0.40f);
    FortPickupLocationData->SetItemOwner(Pawn);
    FortPickupLocationData->SetPickupGuid(Pickup->GetPrimaryPickupItemEntry().GetItemGuid());
    FortPickupLocationData->SetPickupTarget(Pawn);
    FortPickupLocationData->SetStartDirection((FVector_NetQuantizeNormal)InStartDirection);
    Pickup->SetPickupLocationData(*FortPickupLocationData);
    Pickup->OnRep_PickupLocationData();

    Pickup->SetbPickedUp(true);
    Pickup->CallFunc<void>("FortPickup", "OnRep_bPickedUp");
    Pickup->CallFunc<void>("Actor", "K2_DestroyActor");
}
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

    UE_LOG(LogNeon, Log, "ServerHandlePickupInfo");
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
    UE_LOG(LogNeon, Log, "CompletePickupAnimation");
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
    
    UE_LOG(LogNeon, Log, "ServerHandlePickup");
    if (!Pawn || !Pickup || Pickup->GetbPickedUp())
        return;

    auto PlayerController = (AFortPlayerControllerAthena*)Pawn->GetController();
    auto PickupEntry = Pickup->GetPrimaryPickupItemEntry();
    
    if (!PlayerController || !PickupEntry.GetItemDefinition())
        return;

    auto WorldInventory = PlayerController->GetWorldInventory();
    if (!WorldInventory)
        return;

    auto MyFortPawn = PlayerController->GetMyFortPawn();
    if (!MyFortPawn)
        return;

    auto Definition = PickupEntry.GetItemDefinition();
    const float MaxStack = Definition->GetMaxStackSize();
    
    FFortItemList& Inventory = WorldInventory->GetInventory();
    TArray<UFortWorldItem*>& ItemInstances = Inventory.GetItemInstances();
    TArray<FFortItemEntry>& ReplicatedEntries = Inventory.GetReplicatedEntries();
    
    int ItemCount = 0;
    for (int32 i = 0; i < ReplicatedEntries.Num(); i++) {
        if (FortLootPackage::GetQuickbar(ReplicatedEntries[i].GetItemDefinition()) == EFortQuickBars::Primary) 
            ++ItemCount;
    }

    auto HandlePickup = [&]() {
        int AmmoCount = 0;
        if (auto* WeaponDef = Cast<UFortWeaponItemDefinition>(PickupEntry.GetItemDefinition()))
            AmmoCount = AFortInventory::GetStats(WeaponDef)->GetClipSize();

        if (ItemCount == 5 && FortLootPackage::GetQuickbar(PickupEntry.GetItemDefinition()) == EFortQuickBars::Primary) {
            if (MyFortPawn && MyFortPawn->Get<AFortWeapon*>("FortPawn", "CurrentWeapon") &&
                FortLootPackage::GetQuickbar(MyFortPawn->Get<AFortWeapon*>("FortPawn", "CurrentWeapon")->GetWeaponData()) == EFortQuickBars::Primary) {
                
                FGuid CurrentWeaponGuid = MyFortPawn->Get<AFortWeapon*>("FortPawn", "CurrentWeapon")->GetItemEntryGuid();
                FFortItemEntry* foundItemEntry = nullptr;
                
                for (int32 i = 0; i < ReplicatedEntries.Num(); i++) {
                    if (ReplicatedEntries[i].GetItemGuid() == CurrentWeaponGuid) {
                        foundItemEntry = &ReplicatedEntries[i];
                        break;
                    }
                }
                
                if (foundItemEntry) {
                    AFortInventory::SpawnPickupDirect(PlayerController->GetViewTarget()->K2_GetActorLocation(), 
                        foundItemEntry->GetItemDefinition(), foundItemEntry->GetCount(), foundItemEntry->GetLoadedAmmo(), EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::Unset, MyFortPawn, true);
                    AFortInventory::Remove(PlayerController, MyFortPawn->Get<AFortWeapon*>("FortPawn", "CurrentWeapon")->GetItemEntryGuid());
                    AFortInventory::GiveItem(PlayerController, PickupEntry.GetItemDefinition(), PickupEntry.GetCount(), AmmoCount, PickupEntry.GetLevel());
                    return;
                }
            }
            AFortInventory::SpawnPickupDirect(PlayerController->GetViewTarget()->K2_GetActorLocation(),
                PickupEntry.GetItemDefinition(), PickupEntry.GetCount(), PickupEntry.GetLoadedAmmo(), EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::Unset, MyFortPawn, true);
        } else {
            AFortInventory::GiveItem(PlayerController, PickupEntry.GetItemDefinition(), PickupEntry.GetCount(), AmmoCount, PickupEntry.GetLevel());
        }
    };

    if (PickupEntry.GetItemDefinition()->IsStackable()) {
        FFortItemEntry* foundStackableEntry = nullptr;
        
        for (int32 i = 0; i < ReplicatedEntries.Num(); i++) {
            if (ReplicatedEntries[i].GetItemDefinition() == PickupEntry.GetItemDefinition() && 
                ReplicatedEntries[i].GetCount() < MaxStack) {
                foundStackableEntry = &ReplicatedEntries[i];
                break;
            }
        }

        if (foundStackableEntry) {
            FFortItemEntryStateValue* State = nullptr;
            TArray<FFortItemEntryStateValue>& StateValues = foundStackableEntry->GetStateValues();
            
            for (int32 i = 0; i < StateValues.Num(); i++) {
                if (StateValues[i].GetStateType() == EFortItemEntryState::ShouldShowItemToast) {
                    State = &StateValues[i];
                    break;
                }
            }
            
            if (!State) {
             //   auto Index = foundStackableEntry->GetStateValues().AddUnitalized(StaticClassImpl("FFortItemEntryStateValue")->GetSize());
               // foundStackableEntry->GetStateValues()[Index].SetIntValue()
            } else {
                State->SetIntValue(true);
            }

            foundStackableEntry->SetCount(foundStackableEntry->GetCount() + PickupEntry.GetCount());
            if (foundStackableEntry->GetCount() > MaxStack) {
                int overflow = foundStackableEntry->GetCount() - MaxStack;
                foundStackableEntry->SetCount(MaxStack);
                if (PickupEntry.GetItemDefinition()->GetbAllowMultipleStacks() && ItemCount < 5) {
                    AFortInventory::GiveItem(PlayerController, PickupEntry.GetItemDefinition(), overflow, 
                        Cast<UFortWeaponItemDefinition>(PickupEntry.GetItemDefinition()) ? AFortInventory::GetStats(Cast<UFortWeaponItemDefinition>(PickupEntry.GetItemDefinition()))->GetClipSize() : 0, PickupEntry.GetLevel());
                } else {
                    AFortInventory::SpawnPickupDirect(PlayerController->GetViewTarget()->K2_GetActorLocation(),
                        PickupEntry.GetItemDefinition(), overflow, PickupEntry.GetLoadedAmmo(), EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::Unset, MyFortPawn, true);
                }
            }
            AFortInventory::ReplaceEntry(PlayerController, *foundStackableEntry);
        } else {
            if (PickupEntry.GetCount() > MaxStack) {
                int overflow = PickupEntry.GetCount() - MaxStack;
                PickupEntry.SetCount(MaxStack);
                if (PickupEntry.GetItemDefinition()->GetbAllowMultipleStacks() && ItemCount < 5) {
                    AFortInventory::GiveItem(PlayerController, PickupEntry.GetItemDefinition(), overflow,
                        Cast<UFortWeaponItemDefinition>(PickupEntry.GetItemDefinition()) ? AFortInventory::GetStats(Cast<UFortWeaponItemDefinition>(PickupEntry.GetItemDefinition()))->GetClipSize() : 0, PickupEntry.GetLevel());
                } else {
                    AFortInventory::SpawnPickupDirect(PlayerController->GetViewTarget()->K2_GetActorLocation(),
                        PickupEntry.GetItemDefinition(), overflow, PickupEntry.GetLoadedAmmo(), EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::Unset, MyFortPawn, true);
                }
            }
            HandlePickup();
        }
    } else {
        HandlePickup();
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
#include "pch.h"
#include "../Header/FortPlayerPawn.h"

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

    AFortInventory::GiveItem((AFortPlayerControllerAthena*)Pawn->GetController(), Pickup->GetPrimaryPickupItemEntry().GetItemDefinition(), Pickup->GetPrimaryPickupItemEntry().GetCount(), 30, 0);

    Pawn->GetIncomingPickups().Add(Pickup);

    int32 FortPickupLocationDataSize = StaticClassImpl("FortPickupLocationData")->GetSize();
    FFortPickupLocationData* FortPickupLocationData = (FFortPickupLocationData*)VirtualAlloc(0, FortPickupLocationDataSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    
    FortPickupLocationData->SetbPlayPickupSound(bPlayPickupSound);
    FortPickupLocationData->SetFlyTime(0.3f);
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


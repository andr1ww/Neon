#include "pch.h"
#include "../Header/FortPlayerPawn.h"

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

#include "pch.h"
#include "../Header/FortPlayerController.h"

void AFortPlayerControllerAthena::ServerAcknowledgePossession(AFortPlayerControllerAthena* PlayerController, APawn* Pawn)
{
    PlayerController->SetAcknowledgedPawn(Pawn);
}

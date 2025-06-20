#include "pch.h"
#include "../Header/FortPlayerController.h"

#include "Neon/Finder/Header/Finder.h"

void* (*ApplyCharacterCustomization)(UObject*, APawn*) = decltype(ApplyCharacterCustomization)(Finder->ApplyCharacterCustomization());

void AFortPlayerControllerAthena::ServerAcknowledgePossession(AFortPlayerControllerAthena* PlayerController, APawn* Pawn)
{
    UE_LOG(LogNeon, Log, "ServerAcknowledgePossession");
    PlayerController->SetAcknowledgedPawn(Pawn);
    
    ApplyCharacterCustomization(PlayerController->Get<UObject*>("APlayerController", "PlayerState"), Pawn);
}

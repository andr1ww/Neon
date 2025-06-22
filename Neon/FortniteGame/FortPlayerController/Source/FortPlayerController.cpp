#include "pch.h"
#include "../Header/FortPlayerController.h"

#include "Neon/Finder/Header/Finder.h"

void* (*ApplyCharacterCustomization)(UObject*, APawn*) = decltype(ApplyCharacterCustomization)(Finder->ApplyCharacterCustomization());

void AFortPlayerControllerAthena::ServerAcknowledgePossession(AFortPlayerControllerAthena* PlayerController, FFrame& Stack) 
{
    APawn* PawnToAcknowledge;
    Stack.StepCompiledIn(&PawnToAcknowledge);
    Stack.IncrementCode();
    
    UE_LOG(LogNeon, Log, "ServerAcknowledgePossession");
    if (!PlayerController) return;
    PlayerController->SetAcknowledgedPawn(PawnToAcknowledge);
    
   // ApplyCharacterCustomization(PlayerController->Get<UObject*>("PlayerController", "PlayerState"), PawnToAcknowledge);
}
 
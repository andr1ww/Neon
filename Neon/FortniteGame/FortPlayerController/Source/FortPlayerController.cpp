#include "pch.h"
#include "../Header/FortPlayerController.h"

#include "Neon/Finder/Header/Finder.h"

void* (*ApplyCharacterCustomization)(UObject*, APawn*) = decltype(ApplyCharacterCustomization)(Finder->ApplyCharacterCustomization());

void AFortPlayerControllerAthena::ServerAcknowledgePossession(AFortPlayerControllerAthena* PlayerController, FFrame& Stack) 
{
    APawn* PawnToAcknowledge;
    Stack.StepCompiledIn(&PawnToAcknowledge);
    Stack.IncrementCode();
    
    if (!PlayerController) return;
    PlayerController->SetAcknowledgedPawn(PawnToAcknowledge);
    PlayerController->GetPlayerState().SetHeroType(PlayerController->GetCosmeticLoadoutPC().GetCharacter().GetHeroDefinition());
    
   ApplyCharacterCustomization(PlayerController->Get<UObject*>("PlayerController", "PlayerState"), PawnToAcknowledge);
}
 
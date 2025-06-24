#include "pch.h"
#include "../Header/FortPlayerController.h"

#include "FortniteGame/AbilitySystemComponent/Header/AbilitySystemComponent.h"
#include "Neon/Finder/Header/Finder.h"

void AFortPlayerControllerAthena::ServerAcknowledgePossession(AFortPlayerControllerAthena* PlayerController, FFrame& Stack) 
{
    APawn* PawnToAcknowledge;
    Stack.StepCompiledIn(&PawnToAcknowledge);
    Stack.IncrementCode();
    
    if (!PlayerController) return;
    PlayerController->SetAcknowledgedPawn(PawnToAcknowledge);
PlayerController->GetPlayerState()->SetHeroType(PlayerController->GetCosmeticLoadoutPC().GetCharacter()->GetHeroDefinition());
    void* (*ApplyCharacterCustomization)(AFortPlayerStateAthena*, APawn*) = decltype(ApplyCharacterCustomization)(Finder->ApplyCharacterCustomization());
    
    ApplyCharacterCustomization(PlayerController->GetPlayerState(), PawnToAcknowledge);
    //static UFortAbilitySet* AbilitySet = (UFortAbilitySet*)GUObjectArray.FindObject("GAS_AthenaPlayer");
    //UAbilitySystemComponent::GiveAbilitySet(PlayerController->GetPlayerState().GetAbilitySystemComponent(), AbilitySet);
}
 
#include "pch.h"
#include "../Header/FortPlayerController.h"

#include "FortniteGame/AbilitySystemComponent/Header/AbilitySystemComponent.h"
#include "Neon/Finder/Header/Finder.h"

void* (*ApplyCharacterCustomization)(UObject*, APawn*) = decltype(ApplyCharacterCustomization)(Finder->ApplyCharacterCustomization());

void AFortPlayerControllerAthena::ServerAcknowledgePossession(AFortPlayerControllerAthena* PlayerController, FFrame& Stack)
{
    APawn* PawnToAcknowledge;
    Stack.StepCompiledIn(&PawnToAcknowledge);
    Stack.IncrementCode();

    if (!PlayerController) return;
    PlayerController->SetAcknowledgedPawn(PawnToAcknowledge);
    PlayerController->GetPlayerState()->SetHeroType(PlayerController->GetCosmeticLoadoutPC().GetCharacter().GetHeroDefinition());

    ApplyCharacterCustomization(PlayerController->Get<L"PlayerState", UObject*>(), PawnToAcknowledge);
    //static UFortAbilitySet* AbilitySet = (UFortAbilitySet*)GUObjectArray.FindObject("GAS_AthenaPlayer");
    //UAbilitySystemComponent::GiveAbilitySet(PlayerController->GetPlayerState().GetAbilitySystemComponent(), AbilitySet);
}
 
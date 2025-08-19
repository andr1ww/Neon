#include "pch.h"
#include "../Header/FortAthenaMutator.h"

void AFortAthenaMutator_GiveItemsAtGamePhaseStep::OnGamePhaseStepChanged(AFortAthenaMutator_GiveItemsAtGamePhaseStep* Mutator, FFrame& Stack)
{
    EAthenaGamePhaseStep GamePhaseStep;
    Stack.StepCompiledIn(&GamePhaseStep);
    Stack.IncrementCode();

    UE_LOG(LogNeon, Log, __FUNCTION__);

    if (Mutator->GetPhaseToGiveItems() == GamePhaseStep) {
        for (auto& Controller : ((AFortGameModeAthena*)UWorld::GetWorld()->GetAuthorityGameMode())->GetAlivePlayers())
            for (auto& Item : Mutator->GetItemsToGive()) {
                int ClipSize = Item.ItemToDrop->IsA<UFortWeaponItemDefinition>() ? AFortInventory::GetStats((UFortWeaponItemDefinition*)Item.ItemToDrop)->GetClipSize() : 0;
                AFortInventory::GiveItem(Controller, Item.ItemToDrop, Item.NumberToGive.Value, ClipSize, 0);
            }
    }
}

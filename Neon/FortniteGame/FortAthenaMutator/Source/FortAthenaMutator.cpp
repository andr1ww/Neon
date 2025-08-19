#include "pch.h"
#include "../Header/FortAthenaMutator.h"

#include "Engine/GameplayStatics/Header/GameplayStatics.h"

void AFortAthenaMutator_GiveItemsAtGamePhaseStep::OnGamePhaseStepChanged(AFortAthenaMutator_GiveItemsAtGamePhaseStep* Mutator, FFrame& Stack)
{
    EAthenaGamePhaseStep GamePhaseStep;
    Stack.StepCompiledIn(&GamePhaseStep);
    Stack.IncrementCode();

    UE_LOG(LogNeon, Log, __FUNCTION__);

    if (UWorld::GetWorld()->GetAuthorityGameMode()->GetCurrentPlaylistName().ToString().ToString().contains("Barrier"))
    {
        static bool bSpawnedWall = false;
        if (!bSpawnedWall)
        {
            bSpawnedWall = true;
            UGameplayStatics::SpawnActorOG<AActor>(Runtime::StaticFindObject<UClass>("/Game/Athena/Playlists/Barrier/Barrier.Barrier_C"), FVector{ 0, 0, -2000 }, FRotator{});
        }
    }

    if (Mutator->GetPhaseToGiveItems() == GamePhaseStep) {
        for (auto& Controller : ((AFortGameModeAthena*)UWorld::GetWorld()->GetAuthorityGameMode())->GetAlivePlayers())
            for (auto& Item : Mutator->GetItemsToGive()) {
                int ClipSize = Item.ItemToDrop->IsA<UFortWeaponItemDefinition>() ? AFortInventory::GetStats((UFortWeaponItemDefinition*)Item.ItemToDrop)->GetClipSize() : 0;
                AFortInventory::GiveItem(Controller, Item.ItemToDrop, Item.NumberToGive.Value, ClipSize, 0);
            }
    }
}

void AFortAthenaMutator_Barrier::OnGamePhaseStepChanged(AFortAthenaMutator_Barrier* Mutator, FFrame& Stack)
{
    EAthenaGamePhaseStep GamePhaseStep;
    Stack.StepCompiledIn(&GamePhaseStep);
    Stack.IncrementCode();

    UE_LOG(LogNeon, Log, __FUNCTION__);

    if (UWorld::GetWorld()->GetAuthorityGameMode()->GetCurrentPlaylistName().ToString().ToString().contains("Barrier"))
    {
        static bool bSpawnedWall = false;
        if (!bSpawnedWall)
        {
            bSpawnedWall = true;
            UGameplayStatics::SpawnActorOG<AActor>(Runtime::StaticFindObject<UClass>("/Game/Athena/Playlists/Barrier/Barrier.Barrier_C"), FVector{ 0, 0, -2000 }, FRotator{});
        }
    }
}

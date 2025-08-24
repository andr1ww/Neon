#include "pch.h"
#include "../Header/FortMinigameSettingsBuilding.h"

#include "Engine/NetDriver/Header/NetDriver.h"

void AFortMinigameSettingsBuilding::BeginPlay(AFortMinigameSettingsBuilding* Building)
{
    auto GameState = UWorld::GetWorld()->GetGameState();

    if (!GameState)
        return;

    Building->SetSettingsVolume(GameState->GetVolumeManager()->GetVolumeForActor(Building));

    UE_LOG(LogNeon, Log, __FUNCTION__);
}

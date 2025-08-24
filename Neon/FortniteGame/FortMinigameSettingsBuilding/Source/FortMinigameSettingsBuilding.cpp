#include "pch.h"
#include "../Header/FortMinigameSettingsBuilding.h"

#include "Engine/NetDriver/Header/NetDriver.h"

void AFortMinigameSettingsBuilding::BeginPlay(AFortMinigameSettingsBuilding* Building)
{
    auto GameState = UWorld::GetWorld()->GetGameState();

    if (!GameState || !Building)
        return;

    Building->SetSettingsVolume((AFortVolume*)Building->GetOwner());

    UE_LOG(LogNeon, Log, __FUNCTION__);
    UE_LOG(LogNeon, Log, "Volume: %s", Building->GetOwner()->GetFName().ToString().ToString().c_str());
}

#pragma once
#include "pch.h"

#include "Engine/Actor/Header/Actor.h"
#include "FortniteGame/FortAthenaCreativePortal/Header/FortAthenaCreativePortal.h"

class AFortMinigameSettingsBuilding : public AActor
{
public:
    DefHookOg(void, BeginPlay, AFortMinigameSettingsBuilding*);
    DEFINE_PTR(AFortVolume, AFortMinigameSettingsBuilding, SettingsVolume)
public:
    DECLARE_STATIC_CLASS(AFortMinigameSettingsBuilding)
    DECLARE_DEFAULT_OBJECT(AFortMinigameSettingsBuilding)
};

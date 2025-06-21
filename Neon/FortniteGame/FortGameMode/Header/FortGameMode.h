#pragma once
#include "pch.h"
#include "Engine/Actor/Header/Actor.h"
#include "Engine/PlayerController/Header/PlayerController.h"
#include "Engine/Transform/Header/Transform.h"
#include "FortniteGame/FortGameState/Header/FortGameState.h"

class AFortGameModeAthena : public SDK::UObject
{
public:
    DEFINE_MEMBER(AFortGameStateAthena*, AFortGameModeAthena,GameState);
    DEFINE_MEMBER(int32, AFortGameModeAthena, CurrentPlaylistId);
    DEFINE_MEMBER(FName, AFortGameModeAthena, CurrentPlaylistName);
    DEFINE_MEMBER(int32, AFortGameModeAthena, WarmupRequiredPlayerCount);
    DEFINE_MEMBER(int32, AFortGameModeAthena, NumPlayers);
    DEFINE_BOOL(AFortGameModeAthena, bWorldIsReady);
public:
    DECLARE_DEFAULT_OBJECT(AFortGameModeAthena);
    DECLARE_STATIC_CLASS(AFortGameModeAthena);
public:
    static bool ReadyToStartMatch(AFortGameModeAthena* GameMode);
    static APawn* SpawnDefaultPawnFor(AFortGameModeAthena* GameMode, APlayerController* NewPlayer, AActor* StartSpot);
};

inline bool (*ReadyToStartMatchOriginal)(AFortGameModeAthena* GameMode);

#pragma once
#include "pch.h"
#include "Engine/Actor/Header/Actor.h"
#include "Engine/PlayerController/Header/PlayerController.h"
#include "Engine/Transform/Header/Transform.h"
#include "FortniteGame/FortGameState/Header/FortGameState.h"
#include "FortniteGame/FortPlayerController/Header/FortPlayerController.h"
#include "FortniteGame/FortServerBotManager/Header/FortServerBotManager.h"

struct FItemAndCount final 
{
public:
    DEFINE_MEMBER(int32, FItemAndCount, Count);
    DEFINE_PTR(UFortItemDefinition, FItemAndCount, Item);
};

class AGameModeBase : public UObject
{
public:
    DEFINE_PTR(AFortGameStateAthena, AGameModeBase, GameState);
};

class AFortGameMode : public AGameModeBase
{
public:
    DEFINE_MEMBER(int32, AFortGameMode, CurrentPlaylistId);
    DEFINE_MEMBER(FName, AFortGameMode, CurrentPlaylistName);
    DEFINE_BOOL(AFortGameMode, bWorldIsReady);
};

static inline AFortAthenaMutator_Bots* BotMutator = nullptr;

class AFortGameModeAthena : public AFortGameMode
{
public:
    DEFINE_MEMBER(int32, AFortGameModeAthena, WarmupRequiredPlayerCount);
    DEFINE_MEMBER(int32, AFortGameModeAthena, NumPlayers);
    DEFINE_MEMBER(TArray<FItemAndCount>, AFortGameModeAthena, StartingItems);
    DEFINE_MEMBER(TArray<AFortPlayerControllerAthena*>, AFortGameModeAthena, AlivePlayers);
    DEFINE_PTR(UFortServerBotManagerAthena, AFortGameModeAthena, ServerBotManager);
public:
    DECLARE_DEFAULT_OBJECT(AFortGameModeAthena);
    DECLARE_STATIC_CLASS(AFortGameModeAthena);
public:
    static bool ReadyToStartMatch(AFortGameModeAthena* GameMode, FFrame& Stack, bool* Result);
    static APawn* SpawnDefaultPawnFor(AFortGameModeAthena* GameMode, AFortPlayerControllerAthena* NewPlayer, AActor* StartSpot);
};

inline bool (*ReadyToStartMatchOriginal)(AFortGameModeAthena* GameMode);

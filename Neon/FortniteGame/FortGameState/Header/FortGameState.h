#pragma once
#include "pch.h"

#include "Engine/Array/Header/Array.h"
#include "FortniteGame/BuildingSMActor/Header/BuildingSMActor.h"
#include "FortniteGame/FortAthenaMapInfo/Header/FortAthenaMapInfo.h"
#include "FortniteGame/FortPlaylistAthena/Header/FortPlaylistAthena.h"

class AFortGameState : public UObject
{
public:
    DEFINE_MEMBER(TArray<struct FAdditionalLevelStreamed>, AFortGameState, AdditionalPlaylistLevelsStreamed);
};

using Map = TMap<TSubclassOf<ABuildingActor>, int32>;

class AFortGameStateAthena : public AFortGameState
{
public:
    DEFINE_MEMBER(Map, AFortGameStateAthena, AllPlayerBuildableClassesIndexLookup);
    DEFINE_MEMBER(TArray<TSubclassOf<ABuildingActor>>, UWorld, BuildingActorClasses);
    DEFINE_PTR(FPlaylistPropertyArray, AFortGameStateAthena, CurrentPlaylistInfo)
    DEFINE_PTR(UFortPlaylistAthena, AFortGameStateAthena, CurrentPlaylistData)
    void OnRep_CurrentPlaylistId();
    void OnRep_CurrentPlaylistData();
    void OnRep_CurrentPlaylistInfo();
    DEFINE_MEMBER(int32, AFortGameStateAthena, CurrentPlaylistId);
    DEFINE_PTR(AFortAthenaMapInfo, AFortGameStateAthena, MapInfo);
public:
    DECLARE_STATIC_CLASS(AFortGameStateAthena);
};



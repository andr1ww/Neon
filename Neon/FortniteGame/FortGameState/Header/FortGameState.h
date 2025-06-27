#pragma once
#include "pch.h"

#include "Engine/Array/Header/Array.h"
#include "FortniteGame/FortAthenaMapInfo/Header/FortAthenaMapInfo.h"
#include "FortniteGame/FortPlaylistAthena/Header/FortPlaylistAthena.h"

class AFortGameStateAthena : public SDK::UObject
{
public:
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



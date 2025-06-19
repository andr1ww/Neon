#pragma once
#include "pch.h"

#include "FortniteGame/FortAthenaMapInfo/Header/FortAthenaMapInfo.h"
#include "FortniteGame/FortPlaylistAthena/Header/FortPlaylistAthena.h"

class AFortGameStateAthena : public SDK::UObject
{
private:
    DEFINE_MEMBER(FPlaylistPropertyArray, AFortGameStateAthena, CurrentPlaylistInfo)
    DEFINE_MEMBER(UFortPlaylistAthena*, AFortGameStateAthena, CurrentPlaylistData)
    void OnRep_CurrentPlaylistId();
    void OnRep_CurrentPlaylistInfo();
public:
    DEFINE_MEMBER(int32, AFortGameStateAthena, CurrentPlaylistId);
    DEFINE_MEMBER(AFortAthenaMapInfo*, AFortGameStateAthena, MapInfo);

public:
    void SetPlaylist(class AFortGameModeAthena* GameMode, UFortPlaylistAthena* PlaylistToUse);
public:
    DECLARE_STATIC_CLASS(AFortGameStateAthena);
};



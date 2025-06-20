#include "pch.h"
#include "../Header/FortGameState.h"

void AFortGameStateAthena::OnRep_CurrentPlaylistId()
{
    static auto OnRep_CurrentPlaylistId = (UFunction*)GUObjectArray.FindObject("/Script/FortniteGame.FortGameStateAthena.OnRep_CurrentPlaylistId");

    if (OnRep_CurrentPlaylistId)
        this->ProcessEvent(OnRep_CurrentPlaylistId, nullptr);
}


void AFortGameStateAthena::OnRep_CurrentPlaylistInfo()
{
    static auto OnRep_CurrentPlaylistInfo = (UFunction*)GUObjectArray.FindObject("/Script/FortniteGame.FortGameStateAthena.OnRep_CurrentPlaylistInfo");

    if (OnRep_CurrentPlaylistInfo)
        this->ProcessEvent(OnRep_CurrentPlaylistInfo, nullptr);
}

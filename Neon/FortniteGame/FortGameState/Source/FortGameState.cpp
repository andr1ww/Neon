#include "pch.h"
#include "../Header/FortGameState.h"

void AFortGameStateAthena::OnRep_CurrentPlaylistId()
{
    GameState->Call<"OnRep_CurrentPlaylistId">();
}

void AFortGameStateAthena::OnRep_CurrentPlaylistInfo()
{
	GameState->Call<"OnRep_CurrentPlaylistInfo">();
}

void AFortGameStateAthena::OnRep_CurrentPlaylistData()
{
    GameState->Call<"OnRep_CurrentPlaylistData">();
}
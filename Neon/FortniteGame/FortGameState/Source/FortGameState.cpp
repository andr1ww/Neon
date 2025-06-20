#include "pch.h"
#include "../Header/FortGameState.h"

void AFortGameStateAthena::OnRep_CurrentPlaylistId()
{
    this->CallFunc<void>("FortGameStateAthena","OnRep_CurrentPlaylistId");
}


void AFortGameStateAthena::OnRep_CurrentPlaylistInfo()
{
    this->CallFunc<void>("FortGameStateAthena","OnRep_CurrentPlaylistInfo");
}

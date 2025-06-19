#include "pch.h"
#include "../Header/FortGameMode.h"

bool AFortGameModeAthena::ReadyToStartMatch(AFortGameModeAthena* GameMode)
{
    AFortGameStateAthena* GameState = GameMode->GetGameState();
    if (!GameState)
        return false;

    if (GameMode->Get<int32>("FortGameModeAthena", "CurrentPlaylistId") == -1)
    {
        
    }


    auto Ret = GameMode->Get<int32>("FortGameModeAthena", "NumPlayers") > 0;
    return Ret;
}

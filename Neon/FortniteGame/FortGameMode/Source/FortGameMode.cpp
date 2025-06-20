#include "pch.h"
#include "../Header/FortGameMode.h"

#include "Engine/NetDriver/Header/NetDriver.h"

bool AFortGameModeAthena::ReadyToStartMatch(AFortGameModeAthena* GameMode)
{
    AFortGameStateAthena* GameState = GameMode->GetGameState();
    if (!GameState)
        return false;

    if (GameMode->Get<int32>("FortGameModeAthena", "CurrentPlaylistId") == -1)
    {
        UFortPlaylistAthena* Playlist = (UFortPlaylistAthena*)GUObjectArray.FindObject("Playlist_DefaultSolo");

        GameState->GetCurrentPlaylistInfo().SetBasePlaylist(Playlist);
        GameState->GetCurrentPlaylistInfo().SetOverridePlaylist(Playlist);
        GameState->GetCurrentPlaylistInfo().SetPlaylistReplicationKey(GameState->GetCurrentPlaylistInfo().GetPlaylistReplicationKey() + 1);
        GameState->GetCurrentPlaylistInfo().MarkArrayDirty();

        GameState->SetCurrentPlaylistId(Playlist->GetPlaylistId());
        GameMode->SetCurrentPlaylistId(Playlist->GetPlaylistId());
        GameMode->SetCurrentPlaylistName(Playlist->GetPlaylistName());
        
        GameState->OnRep_CurrentPlaylistId();
        GameState->OnRep_CurrentPlaylistInfo();

        GameMode->SetWarmupRequiredPlayerCount(1);
    }

    if (!GameState->GetMapInfo()) return false;

    if (!UWorld::GetWorld()->GetNetDriver())
    {
     // listening   
    }
    
    auto Ret = GameMode->GetbWorldIsReady() && GameMode->GetNumPlayers() > 0;
    return Ret;
}

#pragma once
#include "pch.h"

#include "FortniteGame/FortPlayerController/Header/FortPlayerController.h"

namespace Nexa
{
    struct State
    {
        FPlaylistData PlaylistData = FPlaylistData();
        int StartingPlayers = 0;
        bool bHasPlayerJoined = false;
    };
    
    inline State& GetState()
    {
        static State instance;
        return instance;
    }
    
    inline void SetPlaylistData(FPlaylistData Data)
    {
        GetState().PlaylistData = Data;
    }

    inline void SetStartingPlayers(int Players)
    {
        GetState().StartingPlayers = Players;
    }

    inline void SetHasPlayerJoined(bool HasJoined)
    {
        GetState().bHasPlayerJoined = HasJoined;
    }
}

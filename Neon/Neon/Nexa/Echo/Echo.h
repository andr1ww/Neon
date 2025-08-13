#pragma once
#include "pch.h"
#include <Neon/Neon/Nexa/Libraries/json.hpp>

#include "FortniteGame/FortPlaylistAthena/Header/FortPlaylistAthena.h"
using json = nlohmann::json;

namespace Nexa::Echo
{
    void CreateEchoSession();
    string FetchEchoSessionPlaylist();
    void EchoSessionStarted();
    void CloseEchoSession();
    void LowerEchoSessionCount();
    void EchoSessionUpdate(UFortPlaylistAthena* Playlist);
    void EchoSessionHeartbeat();

    inline DWORD WINAPI Heartbeat(LPVOID)
    {
        while (true)
        {
            Sleep(30000);
            std::thread t([]() {
                Nexa::Echo::EchoSessionHeartbeat();
            });
            t.detach(); 
        }
    }
}
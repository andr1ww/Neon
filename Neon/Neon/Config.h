#pragma once
#include "pch.h"

namespace Config
{
    inline bool bLogProcessEvent = true;
    inline bool bLateGame = false;
    inline bool bCreative = false;
    inline bool bGameSessions = false;
    inline bool bEchoSessions = true;
    inline std::string Playlist = "Playlist_DefaultSolo";
    inline int Port = 7777;

    namespace Echo
    {
        inline string ServerAddress;
        inline string Region;
        inline string Session;
    }
}
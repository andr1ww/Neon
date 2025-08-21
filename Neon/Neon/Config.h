#pragma once
#include "pch.h"

namespace Config
{
    inline bool bLogProcessEvent = false;
    inline bool bLateGame = false;
    inline bool bCreative = true;
    inline bool bGameSessions = false;
    inline bool bEchoSessions = false;
    inline std::string Playlist = "Playlist_DefaultSolo";
    inline int Port = 7777;

    namespace Echo
    {
        inline string ServerAddress;
        inline string Region;
        inline string Session;
    }
}
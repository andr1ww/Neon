#pragma once
#include "pch.h"

namespace Config
{
    inline bool bLateGame = true;
    inline bool bCreative = false;
    inline bool bGameSessions = false;
    inline bool bEchoSessions = true;
    inline int Port = 7777;

    namespace Echo
    {
        inline string ServerAddress;
        inline string Region;
        inline string Session;
    }
}
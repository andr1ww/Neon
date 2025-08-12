#pragma once
#include "pch.h"
#include <regex>

#include "FortniteGame/FortPlayerState/Header/FortPlayerState.h"

namespace Nexa::Helpers
{
    struct PlayerData
    {
        std::string AccountID;
    };

    inline std::unordered_map<AFortPlayerStateAthena*, PlayerData> PlayerMap;

    inline std::string GetAccountID(AFortPlayerStateAthena* PlayerState)
    {
        if (!PlayerState) return "";
        
        auto it = PlayerMap.find(PlayerState);
        if (it != PlayerMap.end())
        {
            return it->second.AccountID;
        }
        
        uint8* ReplicationBytesData = PlayerState->GetUniqueId().ReplicationBytes.GetData();
        uint8 AIDLen = *(uint8*)(ReplicationBytesData + 1);
        std::stringstream ss;

        for (int i = 0; i < AIDLen; i++)
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)*(uint8*)(ReplicationBytesData + 2 + i);

        PlayerData data;
        data.AccountID = ss.str();
        PlayerMap[PlayerState] = data;
        
        return data.AccountID;
    }
}

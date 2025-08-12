#include "pch.h"
#include "Echo.h"
#include "Neon/Config.h"
#include "Neon/Nexa/Curl/Curl.h"

void Nexa::Echo::CreateEchoSession() {
    vector<const char*> headers = {"Content-Type: application/text"};
    string IPConfigRes = Curl::Get("https://ifconfig.me/ip", headers);

    if (IPConfigRes.empty())
    {
        UE_LOG(LogNeon, Log, TEXT("Failed to get IP"));
        return;
    }

    Config::Echo::ServerAddress = IPConfigRes;

    json Session;
    try {
        Session = {
            {"ServerAddr", IPConfigRes},
            {"ServerPort", Config::Port},
            {"Attributes", {
                {"GameType", "EFortGameType::BR"},
                {"bIsTournament", false},
                {"bLimitedTimeMode", false},
                {"RatingType", "ERewardPlacementBonusType::Solo"},
                {"MaxPlayers", 100},
                {"MaxTeamCount", 100},
                {"MaxTeamSize", 1},
                {"MaxSocialPartySize", 1},
                {"MaxSquadSize", 1}
            }},
            {"JoinInProgress", false},
            {"Version", "12.61"}
        };
    } catch (const nlohmann::json::exception& e) {
        UE_LOG(LogNeon, Log, TEXT("Failed to create echo session JSON: %hs"), e.what());
        return;
    }

    string Endpoint = "http://147.93.1.220:2087/nxa/echo/session/construct";
    string SessionRes;

    try {
        string jsonStr = Session.dump();
        vector<const char*> postHeaders = {"Content-Type: application/json"};
        SessionRes = Curl::Post(Endpoint, jsonStr, postHeaders);
    } catch (const nlohmann::json::exception& e) {
        UE_LOG(LogNeon, Log, TEXT("Failed to create echo session: %hs"), e.what());
        return;
    } catch (const std::exception& e) {
        UE_LOG(LogNeon, Log, TEXT("Error creating echo session: %hs"), e.what());
        return;
    }

    if (SessionRes.empty())
    {
        UE_LOG(LogNeon, Log, TEXT("Empty response from echo session create"));
        return;
    }

    try {
        json SessionJson = json::parse(SessionRes);
        if (SessionJson.contains("id")) {
            Config::Echo::Session = SessionJson["id"];
        } else {
            UE_LOG(LogNeon, Log, TEXT("Invalid session response format"));
        }
        if (SessionJson.contains("ServerRegion")) {
            Config::Echo::Region = SessionJson["ServerRegion"];
        } else {
            UE_LOG(LogNeon, Log, TEXT("Invalid session response format"));
        }
    } catch (const nlohmann::json::exception& e) {
        UE_LOG(LogNeon, Log, TEXT("Failed to parse echo session response: %hs"), e.what());
    }
}

string Nexa::Echo::FetchEchoSessionPlaylist()
{
    if (Config::Echo::Session.empty())
    {
        CreateEchoSession();
        return string();
    }
    
    string Endpoint = "http://147.93.1.220:2087/nxa/echo/session/playlist/" + Config::Echo::Session;
    string PlaylistRes;
    
    try {
        PlaylistRes = Curl::Get(Endpoint, vector<const char*>{"Content-Type: application/json"});
    } catch (const nlohmann::json::exception& e) {
        UE_LOG(LogNeon, Log, TEXT("Error fetching echo session playlist: %hs"), e.what());
        return string();
    } catch (const std::exception& e) {
        UE_LOG(LogNeon, Log, TEXT("Error fetching echo session playlist: %hs"), e.what());
        return string();
    }

    if (PlaylistRes.empty())
    {
        UE_LOG(LogNeon, Log, TEXT("Empty response from echo session playlist!"));
        return string();
    }

    try
    {
        json PlaylistJson = json::parse(PlaylistRes);
        if (PlaylistJson["Status"] == "OK")
        {
            return PlaylistJson["Playlist"];
        } else {
            return string();
        }
        
    } catch (const nlohmann::json::exception& e) {
        UE_LOG(LogNeon, Log, TEXT("Error parsing echo session playlist: %hs"), e.what());
        return string();
    } catch (const std::exception& e) {
        UE_LOG(LogNeon, Log, TEXT("Error parsing echo session playlist: %hs"), e.what());
        return string();
    }
}

void Nexa::Echo::EchoSessionStarted()
{
    string Endpoint = "http://147.93.1.220:2087/nxa/echo/session/start/" + Config::Echo::Session;
    string StartedRes;
    
    try {
        StartedRes = Curl::Post(Endpoint, string(), vector{"Content-Type", "application/json"});
    } catch (const nlohmann::json::exception& e) {
        return;
    } catch (const std::exception& e) {
        return;
    }

    if (StartedRes.empty())
    {
        return;
    }

    UE_LOG(LogNeon, Log, "Echo session started on Port: %d", Config::Port);
}

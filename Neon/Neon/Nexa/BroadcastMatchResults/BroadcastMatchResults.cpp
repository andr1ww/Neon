#include "pch.h"
#include "BroadcastMatchResults.h"

#include "../Nexa.h"
#include "../NexaAuth.h"
#include "../NexaHelpers.h"
#include "../Curl/Curl.h"
#include "FortniteGame/FortQuestManager/Header/FortQuestManager.h"
#include "Neon/Config.h"


void Nexa::BroadcastMatchResults(AFortPlayerControllerAthena* Controller, AFortPlayerStateAthena* PlayerState, FPlaylistData PlaylistData, AFortGameModeAthena* GameMode)
{
    static int StartingPlayers = 0;
    if (StartingPlayers == 0)
    {
        StartingPlayers = GameMode->GetAlivePlayers().Num();
    }
    
    AFortGameStateAthena* GameState = UWorld::GetWorld()->GetGameState();
    std::string AccountID = Helpers::GetAccountID(PlayerState);
    UE_LOG(LogNeon, Log, "Broadcasting Match Results for AccountID: %s", AccountID.c_str());
    json Payload;

    // player controller object
    Payload["FortPlayerControllerAthena"]["XPComponent"]["TotalXpEarned"] = Controller->GetXPComponent()->GetTotalXpEarned();
    if (Controller->GetMatchReport() != nullptr) {
        Payload["FortPlayerControllerAthena"]["MatchReport"]["Place"] = Controller->GetMatchReport()->GetTeamStats().GetPlace();
        Payload["FortPlayerControllerAthena"]["MatchReport"]["TotalPlayers"] = Controller->GetMatchReport()->GetTeamStats().GetTotalPlayers();
    } else
    {
        Payload["FortPlayerControllerAthena"]["MatchReport"]["Place"] = 20;
        Payload["FortPlayerControllerAthena"]["MatchReport"]["TotalPlayers"] = StartingPlayers;
    }
    
    // player state object 
    Payload["FortPlayerStateAthena"]["KillScore"] = PlayerState->GetKillScore();
    Payload["FortPlayerStateAthena"]["TeamKillScore"] = PlayerState->GetTeamKillScore();
    Payload["FortPlayerStateAthena"]["DeathInfo"]["DeathCause"] = PlayerState->GetDeathInfo().GetDeathCause();
    Payload["FortPlayerStateAthena"]["DeathInfo"]["DeathLocation"]["X"] = PlayerState->GetDeathInfo().GetDeathLocation().X;
    Payload["FortPlayerStateAthena"]["DeathInfo"]["DeathLocation"]["Y"] = PlayerState->GetDeathInfo().GetDeathLocation().Y;
    Payload["FortPlayerStateAthena"]["DeathInfo"]["DeathLocation"]["Z"] = PlayerState->GetDeathInfo().GetDeathLocation().Z;

    // playlist object 
    Payload["PlaylistData"]["PlaylistName"] = GameMode->GetCurrentPlaylistName().ToString().ToString().c_str();
    if (!PlaylistData.TournamentId.empty())
    {
        Payload["PlaylistData"]["TournamentId"] = PlaylistData.TournamentId.c_str();
        Payload["PlaylistData"]["EventWindowId"] = PlaylistData.EventWindowId.c_str();
    }

    // game mode object 
    Payload["FortGameModeAthena"]["bSafeZonePaused"] = false;
    Payload["FortGameStateAthena"]["GamePhase"] = std::map<EAthenaGamePhase, std::string>{
        {EAthenaGamePhase::None, "EAthenaGamePhase::None"},
        {EAthenaGamePhase::Setup, "EAthenaGamePhase::Setup"},
        {EAthenaGamePhase::Warmup, "EAthenaGamePhase::Warmup"},
        {EAthenaGamePhase::Aircraft, "EAthenaGamePhase::Aircraft"},
        {EAthenaGamePhase::SafeZones, "EAthenaGamePhase::SafeZones"},
        {EAthenaGamePhase::EndGame, "EAthenaGamePhase::EndGame"},
    }.contains(GameState->GetGamePhase()) ? std::map<EAthenaGamePhase, std::string>{
        {EAthenaGamePhase::None, "EAthenaGamePhase::None"},
        {EAthenaGamePhase::Setup, "EAthenaGamePhase::Setup"},
        {EAthenaGamePhase::Warmup, "EAthenaGamePhase::Warmup"},
        {EAthenaGamePhase::Aircraft, "EAthenaGamePhase::Aircraft"},
        {EAthenaGamePhase::SafeZones, "EAthenaGamePhase::SafeZones"},
        {EAthenaGamePhase::EndGame, "EAthenaGamePhase::EndGame"},
    }[GameState->GetGamePhase()] : "Unknown";
    
    // nexa object 
    Payload["Nexa"]["StartingPlayers"] = Nexa::GetState().StartingPlayers ? Nexa::GetState().StartingPlayers : 0;
    Payload["Nexa"]["AccountId"] = AccountID;
    std::vector<FNexaBroadcastQuestProgress> QuestProgress = UFortQuestManager::GetNexaBroadcastQuestProgress(Controller);
    Payload["Nexa"]["BroadcastQuestProgress"] = json::array();
    for (const auto& Progress : QuestProgress) {
        json ProgressJson;
        ProgressJson["BackendName"] = Progress.BackendName;
        ProgressJson["Count"] = Progress.Count;
        Payload["Nexa"]["BroadcastQuestProgress"].push_back(ProgressJson);
    }
    
    std::string endpoint = "http://147.93.1.220/nxa/server/api/v1/" + Config::Echo::Session + "/BroadcastMatchRewards";

    static std::vector<std::thread> activeThreads;
    for (auto& thread : activeThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
    std::thread t([endpoint, Payload]() {
        int r = 3;
        while (r > 0)
        {
            string authHeader = "Authorization: Basic " + NexaAuth::NexaAuthToken;
            vector<const char*> headers = {"Content-Type: application/json", authHeader.c_str()};
            string res = Curl::Post(endpoint, Payload.dump(), headers);
            if (!res.empty())
            {
                break;
            }
            --r;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });
    activeThreads.push_back(std::move(t));
}

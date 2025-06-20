#include "pch.h"
#include "../Header/FortGameMode.h"

#include "Engine/Kismet/Header/Kismet.h"
#include "Engine/NetDriver/Header/NetDriver.h"
#include "Engine/UEngine/Header/UEngine.h"
#include "Neon/Finder/Header/Finder.h"

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

        GameState->CallFunc<void>("FortGameStateAthena", "OnFinishedStreamingAdditionalPlaylistLevel");
        GameState->CallFunc<void>("FortGameStateAthena", "OnRep_AdditionalPlaylistLevelsStreamed");
        GameState->Set("FortGameStateAthena", "bIsUsingDownloadOnDemand", false);
    }

    if (!GameState->GetMapInfo()) return false;

    if (!UWorld::GetWorld()->GetNetDriver())
    {
        FName GameNetDriver = UKismetStringLibrary::GetDefaultObj()->CallFunc<FName>("KismetStringLibrary","Conv_StringToName",FString(L"GameNetDriver"));
        UNetDriver* NetDriver = nullptr;
        static void* (*GetWorldContextFromObject)(UEngine*, UWorld*) = decltype(GetWorldContextFromObject)(Finder->GetWorldContextFromObject());
        void* WorldContext = GetWorldContextFromObject(UEngine::GetEngine(), UWorld::GetWorld());
        static UNetDriver* (*CreateNetDriver_Local)(UEngine*, void* WorldContext, FName) = decltype(CreateNetDriver_Local)(Finder->CreateNetDriverLocal());

        NetDriver = CreateNetDriver_Local(UEngine::GetEngine(), WorldContext, GameNetDriver);

        UWorld::GetWorld()->SetNetDriver(NetDriver);

        if (UWorld::GetWorld()->GetNetDriver())
        {
            UWorld::GetWorld()->GetNetDriver()->SetNetDriverName(GameNetDriver);
            UWorld::GetWorld()->GetNetDriver()->Set<UWorld*>("NetDriver", "World", UWorld::GetWorld());

            FURL URL{};
            URL.Port = 7777;

            UWorld::GetWorld()->GetNetDriver()->InitListen(UWorld::GetWorld(), URL, false);
            UWorld::GetWorld()->GetNetDriver()->SetWorld(UWorld::GetWorld());
            
            for (int i = 0; i < UWorld::GetWorld()->GetLevelCollections().Num(); i++)
            {
                UWorld::GetWorld()->GetLevelCollections()[i].NetDriver = UWorld::GetWorld()->GetNetDriver();
            }

            SetConsoleTitleA("Neon | Listening on Port: 7777");
        }
    }
    
    auto Ret = GameMode->GetbWorldIsReady() && GameMode->GetNumPlayers() > 0;
    return Ret;
}

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

    static bool bSetup = false;

    if (!bSetup)
    {
        bSetup = true;
        UFortPlaylistAthena* Playlist = (UFortPlaylistAthena*)GUObjectArray.FindObject("Playlist_DefaultSolo");

        if (Fortnite_Version >= 6.10)
        {
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
        } else
        {
            GameState->Set("FortGameStateAthena", "CurrentPlaylistData", Playlist);
            
            GameState->SetCurrentPlaylistId(Playlist->GetPlaylistId());
            GameMode->SetCurrentPlaylistId(Playlist->GetPlaylistId());
            GameMode->SetCurrentPlaylistName(Playlist->GetPlaylistName());
        
            GameState->OnRep_CurrentPlaylistId();
            GameState->OnRep_CurrentPlaylistData();
        }
    }

    if (!GameState->GetMapInfo()) return false;

    if (!UWorld::GetWorld()->GetNetDriver())
    {
        FName GameNetDriver = UKismetStringLibrary::GetDefaultObj()->CallFunc<FName>("KismetStringLibrary","Conv_StringToName",FString(L"GameNetDriver"));
        UNetDriver* NetDriver = nullptr;

        if (Fortnite_Version >= 16.40)
        {
            static void* (*GetWorldContextFromObject)(UEngine*, UWorld*) = decltype(GetWorldContextFromObject)(Finder->GetWorldContextFromObject());
            void* WorldContext = GetWorldContextFromObject(UEngine::GetEngine(), UWorld::GetWorld());
            static UNetDriver* (*CreateNetDriver_Local)(UEngine*, void* WorldContext, FName) = decltype(CreateNetDriver_Local)(Finder->CreateNetDriverLocal());
            NetDriver = CreateNetDriver_Local(UEngine::GetEngine(), WorldContext, GameNetDriver);
        } else
        {
			NetDriver = ((UNetDriver * (*)(UObject*, UObject*, FName)) Finder->CreateNetDriver())(UEngine::GetEngine(), UWorld::GetWorld(), GameNetDriver);
        }
        
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
            GameMode->SetbWorldIsReady(true);
        }
    }
    
    auto Ret = GameMode->GetNumPlayers() > 0;
    return Ret;
}

AActor* AFortGameModeAthena::SpawnDefaultPawnFor(AFortGameModeAthena* GameMode, APlayerController* NewPlayer, AActor* StartSpot)
{
    FTransform SpawnTransform = StartSpot ? StartSpot->CallFunc<FTransform>("Actor", "GetTransform") : FTransform::FTransform();
    SpawnTransform.Translation.Z += 250.f;

    AActor* Pawn = GameMode->CallFunc<AActor*>("GameModeBase", "SpawnDefaultPawnAtTransform", NewPlayer, SpawnTransform);

    return Pawn;
}

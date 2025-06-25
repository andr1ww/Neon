#include "pch.h"
#include "../Header/FortGameMode.h"

#include "Engine/Kismet/Header/Kismet.h"
#include "Engine/NetDriver/Header/NetDriver.h"
#include "Engine/UEngine/Header/UEngine.h"
#include "FortniteGame/FortInventory/Header/FortInventory.h"
#include "FortniteGame/FortPlayerController/Header/FortPlayerController.h"
#include "Neon/Finder/Header/Finder.h"
#include "Neon/Runtime/Runtime.h"

bool AFortGameModeAthena::ReadyToStartMatch(AFortGameModeAthena* GameMode)
{
    AFortGameStateAthena* GameState = GameMode->GetGameState();
    if (!GameState)
    {
        ReadyToStartMatchOriginal(GameMode);
        return false;
    }
    
    static bool bSetup = false;

    if (!bSetup)
    {
        bSetup = true;
        UFortPlaylistAthena* Playlist = (UFortPlaylistAthena*)GUObjectArray.FindObject("Playlist_DefaultSolo");

        if (Fortnite_Version >= 6.10)
        {
            static int CurrentPlaylistInfoOffset = Runtime::GetOffset(GameState, "CurrentPlaylistInfo");
            
            FPlaylistPropertyArray& CurrentPlaylistInfoPtr = *reinterpret_cast<FPlaylistPropertyArray*>(__int64(GameState) + CurrentPlaylistInfoOffset);
            CurrentPlaylistInfoPtr.SetBasePlaylist(Playlist);
            CurrentPlaylistInfoPtr.SetOverridePlaylist(Playlist);
            CurrentPlaylistInfoPtr.SetPlaylistReplicationKey(CurrentPlaylistInfoPtr.GetPlaylistReplicationKey() + 1);
            CurrentPlaylistInfoPtr.MarkArrayDirty();

            GameState->SetCurrentPlaylistId(Playlist->GetPlaylistId());
            GameMode->SetCurrentPlaylistId(Playlist->GetPlaylistId());
            GameMode->SetCurrentPlaylistName(Playlist->GetPlaylistName());
        
            GameState->OnRep_CurrentPlaylistId();
            GameState->OnRep_CurrentPlaylistInfo();

            GameMode->SetWarmupRequiredPlayerCount(1);
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

    if (!GameState->GetMapInfo())
    {
        ReadyToStartMatchOriginal(GameMode);
        return false;
    }

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

            GameState->OnRep_CurrentPlaylistId();
            GameState->OnRep_CurrentPlaylistInfo();
        }
    }

    if (GameMode->GetAlivePlayers().Num() > 0)
    {
        auto AlivePlayers = GameMode->GetAlivePlayers();
        for (int i = 0; i < AlivePlayers.Num(); i++)
        {
            AFortPlayerControllerAthena* PlayerController = AlivePlayers[i];
            if (PlayerController->GetbHasServerFinishedLoading())
            {
                return true;
            }
        }
    }
    
    return ReadyToStartMatchOriginal(GameMode);
}

APawn* AFortGameModeAthena::SpawnDefaultPawnFor(AFortGameModeAthena* GameMode, AFortPlayerControllerAthena* NewPlayer, AActor* StartSpot)
{
    static const UClass* PlayerPawnClass = (UClass*)GUObjectArray.FindObject("PlayerPawn_Athena_C");

    if (!PlayerPawnClass)
    {
        UE_LOG(LogNeon, Fatal, "PlayerPawn_Athena_C not found!");
        return nullptr;
    }

    if (GameMode)
    {
        GameMode->Set("GameModeBase", "DefaultPawnClass", PlayerPawnClass);
    }
    else
    {
        UE_LOG(LogNeon, Fatal, "GameMode is null in SpawnDefaultPawnFor!");
        return nullptr;
    }

    auto Pawn = GameMode->CallFunc<APawn*>("GameModeBase", "SpawnDefaultPawnAtTransform", NewPlayer,  StartSpot->CallFunc<FTransform>("Actor", "GetTransform"));;

   for (auto& I : GameMode->GetStartingItems())
    {
    	auto Item = I.GetItem();

    	AFortInventory::GiveItem(NewPlayer, Item , 1, 1, 1);
    } 
    
    if (Fortnite_Version.GetMajorVersion() <= 8.50) {

    }
    else {
        auto Pickaxe = NewPlayer->GetCosmeticLoadoutPC().GetPickaxe();
        auto WeaponDef = Pickaxe->GetWeaponDefinition();
        
     //   AFortInventory::GiveItem(NewPlayer, WeaponDef, 1, 0, 1); 
    } 
    
    return Pawn;
}

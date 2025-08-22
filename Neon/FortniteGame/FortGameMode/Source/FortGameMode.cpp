#include "pch.h"
#include "../Header/FortGameMode.h"

#include "Engine/GameplayStatics/Header/GameplayStatics.h"
#include "Engine/ItemAndCount/Header/ItemAndCount.h"
#include "Engine/Kismet/Header/Kismet.h"
#include "Engine/NetDriver/Header/NetDriver.h"
#include "Engine/UEngine/Header/UEngine.h"
#include "FortniteGame/BuildingGameplayActor/Header/BuildingGameplayActor.h"
#include "FortniteGame/FortInventory/Header/FortInventory.h"
#include "FortniteGame/FortPlayerController/Header/FortPlayerController.h"
#include "FortniteGame/FortAthenaAIBotController/Header/FortAthenaAIBotController.h"
#include "FortniteGame/FortLootPackage/Header/FortLootPackage.h"
#include "FortniteGame/FortQuestManager/Header/FortQuestManager.h"
#include "Neon/Config.h"
#include "Neon/Finder/Header/Finder.h"
#include "Neon/Nexa/Nexa.h"
#include "Neon/Nexa/NexaAuth.h"
#include "Neon/Nexa/NexaHelpers.h"
#include "Neon/Nexa/Curl/Curl.h"
#include "Neon/Nexa/Echo/Echo.h"
#include "Neon/Runtime/Runtime.h"

std::vector<std::string> split(std::string s, std::string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

void SetPlaylist(AFortGameModeAthena* GameMode, UFortPlaylistAthena* Playlist)
{
    AFortGameStateAthena* GameState = UWorld::GetWorld()->GetGameState();
    
    if (!GameState || !GameMode)
    {
        return;
    }

    if (!Config::bCreative)
    {
        GameState->SetDefaultRebootMachineHotfix(1);
        Playlist->SetbAutoAcquireSpawnChip(true);
        auto AllRebootVans = UGameplayStatics::GetAllActorsOfClass(UWorld::GetWorld(), ABuildingGameplayActorSpawnMachine::StaticClass());

        for (int i = 0; i < AllRebootVans.Num(); i++)
        {
            auto CurrentRebootVan = (ABuildingGameplayActorSpawnMachine*)AllRebootVans[i];
            static auto FortPlayerStartClass = APlayerStart::StaticClass();
            TArray<AActor*> AllActors = UGameplayStatics::GetAllActorsOfClass(UWorld::GetWorld(), FortPlayerStartClass);
            AActor* ClosestActor = nullptr;
            float ClosestDistance = 450;
            auto RebootLocation = CurrentRebootVan->GetActorLocation();

            for (int i = 0; i < AllActors.Num(); ++i)
            {
                auto Actor = AllActors[i];
                if (!Actor || Actor == CurrentRebootVan)
                    continue;

                auto ActorLocation = Actor->GetActorLocation();
                float Distance = std::sqrt(std::pow(RebootLocation.X - ActorLocation.X, 2) + std::pow(RebootLocation.Y - ActorLocation.Y, 2) + std::pow(RebootLocation.Z - ActorLocation.Z, 2)) / 100.0f;
            
                if (Distance <= 450 && Distance < ClosestDistance)
                {
                    ClosestDistance = Distance;
                    ClosestActor = Actor;
                }
            }

            CurrentRebootVan->SetResurrectLocation(ClosestActor);
        }

        AllRebootVans.Free();
    }
    
    if (Fortnite_Version >= 6.10)
    {
        static int CurrentPlaylistInfoOffset = Runtime::GetOffset(GameState, "CurrentPlaylistInfo");

        FPlaylistPropertyArray* CurrentPlaylistInfoPtr = reinterpret_cast<FPlaylistPropertyArray*>(__int64(GameState) + CurrentPlaylistInfoOffset);

        if (!CurrentPlaylistInfoPtr) {
            return;
        }
            
        FPlaylistPropertyArray& CurrentPlaylistInfo = *reinterpret_cast<FPlaylistPropertyArray*>(__int64(GameState) + CurrentPlaylistInfoOffset);
            
        CurrentPlaylistInfo.SetBasePlaylist(Playlist);
        CurrentPlaylistInfo.SetOverridePlaylist(Playlist);
        CurrentPlaylistInfo.SetPlaylistReplicationKey(CurrentPlaylistInfo.GetPlaylistReplicationKey() + 1);
        CurrentPlaylistInfo.MarkArrayDirty();

        GameState->SetCurrentPlaylistId(Playlist->GetPlaylistId());
        GameMode->SetCurrentPlaylistId(Playlist->GetPlaylistId());
        GameMode->SetCurrentPlaylistName(Playlist->GetPlaylistName());
        GameState->SetAirCraftBehavior((EAirCraftBehavior)(Playlist->GetAirCraftBehavior()));

        GameState->OnRep_CurrentPlaylistId();
        GameState->OnRep_CurrentPlaylistInfo();
        GameState->OnRep_AdditionalPlaylistLevelsStreamed();
        GameMode->GetGameSession()->SetMaxPlayers(Playlist->GetMaxPlayers());
            
        GameMode->SetWarmupRequiredPlayerCount(1);

        static int AdditionalLevelsOffset = Runtime::GetOffset(Playlist, "AdditionalLevels");
        static int AdditionalLevelsServerOffset = Runtime::GetOffset(Playlist, "AdditionalLevelsServerOnly");

        if (AdditionalLevelsOffset != -1)
        {
            auto AdditionalLevels = *reinterpret_cast<TArray<TSoftObjectPtr<UWorld>>*>(__int64(Playlist) + AdditionalLevelsOffset);
            for (size_t i = 0; i < AdditionalLevels.Num(); i++)
            {
                FVector Loc{};
                FRotator Rot{};
                bool Success = false;
                ULevelStreamingDynamic::LoadLevelInstance(UWorld::GetWorld(), UKismetStringLibrary::Conv_NameToString(AdditionalLevels[i].SoftObjectPtr.ObjectID.AssetPathName), Loc, Rot, &Success, FString());
                FAdditionalLevelStreamed NewLevel{};
                NewLevel.bIsServerOnly = false;
                NewLevel.LevelName = AdditionalLevels[i].SoftObjectPtr.ObjectID.AssetPathName;
                UE_LOG(LogNeon, Log, "Additional Level: %s", NewLevel.LevelName.ToString().ToString().c_str());
                TArray<FAdditionalLevelStreamed>& Levels = GameState->GetAdditionalPlaylistLevelsStreamed();
                Levels.Add(NewLevel, StaticClassImpl("AdditionalLevelStreamed")->GetSize());
           }
        }

        if (AdditionalLevelsServerOffset != -1)
        {
            auto AdditionalServerLevels = *reinterpret_cast<TArray<TSoftObjectPtr<UWorld>>*>(__int64(Playlist) + AdditionalLevelsServerOffset);
            for (size_t i = 0; i < AdditionalServerLevels.Num(); i++)
            {
                FVector Loc{};
                FRotator Rot{};
                bool Success = false;
                ULevelStreamingDynamic::LoadLevelInstance(UWorld::GetWorld(), UKismetStringLibrary::Conv_NameToString(AdditionalServerLevels[i].SoftObjectPtr.ObjectID.AssetPathName), Loc, Rot, &Success, FString());
                FAdditionalLevelStreamed NewLevel{};
                NewLevel.bIsServerOnly = true;
                NewLevel.LevelName = AdditionalServerLevels[i].SoftObjectPtr.ObjectID.AssetPathName;
                UE_LOG(LogNeon, Log, "Additional Server Level: %s", NewLevel.LevelName.ToString().ToString().c_str());
                TArray<FAdditionalLevelStreamed>& Levels = GameState->GetAdditionalPlaylistLevelsStreamed();
                Levels.Add(NewLevel, StaticClassImpl("AdditionalLevelStreamed")->GetSize());
            }

            GameState->OnRep_AdditionalPlaylistLevelsStreamed();
            GameState->OnFinishedStreamingAdditionalPlaylistLevel();
        }

   /*     if (Fortnite_Version == 12.41)
        {
            ABuildingFoundation* JerkyFoundation = Runtime::StaticFindObject<ABuildingFoundation>("/Game/Athena/Apollo/Maps/Apollo_POI_Foundations.Apollo_POI_Foundations.PersistentLevel.LF_Athena_POI_19x19_2");
            JerkyFoundation->SetDynamicFoundationType(EDynamicFoundationType::Static);
            JerkyFoundation->SetbServerStreamedInLevel(true);
            JerkyFoundation->OnRep_ServerStreamedInLevel();
            JerkyFoundation->GetDynamicFoundationRepData().SetEnabledState(EDynamicFoundationEnabledState::Enabled);
            JerkyFoundation->OnRep_DynamicFoundationRepData();
            JerkyFoundation->SetDynamicFoundationEnabled(true);
        }
*/
        static UClass* PlayerPawnClass = (UClass*)GUObjectArray.FindObject("PlayerPawn_Athena_C");

        if (GameMode)
        {
            GameMode->SetDefaultPawnClass(PlayerPawnClass);
        }
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

bool AFortGameModeAthena::ReadyToStartMatch(AFortGameModeAthena* GameMode, FFrame& Stack, bool* Result)
{
    Stack.IncrementCode();
    AFortGameStateAthena* GameState = UWorld::GetWorld()->GetGameState();
    
    if (!GameState || !GameMode)
    {
        return *Result = false;
    }

    GameState->SetGamePhase(EAthenaGamePhase::Warmup);
    GameState->OnRep_GamePhase(EAthenaGamePhase::Setup);
    
    if (!GameState->GetMapInfo())
    {
        return *Result = false;
    }

    GameMode->SetbWorldIsReady(true);
    GameMode->SetbDBNOEnabled(true);
    GameState->SetbDBNODeathEnabled(false);
    GameMode->SetbAlwaysDBNO(false);
    
    static bool bInit = false;

    if (Fortnite_Version < 13.00 && Fortnite_Version >= 12.50)
    {
        static bool bInitializedTeams = false;
        if (!bInitializedTeams)
        {
            GameState->GetTeamArray().Reset();
            GameState->GetSquadArray().Reset();

            for (int i = 0; i < 103; ++i) 
            {
                TArray<TWeakObjectPtr<AFortPlayerStateAthena>> TeamArray(4);
                GameState->GetTeamArray().Add(TeamArray);
                GameState->GetSquadArray().Add(TeamArray);
            }

            bInitializedTeams = true;
        }
    }
    
    if (GameMode->GetCurrentPlaylistId() == -1)
    {
        UFortPlaylistAthena* Playlist = Config::bCreative ? (UFortPlaylistAthena*)GUObjectArray.FindObject("Playlist_PlaygroundV2") : 
            (UFortPlaylistAthena*)GUObjectArray.FindObject(Config::Playlist);
        
        if (!Config::bEchoSessions)
        {
            if (Playlist->GetAISettings()) {
                GameMode->SetAISettings(Playlist->GetAISettings());
            }
            SetPlaylist(GameMode, Playlist);
        }
        else
        {
            string PlaylistName = Nexa::Echo::FetchEchoSessionPlaylist();
            if (!PlaylistName.empty())
            {
                std::string PlaylistDirectory = PlaylistName;
                if (PlaylistDirectory == "showdownalt") PlaylistDirectory = "showdown";

                std::string PlaylistBaseName = PlaylistName;
                size_t underscorePos = PlaylistBaseName.find('_');
                if (underscorePos != std::string::npos) {
                    PlaylistBaseName = PlaylistBaseName.substr(0, underscorePos);
                }

                std::string DirectoryPath = PlaylistBaseName;
                if (DirectoryPath == "showdownalt") DirectoryPath = "showdown";

                std::string path = "/Game/Athena/Playlists/" + 
                    (PlaylistName.substr(0, 7) == "default" ? "" : (DirectoryPath + "/")) + 
                    "playlist_" + PlaylistName + ".playlist_" + PlaylistName;

                std::cout << path << std::endl;
                Playlist = Runtime::StaticFindObject<UFortPlaylistAthena>(path);

                FPlaylistData PlaylistData{};
                
                if (NexaAuth::NexaAuthToken.empty() || NexaAuth::NexaAuthToken == "")
                {
                    NexaAuth::SetAuthToken();
                }
                
                if (Playlist)
                {
                    PlaylistData.PlaylistName = Playlist->GetPlaylistName();

                    if (PlaylistName.find("vamp") != std::string::npos) {
                        PlaylistData.EventWindowId = "ARENA_S" + Fortnite_Version.GetMajorVersion();
                        PlaylistData.TournamentId = "eventTemplate_ARENA_S" + Fortnite_Version.GetMajorVersion();
                        UE_LOG(LogNeon, Log, "Enabling LateGame");
                        Config::bLateGame = true;
                    }
                    
                    SetPlaylist(GameMode, Playlist);
                    Nexa::Echo::EchoSessionUpdate(Playlist);
                    if (PlaylistName.find("showdownalt") != std::string::npos)
                    {
                        PlaylistData.EventWindowId = "LG_ARENA_S" + Fortnite_Version.GetMajorVersion();
                        PlaylistData.TournamentId = "eventTemplate_LG_ARENA_S" + Fortnite_Version.GetMajorVersion();
                    }
			
                    Nexa::SetPlaylistData(PlaylistData);
                } else
                {
                    std::string path = "/Game/Athena/Playlists/" + 
               (PlaylistName.substr(0, 7) == "default" ? "" : (DirectoryPath + "/")) + 
               "playlist_" + PlaylistName + (PlaylistName.contains("duo") || PlaylistName.contains("trio") || PlaylistName.contains("squad") ? "s" : "") + 
               ".playlist_" + PlaylistName + (PlaylistName.contains("duo") || PlaylistName.contains("trio") || PlaylistName.contains("squad") ? "s" : "");
                    std::cout << path << std::endl;
                    Playlist = Runtime::StaticFindObject<UFortPlaylistAthena>(path);
                    if (Playlist)
                    {
                        SetPlaylist(GameMode, Playlist);
                        Nexa::Echo::EchoSessionUpdate(Playlist);

                        FPlaylistData PlaylistData{};
                        PlaylistData.PlaylistName = Playlist->GetPlaylistName();

                        if (PlaylistName.find("vamp") != std::string::npos) {
                            PlaylistData.EventWindowId = "ARENA_S" + std::to_string(Fortnite_Version.GetMajorVersion());
                            PlaylistData.TournamentId = "eventTemplate_ARENA_S" + std::to_string(Fortnite_Version.GetMajorVersion());
                            UE_LOG(LogNeon, Log, "Enabling LateGame");
                            Config::bLateGame = true;
                        }
                        
                        if (PlaylistName.find("showdownalt") != std::string::npos)
                        {
                            PlaylistData.EventWindowId = "ARENA_S" + std::to_string(Fortnite_Version.GetMajorVersion());
                            PlaylistData.TournamentId = "eventTemplate_ARENA_S" + std::to_string(Fortnite_Version.GetMajorVersion());
                        }
			
                        Nexa::SetPlaylistData(PlaylistData);
                    } else
                    {
                        return *Result = false;
                    }
                }
            } else
            {
                return *Result = false;
            }
        }
    }
    
    static auto FortPlayerStartCreativeClass = Runtime::StaticFindObject<UClass>("/Script/FortniteGame.FortPlayerStartCreative");
    static auto FortPlayerStartWarmupClass = Runtime::StaticFindObject<UClass>("/Script/FortniteGame.FortPlayerStartWarmup");
    TArray<AActor*> Actors = UGameplayStatics::GetAllActorsOfClass(UWorld::GetWorld(), Config::bCreative ? FortPlayerStartCreativeClass : FortPlayerStartWarmupClass);
    int WarmupSpots = Actors.Num();
    Actors.Free();

    if (WarmupSpots == 0) return *Result = false;

    if (Fortnite_Version <= 13.40 && Fortnite_Version >= 12.00 && !bInit)
    {
        bInit = true;
        
        if (!Config::bLateGame && !Config::bCreative)
        {
            if (auto Manager = (UFortServerBotManagerAthena*)UGameplayStatics::SpawnObject(UFortServerBotManagerAthena::StaticClass(), GameMode))
            {
                *(bool*)(__int64(Manager) + 0x4d0) = 1;
                Manager->SetCachedGameMode(GameMode);
                Manager->SetCachedGameState(GameState);
                GameMode->SetServerBotManager(Manager);
            }
            
            GameMode->SetServerBotManagerClass(UFortServerBotManagerAthena::StaticClass());
            
            AFortAIDirector* AIDirector = UGameplayStatics::SpawnActorOG<AFortAIDirector>(AFortAIDirector::StaticClass(), {});
            GameMode->SetAIDirector(AIDirector);
            GameMode->GetAIDirector()->CallFunc<void>("FortAIDirector", "Activate");

            AFortAIGoalManager* AIGoalManager = UGameplayStatics::SpawnActorOG<AFortAIGoalManager>(AFortAIGoalManager::StaticClass(), {});
            GameMode->SetAIGoalManager(AIGoalManager);
        }

        UE_LOG(LogNeon, Log, "Waiting for players...");
    }
    
    if (Config::bGameSessions)
    {
        static bool bSetPlaylist = false;
        if (!bSetPlaylist)
        {
            UFortPlaylistAthena* Playlist = nullptr;
            if (!GameMode->GetCurrentBucketId().IsValid()) return *Result = false;
            UE_LOG(LogNeon, Log, "Current Bucket ID: %s", GameMode->GetCurrentBucketId().ToString().c_str());
            auto splitBucket = split(GameMode->GetCurrentBucketId().ToString(), ":");
            if (splitBucket.size() > 5) {
                auto PlaylistName = splitBucket[5];
                auto PathPart = split(PlaylistName, "_")[1];
                if (PathPart == "showdownalt") PathPart = "showdown";
                Playlist = Runtime::StaticFindObject<UFortPlaylistAthena>("/Game/Athena/Playlists/" + (PathPart.starts_with("default") ? "" : (PathPart + "/")) + PlaylistName + "." + PlaylistName);
                UE_LOG(LogNeon, Log, PlaylistName.c_str());
                if (PlaylistName.contains("showdownalt")) {
                    Config::bLateGame = true;
                }
                else if (PlaylistName.contains("showdown")) {
// tournament stuff
                }
                if (!Playlist) Playlist = Runtime::StaticFindObject<UFortPlaylistAthena>("/Game/Athena/Playlists/Barrier/Playlist_Barrier_16_B_Lava.Playlist_Barrier_16_B_Lava");
            }

            SetPlaylist(GameMode, Playlist);
        }
    }
    
    UNetDriver* NetDriver = UWorld::GetWorld()->GetNetDriver();
    if (!NetDriver && !Finder->InstructionForCollision())
    {
        FName GameNetDriver = UKismetStringLibrary::Conv_StringToName(L"GameNetDriver");
    
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
    
        NetDriver->SetNetDriverName(GameNetDriver);
        NetDriver->NetDriverSetWorld(UWorld::GetWorld());

        FURL URL{};
        URL.Port = 7777;

        NetDriver->InitListen(UWorld::GetWorld(), URL, false);
        NetDriver->NetDriverSetWorld(UWorld::GetWorld());
            
        for (int i = 0; i < UWorld::GetWorld()->GetLevelCollections().Num(); i++)
        {
            UWorld::GetWorld()->GetLevelCollections()[i].NetDriver = NetDriver;
        }

        SetConsoleTitleA("Neon | Listening on Port: 7777");
    }

    if (Config::bLateGame)
    {
        GameState->SetDefaultParachuteDeployTraceForGroundDistance(1000.f);
    }

    if (Config::bEchoSessions)
    {
        static bool bStarted = false;
        if (!bStarted)
        {
            bStarted = true;
            std::thread t([]() {
                Nexa::Echo::EchoSessionStarted();
            });
            t.detach();
        }
    }

    static bool bSetupLoot = false;
    if (!bSetupLoot && !Config::bCreative)
    {
        FortLootPackage::SpawnFloorLootForContainer(Runtime::StaticLoadObject<UBlueprintGeneratedClass>("/Game/Athena/Environments/Blueprints/Tiered_Athena_FloorLoot_Warmup.Tiered_Athena_FloorLoot_Warmup_C"));
        FortLootPackage::SpawnFloorLootForContainer(Runtime::StaticLoadObject<UBlueprintGeneratedClass>("/Game/Athena/Environments/Blueprints/Tiered_Athena_FloorLoot_01.Tiered_Athena_FloorLoot_01_C"));

        TArray<AActor*> VehicleSpawners = UGameplayStatics::GetAllActorsOfClass(UWorld::GetWorld(), AFortAthenaVehicleSpawner::StaticClass());
        for (auto& VehicleSpawner : VehicleSpawners)
        {
            auto Spawner = (AFortAthenaVehicleSpawner*)VehicleSpawner;
            FTransform Transform = Spawner->GetTransform();
            UGameplayStatics::SpawnActorOG<AFortAthenaVehicle>(Spawner->CallFunc<UClass*>("FortAthenaVehicleSpawner", "GetVehicleClass"), Transform.Translation, Spawner->K2_GetActorRotation());
        }
        
        bSetupLoot = true;
    }

    static bool Double = false;
    if (!Double)
    {
        GameState->OnRep_CurrentPlaylistInfo();
        GameState->OnRep_CurrentPlaylistId();
        
        if (Config::bCreative)
        {
            GameMode->CallFunc<void>("GameMode", "StartMatch");
            GameMode->CallFunc<void>("GameModeBase", "StartPlay");
        }
        
        Double = true;
    }
    
    bool Res = GameState->GetPlayersLeft() > 0;
    
    if (Res && !Config::bCreative)
    {
        auto Time = UGameplayStatics::GetTimeSeconds(UWorld::GetWorld());
        auto WarmupDuration = 60.f;

        GameState->Set("FortGameStateAthena", "WarmupCountdownStartTime", Time);
        GameState->Set("FortGameStateAthena", "WarmupCountdownEndTime", Time + WarmupDuration + 10.f);
        GameMode->Set("FortGameModeAthena", "WarmupCountdownDuration", WarmupDuration);
        GameMode->Set("FortGameModeAthena", "WarmupEarlyCountdownDuration", WarmupDuration);
    }
    
    return *Result = Res;
}

APawn* AFortGameModeAthena::SpawnDefaultPawnFor(AFortGameModeAthena* GameMode, AFortPlayerControllerAthena* NewPlayer, AActor* StartSpot)
{
    if (NewPlayer->GetPawn()) {
        UE_LOG(LogNeon, Warning, "Early return!");
		return 0;
    }

    auto Pawn = GameMode->CallFunc<APawn*>("GameModeBase", "SpawnDefaultPawnAtTransform", NewPlayer,  StartSpot->GetTransform());
    //APawn* Pawn = GameMode->SpawnDefaultPawnAtTransform(NewPlayer,  StartSpot->GetTransform()); yk why this doesent work???
    if (!Pawn)
    {
        UE_LOG(LogNeon, Warning, "SpawnDefaultPawnFor: Cannot Spawn Pawn!");
        return 0;
    }

//    Pawn->Set("FortPlayerPawnAthena", "ReviveFromDBNOTime", 2.f);
    
    auto& StartingItemsArray = GameMode->GetStartingItems();
    static int32 FItemAndCountSize = StaticClassImpl("ItemAndCount")->GetSize();
    for (int i = 0; i < StartingItemsArray.Num(); i++)
    {
        auto Item = (FItemAndCount*) ((uint8*) StartingItemsArray.GetData() + (i * FItemAndCountSize));
        
        if (!Item) {
            UE_LOG(LogNeon, Fatal, "StartingItem is null in SpawnDefaultPawnFor!");
            return Pawn;
        }
    
        AFortInventory::GiveItem(NewPlayer, Item->Item, Item->Count, 1, 1);
    }
    
    if (Fortnite_Version.GetMajorVersion() <= 8.50) {
    
    }
    else {
        auto Pickaxe = NewPlayer->GetCosmeticLoadoutPC().GetPickaxe();
        auto WeaponDef = Pickaxe->GetWeaponDefinition();
        AFortInventory::GiveItem(NewPlayer, WeaponDef, 1, 0, 1); 
    }
    
    return Pawn;
}

void AFortGameModeAthena::HandleStartingNewPlayer(AFortGameModeAthena* GameMode, AFortPlayerControllerAthena* NewPlayer)
{
    if (!NewPlayer->GetMatchReport())
    {
        NewPlayer->SetMatchReport((UAthenaPlayerMatchReport*)UGameplayStatics::SpawnObject(UAthenaPlayerMatchReport::StaticClass(), NewPlayer));
    }

    if (NewPlayer->IsA<AFortAthenaAIBotController>())
    {
        return HandleStartingNewPlayerOG(GameMode, NewPlayer);
    }
    
    AFortGameStateAthena* GameState = UWorld::GetWorld()->GetGameState();
    AFortPlayerStateAthena* PlayerState = NewPlayer->GetPlayerState();
    
    PlayerState->SetSquadId(PlayerState->GetTeamIndex() - 3);
    PlayerState->OnRep_SquadId();

    FGameMemberInfo Member{};
	
    Member.MostRecentArrayReplicationKey = -1;
    Member.ReplicationID = -1;
    Member.ReplicationKey = -1;
    Member.TeamIndex = PlayerState->GetTeamIndex();
    Member.SquadId = PlayerState->GetSquadId();
    Member.MemberUniqueId = PlayerState->GetUniqueId();

    GameState->GetGameMemberInfoArray().GetMembers().Add(Member);
    GameState->GetGameMemberInfoArray().MarkItemDirty(Member);

    if (UWorld::GetWorld()->GetAuthorityGameMode()->GetCurrentPlaylistName().ToString().ToString().contains("Barrier"))
    {
        static bool bSpawnedWall = false;
        if (!bSpawnedWall)
        {
            bSpawnedWall = true;
            UGameplayStatics::SpawnActorOG<AActor>(Runtime::StaticFindObject<UClass>("/Game/Athena/Playlists/Barrier/Barrier.Barrier_C"), FVector{ 0, 0, -2000 }, FRotator{});
        }
    }
    
    if (Fortnite_Version < 13.00 && Fortnite_Version >= 12.50)
    {
        TWeakObjectPtr<AFortPlayerStateAthena> WeakObjectPtr;
        WeakObjectPtr.ObjectIndex = PlayerState->GetUniqueID();
        WeakObjectPtr.ObjectSerialNumber = GUObjectArray.GetSerialNumber(PlayerState->GetUniqueID());
        auto& ElementData2 = UWorld::GetWorld()->GetGameState()->GetTeamArray()[PlayerState->GetTeamIndex()];
        auto& ElementData1 = GameState->GetSquadArray()[PlayerState->GetSquadId()];
        if (!ElementData2.ElementData.GetData() || !ElementData1.ElementData.GetData() || !WeakObjectPtr.Get())
        {
            return HandleStartingNewPlayerOG(GameMode, NewPlayer);
        }
    
        ElementData2.ElementData.Add(WeakObjectPtr);
        ElementData1.ElementData.Add(WeakObjectPtr);
    }
    
    return HandleStartingNewPlayerOG(GameMode, NewPlayer);
}

EFortTeam AFortGameModeAthena::PickTeam(AFortGameModeAthena* GameMode, uint8_t PreferredTeam, AFortPlayerControllerAthena* Controller)
{
    if (Controller->IsA<AFortAthenaAIBotController>())
    {
        return PickTeamOG(GameMode, PreferredTeam, Controller);
    }
    
    static uint8_t CurrentTeam = 3;
    uint8_t ret = CurrentTeam;
    std::string PlayerName = Controller->GetPlayerState()->GetPlayerName().ToString();

    static std::string TeamsJson = "";
    static std::chrono::steady_clock::time_point LastFetchTime = std::chrono::steady_clock::now() - std::chrono::seconds(10);

    auto Now = std::chrono::steady_clock::now();
    if (TeamsJson.empty() || std::chrono::duration_cast<std::chrono::seconds>(Now - LastFetchTime).count() >= 5)
    {
        TeamsJson = Nexa::Curl::Get("http://147.93.1.220:2087/nxa/echo/session/list/teams/" + Config::Echo::Session);
        LastFetchTime = Now;
    }

    static int CurrentPlaylistInfoOffset = Runtime::GetOffset(GameMode->GetGameState(), "CurrentPlaylistInfo");
    FPlaylistPropertyArray& CurrentPlaylistInfo = *reinterpret_cast<FPlaylistPropertyArray*>((__int64)GameMode->GetGameState() + CurrentPlaylistInfoOffset);

    if (!TeamsJson.empty())
    {
        try
        {
            auto teamsArray = nlohmann::json::parse(TeamsJson);

            static std::map<size_t, uint8_t> teamIndexToGameTeam;
            static std::map<uint8_t, int> playersOnTeam;

            for (size_t teamIndex = 0; teamIndex < teamsArray.size(); ++teamIndex)
            {
                const auto& team = teamsArray[teamIndex];

                for (const auto& member : team)
                {
                    if (member.get<std::string>() == PlayerName)
                    {
                        if (teamIndexToGameTeam.find(teamIndex) != teamIndexToGameTeam.end())
                        {
                            ret = teamIndexToGameTeam[teamIndex];
                        }
                        else
                        {
                            ret = CurrentTeam;
                            teamIndexToGameTeam[teamIndex] = ret;
                            CurrentTeam++;
                        }

                        playersOnTeam[ret]++;

                        if (playersOnTeam[ret] >= CurrentPlaylistInfo.GetBasePlaylist()->GetMaxSquadSize())
                        {
                            CurrentTeam++;
                        }
                        
                        return EFortTeam(ret);
                    }
                }
            }
        }
        catch (const std::exception& e)
        {
        }
    }

    ret = CurrentTeam++;
    return EFortTeam(ret);
}

void AFortGameModeAthena::StartAircraftPhase(AFortGameModeAthena* GameMode, char a2)
{
    if (!GameMode) return(StartAircraftPhaseOG(GameMode, a2));
    StartAircraftPhaseOG(GameMode, a2);

    if (Config::bEchoSessions)
    {
        std::thread t([]() {
            Nexa::Echo::CloseEchoSession();
        });
        t.detach();
    }

    for (auto& Player : GameMode->GetAlivePlayers())
    {
        if (!Player) continue;

        AFortInventory* WorldInventory = Player->GetWorldInventory();
        FFortItemList& Inventory = WorldInventory->GetInventory();

        if (WorldInventory)
        {
            for (int i = Inventory.GetItemInstances().Num() - 1; i >= 0; --i) {
                Inventory.GetItemInstances().Remove(i);
            }
            for (int i = Inventory.GetReplicatedEntries().Num() - 1; i >= 0; --i) {
                Inventory.GetReplicatedEntries().Remove(i);
            }
        }
    }
    
    if (Config::bLateGame)
    {
        auto GameState = UWorld::GetWorld()->GetGameState();
        if (!GameState || GameState->GetAircrafts().Num() == 0)
            return;

		auto LocalAircraft = GameState->GetAircrafts()[0];
		FRotator AircraftRotation = LocalAircraft->K2_GetActorRotation();

		float Pitch = AircraftRotation.Pitch * (3.14159265359f / 180.0f);
		float Yaw = AircraftRotation.Yaw * (3.14159265359f / 180.0f);

		FVector AircraftForward{};
		AircraftForward.X = cos(Yaw) * cos(Pitch);
		AircraftForward.Y = sin(Yaw) * cos(Pitch);
		AircraftForward.Z = sin(Pitch);

		float Length = sqrt(AircraftForward.X * AircraftForward.X +
			AircraftForward.Y * AircraftForward.Y +
			AircraftForward.Z * AircraftForward.Z);

		if (Length > 0.0f) {
			AircraftForward.X /= Length;
			AircraftForward.Y /= Length;
			AircraftForward.Z /= Length;
		}

        int Index = GameMode->GetAlivePlayers().Num() >= 25 ? 4 : 5;
        
        FVector SafeZoneCenter = GameMode->GetSafeZoneLocations()[Index];
        SafeZoneCenter.Z += 15000.f;

        const FVector NewLocation = SafeZoneCenter - AircraftForward * 24000.f;
        LocalAircraft->CallFunc<void>("Actor", "K2_SetActorLocation", NewLocation, false, nullptr, true);

        auto MapInfo = GameState->GetMapInfo();
        if (!MapInfo)
            return;

        for (auto& Player : GameMode->GetAlivePlayers())
        {
            if (!Player) continue;

            AFortInventory* WorldInventory = Player->GetWorldInventory();
            FFortItemList& Inventory = WorldInventory->GetInventory();
        
            WorldInventory->SetbRequiresLocalUpdate(true);
            WorldInventory->HandleInventoryLocalUpdate();

            AFortInventory::GiveItem(Player, UFortKismetLibrary::K2_GetResourceItemDefinition(EFortResourceType::Wood), 500, 500, 1);
            AFortInventory::GiveItem(Player, UFortKismetLibrary::K2_GetResourceItemDefinition(EFortResourceType::Stone), 500, 500, 1);
            AFortInventory::GiveItem(Player, UFortKismetLibrary::K2_GetResourceItemDefinition(EFortResourceType::Metal), 500, 500, 1);

            const std::map<EAmmoType, int> Ammo = {{EAmmoType::Assault, 250}, {EAmmoType::Shotgun, 50}, {EAmmoType::Submachine, 400}, {EAmmoType::Rocket, 6}, {EAmmoType::Sniper, 20}};
            for (const auto& [Type, Count] : Ammo) {
                AFortInventory::GiveItem(Player, ItemAndCount::GetAmmo(Type), Count, Count, 1);
            }

            auto GetItem = [](auto GetItemFunc) {
                FItemAndCount Item;
                do { Item = GetItemFunc(); } while (!Item.Item);
                return Item;
            };

            auto Shotgun = GetItem(ItemAndCount::GetShotguns);
            auto AssaultRifle = GetItem(ItemAndCount::GetAssaultRifles);
            auto Sniper = GetItem(ItemAndCount::GetSnipers);
            auto Heal = GetItem(ItemAndCount::GetHeals);
            
            FItemAndCount HealSlot2;
            do {
                HealSlot2 = ItemAndCount::GetHeals();
            } while (!HealSlot2.Item || HealSlot2.Item == Heal.Item);

            auto GiveItem = [&](const FItemAndCount& Item) {
                int ClipSize = Item.Item->IsA<UFortWeaponItemDefinition>() ? 
                    AFortInventory::GetStats((UFortWeaponItemDefinition*)Item.Item)->GetClipSize() : 0;
                AFortInventory::GiveItem(Player, Item.Item, Item.Count, ClipSize, 1);
            };

            GiveItem(AssaultRifle);
            GiveItem(Shotgun);
            GiveItem(Sniper);
            GiveItem(Heal);
            GiveItem(HealSlot2);
        }

        for (auto& FlightInfo : MapInfo->GetFlightInfos())
        {
            FlightInfo.FlightStartLocation = FVector_NetQuantize100(NewLocation);
            FlightInfo.FlightSpeed = 2000;
            FlightInfo.TimeTillFlightEnd = 8.25f;
            FlightInfo.TimeTillDropEnd = 8.25f;
            FlightInfo.TimeTillDropStart = 0.0f;

            const float CurrentTime = UGameplayStatics::GetTimeSeconds(UWorld::GetWorld());
            LocalAircraft->SetFlightStartTime(CurrentTime);
            LocalAircraft->SetFlightEndTime(CurrentTime + 8.25f);
            LocalAircraft->SetFlightInfo(FlightInfo);

            GameState->SetbGameModeWillSkipAircraft(true);
            MapInfo->GetAircraftDesiredDoorOpenTime().Value = 1;

            GameState->SetbAircraftIsLocked(true);
            GameState->SetSafeZonesStartTime(0.0001f);

            GameState->CallFunc<void>("FortGameStateAthena", "OnRep_MapInfo");
        }
    }
}

void AFortGameModeAthena::StartNewSafeZonePhase(AFortGameModeAthena* GameMode, int Phase)
{
    AFortGameStateAthena* GameState = GameMode->GetGameState();

    int Index = GameMode->GetAlivePlayers().Num() >= 25 ? 3 : 4;

    static bool bSetupLG = false;

    if (Phase > 1)
    {
        for (auto& Controller : GameMode->GetAlivePlayers())
        {
            FGameplayTagContainer SourceTags;
            FGameplayTagContainer TargetTags; 
            FGameplayTagContainer ContextTags;
            UFortQuestManager* QuestManager = Controller->GetQuestManager(ESubGame::Athena);

            if (!QuestManager) {
                UE_LOG(LogNeon, Warning, "QuestManager is null for controller");
                continue;
            }

            QuestManager->GetSourceAndContextTags(&SourceTags, &ContextTags);
        
            SourceTags.GameplayTags.Add(FGameplayTag(UKismetStringLibrary::Conv_StringToName(L"Athena.Quests.SurviveStormCircles")));
            UFortQuestManager::SendStatEvent(QuestManager, nullptr, SourceTags, TargetTags, nullptr, nullptr, 1, EFortQuestObjectiveStatEvent::ComplexCustom, ContextTags);
            static UFortAccoladeItemDefinition* NewStormCircle = Runtime::StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeID_SurviveStormCircle.AccoladeID_SurviveStormCircle");
            UFortQuestManager::GiveAccolade(Controller, NewStormCircle);
            if (Config::bLateGame && !bSetupLG)
            {
                for (int i = 0; i < 3; i++) {
                    UFortQuestManager::GiveAccolade(Controller, NewStormCircle);
                }
            }
        }
    }

    if (Config::bLateGame && !bSetupLG)
    {
        bSetupLG = true;
        GameState->SetSafeZonePhase(GameState->GetSafeZonePhase() <= Index ? Index : GameState->GetSafeZonePhase());
        GameMode->SetSafeZonePhase(GameState->GetSafeZonePhase());
        GameState->SetSafeZonesStartTime(0.0001f);
        if (Fortnite_Version <= 13.40)
        {
            return StartNewSafeZonePhaseOG(GameMode, GameState->GetSafeZonePhase());
        }
    }
    
    if (Fortnite_Version <= 13.40)
    {
        StartNewSafeZonePhaseOG(GameMode, Phase);
    }
}

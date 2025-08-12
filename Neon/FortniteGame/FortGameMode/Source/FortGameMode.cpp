#include "pch.h"
#include "../Header/FortGameMode.h"

#include "Engine/GameplayStatics/Header/GameplayStatics.h"
#include "Engine/ItemAndCount/Header/ItemAndCount.h"
#include "Engine/Kismet/Header/Kismet.h"
#include "Engine/NetDriver/Header/NetDriver.h"
#include "Engine/UEngine/Header/UEngine.h"
#include "FortniteGame/FortInventory/Header/FortInventory.h"
#include "FortniteGame/FortPlayerController/Header/FortPlayerController.h"
#include "FortniteGame/FortAthenaAIBotController/Header/FortAthenaAIBotController.h"
#include "FortniteGame/FortLootPackage/Header/FortLootPackage.h"
#include "Neon/Config.h"
#include "Neon/Finder/Header/Finder.h"
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

        if (Fortnite_Version == 12.41)
        {
            ABuildingFoundation* JerkyFoundation = Runtime::StaticFindObject<ABuildingFoundation>("/Game/Athena/Apollo/Maps/Apollo_POI_Foundations.Apollo_POI_Foundations.PersistentLevel.LF_Athena_POI_19x19_2");
            JerkyFoundation->SetDynamicFoundationType(EDynamicFoundationType::Static);
            JerkyFoundation->SetbServerStreamedInLevel(true);
            JerkyFoundation->OnRep_ServerStreamedInLevel();
            JerkyFoundation->GetDynamicFoundationRepData().SetEnabledState(EDynamicFoundationEnabledState::Enabled);
            JerkyFoundation->OnRep_DynamicFoundationRepData();
            JerkyFoundation->SetDynamicFoundationEnabled(true);
        }

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

    if (!GameState->GetMapInfo())
    {
        return *Result = false;
    }

    GameMode->SetbWorldIsReady(true);

    static bool bInit = false;
    if (!bInit)
    {
        bInit = true;
        UFortPlaylistAthena* Playlist = (UFortPlaylistAthena*)GUObjectArray.FindObject("Playlist_DefaultSolo");
        SetPlaylist(GameMode, Playlist);
        
        if (Fortnite_Version <= 13.40 && Fortnite_Version >= 12.00)
        {
            if (Playlist->GetAISettings()) {
                GameMode->SetAISettings(Playlist->GetAISettings());
            }
                
            if (!Config::bLateGame)
            {
                GameMode->SetServerBotManager((UFortServerBotManagerAthena*)UGameplayStatics::SpawnObject(UFortServerBotManagerAthena::StaticClass(), GameMode));
                GameMode->GetServerBotManager()->SetCachedGameMode(GameMode);
                GameMode->GetServerBotManager()->SetCachedGameState(GameState);
                
                auto BotMutator = UGameplayStatics::SpawnActor<AFortAthenaMutator_Bots>({});
                GameMode->GetServerBotManager()->SetCachedBotMutator(BotMutator);
                BotMutator->Set("FortAthenaMutator", "CachedGameMode", GameMode);
                BotMutator->Set("FortAthenaMutator", "CachedGameState", GameState);
                FBotMutator::Set(BotMutator);
                GameMode->SetServerBotManagerClass(UFortServerBotManagerAthena::StaticClass());

                AFortAIDirector* AIDirector = UGameplayStatics::SpawnActor<AFortAIDirector>({});
                GameMode->SetAIDirector(AIDirector);
                if (GameMode->GetAIDirector())
                {
                    GameMode->GetAIDirector()->CallFunc<void>("FortAIDirector", "Activate");
                }

                AFortAIGoalManager* AIGoalManager = UGameplayStatics::SpawnActor<AFortAIGoalManager>({});
                GameMode->SetAIGoalManager(AIGoalManager);
            }
        }
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
    
    bool Res = GameMode->GetAlivePlayers().Num() >= GameMode->GetWarmupRequiredPlayerCount();

    if (Res)
    {
        TArray<AActor*> WarmupActors;
        UClass* WarmupClass = Runtime::StaticLoadObject<UClass>("/Game/Athena/Environments/Blueprints/Tiered_Athena_FloorLoot_Warmup.Tiered_Athena_FloorLoot_Warmup_C");
        WarmupActors = UGameplayStatics::GetAllActorsOfClass(UWorld::GetWorld(), WarmupClass);
            
        for (auto& WarmupActor : WarmupActors)
        {
            auto Container = (ABuildingContainer*)WarmupActor;

            Container->BP_SpawnLoot(nullptr);

            Container->K2_DestroyActor();
        }
        WarmupActors.Free();

        WarmupClass = Runtime::StaticLoadObject<UClass>("/Game/Athena/Environments/Blueprints/Tiered_Athena_FloorLoot_01.Tiered_Athena_FloorLoot_01_C");
        WarmupActors = UGameplayStatics::GetAllActorsOfClass(UWorld::GetWorld(), WarmupClass);

        for (auto& WarmupActor : WarmupActors)
        {
            auto Container = (ABuildingContainer*)WarmupActor;

            Container->BP_SpawnLoot(nullptr);

            Container->K2_DestroyActor();
        }
        WarmupActors.Free();
    }
    
    if (Fortnite_Version <= 13.40 && Fortnite_Version >= 12.00 && Res)
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

    if (!NewPlayer->GetMatchReport())
    {
        NewPlayer->SetMatchReport((UAthenaPlayerMatchReport*)UGameplayStatics::SpawnObject(UAthenaPlayerMatchReport::StaticClass(), NewPlayer));
    }
    
    return Pawn;
}

void AFortGameModeAthena::StartAircraftPhase(AFortGameModeAthena* GameMode, char a2)
{
    if (!GameMode) return(StartAircraftPhaseOG(GameMode, a2));
    StartAircraftPhaseOG(GameMode, a2);

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
            TArray<FGuid> GuidsToRemove;
        
            for (UFortWorldItem* Item : Inventory.GetItemInstances()) {
                if (Item) GuidsToRemove.Add(Item->GetItemEntry().GetItemGuid());
            }
        
            for (const FGuid& Guid : GuidsToRemove) { AFortInventory::Remove(Player, Guid, -1); }
        
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
    
    if (Config::bLateGame)
    {
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

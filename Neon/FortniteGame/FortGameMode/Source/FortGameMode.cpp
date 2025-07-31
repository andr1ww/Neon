#include "pch.h"
#include "../Header/FortGameMode.h"

#include "Engine/GameplayStatics/Header/GameplayStatics.h"
#include "Engine/Kismet/Header/Kismet.h"
#include "Engine/NetDriver/Header/NetDriver.h"
#include "Engine/UEngine/Header/UEngine.h"
#include "FortniteGame/FortInventory/Header/FortInventory.h"
#include "FortniteGame/FortPlayerController/Header/FortPlayerController.h"
#include "FortniteGame/FortAthenaAIBotController/Header/FortAthenaAIBotController.h"
#include "Neon/Finder/Header/Finder.h"
#include "Neon/Runtime/Runtime.h"

bool AFortGameModeAthena::ReadyToStartMatch(AFortGameModeAthena* GameMode, FFrame& Stack, bool* Result)
{
    Stack.IncrementCode();
    AFortGameStateAthena* GameState = GameMode->GetGameState();
    if (!GameState)
    {
        return *Result = false;
    }

    if (!GameState->GetMapInfo())
    {
        return *Result = false;
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
            GameState->CallFunc<void>("FortGameStateAthena","OnRep_AdditionalPlaylistLevelsStreamed");
            
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
                    ULevelStreamingDynamic::LoadLevelInstance(UWorld::GetWorld(), UKismetStringLibrary::GetDefaultObj()->CallFunc<FString>("KismetStringLibrary","Conv_NameToString",AdditionalLevels[i].SoftObjectPtr.ObjectID.AssetPathName), Loc, Rot, &Success, FString());
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
                    ULevelStreamingDynamic::LoadLevelInstance(UWorld::GetWorld(), UKismetStringLibrary::GetDefaultObj()->CallFunc<FString>("KismetStringLibrary","Conv_NameToString",AdditionalServerLevels[i].SoftObjectPtr.ObjectID.AssetPathName), Loc, Rot, &Success, FString());
                    FAdditionalLevelStreamed NewLevel{};
                    NewLevel.bIsServerOnly = true;
                    NewLevel.LevelName = AdditionalServerLevels[i].SoftObjectPtr.ObjectID.AssetPathName;
                    UE_LOG(LogNeon, Log, "Additional Server Level: %s", NewLevel.LevelName.ToString().ToString().c_str());
                    TArray<FAdditionalLevelStreamed>& Levels = GameState->GetAdditionalPlaylistLevelsStreamed();
                    Levels.Add(NewLevel, StaticClassImpl("AdditionalLevelStreamed")->GetSize());
                }

                GameState->CallFunc<void>("FortGameState", "OnRep_AdditionalPlaylistLevelsStreamed");
                GameState->CallFunc<void>("FortGameState", "OnFinishedStreamingAdditionalPlaylistLevel");
            }

            if (Fortnite_Version == 12.41)
            {
                ABuildingFoundation* JerkyFoundation = Runtime::StaticFindObject<ABuildingFoundation>("/Game/Athena/Apollo/Maps/Apollo_POI_Foundations.Apollo_POI_Foundations.PersistentLevel.LF_Athena_POI_19x19_2");
                JerkyFoundation->SetDynamicFoundationType(EDynamicFoundationType::Static);
                JerkyFoundation->SetbServerStreamedInLevel(true);
                JerkyFoundation->CallFunc<void>("BuildingFoundation", "OnRep_ServerStreamedInLevel");
                JerkyFoundation->GetDynamicFoundationRepData().SetEnabledState(EDynamicFoundationEnabledState::Enabled);
                JerkyFoundation->CallFunc<void>("BuildingFoundation", "OnRep_DynamicFoundationRepData");
                JerkyFoundation->CallFunc<void>("BuildingFoundation", "SetDynamicFoundationEnabled", true);
            }
            
            if (Fortnite_Version <= 13.40 && Fortnite_Version >= 12.00)
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
                AIDirector->CallFunc<void>("FortAIDirector", "Activate");

                AFortAIGoalManager* AIGoalManager = UGameplayStatics::SpawnActor<AFortAIGoalManager>({});
                GameMode->SetAIGoalManager(AIGoalManager);
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
    
    if (Fortnite_Version <= 13.40 && Fortnite_Version >= 12.00)
    {
        auto Time = UGameplayStatics::GetTimeSeconds(UWorld::GetWorld());
        auto WarmupDuration = 60.f;

        GameState->Set("FortGameStateAthena", "WarmupCountdownStartTime", Time);
        GameState->Set("FortGameStateAthena", "WarmupCountdownEndTime", Time + WarmupDuration + 10.f);
        GameMode->Set("FortGameModeAthena", "WarmupCountdownDuration", WarmupDuration);
        GameMode->Set("FortGameModeAthena", "WarmupEarlyCountdownDuration", WarmupDuration);
    }
    
    return *Result = GameMode->GetAlivePlayers().Num() >= GameMode->GetWarmupRequiredPlayerCount();;
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

    auto& StartingItemsArray = GameMode->GetStartingItems();
    int32 FItemAndCountSize = StaticClassImpl("ItemAndCount")->GetSize();
    for (int i = 0; i < StartingItemsArray.Num(); i++)
    {
        auto Item = (FItemAndCount*) ((uint8*) StartingItemsArray.GetData() + (i * FItemAndCountSize));
        
        if (!Item) {
            UE_LOG(LogNeon, Fatal, "StartingItem is null in SpawnDefaultPawnFor!");
            return Pawn;
        }
    
        AFortInventory::GiveItem(NewPlayer, Item->GetItem(), Item->GetCount(), 1, 1);
    }
    
    if (Fortnite_Version.GetMajorVersion() <= 8.50) {
    
    }
    else {
        auto Pickaxe = NewPlayer->GetCosmeticLoadoutPC().GetPickaxe();
        auto WeaponDef = Pickaxe->GetWeaponDefinition();
        AFortInventory::GiveItem(NewPlayer, WeaponDef, 1, 0, 1); 
    }

    /*static bool bSpawnedAI = false;
    if (!bSpawnedAI)
    {
        bSpawnedAI = true;
        AFortAthenaAIBotController::SpawnPlayerBot(90);
    }*/
    
    return Pawn;
}

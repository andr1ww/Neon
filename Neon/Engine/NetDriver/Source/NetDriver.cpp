#include "pch.h"
#include "../Header/NetDriver.h"

#include "Engine/GameplayStatics/Header/GameplayStatics.h"
#include "Engine/Kismet/Header/Kismet.h"
#include "Engine/Level/Header/Level.h"
#include "Engine/UEngine/Header/UEngine.h"
#include "Neon/Finder/Header/Finder.h"

UWorld* UWorld::GetWorld()
{
    static UEngine* GEngine = nullptr;

    if (!GEngine)
        GEngine = UEngine::GetEngine();

    if (UGameViewportClient* GameViewportClient = GEngine->GetGameViewport())
    {
        return GameViewportClient->GetWorld();
    }
}

bool UNetDriver::IsActorRelevantToConnection(AActor* Actor, TArray<FNetViewer>& ConnectionViewers)
{
    bool (*IsNetRelevantFor)(const AActor*, const AActor*, const AActor*, const FVector&) = decltype(IsNetRelevantFor)(Actor->GetVTable()[
        (Engine_Version == 416 || Fortnite_Version == 3.3) ? 0x420 / 8 :
        (Fortnite_Version == 1.10 || Fortnite_Version == 1.11 || (Fortnite_Version >= 2.42 && Fortnite_Version <= 3.2)) ? 0x418 / 8 :
        0x9C
    ]);
    
    for (FNetViewer& Viewer : ConnectionViewers)
        if (IsNetRelevantFor(Actor, Viewer.GetInViewer(), Viewer.GetViewTarget(), Viewer.GetViewLocation()))
            return true;

    return false;
}

UNetConnection* UNetDriver::IsActorOwnedByAndRelevantToConnection(const AActor* Actor, const TArray<FNetViewer>& ConnectionViewers, bool& bOutHasNullViewTarget)
{
    bool (*IsRelevancyOwnerFor)(const AActor*, const AActor*, const AActor*, const AActor*) = decltype(IsRelevancyOwnerFor)(Actor->GetVTable()[
        (Engine_Version == 416 || Fortnite_Version == 3.3) ? 0x420 / 8 + 2 :
        (Fortnite_Version == 1.10 || Fortnite_Version == 1.11 || (Fortnite_Version >= 2.42 && Fortnite_Version <= 3.2)) ? 0x418 / 8 + 2:
        0x9C + 2
    ]);
    AActor* (*GetNetOwner)(const AActor*) = decltype(GetNetOwner)(Actor->GetVTable()[
        (Engine_Version == 416 || Fortnite_Version == 3.3) ? 0x420 / 8 + 2 + 4 :
        (Fortnite_Version == 1.10 || Fortnite_Version == 1.11 || (Fortnite_Version >= 2.42 && Fortnite_Version <= 3.2)) ? 0x418 / 8 + 2 + 4 :
        0x9C + 2 + 4
    ]);

    const AActor* AOwner = GetNetOwner(Actor);

    bOutHasNullViewTarget = false;

    for (auto Viewer : ConnectionViewers)
    {
        UNetConnection* ViewerConnection = Viewer.GetConnection();
        bOutHasNullViewTarget |= (ViewerConnection->GetViewTarget() == nullptr);

        if (AOwner == ViewerConnection->GetPlayerController() ||
            (ViewerConnection->GetPlayerController() && (APawn*)AOwner == ViewerConnection->GetPlayerController()->GetPawn()) ||
            (ViewerConnection->GetViewTarget() && IsRelevancyOwnerFor(ViewerConnection->GetViewTarget(), Actor, AOwner, ViewerConnection->GetOwningActor())))
        {
            return ViewerConnection;
        }
    }

    return nullptr;
}

int32 UNetDriver::ServerReplicateActors_PrepConnections(UNetDriver* NetDriver, float DeltaSeconds)
{
    int32 NumClientsToTick = NetDriver->GetClientConnections().Num();
    if (NumClientsToTick == 0)
    {
        return 0;
    }

    bool bFoundReadyConnection = false;

    for (int32 ConnIdx = 0; ConnIdx < NetDriver->GetClientConnections().Num(); ConnIdx++)
    {
        UNetConnection* Connection = NetDriver->GetClientConnections()[ConnIdx];
        if (!Connection)
        {
            continue;
        }

        AActor* OwningActor = Connection->GetOwningActor();
        if (OwningActor != NULL)
        {
            bFoundReadyConnection = true;

            AActor* DesiredViewTarget = OwningActor;
            if (Connection->GetPlayerController())
            {
                if (AActor* ViewTarget = Connection->GetPlayerController()->GetViewTarget())
                {
                    DesiredViewTarget = ViewTarget;
                }
            }
            Connection->SetViewTarget(DesiredViewTarget);

            for (int32 ChildIdx = 0; ChildIdx < Connection->GetChildren().Num(); ChildIdx++)
            {
                UNetConnection* Child = Connection->GetChildren()[ChildIdx];
                APlayerController* ChildPlayerController = Child->GetPlayerController();
                AActor* DesiredChildViewTarget = Child->GetOwningActor();

                if (ChildPlayerController)
                {
                    AActor* ChildViewTarget = ChildPlayerController->GetViewTarget();

                    if (ChildViewTarget)
                    {
                        DesiredChildViewTarget = ChildViewTarget;
                    }
                }

                Child->SetViewTarget(DesiredChildViewTarget);
            }
        }
        else
        {
            Connection->SetViewTarget(NULL);
            for (int32 ChildIdx = 0; ChildIdx < Connection->GetChildren().Num(); ChildIdx++)
            {
                Connection->GetChildren()[ChildIdx]->SetViewTarget(NULL);
            }
        }
    }

    return bFoundReadyConnection ? NumClientsToTick : 0;
}

__forceinline float FRand()
{
    constexpr int32 RandMax = 0x00ffffff < RAND_MAX ? 0x00ffffff : RAND_MAX;
    return (rand() & RandMax) / (float)RandMax;
}

void UNetDriver::ServerReplicateActors_BuildConsiderList(UNetDriver* Driver, TArray<AActor*>& OutConsiderList, const float ServerTickTime)
{
    if (!Driver) {
        return;
    }
    
    static void (*CallPreReplication)(AActor* Actor, UNetDriver* Driver) = decltype(CallPreReplication)(Finder->CallPreReplication());
    static void (*RemoveNetworkActor)(UNetDriver*, AActor*) = decltype(RemoveNetworkActor)(Finder->RemoveNetworkActor());

    auto NetworkObjectList = *(*(class TSharedPtr<FNetworkObjectList>*)(__int64(Driver) + 0x720));
    
    UWorld* World = UWorld::GetWorld();
    if (!World) {
        return;
    }
    
    const float CurrentTime = UGameplayStatics::GetTimeSeconds(World);
    for (auto& ActorInfo : NetworkObjectList.ActiveNetworkObjects)
    {
        if (!ActorInfo.Get()) {
            continue;
        }
        
        auto Actor = ActorInfo->Actor;
        if (!Actor) {
            continue;
        }
        
        ENetRole RemoteRole = Actor->Get<ENetRole>("Actor", "RemoteRole");
        if (RemoteRole == ENetRole::ROLE_None) {
            continue;
        }
        
        ENetDormancy NetDormancy = Actor->Get<ENetDormancy>("Actor", "NetDormancy");
        if (NetDormancy == ENetDormancy::DORM_Initial) {
            continue;
        }

        ULevel* Level = ULevel::GetLevel(Actor);
        if (!Level) {
            continue;
        }
        
        UWorld* LevelWorld = Level->GetOwningWorld();
        if (!LevelWorld) {
            continue;
        }
        
        if (Level == LevelWorld->GetCurrentLevelPendingVisibility() || 
            Level == LevelWorld->GetCurrentLevelPendingInvisibility()) {
            continue;
            }

        if (!ActorInfo->bPendingNetUpdate) {
            float NetUpdateFrequency = Actor->Get<float>("Actor", "NetUpdateFrequency");
            if (NetUpdateFrequency <= 0.0f) {
                NetUpdateFrequency = 1.0f;
            }
            const float NextUpdateDelta = 1.f / NetUpdateFrequency;
            ActorInfo->NextUpdateTime = CurrentTime + FRand() * ServerTickTime + NextUpdateDelta;
            ActorInfo->LastNetUpdateTimeStamp = CurrentTime;
        }

        ActorInfo->bPendingNetUpdate = true;
        
        OutConsiderList.Add(Actor);
        
        CallPreReplication(Actor, Driver);
    }
}

static UActorChannel* FindActorChannelRef(UNetConnection* Connection, AActor* Actor) {
    static UActorChannel* (*CreateChannel)(UNetConnection*, FName*, EChannelCreateFlags, int32_t) = decltype(CreateChannel)(IMAGEBASE + 0x1194D48);
    static void (*SetChannelActor)(UChannel*, AActor*) = decltype(SetChannelActor)(IMAGEBASE + 0x10AABAC);
    for (UChannel* Channel : Connection->GetOpenChannels()) {
        if (Channel->GetClass() == UActorChannel::StaticClass()) {
            if (Cast<UActorChannel>(Channel)->GetActor() == Actor) return Cast<UActorChannel>(Channel);
        }
    }
    FName ActorName = UKismetStringLibrary::GetDefaultObj()->CallFunc<FName>("KismetStringLibrary","Conv_StringToName",FString(L"Actor"));
    UActorChannel* Channel = CreateChannel(Connection, &ActorName, EChannelCreateFlags::OpenedLocally, -1);
    if (Channel) {
        SetChannelActor(Channel, Actor);
        Channel->SetConnection(Connection);
        return Channel;
    }
}

static bool ReplicateToClient(UNetDriver* Driver, AActor* Actor, UNetConnection* Client) {
    static void (*CallPreReplication)(AActor * Actor, UNetDriver * Driver) = decltype(CallPreReplication)(IMAGEBASE + 0x9CC9E34);
    static void (*ReplicateActor)(UActorChannel * Ch) = decltype(ReplicateActor)(IMAGEBASE + 0x9DEACC0);
    if (!Client || !Actor) {
        return false;
    }
    
    auto Ch = FindActorChannelRef(Client, Actor);
    if (!Ch) {
        return false;
    }
    CallPreReplication(Actor, Driver);
    ReplicateActor(Ch); 
	
    return true;
}

void UNetDriver::ServerReplicateActors_ProcessActors(UNetDriver* Driver, UNetConnection* Connection, TArray<AActor*>& Actors) 
{
    if (!Driver) {
        return;
    }

    const int32 ActorCount = Actors.Num();
    if (ActorCount <= 0) {
        return;
    }
    
    if (!Connection) {
        return;
    }
    
    for (int32 i = 0; i < ActorCount; i++)
    {
        AActor* Actor = Actors[i];
        
        if (!Actor) {
            continue;
        }

        if (Actor->IsA(APlayerController::StaticClass()) && Connection->GetPlayerController() && Actor != Connection->GetPlayerController()) {
            continue;
        }

        ReplicateToClient(Driver, Actor, Connection);
    }
}

int32 UNetDriver::ServerReplicateActors(UNetDriver* NetDriver, float DeltaSeconds)
{
    ++NetDriver->ReplicationFrame();
    const int32 NumClientsToTick = ServerReplicateActors_PrepConnections(NetDriver, DeltaSeconds);
    if (NumClientsToTick == 0)
    {
        return 0;
    }

    static void (*SendClientAdjustment)(APlayerController*) = decltype(SendClientAdjustment)(Finder->SendClientAdjustment());

    TArray<AActor*> ConsiderList;

    const float ServerTickTime = 1.0f / 30.f; // GetMaxTickRate(UFortEngine::GetGameEngine(), DeltaSeconds);
    ServerReplicateActors_BuildConsiderList(NetDriver, ConsiderList, ServerTickTime);
    
    for (int32 i = 0; i < NetDriver->GetClientConnections().Num(); i++)
    {
        UNetConnection* Connection = NetDriver->GetClientConnections()[i];
        if (!Connection || !Connection->GetViewTarget() || !Connection->GetPlayerController()) {
            continue;
        }
        SendClientAdjustment(Connection->GetPlayerController());
        ServerReplicateActors_ProcessActors(NetDriver, Connection, ConsiderList);
    }

    return ConsiderList.Num();
}

void UNetDriver::TickFlush(UNetDriver* NetDriver, float DeltaSeconds)
{
    if (GetAsyncKeyState(VK_F5) & 0x1) {
        ExecuteConsoleCommand(UWorld::GetWorld(), L"startaircraft", nullptr);
        Sleep(400);
    }
    
    if (GetAsyncKeyState(VK_F6) & 0x1)
    {
        static std::string Loader = std::string();
        static std::string StartEventPath = std::string();
        static std::string OnReadyPath = std::string();
        static bool bStartedEvent = false;
        if (!bStartedEvent)
        {
            if (Fortnite_Version == 12.41)
            {
                Loader = "/CycloneJerky/Gameplay/BP_Jerky_Loader.BP_Jerky_Loader_C";
                StartEventPath = "/CycloneJerky/Gameplay/BP_Jerky_Loader.BP_Jerky_Loader_C.startevent";
            } else if (Fortnite_Version == 12.61)
            {
                Loader = "/Fritter/BP_Fritter_Loader.BP_Fritter_Loader_C";
                StartEventPath = "/Fritter/BP_Fritter_Loader.BP_Fritter_Loader_C.startevent";
            }
        
            TArray<AActor*> ret = UGameplayStatics::GetAllActorsOfClass(UWorld::GetWorld(), Runtime::StaticFindObject<UClass>(Loader));
        
            if (ret.Num() == 0) {
                return;
            }
            auto Loader = ret[0];
        
            auto StartEvent = Runtime::StaticFindObject<UFunction>(StartEventPath);
            struct StartEventParams {
                float _Maybe_StartTime;
            };
            StartEventParams Parms{};
            Parms._Maybe_StartTime = 0.f;
            Loader->ProcessEvent(StartEvent, &Parms);
            bStartedEvent = true;
        }
    }
    
    if (NetDriver->GetClientConnections().Num() > 0)
    {
        if (Finder->RepDriverServerReplicateActors() && Fortnite_Version.GetMajorVersion() <= 20)
        {
            reinterpret_cast<void(*)(UReplicationDriver*)>(NetDriver->GetReplicationDriver()->GetVTable()[Finder->RepDriverServerReplicateActors()])(NetDriver->GetReplicationDriver());
        } else
        {
            ServerReplicateActors(NetDriver, DeltaSeconds);
        }
    }
    
	return TickFlushOriginal(NetDriver, DeltaSeconds);
}

bool UNetDriver::InitListen(UWorld* NetWorkNotify, FURL URL, bool bReuseAddressAndPort)
{
    static bool (*InitListen)(UNetDriver*, UWorld*, FURL&, bool, FString&) = decltype(InitListen)(Finder->InitListen());
    FString Error;

    UWorld* World = UWorld::GetWorld();
    return InitListen(this, World, URL, bReuseAddressAndPort, Error);
}

void UNetDriver::SetWorld(UWorld* World)
{
    static void (*SetWorld)(UNetDriver*, UWorld*) = nullptr;
    if (!SetWorld)
        SetWorld = decltype(SetWorld)(Finder->SetWorld());
    return SetWorld(this, World);
}
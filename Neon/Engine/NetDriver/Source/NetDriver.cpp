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

bool UNetDriver::IsActorRelevantToConnection(const AActor* Actor, const TArray<FNetViewer>& ConnectionViewers)
{
    bool (*IsNetRelevantFor)(const AActor*, const AActor*, const AActor*, const FVector&) = decltype(IsNetRelevantFor)(Actor->GetVTable()[
        (Engine_Version == 416 || Fortnite_Version == 3.3) ? 0x420 / 8 :
        (Fortnite_Version == 1.10 || Fortnite_Version == 1.11 || (Fortnite_Version >= 2.42 && Fortnite_Version <= 3.2)) ? 0x418 / 8 :
        0x9C
    ]);
    
    for (const FNetViewer& Viewer : ConnectionViewers)
        if (IsNetRelevantFor(Actor, Viewer.InViewer, Viewer.ViewTarget, Viewer.ViewLocation))
            return true;
	
    return false;
}

UNetConnection* UNetDriver::IsActorOwnedByAndRelevantToConnection(const AActor* Actor, const TArray<FNetViewer>& ConnectionViewers, bool& bOutHasNullViewTarget)
{
    bool (*IsRelevancyOwnerFor)(const AActor*, const AActor*, const AActor*, const AActor*) = decltype(IsRelevancyOwnerFor)(Actor->GetVTable()[0x9e]);
    AActor* (*GetNetOwner)(const AActor*) = decltype(GetNetOwner)(Actor->GetVTable()[0xa2]);

    const AActor* AOwner = GetNetOwner(Actor);

    bOutHasNullViewTarget = false;

    for (auto& Viewer : ConnectionViewers)
    {
        UNetConnection* ViewerConnection = Viewer.Connection;
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

    bool bFoundReadyConnection = false;

    for (int32 ConnIdx = 0; ConnIdx < NetDriver->GetClientConnections().Num(); ConnIdx++)
    {
        UNetConnection* Connection = NetDriver->GetClientConnections()[ConnIdx];
        check(Connection);

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
    static void (*CallPreReplication)(AActor* Actor, UNetDriver* Driver) = decltype(CallPreReplication)(Finder->CallPreReplication());
    static void (*RemoveNetworkActor)(UNetDriver*, AActor*) = decltype(RemoveNetworkActor)(Finder->RemoveNetworkActor());

    auto NetworkObjectList = *(*(class TSharedPtr<FNetworkObjectList>*)(__int64(Driver) + 0x720));
    const float CurrentTime = UGameplayStatics::GetTimeSeconds(UWorld::GetWorld());
    
    TArray<AActor*> ActorsToRemove;

    for (auto& ActorInfo : NetworkObjectList.ActiveNetworkObjects)
    {
        auto Actor = ActorInfo->Actor;

        if (!Actor)
        {
            continue;
        }
		
        if (!Actor || Actor->Get<uint8>("Actor", "bActorIsBeingDestroyed") || Actor->Get<ENetRole>("Actor", "RemoteRole") == ENetRole::ROLE_None || Actor->Get<FName>("Actor", "NetDriverName").ToString().ToString().c_str() != Driver->GetNetDriverName().ToString().ToString().c_str())
        {
            ActorsToRemove.Add(Actor);
            continue;
        }

        if (Actor->Get<ENetDormancy>("Actor", "NetDormancy") == ENetDormancy::DORM_Initial)
        {
            continue;
        }

        ULevel* Level = ULevel::GetLevel(Actor);
        if (!Level || Level == Level->GetOwningWorld()->GetCurrentLevelPendingVisibility() || Level == Level->GetOwningWorld()->GetCurrentLevelPendingInvisibility())
        {
            continue;
        }

        if (!ActorInfo->bPendingNetUpdate)
        {
            const float NextUpdateDelta = 1.f / Actor->Get<float>("Actor", "NetUpdateFrequency");
            ActorInfo->NextUpdateTime = CurrentTime + FRand() * ServerTickTime + NextUpdateDelta;
            ActorInfo->LastNetUpdateTimeStamp = CurrentTime;
        }

        ActorInfo->bPendingNetUpdate = true;
        
        OutConsiderList.Add(Actor);
        CallPreReplication(Actor, Driver);
    }
	
    for (auto& Actor : ActorsToRemove) {
        RemoveNetworkActor(Driver, Actor);
    }
    ActorsToRemove.Free();
}


int32 UNetDriver::ServerReplicateActors_ProcessActors(UNetDriver* Driver, UNetConnection* Connection, TArray<AActor*>& Actors) 
{
    if (!Connection) {
        return 0;
    }
    
    TArray<FNetViewer> ConnectionViewers;
	FNetViewer MainViewer;
    MainViewer.Connection = Connection;
    MainViewer.InViewer = Connection->GetPlayerController();
    MainViewer.ViewTarget = Connection->GetViewTarget();
    if (Connection->GetViewTarget()) {
        MainViewer.ViewLocation = Connection->GetViewTarget()->GetActorLocation();
    }
    ConnectionViewers.Add(MainViewer);
    
    const int32 ChildCount = Connection->GetChildren().Num();
    for (int32 ChildIdx = 0; ChildIdx < ChildCount; ChildIdx++) {
        UNetConnection* ChildConnection = Connection->GetChildren()[ChildIdx];
        if (ChildConnection) {
            FNetViewer ChildViewer;
            ChildViewer.Connection = ChildConnection;
            ChildViewer.InViewer = ChildConnection->GetPlayerController();
            ChildViewer.ViewTarget = ChildConnection->GetViewTarget();
            if (ChildConnection->GetViewTarget()) {
                ChildViewer.ViewLocation = ChildConnection->GetViewTarget()->GetActorLocation();
            }
            ConnectionViewers.Add(ChildViewer);
        }
    }
    
    int32 FinalActors = 0;
    const int32 ActorCount = Actors.Num();
	AActor* const* ActorData = Actors.GetData();
    
    for (int32 i = 0; i < ActorCount; i++) {
        AActor* Actor = ActorData[i];
        if (!Actor) {
            continue;
        }
        
        bool bHasNullViewTarget = false;
        UNetConnection* OwningConnection = IsActorOwnedByAndRelevantToConnection(Actor, ConnectionViewers, bHasNullViewTarget);
        
        if (OwningConnection) {
            ReplicateToClient(Driver, Actor, OwningConnection);
            ++FinalActors; 
        }
        else if (IsActorRelevantToConnection(Actor, ConnectionViewers)) {
            ReplicateToClient(Driver, Actor, Connection);
            ++FinalActors;
        }
    }

    return FinalActors;
}


int32 UNetDriver::ServerReplicateActors(UNetDriver* NetDriver, float DeltaSeconds)
{
    ++NetDriver->ReplicationFrame();
    const int32 NumClientsToTick = ServerReplicateActors_PrepConnections(NetDriver, DeltaSeconds);
    if (NumClientsToTick == 0)
    {
        return 0;
    }

    static float (*GetMaxTickRate)(UGameEngine*, float DeltaSeconds) = decltype(GetMaxTickRate)(Finder->GetMaxTickRate());
    static UActorChannel* (*CreateChannel)(UNetConnection*, FName*, EChannelCreateFlags, int32_t) = decltype(CreateChannel)(Finder->CreateChannel());
    static void (*SendClientAdjustment)(APlayerController*) = decltype(SendClientAdjustment)(Finder->SendClientAdjustment());

    static TArray<AActor*> ConsiderList;
    static TArray<APlayerController*> ControllersToAdjust;
    static FString ActorStr = L"Actor";
    static FName ActorName = UKismetStringLibrary::GetDefaultObj()->CallFunc<FName>("KismetStringLibrary","Conv_StringToName", ActorStr);

    const float ServerTickTime = 1.0f / GetMaxTickRate(UFortEngine::GetGameEngine(), DeltaSeconds);
    ServerReplicateActors_BuildConsiderList(NetDriver, ConsiderList, ServerTickTime);

    for (int32 i = 0; i < NetDriver->GetClientConnections().Num(); i++)
    {
        UNetConnection* Connection = NetDriver->GetClientConnections()[i];
        if (!Connection || !Connection->GetViewTarget() || !Connection->GetPlayerController()) {
            continue;
        }
       
        ControllersToAdjust.Add(Connection->GetPlayerController());
        ServerReplicateActors_ProcessActors(NetDriver, Connection, ConsiderList);
        TArray<FActorDestructionInfo*> DeletionEntries;
       
        if (DeletionEntries.Num() > 0) {
            auto ClientVisibleLevelNames = GetClientVisibleLevelNames(Connection);
           
            for (FActorDestructionInfo* DeletionEntry : DeletionEntries) {
                bool bFound = false;
               
                for (FName& ClientVisibleLevelName : *ClientVisibleLevelNames) {
                    if (ClientVisibleLevelName.ToString().ToString().c_str() == DeletionEntry->StreamingLevelName.ToString().ToString().c_str()) {
                        bFound = true;
                        break;
                    }
                }
               
                if (!bFound) {
                    continue;
                }
               
                UActorChannel* Channel = (UActorChannel*)CreateChannel(Connection, &ActorName, EChannelCreateFlags::OpenedLocally, -1);
            }
        }
    }

    for (APlayerController* Controller : ControllersToAdjust) {
        SendClientAdjustment(Controller);
    }
}

void UNetDriver::TickFlush(UNetDriver* NetDriver, float DeltaSeconds)
{
    if (Finder->RepDriverServerReplicateActors() && Fortnite_Version.GetMajorVersion() <= 20)
    {
        UE_LOG(LogNeon, Log, "Using Replication Driver for ServerReplicateActors");
        reinterpret_cast<void(*)(UObject*)>(NetDriver->GetReplicationDriver()->GetVTable()[Finder->RepDriverServerReplicateActors()])(NetDriver->GetReplicationDriver());
    } else
    {
        UE_LOG(LogNeon, Log, "Using UNetDriver::ServerReplicateActors for ServerReplicateActors");
        if (NetDriver->GetClientConnections().Num() > 0)
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
    printf("Valid World: %d", World ? 1 : 0);
    return InitListen(this, World, URL, bReuseAddressAndPort, Error);
}

void UNetDriver::SetWorld(UWorld* World)
{
    static void (*SetWorld)(UNetDriver*, UWorld*) = nullptr;
    if (!SetWorld)
        SetWorld = decltype(SetWorld)(Finder->SetWorld());
    return SetWorld(this, World);
}

void UNetDriver::DispatchRequest(void* unknown_1, void* MCPData, int MCPCode)
{
    if (Fortnite_Version < 8.01) *(int*)(__int64(MCPData) + (Fortnite_Version < 4.2 ? 0x60 : 0x28)) = 3;
    return DispatchRequestOriginal(unknown_1, MCPData, MCPCode);
}

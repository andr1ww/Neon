#include "pch.h"
#include "../Header/NetDriver.h"

#include "Engine/GameplayStatics/Header/GameplayStatics.h"
#include "Engine/Kismet/Header/Kismet.h"
#include "Engine/Level/Header/Level.h"
#include "Engine/UEngine/Header/UEngine.h"
#include "Neon/Finder/Header/Finder.h"
#pragma check_stack(off)
#pragma runtime_checks("", off)
#pragma optimize("", off)

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

    __int64 DriverAddress = __int64(Driver);
    
    auto NetworkObjectListSharedPtr = (class TSharedPtr<FNetworkObjectList>*)(DriverAddress + 0x720);
    if (!NetworkObjectListSharedPtr) {
        return;
    }
    
    if (!NetworkObjectListSharedPtr->Object) {
        return;
    }
    
    TSharedPtr<FNetworkObjectList> LocalNetworkObjectListRef = *NetworkObjectListSharedPtr;
    auto NetworkObjectList = LocalNetworkObjectListRef.Get();
    if (!NetworkObjectList) {
        return;
    }
    
    UWorld* World = UWorld::GetWorld();
    if (!World) {
        return;
    }
    
    const float CurrentTime = UGameplayStatics::GetTimeSeconds(World);
    for (auto& ActorInfo : NetworkObjectList->ActiveNetworkObjects)
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
        
        if (CallPreReplication) {
            CallPreReplication(Actor, Driver);
        }
    }
}

#pragma check_stack(off)
#pragma runtime_checks("", off)
#pragma optimize("", off)
void UNetDriver::ServerReplicateActors_ProcessActors(UNetDriver* Driver, UNetConnection* Connection, TArray<AActor*>& Actors) 
{
    static bool (*DemoReplicateActor)(UNetDriver*, AActor*, UNetConnection*, bool) = decltype(DemoReplicateActor)(Finder->DemoReplicateActor());

    if (!Connection || !Driver) {
        return;
    }
    
    const int32 ActorCount = Actors.Num();
    if (ActorCount <= 0) {
        return;
    }
    
    TArray<FNetViewer> ConnectionViewers;
    
    if (Connection->GetPlayerController() || Connection->GetViewTarget())
    {
        FNetViewer MainViewer{};
        MainViewer.SetConnection(Connection);
        MainViewer.SetInViewer(Connection->GetPlayerController());
    
        AActor* ViewTarget = Connection->GetViewTarget();
        MainViewer.SetViewTarget(ViewTarget);
    
        if (ViewTarget)
        {
            MainViewer.SetViewLocation(ViewTarget->GetActorLocation());
        }
        else
        {
            if (Connection->GetPlayerController())
            {
                MainViewer.SetViewLocation(Connection->GetPlayerController()->GetActorLocation());
            }
            else
            {
                MainViewer.SetViewLocation(FVector(0,0,0));
            }
        }
    
        ConnectionViewers.Add(MainViewer);
    }
    
    const int32 ChildCount = Connection->GetChildren().Num();
    for (int32 ChildIdx = 0; ChildIdx < ChildCount; ChildIdx++) {
        UNetConnection* ChildConnection = Connection->GetChildren()[ChildIdx];
        if (ChildConnection && (ChildConnection->GetPlayerController() || ChildConnection->GetViewTarget()))
        {
            FNetViewer ChildViewer{};
            ChildViewer.SetConnection(ChildConnection);
            ChildViewer.SetInViewer(ChildConnection->GetPlayerController());
        
            AActor* ChildViewTarget = ChildConnection->GetViewTarget();
            ChildViewer.SetViewTarget(ChildViewTarget);
        
            if (ChildViewTarget)
            {
                ChildViewer.SetViewLocation(ChildViewTarget->GetActorLocation());
            }
            else if (ChildConnection->GetPlayerController())
            {
                ChildViewer.SetViewLocation(ChildConnection->GetPlayerController()->GetActorLocation());
            }
            else
            {
                ChildViewer.SetViewLocation(FVector(0,0,0));
            }
        
            ConnectionViewers.Add(ChildViewer);
        }
    }
    
    if (ConnectionViewers.Num() == 0) {
        return;
    }
    
    for (int32 i = 0; i < ActorCount; i++)
    {
        AActor* Actor = Actors[i];
        
        if (!Actor) {
            continue;
        }

        DemoReplicateActor(Driver, Actor, Connection, false);
    }
}

#pragma runtime_checks("", off)
int32 UNetDriver::ServerReplicateActors(UNetDriver* NetDriver, float DeltaSeconds)
{
    ++NetDriver->ReplicationFrame();
    const int32 NumClientsToTick = ServerReplicateActors_PrepConnections(NetDriver, DeltaSeconds);
    if (NumClientsToTick == 0)
    {
        return 0;
    }

    static float (*GetMaxTickRate)(UGameEngine*, float DeltaSeconds) = decltype(GetMaxTickRate)(Finder->GetMaxTickRate());
    static void (*SendClientAdjustment)(APlayerController*) = decltype(SendClientAdjustment)(Finder->SendClientAdjustment());

    TArray<AActor*> ConsiderList;
    TArray<APlayerController*> ControllersToAdjust;

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
    } 

    for (APlayerController* Controller : ControllersToAdjust) {
        SendClientAdjustment(Controller);
    }
}

void UNetDriver::TickFlush(UNetDriver* NetDriver, float DeltaSeconds)
{
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

void UNetDriver::DispatchRequest(__int64 a1, __int64* a2, int a3)
{
    if (Fortnite_Version < 8.01) *(int*)(__int64(a2) + (Fortnite_Version < 4.2 ? 0x60 : 0x28)) = 3;
    return DispatchRequestOriginal(a1, a2, 3);
}

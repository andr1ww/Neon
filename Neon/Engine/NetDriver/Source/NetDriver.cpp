#include "pch.h"
#include "../Header/NetDriver.h"

#include "Engine/GameplayStatics/Header/GameplayStatics.h"
#include "Engine/Level/Header/Level.h"
#include "Engine/UEngine/Header/UEngine.h"

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
    static void (*CallPreReplication)(AActor* Actor, UNetDriver* Driver) = decltype(CallPreReplication)(IMAGEBASE + 0x9CC9E34);
    static void (*RemoveNetworkActor)(UNetDriver*, AActor*) = decltype(RemoveNetworkActor)(IMAGEBASE + 0x10AD474);

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
		
        if (!Actor || Actor->bActorIsBeingDestroyed || Actor->RemoteRole == ENetRole::ROLE_None || Actor->NetDriverName != Driver->GetNetDriverName())
        {
            ActorsToRemove.Add(Actor);
            continue;
        }

        if (Actor->NetDormancy == ENetDormancy::DORM_Initial)
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
            const float NextUpdateDelta = 1.f / Actor->NetUpdateFrequency;
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

int32 UNetDriver::ServerReplicateActors(UNetDriver* NetDriver, float DeltaSeconds)
{
    ++NetDriver->ReplicationFrame();
    const int32 NumClientsToTick = ServerReplicateActors_PrepConnections(NetDriver, DeltaSeconds);
    if (NumClientsToTick == 0)
    {
        return 0;
    }
    
}

void UNetDriver::TickFlush(UNetDriver* NetDriver, float DeltaSeconds)
{
    
}

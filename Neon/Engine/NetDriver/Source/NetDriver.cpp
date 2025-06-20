#include "pch.h"
#include "../Header/NetDriver.h"

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

int32 UNetDriver::ServerReplicateActors(UNetDriver* NetDriver, float DeltaSeconds)
{
    ++NetDriver->ReplicationFrame();
    
}

void UNetDriver::TickFlush(UNetDriver* NetDriver, float DeltaSeconds)
{
    
}

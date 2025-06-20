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

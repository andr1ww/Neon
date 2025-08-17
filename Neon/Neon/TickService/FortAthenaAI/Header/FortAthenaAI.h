#pragma once
#include "pch.h"

#include "FortniteGame/FortPlayerPawn/Header/FortPlayerPawn.h"

struct FortAthenaAI
{
    AFortAthenaAIBotController* Controller = nullptr;
    AFortPlayerPawn* Pawn = nullptr;
    FVector Target = FVector(0,0,0);
    bool bThankedBusDriver = false;
    bool bSkydiving = false;
    bool Emoting = false;
    float LastEmoteTime = 0.0f;
    FVector OldVelocity = FVector(0,0,0);
};

namespace TickService
{
    namespace FortAthenaAIService
    {
        void AddToService(AFortAthenaAIBotController* Controller, AFortPlayerPawn* Pawn);
        void Tick();
        void WarmupPhase(FortAthenaAI& AI, float CurrentTime);
        void AircraftPhase(FortAthenaAI& AI, float CurrentTime);
    }
}

#pragma once
#include "pch.h"

#include "FortniteGame/FortPlayerPawn/Header/FortPlayerPawn.h"

struct FortAthenaAI
{
    AFortAthenaAIBotController* Controller;
    AFortPlayerPawn* Pawn;
    bool Emoting = false;
    float LastEmoteTime = 0.0f;
};

namespace TickService
{
    namespace FortAthenaAIService
    {
        void AddToService(AFortAthenaAIBotController* Controller, AFortPlayerPawn* Pawn);
        void Tick();

        void WarmupPhase(FortAthenaAI& AI);
    }
}

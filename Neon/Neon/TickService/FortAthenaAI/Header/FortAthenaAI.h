#pragma once
#include "pch.h"

#include "FortniteGame/FortPlayerPawn/Header/FortPlayerPawn.h"

static const std::vector<FVector> DropZoneLocations = {
    { 106902, -84901, -1834 }, { 107834, -78584, -783 }, { 120009, -84032, -3370 }, { 112255, -91220, -3011 },
    { 99820, -82200, -3370 }, { 96764, 29166, -2226 }, { 104078, 26670, -1834 }, { 94032, 51513, 4693 },
    { 76334, 90345, -1450 }, { 81669, 90762, 469 }, { 80258, 95433, 69 }, { 86691, 102076, 69 },
    { 113312, 113547, -1837 }, { 116567, 113665, -2602 }, { 109895, 113636, -2986 }, { 106233, 108428, -3762 },
    { 64240, -16323, 69 }, { 60290, -16240, 69 }, { 31257, -77599, 69 }, { 13637, -24219, 69 },
    { 30808, 10669, 69 }, { 30536, 42295, 69 }, { 30437, 69691, 69 }, { 17963, 112955, 69 },
    { 6364, 4866, 69 }, { 6426, 7100, 69 }, { 6245, 1741, 69 }, { 11188, 4863, 69 },
    { 657, 4334, 69 }, { 4084, 7167, 69 }, { -13311, -81033, 69 }, { -28997, -31787, 69 },
    { -64881, -46495, 69 }, { -77143, -80634, 69 }, { -73305, -90910, 69 }, { -88282, 23038, 69 },
    { -88329, 31859, 69 }, { -68041, 29906, 69 }, { -43937, 52030, 69 }, { -54818, 57400, 69 },
    { -68550, 80804, 69 }, { -92971, 78709, 69 }, { -39867, 91323, 69 }, { -38750, 102176, 69 },
    { -19544, 105594, 69 }, { -17383, 112993, 69 }, { -22500, 112479, 69 },
};

struct FortAthenaAI
{
    AFortAthenaAIBotController* Controller = nullptr;
    AFortPlayerPawn* Pawn = nullptr;
    FVector Target = FVector(0,0,0);
    bool bThankedBusDriver = false;
    bool bSkydiving = false;
    bool Emoting = false;
    bool Moving = false;
    float LastEmoteTime = 0.0f;
    FVector OldVelocity = FVector(0,0,0);
    AActor* TargetLoot = nullptr;
    AActor* CurrentFocus = nullptr;
    int32 LastFrame = 0;
};

class UFortGameplayAbility_Sprint : public UObject
{
public:
    DECLARE_STATIC_CLASS(UFortGameplayAbility_Sprint)
};

namespace TickService
{
    namespace FortAthenaAIService
    {
        vector<FortAthenaAI> Get();
        
        void AddToService(AFortAthenaAIBotController* Controller, AFortPlayerPawn* Pawn);
        void RemoveFromService(AFortAthenaAIBotController* Controller);
        
        void Tick();
        void WarmupPhase(FortAthenaAI& AI, float CurrentTime);
        void AircraftPhase(FortAthenaAI& AI, float CurrentTime);
        void SafeZonesPhase(FortAthenaAI& AI, float CurrentTime);
    }
}

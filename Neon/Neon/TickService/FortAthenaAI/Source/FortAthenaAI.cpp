#include "pch.h"
#include "../Header/FortAthenaAI.h"

#include "Engine/GameplayStatics/Header/GameplayStatics.h"
#include "FortniteGame/FortAthenaAIBotController/Header/FortAthenaAIBotController.h"
#include "FortniteGame/FortGameMode/Header/FortGameMode.h"

#include <random>
#include <algorithm>
#include <cmath>

static std::vector<FortAthenaAI> AIs;
static std::vector<UAthenaDanceItemDefinition*> Emotes;
static AFortGameModeAthena* GameMode = nullptr;
static AFortGameStateAthena* GameState = nullptr;
static float LastUpdate = 0.0f;
static std::mt19937 RNG{5489u};
static size_t TickCursor = 0;
static const size_t TickStride = 4;

static inline float FRand()
{
    static std::uniform_real_distribution<float> d(0.0f, 1.0f);
    return d(RNG);
}

static inline float ToDeg(float r) { return r * (180.0f / 3.14159265358979323846f); }
static inline float Clamp(float v, float a, float b) { return max(a, min(v, b)); }

static inline int RandIndex(int max)
{
    std::uniform_int_distribution<int> d(0, max - 1);
    return d(RNG);
}

void TickService::FortAthenaAIService::AddToService(AFortAthenaAIBotController* Controller, AFortPlayerPawn* Pawn)
{
    if (Emotes.empty())
    {
        Emotes = Runtime::GetObjectsOfClass<UAthenaDanceItemDefinition>();
    }
    AIs.emplace_back();
    AIs.back().Controller = Controller;
    AIs.back().Pawn = Pawn;
    AIs.back().bThankedBusDriver = false;
    AIs.back().bSkydiving = false;
    AIs.back().Emoting = false;
    AIs.back().LastEmoteTime = 0.0f;
    AIs.back().Target = FVector(0,0,0);
}

void TickService::FortAthenaAIService::Tick()
{
    const float CurrentTime = UGameplayStatics::GetTimeSeconds(UWorld::GetWorld());
    if (!GameMode || (CurrentTime - LastUpdate) > 1.0f)
    {
        GameMode = UWorld::GetWorld()->GetAuthorityGameMode();
        GameState = GameMode ? GameMode->GetGameState() : nullptr;
        LastUpdate = CurrentTime;
    }
    if (!GameState || AIs.empty()) return;

    const EAthenaGamePhase GamePhase = GameState->GetGamePhase();
    static AActor* Aircraft = nullptr;

    const size_t n = AIs.size();
    const size_t batch = std::max<size_t>(1, (n + TickStride - 1) / TickStride);
    for (size_t j = 0; j < batch; ++j)
    {
        FortAthenaAI& AI = AIs[(TickCursor + j) % n];
        if (!AI.Controller || !AI.Pawn) continue;

        if (GamePhase == EAthenaGamePhase::Warmup)
        {
            WarmupPhase(AI, CurrentTime);
        }
        else if (GamePhase == EAthenaGamePhase::Aircraft)
        {
            static bool bFirst = false;
            if (!bFirst) 
            { 
                bFirst = true; 
                auto Aircrafts = UWorld::GetWorld()->GetGameState()->GetAircrafts();
                if (Aircrafts.IsValidIndex(0)) Aircraft = Aircrafts[0];
                return; 
            }

            AircraftPhase(AI, CurrentTime);
        }

        if (AI.bSkydiving && GameState->GetGamePhase() > EAthenaGamePhase::Aircraft || !AI.bThankedBusDriver && GameState->GetGamePhase() > EAthenaGamePhase::Aircraft)
        {
            if (!AI.bThankedBusDriver && GameState->GetGamePhase() > EAthenaGamePhase::Aircraft)
            {
                AI.bThankedBusDriver = true;
                AI.Controller->ThankBusDriver();
                AI.Pawn->BeginSkydiving(true);
            }
            AI.Pawn->K2_TeleportTo(AI.Target, {});
            AI.bSkydiving = false;
        }
        
        if (AI.bSkydiving)
        {
            const FVector BotPos = AI.Pawn->K2_GetActorLocation();
            FVector DirectionXY = AI.Target - BotPos;
            DirectionXY.Z = 0;
            DirectionXY.Normalize(); 

            if (auto* MoveComp = AI.Pawn->GetMovementComponent())
            {
                if (AI.OldVelocity.IsZero())
                {
                    AI.OldVelocity = MoveComp->GetVelocity();
                }

                FVector DesiredVelocity;

                if (AI.Pawn->GetbIsSkydiving())
                {
                    float SpeedXY = 2000.f;

                    float DistZ = std::abs(BotPos.Z - AI.Target.Z);

                    float MaxSpeedZ = 5000.f;
                    float MinSpeedZ = 500.f;
                    float SpeedZ = clamp(DistZ * 5.f, MinSpeedZ, MaxSpeedZ); 

                    DesiredVelocity = DirectionXY * SpeedXY; 
                    DesiredVelocity.Z = -SpeedZ;             
                }
                else
                {
                    DesiredVelocity = AI.OldVelocity; 
                }

                MoveComp->SetVelocity(DesiredVelocity);
            }

            FRotator LookRot = UKismetMathLibrary::FindLookAtRotation(BotPos, AI.Target);
            LookRot.Pitch -= 20.0f;
            AI.Controller->SetControlRotation(LookRot);
            AI.Pawn->K2_SetActorRotation(LookRot, true);

            AI.Controller->MoveToLocation(AI.Target, 3500.f, true, false, false, true, nullptr, true);

            if (BotPos.Z <= AI.Target.Z + 100.0f)
            {
                AI.bSkydiving = false;
            }
        }
    }
    
    TickCursor = (TickCursor + batch) % n;
}

void TickService::FortAthenaAIService::WarmupPhase(FortAthenaAI& AI, float CurrentTime)
{
    if (AI.Emoting && (CurrentTime - AI.LastEmoteTime) < 10.0f) return;
    if (AI.Emoting && (CurrentTime - AI.LastEmoteTime) > 15.0f) AI.Emoting = false;
    if (Emotes.empty()) return;

    UAthenaDanceItemDefinition* EmoteDef = Emotes[RandIndex((int)Emotes.size())];
    if (!EmoteDef || EmoteDef->IsA<UAthenaEmojiItemDefinition>()) return;

    AI.Emoting = true;
    AI.LastEmoteTime = CurrentTime;

    static FGameplayAbilitySpec* Spec = nullptr;
    static int32 GameplayAbilitySpecSize = 0;
    if (!Spec)
    {
        GameplayAbilitySpecSize = StaticClassImpl("GameplayAbilitySpec")->GetSize();
        Spec = (FGameplayAbilitySpec*)VirtualAlloc(0, GameplayAbilitySpecSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (Spec) new(Spec) FGameplayAbilitySpec();
    }
    if (!Spec) return;

    using GiveAbilityFunc = FGameplayAbilitySpecHandle(__fastcall*)(UAbilitySystemComponent*, FGameplayAbilitySpecHandle*, const FGameplayAbilitySpec&, FGameplayEventData*);
    static auto EmoteAbility = Cast<UGameplayAbility>(UGAB_Emote_Generic_C::GetDefaultObj());

    Spec->MostRecentArrayReplicationKey = -1;
    Spec->ReplicationID = -1;
    Spec->ReplicationKey = -1;
    Spec->GetHandle().Handle = (int32)(FRand() * 2147483647.0f);
    Spec->SetAbility(EmoteAbility);
    Spec->SetSourceObject(EmoteDef);
    Spec->SetInputID(-1);
    Spec->SetLevel(1);

    FGameplayAbilitySpecHandle Handle = Spec->GetHandle();
    if (Finder->GiveAbilityAndActivateOnce())
    {
        ((GiveAbilityFunc)Finder->GiveAbilityAndActivateOnce())(Cast<AFortPlayerStateAthena>(AI.Controller->GetPlayerState())->GetAbilitySystemComponent(), &Handle, *Spec, nullptr);
    }
}

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

void TickService::FortAthenaAIService::AircraftPhase(FortAthenaAI& AI, float CurrentTime)
{
    static float CalledAt = -1.0f;
    static bool bFirst = false;
    static AActor* Aircraft = nullptr;

    if (!bFirst) 
    { 
        CalledAt = CurrentTime; 
        bFirst = true; 
        auto Aircrafts = UWorld::GetWorld()->GetGameState()->GetAircrafts();
        if (Aircrafts.IsValidIndex(0)) Aircraft = Aircrafts[0];
        return; 
    }

    if (!AI.bThankedBusDriver && FRand() < 0.006f)
    {
        AI.bThankedBusDriver = true;
        AI.Controller->ThankBusDriver();
    }
    
    if ((CurrentTime - CalledAt) < 7.0f) return;

    if (!AI.bSkydiving)
    {
        if (FRand() < 0.003f || (CurrentTime - CalledAt) > 15.0f)
        {
            if (!Aircraft) return;
            AI.bSkydiving = true;
            AI.Pawn->K2_TeleportTo(Aircraft->K2_GetActorLocation(), FRotator(0,0,0));
            AI.Pawn->BeginSkydiving(true);
            FVector Target = DropZoneLocations[RandIndex((int)DropZoneLocations.size())];
            Target.X += UKismetMathLibrary::RandomFloatInRange(-800.f, 800.f);
            Target.Y += UKismetMathLibrary::RandomFloatInRange(-800.f, 800.f);
            
            AI.Target = Target;
        }
    }
}

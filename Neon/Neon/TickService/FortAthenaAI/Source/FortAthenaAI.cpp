#include "pch.h"
#include "../Header/FortAthenaAI.h"

#include "Engine/GameplayStatics/Header/GameplayStatics.h"
#include "FortniteGame/FortAthenaAIBotController/Header/FortAthenaAIBotController.h"
#include "FortniteGame/FortGameMode/Header/FortGameMode.h"

#include <random>
#include <algorithm>
#include <cmath>

#include "FortniteGame/FortLootPackage/Header/FortLootPackage.h"

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

static inline int RandIndex(int max)
{
    std::uniform_int_distribution<int> d(0, max - 1);
    return d(RNG);
}

vector<FortAthenaAI> TickService::FortAthenaAIService::Get()
{
    return AIs;
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

void TickService::FortAthenaAIService::RemoveFromService(AFortAthenaAIBotController* Controller)
{
    for (auto it = AIs.begin(); it != AIs.end();)
    {
        if (it->Controller == Controller)
        {
            it = AIs.erase(it);
        }
        else
        {
            ++it;
        }
    }
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
            AircraftPhase(AI, CurrentTime);
        } else if (GamePhase == EAthenaGamePhase::SafeZones)
        {
            SafeZonesPhase(AI, CurrentTime);
        }
        if (AI.bSkydiving && GameState->GetGamePhase() > EAthenaGamePhase::Aircraft || !AI.bThankedBusDriver && GameState->GetGamePhase() > EAthenaGamePhase::Aircraft)
        {
            if (!AI.bThankedBusDriver && GameState->GetGamePhase() > EAthenaGamePhase::Aircraft)
            {
                AI.bThankedBusDriver = true;
                AI.Controller->ThankBusDriver();
                AI.Pawn->BeginSkydiving(true);
            }
            FVector SafeTarget = AI.Target;
            SafeTarget.Z = max(SafeTarget.Z, 5000.0f); 
            AI.Pawn->K2_TeleportTo(SafeTarget, {});
            AI.bSkydiving = false;
        }
        
        if (AI.bSkydiving)
        {
            AI.Controller->GetBlackboard()->SetValueAsBool(UKismetStringLibrary::Conv_StringToName(L"AIEvaluator_Global_HasEverJumpedFromBusKey"), true);
            AI.Controller->GetBlackboard()->SetValueAsEnum(UKismetStringLibrary::Conv_StringToName(L"AIEvaluator_Loot_ExecutionStatus"), (uint8)EExecutionStatus::ExecutionDenied);
            AI.Controller->GetBlackboard()->SetValueAsEnum(UKismetStringLibrary::Conv_StringToName(L"AIEvaluator_Glide_ExecutionStatus"), (uint8)EExecutionStatus::ExecutionDenied);
            AI.Controller->GetBlackboard()->SetValueAsEnum(UKismetStringLibrary::Conv_StringToName(L"AIEvaluator_Dive_ExecutionStatus"), (uint8)EExecutionStatus::ExecutionAllowed);
            AI.Controller->GetBlackboard()->SetValueAsVector(UKismetStringLibrary::Conv_StringToName(L"AIEvaluator_JumpOffBus_Destination"), AI.Target);
            AI.Controller->GetBlackboard()->SetValueAsVector(UKismetStringLibrary::Conv_StringToName(L"AIEvaluator_Glide_Destination"), AI.Target);
            AI.Controller->GetBlackboard()->SetValueAsVector(UKismetStringLibrary::Conv_StringToName(L"AIEvaluator_Dive_Destination"), AI.Target);
        /*    const FVector BotPos = AI.Pawn->K2_GetActorLocation();
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
            }*/
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
    static auto EmoteAbility = Runtime::StaticFindObject<UGameplayAbility>("/Game/Abilities/Emotes/GAB_Emote_Generic.Default__GAB_Emote_Generic_C");

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

void TickService::FortAthenaAIService::SafeZonesPhase(FortAthenaAI& AI, float CurrentTime)
{
    static auto ChestClass = Runtime::StaticLoadObject<UClass>("/Game/Building/ActorBlueprints/Containers/Tiered_Chest_Athena.Tiered_Chest_Athena_C");
    static auto PickupClass = AFortPickupAthena::StaticClass();
    
    static TArray<AActor*> ChestArray;
    static TArray<AActor*> PickupArray;
    static int32 ChestSearchIndex = 0;
    static int32 PickupSearchIndex = 0;
    static int32 FrameCounter = 0;
    
    AActor* NearestChest = nullptr;
    AActor* NearestPickup = nullptr;
    float ChestDist = 999999.0f;
    float PickupDist = 999999.0f;
    
    if (FrameCounter % 300 == 0) 
    {
        PickupArray = UGameplayStatics::GetAllActorsOfClass(UWorld::GetWorld(), PickupClass);
        ChestArray = UGameplayStatics::GetAllActorsOfClass(UWorld::GetWorld(), ChestClass);
        ChestSearchIndex = 0;
        PickupSearchIndex = 0;
    }

    FrameCounter++;
    FVector BotPos = AI.Pawn->K2_GetActorLocation();

    int32 ChestBatch = 10;
    int32 PickupBatchSize = 15;
    int32 ChestCount = ChestArray.Num();
    int32 PickupCount = PickupArray.Num();

    if (FrameCounter % 10 == 0 && ChestCount > 0) 
    {
        for (int32 i = 0; i < ChestBatch; i++)
        {
            if (ChestSearchIndex >= ChestCount)
            {
                ChestSearchIndex = 0; 
                break;
            }

            AActor* Chest = ChestArray[ChestSearchIndex++];
            if (!Chest) continue;

            FVector ChestPos = Chest->K2_GetActorLocation();
            float dx = BotPos.X - ChestPos.X;
            float dy = BotPos.Y - ChestPos.Y;
            float Dist = sqrtf(dx * dx + dy * dy);

            if (Dist < 2500.0f && Dist < ChestDist)
            {
                ChestDist = Dist;
                NearestChest = Chest;
            }
        }
    }

    if (FrameCounter % 10 == 0 && PickupCount > 0) 
    {
        for (int32 i = 0; i < PickupBatchSize; i++)
        {
            if (PickupSearchIndex >= PickupCount)
            {
                PickupSearchIndex = 0;
                break;
            }

            AActor* PickupActor = PickupArray[PickupSearchIndex++];
            if (!PickupActor) continue;

            AFortPickupAthena* Pickup = (AFortPickupAthena*)PickupActor;
            if (!Pickup->GetPrimaryPickupItemEntry().GetItemDefinition() ||
                Pickup->GetPrimaryPickupItemEntry().GetItemDefinition()->IsA<UFortAmmoItemDefinition>())
                continue;

            if (Pickup->GetbPickedUp()) continue;

            FVector PickupPos = PickupActor->K2_GetActorLocation();
            float dx = BotPos.X - PickupPos.X;
            float dy = BotPos.Y - PickupPos.Y;
            float Dist = sqrtf(dx * dx + dy * dy);

            if (Dist < 2500.0f && Dist < PickupDist)
            {
                PickupDist = Dist;
                NearestPickup = PickupActor;
            }
        }
    }

    AActor* TargetLoot = nullptr;

    if (NearestPickup && NearestChest)
    {
        if (PickupDist < ChestDist)
        {
            TargetLoot = NearestPickup;
        }
        else
        {
            TargetLoot = NearestChest;
        }
    }
    else if (NearestPickup)
    {
        TargetLoot = NearestPickup;
    }
    else if (NearestChest)
    {
        TargetLoot = NearestChest;
    }

    if (AI.TargetLoot != TargetLoot)
    {
        AI.TargetLoot = TargetLoot;
        if (AI.Pawn->GetbStartedInteractSearch())
        {
            AI.Pawn->SetbStartedInteractSearch(false);
            AI.Pawn->OnRep_StartedInteractSearch();
        }
    }

    if (AI.TargetLoot)
    {
        FVector LootPos = AI.TargetLoot->K2_GetActorLocation();
        float dx = BotPos.X - LootPos.X;
        float dy = BotPos.Y - LootPos.Y;
        float Distance = sqrtf(dx * dx + dy * dy);

        FRotator LookRot = UKismetMathLibrary::FindLookAtRotation(BotPos, LootPos);
        AI.Controller->SetControlRotation(LookRot);
        AI.Pawn->K2_SetActorRotation(LookRot, true);

        if (Distance < 300.0f)
        {
            AI.Controller->StopMovement();

            if (AI.TargetLoot->IsA(AFortPickupAthena::StaticClass()))
            {
                AFortPickupAthena* Pickup = (AFortPickupAthena*)AI.TargetLoot;
                if (Pickup && !Pickup->GetbPickedUp())
                {
                    Pickup->GetPickupLocationData().bPlayPickupSound = true;
                    Pickup->GetPickupLocationData().FlyTime = 0.3f;
                    Pickup->GetPickupLocationData().ItemOwner = AI.Pawn;
                    Pickup->GetPickupLocationData().PickupGuid = Pickup->GetPrimaryPickupItemEntry().GetItemGuid();
                    Pickup->GetPickupLocationData().PickupTarget = AI.Pawn;
                    Pickup->OnRep_PickupLocationData();
                    Pickup->SetbPickedUp(true);
                    Pickup->OnRep_bPickedUp();
                    AI.TargetLoot = nullptr;
                }
            }
            else
            {
                if (!AI.Pawn->GetbStartedInteractSearch())
                {
                    AI.Pawn->SetbStartedInteractSearch(true);
                    AI.Pawn->OnRep_StartedInteractSearch();
                    AI.LastEmoteTime = CurrentTime;
                }
                else if ((CurrentTime - AI.LastEmoteTime) >= 1.5f)
                {
                    AI.TargetLoot = nullptr;
                    AI.Pawn->SetbStartedInteractSearch(false);
                    AI.Pawn->OnRep_StartedInteractSearch();
                }
            }
        }
        else
        {
            AI.Controller->K2_SetFocus(AI.TargetLoot);
            AI.Controller->MoveToActor(AI.TargetLoot, 50, true, false, true, nullptr, true);
            FVector Velocity = AI.Pawn->GetMovementComponent()->GetVelocity();
            float SpeedSquared = Velocity.X * Velocity.X + Velocity.Y * Velocity.Y + Velocity.Z * Velocity.Z;
            if (SpeedSquared < 25.0f) 
            {
                FVector ForwardDirection = AI.Pawn->GetActorForwardVector();
                ForwardDirection.Z = 0.0f;
                ForwardDirection = ForwardDirection.GetNormalized();
                AI.Pawn->AddMovementInput(ForwardDirection, 1.0f, true);
            }
        }
    }
}
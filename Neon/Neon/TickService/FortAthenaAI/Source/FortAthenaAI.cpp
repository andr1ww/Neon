#include "pch.h"
#include "../Header/FortAthenaAI.h"

#include "Engine/GameplayStatics/Header/GameplayStatics.h"
#include "FortniteGame/FortAthenaAIBotController/Header/FortAthenaAIBotController.h"
#include "FortniteGame/FortGameMode/Header/FortGameMode.h"
#include <random>

#include "FortniteGame/FortLootPackage/Header/FortLootPackage.h"

static std::vector<FortAthenaAI> AIs;
static std::vector<UAthenaDanceItemDefinition*> Emotes;
static AFortGameModeAthena* GameMode = nullptr;
static AFortGameStateAthena* GameState = nullptr;
static float LastUpdate = 0.0f;
static std::mt19937 RNG{5489u};

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

    for (auto& AI : AIs)
    {
        if (!AI.Controller || !AI.Pawn) continue;

        AI.LastFrame++;

        if (GamePhase == EAthenaGamePhase::Warmup)
        {
            if (AI.LastFrame % 10 == 0)
            {
                WarmupPhase(AI, CurrentTime);
            }
        }
        else if (GamePhase == EAthenaGamePhase::Aircraft)
        {
            if (AI.LastFrame % 5 == 0)
            {
                AircraftPhase(AI, CurrentTime);
            }
        } else if (GamePhase == EAthenaGamePhase::SafeZones)
        {
            SafeZonesPhase(AI, CurrentTime);
        }
        
        if (!AI.bLanded && GameState->GetGamePhase() > EAthenaGamePhase::Aircraft)
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
            AI.bLanded = true;
        }
        
        if (AI.bSkydiving)
        {
            static const FName BB_HasEverJumpedFromBusKey   = UKismetStringLibrary::Conv_StringToName(TEXT("AIEvaluator_Global_HasEverJumpedFromBusKey"));
            static const FName BB_LootExecutionStatus      = UKismetStringLibrary::Conv_StringToName(TEXT("AIEvaluator_Loot_ExecutionStatus"));
            static const FName BB_GlideExecutionStatus     = UKismetStringLibrary::Conv_StringToName(TEXT("AIEvaluator_Glide_ExecutionStatus"));
            static const FName BB_DiveExecutionStatus      = UKismetStringLibrary::Conv_StringToName(TEXT("AIEvaluator_Dive_ExecutionStatus"));
            static const FName BB_JumpOffBusDestination    = UKismetStringLibrary::Conv_StringToName(TEXT("AIEvaluator_JumpOffBus_Destination"));
            static const FName BB_GlideDestination         = UKismetStringLibrary::Conv_StringToName(TEXT("AIEvaluator_Glide_Destination"));
            static const FName BB_DiveDestination          = UKismetStringLibrary::Conv_StringToName(TEXT("AIEvaluator_Dive_Destination"));
            
            auto* BB = AI.Controller->GetBlackboard();

            if (!BB->GetValueAsBool(BB_HasEverJumpedFromBusKey))
                BB->SetValueAsBool(BB_HasEverJumpedFromBusKey, true);

            BB->SetValueAsEnum(BB_LootExecutionStatus, (uint8)EExecutionStatus::ExecutionDenied);
            BB->SetValueAsEnum(BB_GlideExecutionStatus, (uint8)EExecutionStatus::ExecutionDenied);
            BB->SetValueAsEnum(BB_DiveExecutionStatus, (uint8)EExecutionStatus::ExecutionAllowed);

            if (BB->GetValueAsVector(BB_JumpOffBusDestination) != AI.Target)
            {
                BB->SetValueAsVector(BB_JumpOffBusDestination, AI.Target);
                BB->SetValueAsVector(BB_GlideDestination, AI.Target);
                BB->SetValueAsVector(BB_DiveDestination, AI.Target);
            }
        }
    }
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
    static int LastTick = -1;
    static int JumpedThisTick = 0;

    int CurrentTick = static_cast<int>(CurrentTime);
    if (CurrentTick != LastTick)
    {
        LastTick = CurrentTick;
        JumpedThisTick = 0;
    }

    if (!bFirst) 
    { 
        CalledAt = CurrentTime; 
        bFirst = true; 
        auto Aircrafts = UWorld::GetWorld()->GetGameState()->GetAircrafts();
        if (Aircrafts.IsValidIndex(0)) Aircraft = Aircrafts[0];
        return; 
    }

    if (!AI.bThankedBusDriver)
    {
        std::mt19937 aiRng(reinterpret_cast<uintptr_t>(&AI));
        std::uniform_real_distribution<float> d(0.0f, 1.0f);
        if (d(aiRng) < 0.0005f)
        {
            AI.bThankedBusDriver = true;
            AI.Controller->ThankBusDriver();
        }
    }

    uintptr_t aiPtr = reinterpret_cast<uintptr_t>(&AI);
    float personalDelay = 7.0f + ((aiPtr % 1000) / 100.0f);

    if ((CurrentTime - CalledAt) < personalDelay) return;

    if (!AI.bSkydiving && JumpedThisTick < 2)
    {
        if (!Aircraft) return;
        
        std::mt19937 aiRng(aiPtr + static_cast<uint32_t>(CurrentTime * 100));
        std::uniform_real_distribution<float> jumpDist(0.0f, 1.0f);
        
        std::mt19937 timeRng(aiPtr + 12345);
        std::uniform_real_distribution<float> timeDist(15.0f, 22.0f);
        float personalJumpTime = CalledAt + timeDist(timeRng);
        
        bool shouldJump = jumpDist(aiRng) < 0.0005f || CurrentTime > personalJumpTime;
        
        if (shouldJump)
        {
            AI.bSkydiving = true;
            AI.Pawn->K2_TeleportTo(Aircraft->K2_GetActorLocation(), FRotator(0,0,0));
            AI.Pawn->BeginSkydiving(true);
            if (!AI.bThankedBusDriver)
            {
                AI.bThankedBusDriver = true;
                AI.Controller->ThankBusDriver();
            }
            std::uniform_int_distribution<int> zoneDist(0, (int)DropZoneLocations.size() - 1);
            FVector Target = DropZoneLocations[zoneDist(aiRng)];
            std::uniform_real_distribution<float> offsetDist(-800.f, 4000.f);
            Target.X += offsetDist(aiRng);
            Target.Y += offsetDist(aiRng);
            AI.Target = Target;
            JumpedThisTick++;
        }
    }
}

void TickService::FortAthenaAIService::SafeZonesPhase(FortAthenaAI& AI, float CurrentTime)
{
    static auto ChestClass = Runtime::StaticLoadObject<UClass>("/Game/Building/ActorBlueprints/Containers/Tiered_Chest_Athena.Tiered_Chest_Athena_C");
    static auto PickupClass = AFortPickupAthena::StaticClass();

    static TArray<AActor*> ChestArray;
    static TArray<AActor*> PickupArray;
    static bool bArraysPopulated = false;
    static int32 PickupIndex = 0;
    static int32 ChestIndex = 0;

    if (!bArraysPopulated)
    {
        bArraysPopulated = true;
        PickupArray = UGameplayStatics::GetAllActorsOfClass(UWorld::GetWorld(), PickupClass);
        ChestArray = UGameplayStatics::GetAllActorsOfClass(UWorld::GetWorld(), ChestClass);
    }

    FVector BotLocation = AI.Pawn->K2_GetActorLocation();
    float NearestDistanceSq = FLT_MAX;

    if (!AI.NearestEnemy)
    {
        if (GameMode) {
            for (auto Player : GameMode->GetAlivePlayers())
            {
                if (Player && Player->GetMyFortPawn() && Player->GetMyFortPawn() != AI.Pawn)
                {
                    AFortPlayerStateAthena* PlayerState = (AFortPlayerStateAthena*)Player->GetPlayerState();
                    AFortPlayerStateAthena* AIPlayerState = (AFortPlayerStateAthena*)AI.Pawn->GetPlayerState();
                    if (PlayerState && AIPlayerState)
                    {
                        if (PlayerState->GetTeamIndex() != AIPlayerState->GetTeamIndex())
                        {
                            float DistanceSq = UKismetMathLibrary::Vector_DistanceSquared(BotLocation, Player->GetMyFortPawn()->K2_GetActorLocation());
                            if (DistanceSq < NearestDistanceSq)
                            {
                                NearestDistanceSq = DistanceSq;
                                AI.NearestEnemy = Player->GetMyFortPawn();
                            }
                        }
                    }
                }
            }
        
            for (auto Bot : GameMode->GetAliveBots())
            {
                if (Bot && Bot->GetPawn() && Bot->GetPawn() != AI.Pawn)
                {
                    AFortPlayerStateAthena* PlayerState = (AFortPlayerStateAthena*)Bot->GetPlayerState();
                    AFortPlayerStateAthena* AIPlayerState = (AFortPlayerStateAthena*)AI.Pawn->GetPlayerState();
                    if (PlayerState && AIPlayerState)
                    {
                        if (PlayerState->GetTeamIndex() != AIPlayerState->GetTeamIndex())
                        {
                            float DistanceSq = UKismetMathLibrary::Vector_DistanceSquared(BotLocation, Bot->GetPawn()->K2_GetActorLocation());
                            if (DistanceSq < NearestDistanceSq)
                            {
                                NearestDistanceSq = DistanceSq;
                                AI.NearestEnemy = Bot->GetPawn();
                            }
                        }
                    }
                }
            }
        }
    }
    
    bool bHasWeapon = false;
    if (AI.Pawn && AI.Pawn->GetCurrentWeapon()) {
        auto* CurrentWeapon = AI.Pawn->GetCurrentWeapon();
        if (CurrentWeapon && CurrentWeapon->GetWeaponData()) {
            bHasWeapon = !CurrentWeapon->GetWeaponData()->IsA(UFortWeaponMeleeItemDefinition::StaticClass());
        }
    }

    if (GameState->GetSafeZoneIndicator() && !GameState->GetSafeZoneIndicator()->GetNextCenter().IsZero()) {
        float DistanceToZone = UKismetMathLibrary::Vector_Distance(BotLocation, GameState->GetSafeZoneIndicator()->GetNextCenter());
        bool bInSafeZone = DistanceToZone < GameState->GetSafeZoneIndicator()->GetNextRadius().Size();
            
        if (!bInSafeZone) {
            static const FName BB_StormExecutionStatus = UKismetStringLibrary::Conv_StringToName(TEXT("AIEvaluator_Storm_ExecutionStatus"));
            static const FName BB_StormDestination = UKismetStringLibrary::Conv_StringToName(TEXT("AIEvaluator_Storm_Destination"));
            static const FName BB_StormMovementState = UKismetStringLibrary::Conv_StringToName(TEXT("AIEvaluator_Storm_MovementState"));
                
            auto* BB = AI.Controller->GetBlackboard();
            if (BB) {
                BB->SetValueAsEnum(BB_StormExecutionStatus, (uint8)EExecutionStatus::ExecutionAllowed);
                BB->SetValueAsVector(BB_StormDestination, GameState->GetSafeZoneIndicator()->GetNextCenter());
                BB->SetValueAsEnum(BB_StormMovementState, (uint8)1); 
            }
                
            AI.TargetLoot = nullptr;
            AI.Moving = true;
            return;
        } else
        {
            AI.Moving = false;
        }
    }
    
    if (AI.NearestEnemy && (NearestDistanceSq < 64000000.0f || (AI.bUsingPickaxe && NearestDistanceSq < 16000000.0f)))
    {
        bool bHasLineOfSight = AI.Controller->LineOfSightTo(AI.NearestEnemy, BotLocation, true);

        if (bHasLineOfSight || AI.bUsingPickaxe)
        {
            FVector EnemyLocation = AI.NearestEnemy->K2_GetActorLocation();
            float Distance = UKismetMathLibrary::Vector_Distance(BotLocation, EnemyLocation);
            
            if (AI.bUsingPickaxe)
            {
                FRotator AimRotation = UKismetMathLibrary::FindLookAtRotation(BotLocation, EnemyLocation);
                AI.Controller->SetControlRotation(AimRotation);

                if (CurrentTime - AI.LastFireTime > 0.1f)
                {
                    AI.Pawn->PawnStartFire(0);
                    AI.LastFireTime = CurrentTime;
                }
                else if (UKismetMathLibrary::RandomFloat() < 0.1f)
                {
                    AI.Pawn->PawnStopFire(0);
                }
                return;
            }

            FVector EnemyVelocity = ((AFortPlayerPawn*)AI.NearestEnemy)->GetMovementComponent()->GetVelocity();
            float TimeToTarget = Distance / 30000.0f;
            FVector PredictedPosition = EnemyLocation + (EnemyVelocity * TimeToTarget);

            FVector RandomOffset = FVector(
                UKismetMathLibrary::RandomFloatInRange(-100.0f, 100.0f),
                UKismetMathLibrary::RandomFloatInRange(-100.0f, 100.0f),
                UKismetMathLibrary::RandomFloatInRange(-50.0f, 50.0f)
            );
            PredictedPosition += RandomOffset;

            FRotator AimRotation = UKismetMathLibrary::FindLookAtRotation(BotLocation, PredictedPosition);
            AI.Controller->SetControlRotation(AimRotation);

            if (Distance > 1500.0f)
            {
                AI.Controller->MoveToActor(AI.NearestEnemy, 0.0f, true, false, true, nullptr, true);
            }
            else if (Distance < 500.0f)
            {
                FVector BackwardDirection = (BotLocation - EnemyLocation).GetNormalized();
                AI.Pawn->AddMovementInput(BackwardDirection, 1.0f, true);

                if (!AI.bStrafeLeftInitialized)
                {
                    AI.bStrafeLeft = true;
                    AI.bStrafeLeftInitialized = true;
                }

                if (CurrentTime - AI.StrafeTime > 2.0f)
                {
                    AI.bStrafeLeft = !AI.bStrafeLeft;
                    AI.StrafeTime = CurrentTime;
                }

                FVector StrafeDirection = AI.Pawn->GetActorRightVector() * (AI.bStrafeLeft ? -1.0f : 1.0f);
                AI.Pawn->AddMovementInput(StrafeDirection, 0.7f, true);
            }
            else 
            {
                if (!AI.bStrafeLeftInitialized)
                {
                    AI.bStrafeLeft = true;
                    AI.bStrafeLeftInitialized = true;
                }

                if (CurrentTime - AI.StrafeTime > UKismetMathLibrary::RandomFloatInRange(1.5f, 3.0f))
                {
                    AI.bStrafeLeft = !AI.bStrafeLeft;
                    AI.StrafeTime = CurrentTime;
                }

                FVector StrafeDirection = AI.Pawn->GetActorRightVector() * (AI.bStrafeLeft ? -1.0f : 1.0f);
                AI.Pawn->AddMovementInput(StrafeDirection, 1.0f, true);

                FVector ForwardDirection = (EnemyLocation - BotLocation).GetNormalized();
                AI.Pawn->AddMovementInput(ForwardDirection, 0.5f, true);
            }

            if (CurrentTime - AI.LastFireTime > 0.1f)
            {
                AI.Pawn->PawnStartFire(0);
                AI.LastFireTime = CurrentTime;
            }
            else if (UKismetMathLibrary::RandomFloat() < 0.05f) 
            {
                AI.Pawn->PawnStopFire(0);
            }

            if (Distance > 1000.0f && CurrentTime - AI.CrouchTime > 5.0f && UKismetMathLibrary::RandomFloat() < 0.3f)
            {
                if (!AI.Pawn->GetbIsCrouched())
                    AI.Pawn->Crouch(false);
                else
                    AI.Pawn->UnCrouch(false);

                AI.CrouchTime = CurrentTime;
            }

            return;
        }
        else
        {
            float LostSightDistanceSq = UKismetMathLibrary::Vector_DistanceSquared(BotLocation, AI.NearestEnemy->K2_GetActorLocation());

            if (LostSightDistanceSq < 100000000.0f)
            {
                AI.Controller->MoveToActor(AI.NearestEnemy, 0.0f, true, false, true, nullptr, true);
                AI.CurrentFocus = AI.NearestEnemy;
                return;
            }
            else
            {
                AI.NearestEnemy = nullptr;
            }
        }
    }
    
    AActor* TargetLoot = AI.TargetLoot;

    if (bHasWeapon && AI.NearestEnemy || AI.Moving)
    {
        return;
    }
    
    if (TargetLoot && TargetLoot->IsValidLowLevel())
    {
        FVector TargetPos = TargetLoot->K2_GetActorLocation();
        float dx = BotLocation.X - TargetPos.X;
        float dy = BotLocation.Y - TargetPos.Y;
        float DistanceSq = dx * dx + dy * dy;

        if (DistanceSq > 9000000.0f)
        {
            AI.TargetLoot = nullptr;
        }
        else
        {
            FRotator LookRot = UKismetMathLibrary::FindLookAtRotation(BotLocation, TargetPos);
            LookRot.Pitch = 0.0f;
            AI.Controller->SetControlRotation(LookRot);

            if (DistanceSq < 90000.0f)
            {
                if (TargetLoot->IsA(AFortPickupAthena::StaticClass()))
                {
                    AFortPickupAthena* Pickup = static_cast<AFortPickupAthena*>(TargetLoot);
                    auto& PickupData = Pickup->GetPickupLocationData();
                    PickupData.bPlayPickupSound = true;
                    PickupData.FlyTime = 0.3f;
                    PickupData.ItemOwner = AI.Pawn;
                    PickupData.PickupGuid = Pickup->GetPrimaryPickupItemEntry().GetItemGuid();
                    PickupData.PickupTarget = AI.Pawn;
                    Pickup->OnRep_PickupLocationData();
                    Pickup->SetbPickedUp(true);
                    Pickup->OnRep_bPickedUp();
                    AI.TargetLoot = nullptr;

                    auto* Inventory = AI.Controller->GetInventory();
                    if (Inventory) {
                        const auto& Items = Inventory->GetInventory().GetItemInstances();
                        UFortWeaponItemDefinition* BestWeapon = nullptr;
                        FGuid BestWeaponGuid;
                        int32 HighestRarity = -1;
                        
                        for (const auto& Entry : Items) {
                            if (!Entry || !Entry->GetItemEntry().GetItemDefinition())
                                continue;
                                
                            auto* WeaponDef = Cast<UFortWeaponItemDefinition>(Entry->GetItemEntry().GetItemDefinition());
                            if (WeaponDef && !WeaponDef->IsA(UFortWeaponMeleeItemDefinition::StaticClass())) {
                                int32 Rarity = (int32)WeaponDef->GetRarity();
                                if (Rarity > HighestRarity) {
                                    HighestRarity = Rarity;
                                    BestWeapon = WeaponDef;
                                    BestWeaponGuid = Entry->GetItemEntry().GetItemGuid();
                                }
                            }
                        }
                        
                        if (BestWeapon) {
                            AI.Pawn->EquipWeaponDefinition(BestWeapon, BestWeaponGuid);
                        }
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
                if (AI.CurrentFocus != TargetLoot) {
                    AI.Controller->K2_SetFocus(TargetLoot);
                    AI.CurrentFocus = TargetLoot;
                }
                
               AI.Controller->MoveToActor(TargetLoot, 0.0f, true, false, true, nullptr, true);
                
                static int32 StuckCheckCounter = 0;
                static FVector LastBotPosition = FVector(0, 0, 0);
                
                StuckCheckCounter++;
                if (StuckCheckCounter % 30 == 0)
                {
                    FVector CurrentVelocity = AI.Pawn->GetMovementComponent()->GetVelocity();
                    float VelocityMagnitudeSq = CurrentVelocity.X * CurrentVelocity.X + CurrentVelocity.Y * CurrentVelocity.Y;
                    
                    if (VelocityMagnitudeSq < 625.0f)
                    {
                        FVector DirectionToTarget = TargetPos - BotLocation;
                        DirectionToTarget.Z = 0;
                        DirectionToTarget = DirectionToTarget.GetNormalized();
                        
                        AI.Pawn->AddMovementInput(DirectionToTarget, 1.0f, true);
                        
                        if (VelocityMagnitudeSq < 100.0f)
                        {
                            if (!AI.bUsingPickaxe)
                            {
                                auto* Inventory = AI.Controller->GetInventory();
                                if (Inventory)
                                {
                                    const auto& Items = Inventory->GetInventory().GetItemInstances();
                                    for (const auto& Entry : Items)
                                    {
                                        if (!Entry || !Entry->GetItemEntry().GetItemDefinition())
                                            continue;

                                        auto* ItemDef = Entry->GetItemEntry().GetItemDefinition();
                                        if (ItemDef && ItemDef->IsA<UAthenaPickaxeItemDefinition>())
                                        {
                                            UAthenaPickaxeItemDefinition* PickaxeDef = Cast<UAthenaPickaxeItemDefinition>(ItemDef);
                                            AI.Pawn->EquipWeaponDefinition(PickaxeDef->GetWeaponDefinition(), Entry->GetItemEntry().GetItemGuid());
                                            AI.bUsingPickaxe = true;
                                            break;
                                        }
                                    }
                                }
                            }

                            AI.Pawn->Jump();
                            AI.Pawn->PawnStartFire(0);
                        }
                        else
                        {
                            if (AI.bUsingPickaxe)
                            {
                                auto* Inventory = AI.Controller->GetInventory();
                                if (Inventory)
                                {
                                    const auto& Items = Inventory->GetInventory().GetItemInstances();
                                    UFortWeaponItemDefinition* BestWeapon = nullptr;
                                    FGuid BestWeaponGuid;
                                    int32 HighestRarity = -1;

                                    for (const auto& Entry : Items)
                                    {
                                        if (!Entry || !Entry->GetItemEntry().GetItemDefinition())
                                            continue;

                                        auto* WeaponDef = Cast<UFortWeaponItemDefinition>(Entry->GetItemEntry().GetItemDefinition());
                                        if (WeaponDef && !WeaponDef->IsA(UFortWeaponMeleeItemDefinition::StaticClass()) &&
                                            !WeaponDef->IsA(UAthenaPickaxeItemDefinition::StaticClass()))
                                        {
                                            int32 Rarity = (int32)WeaponDef->GetRarity();
                                            if (Rarity > HighestRarity)
                                            {
                                                HighestRarity = Rarity;
                                                BestWeapon = WeaponDef;
                                                BestWeaponGuid = Entry->GetItemEntry().GetItemGuid();
                                            }
                                        }
                                    }

                                    if (BestWeapon)
                                    {
                                        AI.Pawn->EquipWeaponDefinition(BestWeapon, BestWeaponGuid);
                                    }
                                }

                                AI.bUsingPickaxe = false; 
                            }
                        }
                        
                        FVector RandomOffset = FVector(
                            UKismetMathLibrary::RandomFloatInRange(-200.0f, 200.0f),
                            UKismetMathLibrary::RandomFloatInRange(-200.0f, 200.0f),
                            0.0f
                        );
                        FVector AlternateTarget = TargetPos + RandomOffset;
                        AI.Controller->MoveToLocation(AlternateTarget, 50.0f, true, false, true, true, nullptr, false);
                        
                        if (CurrentTime - AI.StuckCheckTime > 5.0f)
                        {
                            float DistanceMovedSq = UKismetMathLibrary::Vector_DistanceSquared(BotLocation, LastBotPosition);
                            if (DistanceMovedSq < 10000.0f)
                            {
                                FVector TeleportLocation = TargetPos + (DirectionToTarget * -500.0f);
                                AI.Pawn->K2_TeleportTo(TeleportLocation, {});
                                AI.TargetLoot = nullptr;
                            }
                            AI.StuckCheckTime = CurrentTime;
                            LastBotPosition = BotLocation;
                        }
                    }
                    else
                    {
                        if (!AI.NearestEnemy)
                        {
                            AI.Pawn->PawnStopFire(0);
                        }
                    }
                }
            }
            return;
        }
    }

    AActor* BestTarget = nullptr;
    float BestDistanceSq = 6250000.0f;
    if (AI.NearestEnemy && AI.NearestEnemy->IsValidLowLevel())
    {
        FVector EnemyLocation = AI.NearestEnemy->K2_GetActorLocation();
        float EnemyDistSq = UKismetMathLibrary::Vector_DistanceSquared(BotLocation, EnemyLocation);

        if (EnemyDistSq < BestDistanceSq)
        {
            AI.Controller->MoveToActor(AI.NearestEnemy, 200.0f, true, false, true, nullptr, true);
            AI.CurrentFocus = AI.NearestEnemy;
            return; 
        }
    }
    
    int32 PickupsPerFrame = 200;
    int32 ChestsPerFrame = 10;

    if (AI.LastFrame % 20 == 0)
    {
        for (int32 i = 0; i < PickupsPerFrame && PickupIndex < PickupArray.Num(); i++, PickupIndex++)
        {
            AActor* PickupActor = PickupArray[PickupIndex];
            if (!PickupActor || !PickupActor->IsValidLowLevel())
                continue;

            AFortPickupAthena* Pickup = static_cast<AFortPickupAthena*>(PickupActor);
            auto* ItemDef = Pickup->GetPrimaryPickupItemEntry().GetItemDefinition();
            if (!ItemDef || ItemDef->IsA<UFortAmmoItemDefinition>())
                continue;

            FVector PickupPos = PickupActor->K2_GetActorLocation();
            float dx = BotLocation.X - PickupPos.X;
            float dy = BotLocation.Y - PickupPos.Y;
            float distSq = dx * dx + dy * dy;
            if (distSq < BestDistanceSq)
            {
                BestDistanceSq = distSq;
                BestTarget = PickupActor;
            }
        }
        if (PickupIndex >= PickupArray.Num())
            PickupIndex = 0;

        if (!BestTarget)
        {
            for (int32 i = 0; i < ChestsPerFrame && ChestIndex < ChestArray.Num(); i++, ChestIndex++)
            {
                AActor* Chest = ChestArray[ChestIndex];
                if (!Chest || !Chest->IsValidLowLevel())
                    continue;

                FVector ChestPos = Chest->K2_GetActorLocation();
                float dx = BotLocation.X - ChestPos.X;
                float dy = BotLocation.Y - ChestPos.Y;
                float distSq = dx * dx + dy * dy;
                if (distSq < BestDistanceSq)
                {
                    BestDistanceSq = distSq;
                    BestTarget = Chest;
                }
            }
            if (ChestIndex >= ChestArray.Num())
                ChestIndex = 0;
        }

        AI.TargetLoot = BestTarget;
    }
}
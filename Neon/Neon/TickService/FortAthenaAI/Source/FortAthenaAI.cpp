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
        if (FRand() < 0.0003f || (CurrentTime - CalledAt) > 15.0f)
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
    static bool bArraysPopulated = false;
    static int32 PickupIndex = 0;
    static int32 ChestIndex = 0;

    if (!bArraysPopulated)
    {
        bArraysPopulated = true;
        PickupArray = UGameplayStatics::GetAllActorsOfClass(UWorld::GetWorld(), PickupClass);
        ChestArray = UGameplayStatics::GetAllActorsOfClass(UWorld::GetWorld(), ChestClass);
    }

    AActor* TargetLoot = AI.TargetLoot;
    FVector BotPos = AI.Pawn->K2_GetActorLocation();

    if (TargetLoot && TargetLoot->IsValidLowLevel())
    {
        FVector TargetPos = TargetLoot->K2_GetActorLocation();
        float dx = BotPos.X - TargetPos.X;
        float dy = BotPos.Y - TargetPos.Y;
        float DistanceSq = dx * dx + dy * dy;

        if (DistanceSq > 9000000.0f)
        {
            AI.TargetLoot = nullptr;
        }
        else
        {
            FRotator LookRot = UKismetMathLibrary::FindLookAtRotation(BotPos, TargetPos);
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

                    auto* CurrentWeapon = AI.Pawn->GetCurrentWeapon();
                    if (!CurrentWeapon || !CurrentWeapon->GetWeaponData() ||
                        !CurrentWeapon->GetWeaponData()->IsA(UFortWeaponMeleeItemDefinition::StaticClass()))
                        return;

                    auto* Inventory = AI.Controller->GetInventory();
                    if (!Inventory)
                        return;

                    const auto& Items = Inventory->GetInventory().GetItemInstances();
                    for (const auto& Entry : Items)
                    {
                        if (!Entry || !Entry->GetItemEntry().GetItemDefinition())
                            continue;

                        std::string ItemName = Entry->GetItemEntry().GetItemDefinition()->GetFName().ToString().ToString();
                        if (ItemName.contains(("Shotgun")) || ItemName.contains(("SMG")) || 
                            ItemName.contains(("Assault")) || ItemName.contains(("Sniper")) || 
                            ItemName.contains(("Rocket")) || ItemName.contains(("Pistol")) ||
                            ItemName.contains(("Grenade")))
                        {
                            auto* WeaponDef = Cast<UFortWeaponItemDefinition>(Entry->GetItemEntry().GetItemDefinition());
                            if (WeaponDef)
                            {
                                AI.Pawn->EquipWeaponDefinition(WeaponDef, Entry->GetItemEntry().GetItemGuid());
                                break;
                            }
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
                StuckCheckCounter++;
                if (StuckCheckCounter % 30 == 0)
                {
                    FVector Velocity = AI.Pawn->GetMovementComponent()->GetVelocity();
                    if ((Velocity.X * Velocity.X + Velocity.Y * Velocity.Y) < 625.0f)
                    {
                        FVector Forward = TargetPos - BotPos;
                        Forward.Z = 0;
                        Forward.Normalize();
                        AI.Pawn->AddMovementInput(Forward, 1.0f, false);
                    }
                }
            }
            return;
        }
    }

    AActor* BestTarget = nullptr;
    float BestDistanceSq = 6250000.0f;

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
            float dx = BotPos.X - PickupPos.X;
            float dy = BotPos.Y - PickupPos.Y;
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
                float dx = BotPos.X - ChestPos.X;
                float dy = BotPos.Y - ChestPos.Y;
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

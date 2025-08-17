#include "pch.h"
#include "../Header/FortAthenaAI.h"

#include "Engine/GameplayStatics/Header/GameplayStatics.h"
#include "FortniteGame/FortAthenaAIBotController/Header/FortAthenaAIBotController.h"
#include "FortniteGame/FortGameMode/Header/FortGameMode.h"

static std::vector<FortAthenaAI> AIs;
static vector<UAthenaDanceItemDefinition*> Emotes{};

void TickService::FortAthenaAIService::AddToService(AFortAthenaAIBotController* Controller, AFortPlayerPawn* Pawn)
{
    if (Emotes.size() == 0)
    {
        Emotes = Runtime::GetObjectsOfClass<UAthenaDanceItemDefinition>();
    }
    
    FortAthenaAI AI;

    AI.Controller = Controller;
    AI.Pawn = Pawn;
    
    AIs.push_back(AI);
}

void TickService::FortAthenaAIService::Tick()
{
    AFortGameModeAthena* GameMode = UWorld::GetWorld()->GetAuthorityGameMode();
    AFortGameStateAthena* GameState = GameMode->GetGameState();

    for (auto& AI : AIs)
    {
        if (!AI.Controller || !AI.Pawn)
            continue;

        switch (GameState->GetGamePhase())
        {
        case EAthenaGamePhase::Warmup:
            {
                WarmupPhase(AI);
            }
        }
    }
}

void TickService::FortAthenaAIService::WarmupPhase(FortAthenaAI& AI)
{
    float CurrentTime = UGameplayStatics::GetTimeSeconds(UWorld::GetWorld());
    
    if (AI.Emoting && (CurrentTime - AI.LastEmoteTime) < 10.0f)
        return;

    if (AI.Emoting && (CurrentTime - AI.LastEmoteTime) > 15.0f)
    {
        AI.Emoting = false;
    }
    
    auto EmoteDef = Emotes[rand() % Emotes.size()];
    if (!EmoteDef || EmoteDef->IsA<UAthenaEmojiItemDefinition>())
        return;
    AI.Emoting = true;
    AI.LastEmoteTime = CurrentTime;
    
    static FGameplayAbilitySpec* Spec;
    if (!Spec)
    {
        int32 GameplayAbilitySpecSize = StaticClassImpl("GameplayAbilitySpec")->GetSize();
        Spec = (FGameplayAbilitySpec*)VirtualAlloc(0, GameplayAbilitySpecSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    
        if (!Spec) return;
    
        new(Spec) FGameplayAbilitySpec();
    }
    
    using GiveAbilityFunc = FGameplayAbilitySpecHandle(__fastcall*)(
        UAbilitySystemComponent*,
        FGameplayAbilitySpecHandle*, 
        const FGameplayAbilitySpec&,
        FGameplayEventData*
    );

    static auto EmoteAbility = Cast<UGameplayAbility>(UGAB_Emote_Generic_C::GetDefaultObj());
    
    Spec->MostRecentArrayReplicationKey = -1;
    Spec->ReplicationID = -1;
    Spec->ReplicationKey = -1;
    Spec->GetHandle().Handle = rand();
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


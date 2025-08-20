#include "pch.h"
#include "../Header/AbilitySystemComponent.h"

#include "Neon/Finder/Header/Finder.h"

void UAbilitySystemComponent::InternalServerTryActivateAbility(
    UAbilitySystemComponent* AbilitySystemComponent, 
    FGameplayAbilitySpecHandle Handle, 
    bool InputPressed, 
    void* PredictionKey, 
    FGameplayEventData* TriggerEventData)
{
    FGameplayAbilitySpec* Spec = nullptr;
    auto& ActivatableAbilities = AbilitySystemComponent->GetActivatableAbilities();
    auto& Items = ActivatableAbilities.GetItems();
    static int32 GameplayAbilitySpecSize = StaticClassImpl("GameplayAbilitySpec")->GetSize();
    
    for (int i = 0; i < Items.Num(); i++) {
        auto& CurrentSpec = *reinterpret_cast<FGameplayAbilitySpec*>(__int64(Items.GetData()) + (i * GameplayAbilitySpecSize));
        
        if (CurrentSpec.GetHandle().Handle == Handle.Handle)
        {
            Spec = &CurrentSpec;
            break; 
        }
    }

    if (!Spec)
    {
        UE_LOG(LogNeon, Log, "AbilitySystemComponent::InternalServerTryActivateAbility: Spec not found for Handle: %d", Handle.Handle);
        return;
    }

    Spec->SetInputPressed(InputPressed);

    UGameplayAbility* InstancedAbility = nullptr;
    
    using TryActivateFunc = bool (*)(
        UAbilitySystemComponent*, 
        FGameplayAbilitySpecHandle, 
        void*, 
        UGameplayAbility**, 
        void*, 
        const FGameplayEventData*
    );
    
    auto InternalTryActivate = reinterpret_cast<TryActivateFunc>(Finder->InternalTryActivateAbility());
    if (!InternalTryActivate(AbilitySystemComponent, Handle, PredictionKey, &InstancedAbility, nullptr, TriggerEventData))
    {
        Spec->SetInputPressed(false);
    }
    
    AbilitySystemComponent->GetActivatableAbilities().MarkItemDirty(*Spec);
}

void UAbilitySystemComponent::GiveAbility(UAbilitySystemComponent* AbilitySystemComponent, UClass* Ability)
{
    if (!Ability)
        return;

    static std::map<UClass*, UGameplayAbility*> CachedCDOs;
    UGameplayAbility* CachedAbilityCDO = nullptr;

    auto It = CachedCDOs.find(Ability);
    if (It != CachedCDOs.end())
    {
        CachedAbilityCDO = It->second;
    }
    else
    {
        CachedAbilityCDO = Cast<UGameplayAbility>(Ability->GetClassDefaultObject());
        if (!CachedAbilityCDO)
            return;
        CachedCDOs[Ability] = CachedAbilityCDO;
    }

    static FGameplayAbilitySpec* StaticSpec = nullptr;
    static bool bInitialized = false;

    if (!bInitialized)
    {
        int32 GameplayAbilitySpecSize = StaticClassImpl("GameplayAbilitySpec")->GetSize();
        StaticSpec = (FGameplayAbilitySpec*)malloc(GameplayAbilitySpecSize);
        if (!StaticSpec)
            return;
        memset(StaticSpec, 0, GameplayAbilitySpecSize);
        new (StaticSpec) FGameplayAbilitySpec();
        bInitialized = true;
    }

    if (!StaticSpec)
        return;

    using GiveAbilityFunc = FGameplayAbilitySpecHandle(__fastcall)(
        UAbilitySystemComponent*,
        FGameplayAbilitySpecHandle*,
        const FGameplayAbilitySpec&
    );

    StaticSpec->MostRecentArrayReplicationKey = -1;
    StaticSpec->ReplicationID = -1;
    StaticSpec->ReplicationKey = -1;
    StaticSpec->GetHandle().Handle = rand();
    StaticSpec->SetAbility(CachedAbilityCDO);
    StaticSpec->SetSourceObject(nullptr);
    StaticSpec->SetInputID(-1);
    StaticSpec->SetLevel(1);

    FGameplayAbilitySpecHandle Handle = StaticSpec->GetHandle();

    if (Finder->GiveAbility())
    {
        ((GiveAbilityFunc*)Finder->GiveAbility())(AbilitySystemComponent, &Handle, *StaticSpec);
    }
}

void UAbilitySystemComponent::GiveAbilitySet(UAbilitySystemComponent* AbilitySystemComponent, UFortAbilitySet* Set)
{
    if (Set)
    {
        for (int i = 0; i < Set->GetGameplayAbilities().Num(); i++) {
            if (Set->GetGameplayAbilities().IsValidIndex(i))
            {
                UClass* Class = Set->GetGameplayAbilities()[i];
                if (!Class) {
                    continue;
                }
                GiveAbility(AbilitySystemComponent, Class);
            }
        }

        for (int i = 0; i < Set->GetPassiveGameplayEffects().Num(); i++) {
            if (Set->GetPassiveGameplayEffects().IsValidIndex(i))
            {
                FGameplayEffectApplicationInfo& EffectInfo = Set->GetPassiveGameplayEffects()[i];
                if (!EffectInfo.GetGameplayEffect()) {
                    continue;
                }
                FGameplayEffectContextHandle EffectContext{};
                AbilitySystemComponent->BP_ApplyGameplayEffectToSelf(EffectInfo.GetGameplayEffect(), EffectInfo.Level, EffectContext);
            }
        }
    }
}

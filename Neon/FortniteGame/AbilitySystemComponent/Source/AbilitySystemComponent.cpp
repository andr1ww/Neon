#include "pch.h"
#include "../Header/AbilitySystemComponent.h"

#include "Neon/Finder/Header/Finder.h"

void UAbilitySystemComponent::InternalServerTryActivateAbility(
    UAbilitySystemComponent* AbilitySystemComponent, 
    FGameplayAbilitySpecHandle Handle, 
    bool InputPressed, 
    FPredictionKey& PredictionKey, 
    FGameplayEventData* TriggerEventData)
{
    UE_LOG(LogNeon, Log, "InternalServerTryActivateAbility");
    FGameplayAbilitySpec* Spec = nullptr;
    for (int i = 0; i < AbilitySystemComponent->GetActivatableAbilities().GetItems().Num(); i++) {
        FGameplayAbilitySpec& CurrentSpec = AbilitySystemComponent->GetActivatableAbilities().GetItems()[i];
        if (CurrentSpec.GetHandle().Handle == Handle.Handle)
        {
            Spec = &CurrentSpec;
            break; 
        }
    }

    if (!Spec)
    {
        GetDefaultObj()->CallFunc<void>("AbilitySystemComponent", "ClientActivateAbilityFailed", Handle, PredictionKey.GetCurrent());
        return;
    }

    Spec->SetInputPressed(InputPressed);

    UGameplayAbility* InstancedAbility = nullptr;

    using TryActivateFunc = bool (*)(
        UAbilitySystemComponent*, 
        FGameplayAbilitySpecHandle, 
        FPredictionKey, 
        UGameplayAbility**, 
        void*, 
        const FGameplayEventData*
    );

   auto InternalTryActivate = reinterpret_cast<TryActivateFunc>(Finder->InternalTryActivateAbility());
    if (!InternalTryActivate(AbilitySystemComponent, Handle, PredictionKey, &InstancedAbility, nullptr, TriggerEventData))
    {
        GetDefaultObj()->CallFunc<void>("AbilitySystemComponent", "ClientActivateAbilityFailed", Handle, PredictionKey.GetCurrent());
        Spec->SetInputPressed(false);
    }
    
    AbilitySystemComponent->GetActivatableAbilities().MarkItemDirty(*Spec); 
}

void UAbilitySystemComponent::GiveAbility(UAbilitySystemComponent* AbilitySystemComponent, UClass* Ability)
{
    if (!Ability || !Ability->GetClassDefaultObject()) return;
    FGameplayAbilitySpec Spec{};
    
    using ConstructSpecFunc = void (*)(
        FGameplayAbilitySpec*,
        UGameplayAbility*,
        int,
        int,
        UObject*
        );

    
    using GiveAbilityFunc = FGameplayAbilitySpecHandle * (__fastcall*)(
        UAbilitySystemComponent*,
        FGameplayAbilitySpecHandle*,
        FGameplayAbilitySpec
        );


    if (Finder->ConstructSpec())
    {
        ((ConstructSpecFunc)Finder->ConstructSpec())(&Spec, (UGameplayAbility*)Ability->GetClassDefaultObject(), 1, -1, nullptr);
    } 
    if (Finder->GiveAbility())
    {
        ((GiveAbilityFunc)uint64_t(Finder->GiveAbility()))(AbilitySystemComponent, &Spec.GetHandle(), Spec);
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
                UE_LOG(LogNeon, Log, "Ability[%d]: %s", i, Class->GetFName().ToString().ToString().c_str());
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
                UE_LOG(LogNeon, Log, "Passive Effect[%d]: %s", i, EffectInfo.GetGameplayEffect()->GetFName().ToString().ToString().c_str());
                FGameplayEffectContextHandle EffectContext{}; 
                AbilitySystemComponent->CallFunc<void>("AbilitySystemComponent", "BP_ApplyGameplayEffectToSelf", EffectInfo.GetGameplayEffect(), EffectInfo.Level, EffectContext);
            }
        }
    }
}

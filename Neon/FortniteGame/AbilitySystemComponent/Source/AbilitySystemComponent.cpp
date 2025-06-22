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
    FGameplayAbilitySpec* Spec;
    for (int i = 0; i < AbilitySystemComponent->GetActivatableAbilities().GetItems().Num(); i++) {
        FGameplayAbilitySpec& CurrentSpec = AbilitySystemComponent->GetActivatableAbilities().GetItems()[i];
        if (CurrentSpec.GetHandle().Handle == Handle.Handle)
        {
            Spec = &CurrentSpec;
        }
    }

    if (!Spec)
    {
        return GetDefaultObj()->CallFunc<void>("AbilitySystemComponent", "ClientActivateAbilityFailed", Handle, PredictionKey.GetCurrent());
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

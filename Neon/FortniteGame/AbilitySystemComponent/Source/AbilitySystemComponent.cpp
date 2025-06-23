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
    FGameplayAbilitySpec* Spec = nullptr;
    for (int i = 0; i < AbilitySystemComponent->GetActivatableAbilities().GetItems().Num(); i++) {
        FGameplayAbilitySpec& CurrentSpec = AbilitySystemComponent->GetActivatableAbilities().GetItems()[i];
        if (CurrentSpec.GetHandle().Handle == Handle.Handle)
        {
            Spec = &CurrentSpec;
            break; 
        }
    }

    struct AbilitySystemComponent_ClientActivateAbilityFailed final
{
public:
	struct FGameplayAbilitySpecHandle             AbilityToActivate;                                 // 0x0000(0x0004)(Parm, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	int16                                         PredictionKey;                                     // 0x0004(0x0002)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	uint8                                         Pad_6[0x2];                                        // 0x0006(0x0002)(Fixing Struct Size After Last Property [ Dumper-7 ])
    } Params{
        Handle, 
		PredictionKey.GetCurrent()
    };

    if (!Spec)
    {
        GetDefaultObj()->Call(L"ClientActivateAbilityFailed", &Params);
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
        GetDefaultObj()->Call(L"ClientActivateAbilityFailed", &Params);
        uint8 bInputPressed = false;
        Spec->SetInputPressed(bInputPressed);
    }
    
    AbilitySystemComponent->GetActivatableAbilities().MarkItemDirty(*Spec);
}

void UAbilitySystemComponent::GiveAbility(UAbilitySystemComponent* AbilitySystemComponent, UClass* Ability)
{
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

    ((ConstructSpecFunc)uint64_t(Finder->ConstructSpec()))(&Spec, (UGameplayAbility*)Ability->GetClassDefaultObject(), 1, -1, nullptr);
    FGameplayAbilitySpecHandle SpecHandle = Spec.GetHandle();
    ((GiveAbilityFunc)uint64_t(Finder->GiveAbility()))(AbilitySystemComponent, &SpecHandle, Spec);
}

void UAbilitySystemComponent::GiveAbilitySet(UAbilitySystemComponent* AbilitySystemComponent, UFortAbilitySet* Set)
{
    if (Set)
    {
        for (int i = 0; i < Set->GetGameplayAbilities().Num(); i++) {
            UClass* Ability = Set->GetGameplayAbilities()[i];
            if (Ability)
            {
                GiveAbility(AbilitySystemComponent, Ability);
            }
        }
    }
}

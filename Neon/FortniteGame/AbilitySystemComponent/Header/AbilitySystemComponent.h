#pragma once
#include "pch.h"

#include "Engine/Array/Header/Array.h"
#include "Neon/Runtime/Runtime.h"

using FGameplayEventData = char[0xB0]; 

class UGameplayAbility : public UObject
{
    
};
struct FPredictionKey final
{
public:
    DEFINE_MEMBER(int16_t, FPredictionKey, Current)  
};

struct FGameplayAbilitySpecHandle
{
   int Handle;
};

struct FGameplayAbilitySpec : public FFastArraySerializerItem
{
    DEFINE_MEMBER(FGameplayAbilitySpecHandle, FGameplayAbilitySpec, Handle);
    DEFINE_MEMBER(uint8, FGameplayAbilitySpec, InputPressed);
    DEFINE_MEMBER(UGameplayAbility, FGameplayAbilitySpec, Ability);
};

struct FGameplayAbilitySpecContainer : public FFastArraySerializer
{
    DEFINE_MEMBER(TArray<FGameplayAbilitySpec>, FGameplayAbilitySpecContainer, Items);
};

struct FGameplayEffectContextHandle
{
    unsigned char                                      UnknownData00[0x18];                                      
};

struct FGameplayEffectApplicationInfo final
{
public:
    DEFINE_PTR(UClass, FGameplayEffectApplicationInfo, GameplayEffect);
    float                                         Level;                                             // 0x0020(0x0004)(Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    uint8                                         Pad_24[0x4];                                       // 0x0024(0x0004)(Fixing Struct Size After Last Property [ Dumper-7 ])
};

class UFortAbilitySet : public UObject
{
public:
    DEFINE_MEMBER(TArray<TSubclassOf<UClass>>, UFortAbilitySet, GameplayAbilities);
    DEFINE_MEMBER(TArray<FGameplayEffectApplicationInfo>, UFortAbilitySet, PassiveGameplayEffects);
};

class UAbilitySystemComponent : public UObject
{
public:
    DEFINE_MEMBER(FGameplayAbilitySpecContainer, UAbilitySystemComponent, ActivatableAbilities);
public:
    static void InternalServerTryActivateAbility(UAbilitySystemComponent*, FGameplayAbilitySpecHandle, bool, FPredictionKey&, FGameplayEventData*);
    static void GiveAbility(UAbilitySystemComponent* AbilitySystemComponent, UClass* Ability);
    static void GiveAbilitySet(UAbilitySystemComponent* AbilitySystemComponent, UFortAbilitySet* Set);
public:
    DECLARE_STATIC_CLASS(UAbilitySystemComponent);
    DECLARE_DEFAULT_OBJECT(UAbilitySystemComponent);
};

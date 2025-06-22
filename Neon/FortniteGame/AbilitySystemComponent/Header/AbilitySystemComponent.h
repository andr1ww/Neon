#pragma once
#include "pch.h"

#include "Engine/Array/Header/Array.h"

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
};

struct FGameplayAbilitySpecContainer : public FFastArraySerializer
{
    DEFINE_MEMBER(TArray<FGameplayAbilitySpec>, FGameplayAbilitySpecContainer, Items);
};

class UFortAbilitySet : public UObject
{
public:
    DEFINE_MEMBER(TArray<TSubclassOf<class UGameplayAbility>>, UFortAbilitySet, GameplayAbilities);
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

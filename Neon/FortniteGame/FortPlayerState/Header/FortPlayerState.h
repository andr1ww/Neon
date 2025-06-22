#pragma once
#include "pch.h"

#include "FortniteGame/AbilitySystemComponent/Header/AbilitySystemComponent.h"

class UFortHeroType : public UObject
{
    
};

class AFortPlayerStateAthena : public UObject
{
public:
    DEFINE_MEMBER(UFortHeroType, AFortPlayerStateAthena, HeroType);
    DEFINE_MEMBER(UAbilitySystemComponent*, AFortPlayerStateAthena, AbilitySystemComponent);
};
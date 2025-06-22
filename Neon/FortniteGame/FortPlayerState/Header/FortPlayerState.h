#pragma once
#include "pch.h"

class UFortHeroType : public UObject
{
    
};

class AFortPlayerStateAthena : public UObject
{
public:
    DEFINE_MEMBER(UFortHeroType, AFortPlayerStateAthena, HeroType);
};
#pragma once
#include "pch.h"

#include "FortniteGame/AbilitySystemComponent/Header/AbilitySystemComponent.h"
#include "Engine/ObjectPtr/Header/ObjectPtr.h"

enum class EFortCustomPartType : uint8
{
    Head                                     = 0,
    Body                                     = 1,
    Hat                                      = 2,
    Backpack                                 = 3,
    Charm                                    = 4,
    Face                                     = 5,
    NumTypes                                 = 6,
    EFortCustomPartType_MAX                  = 7,
};

class UCustomCharacterPart : public UObject
{
public:
    DEFINE_MEMBER(EFortCustomPartType, UCustomCharacterPart, CharacterPartType)
};

class UFortHeroSpecialization : public UObject
{
public:
    DEFINE_MEMBER(TArray<TSoftObjectPtr<UCustomCharacterPart>>, UFortHeroSpecialization, CharacterParts);
};

class UFortHeroType : public UObject
{
public:
    DEFINE_MEMBER(TArray<TSoftObjectPtr<UFortHeroSpecialization>>, UFortHeroType, Specializations);
};

class AFortPlayerStateZone : public UObject
{
    
};

class AFortPlayerStateAthena : public AFortPlayerStateZone
{
public:
    DEFINE_PTR(UFortHeroType, AFortPlayerStateAthena, HeroType);
    DEFINE_PTR(UAbilitySystemComponent, AFortPlayerStateAthena, AbilitySystemComponent);
};
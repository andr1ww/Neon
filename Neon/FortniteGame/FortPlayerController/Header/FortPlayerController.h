#pragma once
#include "pch.h"

#include "Neon/Runtime/Runtime.h"
#include "Engine/PlayerController/Header/PlayerController.h"
#include "FortniteGame/FortPlayerState/Header/FortPlayerState.h"

class UAthenaCharacterItemDefinition : public UObject
{
public:
    DEFINE_MEMBER(UFortHeroType, UAthenaCharacterItemDefinition, HeroDefinition);
};
    
struct FFortAthenaLoadout final
{
public:
    DEFINE_MEMBER(UAthenaCharacterItemDefinition, FFortAthenaLoadout, Character);
};

class AFortPlayerControllerAthena : public APlayerController
{
public:
    DEFINE_MEMBER(AFortPlayerStateAthena, AFortPlayerControllerAthena, PlayerState);
    DEFINE_MEMBER(FFortAthenaLoadout, AFortPlayerControllerAthena, CosmeticLoadoutPC);
public:
    static void ServerAcknowledgePossession(AFortPlayerControllerAthena* PlayerController, FFrame& Stack);
};

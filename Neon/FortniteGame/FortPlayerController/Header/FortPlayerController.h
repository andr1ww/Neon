#pragma once
#include "pch.h"
#include "Neon/Runtime/Runtime.h"
#include "Engine/PlayerController/Header/PlayerController.h"
#include "FortniteGame/FortInventory/Header/FortInventory.h"
#include "FortniteGame/FortPlayerState/Header/FortPlayerState.h"

class AFortInventory;

class UAthenaCharacterItemDefinition : public UObject 
{
public:
    DEFINE_MEMBER(UFortHeroType, UAthenaCharacterItemDefinition, HeroDefinition);
};

class UFortWeaponMeleeItemDefinition : public UFortItemDefinition
{
    
};

class UAthenaPickaxeItemDefinition : public UObject 
{
public:
    DEFINE_PTR(UFortWeaponMeleeItemDefinition, UAthenaPickaxeItemDefinition, WeaponDefinition);
};

struct FFortAthenaLoadout final 
{
public:
    DEFINE_PTR(UAthenaCharacterItemDefinition, FFortAthenaLoadout, Character);
    DEFINE_PTR(UAthenaPickaxeItemDefinition, FFortAthenaLoadout, Pickaxe);
}; 

class AFortPlayerControllerAthena : public APlayerController 
{
public:
    DEFINE_PTR(AFortPlayerStateAthena, AFortPlayerControllerAthena, PlayerState);
    DEFINE_MEMBER(FFortAthenaLoadout, AFortPlayerControllerAthena, CosmeticLoadoutPC);
    DEFINE_PTR(AFortInventory, AFortPlayerControllerAthena, WorldInventory);
    DEFINE_BOOL(AFortPlayerControllerAthena, bHasServerFinishedLoading);
public:
    static void ServerAcknowledgePossession(AFortPlayerControllerAthena* PlayerController, FFrame& Stack);
};
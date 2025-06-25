#pragma once
#include "pch.h"
#include "Neon/Runtime/Runtime.h"
#include "Engine/PlayerController/Header/PlayerController.h"
#include "FortniteGame/FortInventory/Header/FortInventory.h"
#include "FortniteGame/FortPlayerPawn/Header/FortPlayerPawn.h"
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

class AFortPlayerController : public APlayerController
{
public:
    DEFINE_PTR(AFortInventory, AFortPlayerController, WorldInventory);
    DEFINE_MEMBER(FFortAthenaLoadout, AFortPlayerController, CosmeticLoadoutPC);
    DEFINE_PTR(AFortPlayerPawn, AFortPlayerController, MyFortPawn);
};

class AFortPlayerControllerAthena : public AFortPlayerController 
{
public:
    DEFINE_PTR(AFortPlayerStateAthena, AFortPlayerControllerAthena, PlayerState);
    DEFINE_BOOL(AFortPlayerControllerAthena, bHasServerFinishedLoading);
public:
    static void ServerAcknowledgePossession(AFortPlayerControllerAthena* PlayerController, FFrame& Stack);
    static void ServerExecuteInventoryItem(AFortPlayerControllerAthena* PlayerController, FFrame& Stack);
};
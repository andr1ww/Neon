#pragma once
#include "pch.h"
#include "Neon/Runtime/Runtime.h"
#include "Engine/PlayerController/Header/PlayerController.h"
#include "FortniteGame/FortInventory/Header/FortInventory.h"
#include "FortniteGame/FortPlayerPawn/Header/FortPlayerPawn.h"
#include "FortniteGame/FortPlayerState/Header/FortPlayerState.h"
#include "FortniteGame/Common/Header/ItemDefinition.h"

static TArray<UAthenaCharacterItemDefinition*> Characters = Runtime::GetObjectsOfClass<UAthenaCharacterItemDefinition>();
static TArray<UAthenaPickaxeItemDefinition*> Pickaxes = Runtime::GetObjectsOfClass<UAthenaPickaxeItemDefinition>();

class AAIController : public AController
{

};

class AFortAthenaAIBotController : public AAIController
{
public:
    DEFINE_PTR(AFortInventory, AFortAthenaAIBotController, Inventory);
    DEFINE_PTR(UFortAthenaAIBotInventoryItems, AFortAthenaAIBotController, StartupInventory);

public:
    // Stuff that doesent exist in the fortnite sdk
    static void SpawnPlayerBot(int Count);
};
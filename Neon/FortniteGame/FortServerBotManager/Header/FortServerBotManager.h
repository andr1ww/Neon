#pragma once
#include "pch.h"

#include "Engine/Rotator/Header/Rotator.h"
#include "Engine/Vector/Header/Vector.h"
#include "FortniteGame/FortGameMode/Header/FortGameMode.h"
#include "FortniteGame/FortPlayerPawn/Header/FortPlayerPawn.h"

class AFortGameModeAthena;

class AFortAthenaMutator : public AActor
{
public:
    DEFINE_PTR(AFortGameModeAthena, AFortAthenaMutator, CachedGameMode);
    DEFINE_PTR(AFortGameStateAthena, AFortAthenaMutator, CachedGameState);
};

class AFortAthenaMutator_SpawningPolicyBase : public AFortAthenaMutator
{
};

class AFortAthenaMutator_SpawningPolicyEQS : public AFortAthenaMutator_SpawningPolicyBase
{
};

class AFortAthenaMutator_Bots : public AFortAthenaMutator_SpawningPolicyEQS
{

};

struct FFortAthenaAIBotRunTimeCustomizationData final
{
public:
    FGameplayTag                           PredefinedCosmeticSetTag;                          // 0x0000(0x0008)(Edit, BlueprintVisible, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    float                                         CullDistanceSquared;                               // 0x0008(0x0004)(Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    bool                                          bCheckForOverlaps;                                 // 0x000C(0x0001)(Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    uint8                                         bHasCustomSquadId : 1;                             // 0x000D(0x0001)(BitIndex: 0x00, PropSize: 0x0001 (Edit, BlueprintVisible, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic))
    uint8                                         CustomSquadId;                                     // 0x000E(0x0001)(Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    uint8                                         Pad_1C77[0x1];                                     // 0x000F(0x0001)(Fixing Struct Size After Last Property [ Dumper-7 ])
};

class UFortAthenaAIBotCustomizationData final : public UObject
{
    
};

class UFortServerBotManagerAthena : public UObject
{
public:
    DEFINE_PTR(AFortGameModeAthena, UFortServerBotManagerAthena, CachedGameMode);
    DEFINE_PTR(AFortGameStateAthena, UFortServerBotManagerAthena, CachedGameState);
public:
    static AFortPlayerPawn* SpawnBot(UFortServerBotManagerAthena *BotManager, FVector SpawnLoc, FRotator SpawnRot, UFortAthenaAIBotCustomizationData *BotData, FFortAthenaAIBotRunTimeCustomizationData &RuntimeBotData);
public:
    DECLARE_STATIC_CLASS(UFortServerBotManagerAthena)
    DECLARE_DEFAULT_OBJECT(UFortServerBotManagerAthena)
};
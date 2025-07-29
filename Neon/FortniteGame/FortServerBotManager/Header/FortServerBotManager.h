#pragma once
#include "pch.h"

#include "Engine/Rotator/Header/Rotator.h"
#include "Engine/Vector/Header/Vector.h"
#include "FortniteGame/FortGameMode/Header/FortGameMode.h"
#include "FortniteGame/FortPlayerPawn/Header/FortPlayerPawn.h"

class AFortGameModeAthena;

class AFortAIGoalManager : public AActor
{
    
};

class AFortAIDirector : public AActor
{
    
};

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
public:
    static AFortPlayerPawn* SpawnBot(TSubclassOf<class AFortPlayerPawn> BotPawnClass, const class AActor* InSpawnLocator, const struct FVector& InSpawnLocation, const struct FRotator& InSpawnRotation, const bool bSnapToGround)
    {
        static SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortAthenaMutator_Bots", "SpawnBot");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return nullptr;

        struct FortAthenaMutator_Bots_SpawnBot final
        {
        public:
            TSubclassOf<class AFortPlayerPawn>      BotPawnClass;                                      // 0x0000(0x0008)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            const class AActor*                           InSpawnLocator;                                    // 0x0008(0x0008)(ConstParm, Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            struct FVector                                InSpawnLocation;                                   // 0x0010(0x000C)(ConstParm, Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            struct FRotator                               InSpawnRotation;                                   // 0x001C(0x000C)(ConstParm, Parm, ZeroConstructor, IsPlainOldData, NoDestructor, NativeAccessSpecifierPublic)
            bool                                          bSnapToGround;                                     // 0x0028(0x0001)(ConstParm, Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            class AFortPlayerPawn*                  ReturnValue;                                       // 0x0030(0x0008)(Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
        } FortAthenaMutator_Bots_SpawnBot_Params{ BotPawnClass, InSpawnLocator, InSpawnLocation, InSpawnRotation, bSnapToGround };
    
        SDK::StaticClassImpl("FortAthenaMutator_Bots")->GetClassDefaultObject()->ProcessEvent(Func, &FortAthenaMutator_Bots_SpawnBot_Params);
    
        return FortAthenaMutator_Bots_SpawnBot_Params.ReturnValue;
    }
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

class UFortAthenaAIBotCharacterCustomization : public UObject
{
public:
    DEFINE_MEMBER(FFortAthenaLoadout, UFortAthenaAIBotCharacterCustomization, CustomizationLoadout);
};

class UFortAthenaAIBotCustomizationData final : public UObject
{
public:
    DEFINE_MEMBER(TSubclassOf<class AFortPlayerPawn>, UFortAthenaAIBotCustomizationData, PawnClass);
    DEFINE_PTR(UFortAthenaAIBotCharacterCustomization, UFortAthenaAIBotCustomizationData, CharacterCustomization);
};

class UFortServerBotManagerAthena : public UObject
{
public:
    DEFINE_PTR(AFortGameModeAthena, UFortServerBotManagerAthena, CachedGameMode);
    DEFINE_PTR(AFortGameStateAthena, UFortServerBotManagerAthena, CachedGameState);
public:
    DefHookOg(AFortPlayerPawn*, SpawnBot, UFortServerBotManagerAthena *BotManager, FVector SpawnLoc, FRotator SpawnRot, UFortAthenaAIBotCustomizationData *BotData, FFortAthenaAIBotRunTimeCustomizationData &RuntimeBotData);
public:
    DECLARE_STATIC_CLASS(UFortServerBotManagerAthena)
    DECLARE_DEFAULT_OBJECT(UFortServerBotManagerAthena)
};
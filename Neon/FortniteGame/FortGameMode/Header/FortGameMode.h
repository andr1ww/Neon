#pragma once
#include "pch.h"
#include "Engine/Actor/Header/Actor.h"
#include "Engine/PlayerController/Header/PlayerController.h"
#include "Engine/Transform/Header/Transform.h"
#include "FortniteGame/FortGameState/Header/FortGameState.h"
#include "FortniteGame/FortPlayerController/Header/FortPlayerController.h"
#include "FortniteGame/FortServerBotManager/Header/FortServerBotManager.h"
#include "Engine/ObjectPtr/Header/ObjectPtr.h"
#include "FortniteGame/BuildingSMActor/Header/BuildingSMActor.h"

class AFortGameStateAthena;
class UFortItemDefinition;
class AFortPlayerControllerAthena;
class AFortAIDirector;
class AFortAIGoalManager;
class AFortAthenaMutator_Bots;
class UFortServerBotManagerAthena;

struct FItemAndCount final 
{
public:
    DEFINE_MEMBER(int32, FItemAndCount, Count);
    DEFINE_PTR(UFortItemDefinition, FItemAndCount, Item);
};

class AGameModeBase : public UObject
{
public:
    DEFINE_PTR(AFortGameStateAthena, AGameModeBase, GameState);
};

class AFortGameMode : public AGameModeBase
{
public:
    DEFINE_MEMBER(int32, AFortGameMode, CurrentPlaylistId);
    DEFINE_MEMBER(FName, AFortGameMode, CurrentPlaylistName);
    DEFINE_BOOL(AFortGameMode, bWorldIsReady);
};

struct FBotMutator
{
    static inline AFortAthenaMutator_Bots* Instance = nullptr;

    static AFortAthenaMutator_Bots* Get()
    {
        return Instance;
    }

    static void Set(AFortAthenaMutator_Bots* NewInstance)
    {
        Instance = NewInstance;
    }
};

struct FAdditionalLevelStreamed final
{
public:
    class FName                                   LevelName;                                         // 0x0000(0x0008)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    bool                                          bIsServerOnly;                                     // 0x0008(0x0001)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    uint8                                         Pad_1B33[0x3];                                     // 0x0009(0x0003)(Fixing Struct Size After Last Property [ Dumper-7 ])
};

class ULevelStreamingDynamic : public UObject
{
public:
    uint8                                         bInitiallyLoaded : 1;                              // 0x0148(0x0001)(BitIndex: 0x00, PropSize: 0x0001 (Edit, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic))
    uint8                                         bInitiallyVisible : 1;                             // 0x0148(0x0001)(BitIndex: 0x01, PropSize: 0x0001 (Edit, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic))
    uint8                                         Pad_6BF[0x7];                                      // 0x0149(0x0007)(Fixing Struct Size After Last Property [ Dumper-7 ])
public:
    static ULevelStreamingDynamic* LoadLevelInstance(class UObject* WorldContextObject, const class FString& LevelName, const struct FVector& Location, const struct FRotator& Rotation, bool* bOutSuccess, const class FString& OptionalLevelNameOverride)
     {
         static SDK::UFunction* Func = nullptr;
         SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("LevelStreamingDynamic", "LoadLevelInstance");

         if (Func == nullptr)
             Func = Info.Func;
         if (!Func)
             return nullptr;
    
        struct LevelStreamingDynamic_LoadLevelInstance final
        {
        public:
            class UObject*                                WorldContextObject;                                // 0x0000(0x0008)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            class FString                                 LevelName;                                         // 0x0008(0x0010)(Parm, ZeroConstructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            struct FVector                                Location;                                          // 0x0018(0x000C)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            struct FRotator                               Rotation;                                          // 0x0024(0x000C)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, NativeAccessSpecifierPublic)
            bool                                          bOutSuccess;                                       // 0x0030(0x0001)(Parm, OutParm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            uint8                                         Pad_6C0[0x7];                                      // 0x0031(0x0007)(Fixing Size After Last Property [ Dumper-7 ])
            class FString                                 OptionalLevelNameOverride;                         // 0x0038(0x0010)(Parm, ZeroConstructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            class ULevelStreamingDynamic*                 ReturnValue;                                       // 0x0048(0x0008)(Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
        };

         LevelStreamingDynamic_LoadLevelInstance Parms{};

        Parms.WorldContextObject = WorldContextObject;
        Parms.LevelName = LevelName;
        Parms.Location = Location;
        Parms.Rotation = Rotation;
        Parms.OptionalLevelNameOverride = OptionalLevelNameOverride;
    
         SDK::StaticClassImpl("LevelStreamingDynamic")->GetClassDefaultObject()->ProcessEvent(Func, &Parms);
        
         if (bOutSuccess != nullptr)
             *bOutSuccess = Parms.bOutSuccess;
         
         return Parms.ReturnValue;
     }
public:
    DECLARE_STATIC_CLASS(ULevelStreamingDynamic)
    DECLARE_DEFAULT_OBJECT(ULevelStreamingDynamic)
};

enum class EDynamicFoundationEnabledState : uint8
{
    Unknown                                  = 0,
    Enabled                                  = 1,
    Disabled                                 = 2,
    EDynamicFoundationEnabledState_MAX       = 3,
};

// Enum FortniteGame.EDynamicFoundationType
// NumValues: 0x0005
enum class EDynamicFoundationType : uint8
{
    Static                                   = 0,
    StartEnabled_Stationary                  = 1,
    StartEnabled_Dynamic                     = 2,
    StartDisabled                            = 3,
    EDynamicFoundationType_MAX               = 4,
};

// 0x001C (0x001C - 0x0000)
struct FDynamicBuildingFoundationRepData final
{
public:
    struct FRotator                               Rotation;                                          // 0x0000(0x000C)(ZeroConstructor, IsPlainOldData, NoDestructor, NativeAccessSpecifierPublic)
    struct FVector                                Translation;    
DEFINE_MEMBER(EDynamicFoundationEnabledState, FDynamicBuildingFoundationRepData, EnabledState);                                  // 0x000C(0x000C)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    uint8                                         Pad_1D3B[0x3];                                     // 0x0019(0x0003)(Fixing Struct Size After Last Property [ Dumper-7 ])
};

class ABuildingFoundation : public ABuildingSMActor
{
public:
    DEFINE_MEMBER(EDynamicFoundationType, ABuildingFoundation, DynamicFoundationType);
    DEFINE_MEMBER(bool, ABuildingFoundation, bServerStreamedInLevel);
    DEFINE_MEMBER(FDynamicBuildingFoundationRepData, ABuildingFoundation, DynamicFoundationRepData);
    DEFINE_MEMBER(EDynamicFoundationEnabledState, ABuildingFoundation, FoundationEnabledState);
};

class AFortGameModeAthena : public AFortGameMode
{
public:
    DEFINE_MEMBER(int32, AFortGameModeAthena, WarmupRequiredPlayerCount);
    DEFINE_MEMBER(int32, AFortGameModeAthena, NumPlayers);
    DEFINE_MEMBER(TArray<FItemAndCount>, AFortGameModeAthena, StartingItems);
    DEFINE_MEMBER(TArray<AFortPlayerControllerAthena*>, AFortGameModeAthena, AlivePlayers);
    DEFINE_PTR(UFortServerBotManagerAthena, AFortGameModeAthena, ServerBotManager);
    DEFINE_MEMBER(TSubclassOf<class UFortServerBotManagerAthena>, AFortGameModeAthena, ServerBotManagerClass);
    DEFINE_PTR(AFortAIDirector, AFortGameModeAthena, AIDirector);
    DEFINE_PTR(AFortAIGoalManager, AFortGameModeAthena, AIGoalManager);
public:
    DECLARE_DEFAULT_OBJECT(AFortGameModeAthena);
    DECLARE_STATIC_CLASS(AFortGameModeAthena);
public:
    static bool ReadyToStartMatch(AFortGameModeAthena* GameMode, FFrame& Stack, bool* Result);
    static APawn* SpawnDefaultPawnFor(AFortGameModeAthena* GameMode, AFortPlayerControllerAthena* NewPlayer, AActor* StartSpot);
};

inline bool (*ReadyToStartMatchOriginal)(AFortGameModeAthena* GameMode);

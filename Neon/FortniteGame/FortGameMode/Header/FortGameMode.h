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

class AGameSession : public UObject
{
public:
    DEFINE_MEMBER(int32, AGameSession, MaxPlayers);
public:
    DECLARE_STATIC_CLASS(AGameSession)
    DECLARE_DEFAULT_OBJECT(AGameSession)
};

class AGameModeBase : public UObject
{
public:
    DEFINE_MEMBER(TSubclassOf<class APawn>, AGameModeBase, DefaultPawnClass);
    
    DEFINE_PTR(AGameSession, AGameModeBase, GameSession);
    DEFINE_PTR(AFortGameStateAthena, AGameModeBase, GameState);
public:
    void RestartPlayer(AController* Controller)
    {
        static SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("GameModeBase", "RestartPlayer");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return;
    
        struct { AController* NewPlayer; } RestartPlayerParams{ Controller };
        
        this->ProcessEvent(Func, &RestartPlayerParams);
    }

    APawn* SpawnDefaultPawnAtTransform(class AController* NewPlayer, const struct FTransform& SpawnTransform)
    {
        static SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("GameModeBase", "SpawnDefaultPawnAtTransform");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return nullptr;

        struct GameModeBase_SpawnDefaultPawnAtTransform final
        {
        public:
            class AController* NewPlayer;
            struct FTransform SpawnTransform;
            class APawn* ReturnValue;                                       
        };
        GameModeBase_SpawnDefaultPawnAtTransform Params{};
        Params.NewPlayer = NewPlayer;
        Params.SpawnTransform = std::move(SpawnTransform);
        
        this->ProcessEvent(Func, &Params);

        return Params.ReturnValue;
    }
public:
    DECLARE_STATIC_CLASS(AGameModeBase)
    DECLARE_DEFAULT_OBJECT(AGameModeBase)
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

class ANavigationObjectBase : public AActor {

};

class APlayerStart : public ANavigationObjectBase {

};

class AFortPlayerStartWarmup : public APlayerStart
{
public:
    DECLARE_STATIC_CLASS(AFortPlayerStartWarmup)
    DECLARE_DEFAULT_OBJECT(AFortPlayerStartWarmup)
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
public:
    void OnRep_ServerStreamedInLevel()
    {
        static SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("BuildingFoundation", "OnRep_ServerStreamedInLevel");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return;
		
        this->ProcessEvent(Func, nullptr);
    }

    void OnRep_DynamicFoundationRepData()
    {
        static SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("BuildingFoundation", "OnRep_DynamicFoundationRepData");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return;
		
        this->ProcessEvent(Func, nullptr);
    }

    void SetDynamicFoundationEnabled(bool bEnabled)
    {
        static SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("BuildingFoundation", "SetDynamicFoundationEnabled");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return;

        struct BuildingFoundation_SetDynamicFoundationEnabled final
        {
        public:
            bool bEnabled;
        };
        BuildingFoundation_SetDynamicFoundationEnabled Params;
        Params.bEnabled = bEnabled;
		
        this->ProcessEvent(Func, &Params);
    }
};

class AFortGameModeAthena : public AFortGameMode
{
public:
    typedef TMap<FName, FName> Redirect;
    DEFINE_MEMBER(Redirect, AFortGameModeAthena, RedirectAthenaLootTierGroups);
    DEFINE_MEMBER(int32, AFortGameModeAthena, WarmupRequiredPlayerCount);
    DEFINE_MEMBER(TArray<FItemAndCount>, AFortGameModeAthena, StartingItems);

    DEFINE_MEMBER(int32, AFortGameModeAthena, NumPlayers);
    DEFINE_MEMBER(int32, AFortGameModeAthena, NumBots);

    DEFINE_MEMBER(TArray<AFortPlayerControllerAthena*>, AFortGameModeAthena, AlivePlayers);
    DEFINE_MEMBER(TArray<AFortAthenaAIBotController*>, AFortGameModeAthena, AliveBots);

    DEFINE_PTR(AFortAIDirector, AFortGameModeAthena, AIDirector);
    DEFINE_PTR(AFortAIGoalManager, AFortGameModeAthena, AIGoalManager);
    DEFINE_PTR(UAthenaAISettings, AFortGameModeAthena, AISettings);

    DEFINE_MEMBER(TSubclassOf<class UFortServerBotManagerAthena>, AFortGameModeAthena, ServerBotManagerClass);
    DEFINE_PTR(UFortServerBotManagerAthena, AFortGameModeAthena, ServerBotManager);
public:
    DECLARE_DEFAULT_OBJECT(AFortGameModeAthena);
    DECLARE_STATIC_CLASS(AFortGameModeAthena);
public:
    static bool ReadyToStartMatch(AFortGameModeAthena* GameMode, FFrame& Stack, bool* Result);
    static APawn* SpawnDefaultPawnFor(AFortGameModeAthena* GameMode, AFortPlayerControllerAthena* NewPlayer, AActor* StartSpot);
};

inline bool (*ReadyToStartMatchOriginal)(AFortGameModeAthena* GameMode);

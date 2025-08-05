#pragma once
#include "pch.h"

#include "Engine/Rotator/Header/Rotator.h"
#include "Engine/Vector/Header/Vector.h"
#include "FortniteGame/FortGameMode/Header/FortGameMode.h"
#include "FortniteGame/FortPlayerPawn/Header/FortPlayerPawn.h"
#include "FortniteGame/FortAthenaAIBotController/Header/FortAthenaAIBotController.h"
#include "Engine/DataTable/Header/DataTable.h"
#include "Engine/NetDriver/Header/NetDriver.h"
#include "Engine/ObjectPtr/Header/ObjectPtr.h"

class AFortGameModeAthena;
class UActorComponent;
class ANavigationData;

enum class EAlertLevel : uint8
{
    Unaware                                  = 0,
    Alerted                                  = 1,
    LKP                                      = 2,
    Threatened                               = 3,
    Count                                    = 4,
    EAlertLevel_MAX                          = 5,
};

enum class EExecutionStatus : uint8
{
    ExecutionError = 0,
    ExecutionDenied = 1,
    ExecutionSuccess = 2,
    ExecutionPending = 3,
    ExecutionAllowed = 4,
    EExecutionStatus_MAX = 5,
};

namespace EBTExecutionMode
{
    enum Type
    {
        SingleRun,
        Looped,
    };
}

namespace EBTActiveNode
{
    enum Type
    {
        Composite,
        ActiveTask,
        AbortingTask,
        InactiveTask,
    };
}

namespace EBTTaskStatus
{
    enum Type
    {
        Active,
        Aborting,
        Inactive,
    };
}

class UAthenaAISettings final : public UDataAsset {

};

class AFortAIGoalManager : public AActor
{
    
};

class UBrainComponent : public UActorComponent {
public:
    void StartLogic() {
        static SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("BrainComponent", "StartLogic");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return;

        this->ProcessEvent(Func, nullptr);
    }

    void RestartLogic() {
        static SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("BrainComponent", "RestartLogic");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return;

        this->ProcessEvent(Func, nullptr);
    }
public:
	DECLARE_STATIC_CLASS(UBrainComponent);
	DECLARE_DEFAULT_OBJECT(UBrainComponent);
};

class UBlackboardData : public UDataAsset {
public:
	DECLARE_STATIC_CLASS(UBlackboardData);
	DECLARE_DEFAULT_OBJECT(UBlackboardData);
};

class UBlackboardComponent final : public UActorComponent {
private:
    struct BlackboardComponent_SetValueAsEnum final
    {
    public:
        FName KeyName;
        uint8 EnumValue;
    };

    struct BlackboardComponent_SetValueAsBool final
    {
    public:
        FName KeyName;
        bool BoolValue;
    };
public:
    void SetValueAsEnum(const class FName& KeyName, uint8 EnumValue) {
        static SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("BlackboardComponent", "SetValueAsEnum");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return;

		BlackboardComponent_SetValueAsEnum Params;
		Params.KeyName = KeyName;
		Params.EnumValue = EnumValue;

        this->ProcessEvent(Func, &Params);
    }

    void SetValueAsBool(const class FName& KeyName, bool BoolValue) {
        static SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("BlackboardComponent", "SetValueAsBool");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return;

        BlackboardComponent_SetValueAsBool Params;
        Params.KeyName = KeyName;
        Params.BoolValue = BoolValue;

        this->ProcessEvent(Func, &Params);
    }
public:
	DECLARE_STATIC_CLASS(UBlackboardComponent);
	DECLARE_DEFAULT_OBJECT(UBlackboardComponent);
};

class UFortAthenaAIBotAttackingDigestedSkillSet : public UObject
{
public:
    DECLARE_STATIC_CLASS(UFortAthenaAIBotAttackingDigestedSkillSet)
    DECLARE_DEFAULT_OBJECT(UFortAthenaAIBotAttackingDigestedSkillSet)
};

class UBehaviorTree : public UObject {
public:
    DEFINE_PTR(UBlackboardData, UBehaviorTree, BlackboardAsset);
public:
	DECLARE_STATIC_CLASS(UBehaviorTree);
	DECLARE_DEFAULT_OBJECT(UBehaviorTree);
};

class UBehaviorTreeComponent : public UBrainComponent {
public:
    DEFINE_PTR(UBehaviorTree, UBehaviorTreeComponent, DefaultBehaviorTreeAsset);
public:
	DECLARE_STATIC_CLASS(UBehaviorTreeComponent);
	DECLARE_DEFAULT_OBJECT(UBehaviorTreeComponent);
};

class AFortAIDirector : public AActor
{
public:
    DECLARE_STATIC_CLASS(AFortAIDirector);
    DECLARE_DEFAULT_OBJECT(AFortAIDirector);
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

struct alignas(0x04) FColor final
{
    public:
    uint8                                         B;                                                 // 0x0000(0x0001)(Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    uint8                                         G;                                                 // 0x0001(0x0001)(Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    uint8                                         R;                                                 // 0x0002(0x0001)(Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    uint8                                         A;                                                 // 0x0003(0x0001)(Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
};

struct FNavDataConfig final 
{
    DEFINE_MEMBER(FColor, FNavDataConfig, Color); // 0x0000(0x0004)(Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    DEFINE_MEMBER(FName, FNavDataConfig, Name); // 0x0000(0x0008)(Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    DEFINE_MEMBER(FVector, FNavDataConfig, DefaultQueryExtent);
    DEFINE_MEMBER(TSubclassOf<AActor>, FNavDataConfig, NavigationDataClass); // 0x0010(0x0008)(Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    DEFINE_MEMBER(TSoftClassPtr<class UClass>, FNavDataConfig, NavDataClass); // 0x0000(0x0028)(Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic)
};

class AFortAthenaMutator_SpawningPolicyEQS : public AFortAthenaMutator_SpawningPolicyBase
{
};

class AAthenaNavMesh : public UObject
{
public:
    DECLARE_STATIC_CLASS(AAthenaNavMesh)
    DECLARE_DEFAULT_OBJECT(AAthenaNavMesh)
};

class AFortAthenaMutator_Bots : public AFortAthenaMutator_SpawningPolicyEQS
{
public:
    AFortPlayerPawn* SpawnBot(TSubclassOf<class AFortPlayerPawn> BotPawnClass, const class AActor* InSpawnLocator, const struct FVector& InSpawnLocation, const struct FRotator& InSpawnRotation, const bool bSnapToGround)
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
    
        this->ProcessEvent(Func, &FortAthenaMutator_Bots_SpawnBot_Params);
    
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

class UFortBotNameSettings : public UObject
{
    
};

class UFortAthenaAIBotCustomizationData final : public UObject
{
public:
    DEFINE_MEMBER(TSubclassOf<class AFortPlayerPawn>, UFortAthenaAIBotCustomizationData, PawnClass);
    DEFINE_PTR(UBehaviorTree, UFortAthenaAIBotCustomizationData, BehaviorTree);
    DEFINE_PTR(UFortAthenaAIBotCharacterCustomization, UFortAthenaAIBotCustomizationData, CharacterCustomization);
	DEFINE_PTR(UFortAthenaAIBotInventoryItems, UFortAthenaAIBotCustomizationData, StartupInventory);
    DEFINE_PTR(UFortBotNameSettings, UFortAthenaAIBotCustomizationData, BotNameSettings);   

	DEFINE_MEMBER(float, UFortAthenaAIBotCustomizationData, SkillLevel);
};

struct FBotInventory {
    static std::map<AFortAthenaAIBotController*, UFortAthenaAIBotInventoryItems*>& GetMap()
    {
        static std::map<AFortAthenaAIBotController*, UFortAthenaAIBotInventoryItems*> Map;
        return Map;
    }
    
    static void SetInventory(AFortAthenaAIBotController* Controller, UFortAthenaAIBotInventoryItems* Inventory)
    {
        GetMap()[Controller] = Inventory;
    }
    
    static UFortAthenaAIBotInventoryItems* GetInventory(AFortAthenaAIBotController* Controller)
    {
        auto& map = GetMap();
        auto it = map.find(Controller);
        return it != map.end() ? it->second : nullptr;
    }
    
    static void RemoveInventory(AFortAthenaAIBotController* Controller)
    {
        auto& map = GetMap();
        auto it = map.find(Controller);
        if (it != map.end())
        {
            map.erase(it);
        }
    }
};


class UNavigationSystemV1 : public UObject
{
public:
    DEFINE_MEMBER(TArray<FNavDataConfig>, UNavigationSystemV1, SupportedAgents);
    DEFINE_BOOL(UNavigationSystemV1, bAutoCreateNavigationData);
    DEFINE_PTR(ANavigationData, UNavigationSystemV1, MainNavData);
public:
    DECLARE_DEFAULT_OBJECT(UNavigationSystemV1)
    DECLARE_STATIC_CLASS(UNavigationSystemV1)
};

class UFortNavSystem : public UNavigationSystemV1
{
public:
    DECLARE_STATIC_CLASS(UFortNavSystem);
    DECLARE_DEFAULT_OBJECT(UFortNavSystem)
};

class UAthenaNavSystem : public UFortNavSystem
{
public:
    DECLARE_DEFAULT_OBJECT(UAthenaNavSystem);
    DECLARE_STATIC_CLASS(UAthenaNavSystem);
};

class UNavigationSystemConfig : public UObject {
public:
    DECLARE_STATIC_CLASS(UNavigationSystemConfig);
    DECLARE_DEFAULT_OBJECT(UNavigationSystemConfig);
};

class UNavigationSystemModuleConfig : public UNavigationSystemConfig {
public:
    DEFINE_BOOL(UNavigationSystemModuleConfig, bAutoSpawnMissingNavData);
    DEFINE_BOOL(UNavigationSystemModuleConfig, bSpawnNavDataInNavBoundsLevel);
public:
	DECLARE_STATIC_CLASS(UNavigationSystemModuleConfig);
	DECLARE_DEFAULT_OBJECT(UNavigationSystemModuleConfig);
};

class UFortNavSystemConfig : public UNavigationSystemModuleConfig {
public:
	DECLARE_STATIC_CLASS(UFortNavSystemConfig);
	DECLARE_DEFAULT_OBJECT(UFortNavSystemConfig);
};

class UAthenaNavSystemConfig final : public UFortNavSystemConfig {
public:
    DEFINE_BOOL(UAthenaNavSystemConfig, bPrioritizeNavigationAroundSpawners);
public:
	DECLARE_STATIC_CLASS(UAthenaNavSystemConfig);
	DECLARE_DEFAULT_OBJECT(UAthenaNavSystemConfig);
};

class AFortAthenaPatrolPoint final : public AActor {
public:
	DECLARE_STATIC_CLASS(AFortAthenaPatrolPoint);
    DECLARE_DEFAULT_OBJECT(AFortAthenaPatrolPoint);
};

class AFortAthenaPatrolPath final : public AActor {
public:
    DEFINE_MEMBER(TArray<class AFortAthenaPatrolPoint*>, AFortAthenaPatrolPath, PatrolPoints);
public:
	DECLARE_STATIC_CLASS(AFortAthenaPatrolPath);
    DECLARE_DEFAULT_OBJECT(AFortAthenaPatrolPath);
};

class UFortAthenaNpcPatrollingComponent final : public UActorComponent {
public:
    struct FortAthenaNpcPatrollingComponent_SetPatrolPath final
    {
    public:
        const AFortAthenaPatrolPath* NewPatrolPath;
    };
public:
    void SetPatrolPath(const AFortAthenaPatrolPath* NewPatrolPath) {
        static class UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("AIController", "OnUsingBlackBoard");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return;

        FortAthenaNpcPatrollingComponent_SetPatrolPath Params;
        Params.NewPatrolPath = NewPatrolPath;

        this->ProcessEvent(Func, &Params);
    }
public:
    DECLARE_STATIC_CLASS(UFortAthenaNpcPatrollingComponent);
    DECLARE_DEFAULT_OBJECT(UFortAthenaNpcPatrollingComponent);
};

class UFortServerBotManagerAthena : public UObject
{
public:
    DEFINE_PTR(AFortGameModeAthena, UFortServerBotManagerAthena, CachedGameMode);
    DEFINE_PTR(AFortGameStateAthena, UFortServerBotManagerAthena, CachedGameState);
    DEFINE_PTR(AFortAthenaMutator_Bots, UFortServerBotManagerAthena, CachedBotMutator);
public:
    DefHookOg(void, InitializeForWorld, UNavigationSystemV1*, UWorld*, uint8);
    DefHookOg(void, CreateAndConfigureNavigationSystem, UAthenaNavSystemConfig*, UWorld*);

    DefHookOg(AFortPlayerPawn*, SpawnBot, UFortServerBotManagerAthena *BotManager, FVector SpawnLoc, FRotator SpawnRot, UFortAthenaAIBotCustomizationData *BotData, FFortAthenaAIBotRunTimeCustomizationData &RuntimeBotData);
    DefHookOg(void, OnAlertLevelChanged, UObject* Context, FFrame& Stack);
public:
    // Unofficial stuff
    static void StartTree(UBehaviorTreeComponent* BTComp, UBehaviorTree* BTAsset, EBTExecutionMode::Type Mode = EBTExecutionMode::Looped);
    static bool RunBehaviorTree(AFortAthenaAIBotController* PC, UBehaviorTree* BTAsset);
public:
    DECLARE_STATIC_CLASS(UFortServerBotManagerAthena)
    DECLARE_DEFAULT_OBJECT(UFortServerBotManagerAthena)
};
#pragma once
#include "pch.h"
#include "Neon/Runtime/Runtime.h"
#include "Engine/PlayerController/Header/PlayerController.h"
#include "FortniteGame/FortInventory/Header/FortInventory.h"
#include "FortniteGame/FortPlayerPawn/Header/FortPlayerPawn.h"
#include "FortniteGame/FortPlayerState/Header/FortPlayerState.h"
#include "FortniteGame/Common/Header/ItemDefinition.h"


class UBehaviorTree;
class UBrainComponent;
class UBlackboardComponent;
class UFortAthenaNpcPatrollingComponent;

enum class EAlertLevel : uint8
{
    Unaware                                  = 0,
    Alerted                                  = 1,
    LKP                                      = 2,
    Threatened                               = 3,
    Count                                    = 4,
    EAlertLevel_MAX                          = 5,
};

enum class EPathFollowingRequestResult : uint8
{
    Failed                                   = 0,
    AlreadyAtGoal                            = 1,
    RequestSuccessful                        = 2,
    EPathFollowingRequestResult_MAX          = 3,
};

class ANavigationData : public AActor
{
public:
    
public:
    DECLARE_STATIC_CLASS(ANavigationData);
    DECLARE_DEFAULT_OBJECT(ANavigationData);
};

class UPathFollowingComponent : public UObject
{
public:
    DEFINE_PTR(ANavigationData, UPathFollowingComponent, MyNavData);
public:
    DECLARE_STATIC_CLASS(UPathFollowingComponent);
    DECLARE_DEFAULT_OBJECT(UPathFollowingComponent);
};

class AAIController : public AController
{
public:
    DEFINE_PTR(UPathFollowingComponent, AAIController, PathFollowingComponent);
    DEFINE_PTR(UBrainComponent, AAIController, BrainComponent);
    DEFINE_PTR(UBlackboardComponent, AAIController, Blackboard);
private:
    struct AIController_RunBehaviorTree final
    {
    public:
        UBehaviorTree* BTAsset;
        bool ReturnValue;
    };

    struct AIController_UseBlackboard final
    {
    public:
        class UBlackboardData* BlackboardAsset;
        class UBlackboardComponent* BlackboardComponent;
        bool ReturnValue;
    };

    struct AIController_OnUsingBlackBoard final
    {
    public:
        class UBlackboardComponent* BlackboardComp;
        class UBlackboardData* BlackboardAsset;
    };
public:
    bool RunBehaviorTree(UBehaviorTree* BTAsset) {
        static class UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("AIController", "RunBehaviorTree");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return false;

        AIController_RunBehaviorTree Params;
		Params.BTAsset = BTAsset;

        this->ProcessEvent(Func, &Params);

        return Params.ReturnValue;
    }

    bool UseBlackboard(class UBlackboardData* BlackboardAsset, class UBlackboardComponent** BlackboardComponent) {
        static class UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("AIController", "UseBlackboard");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return false;

        AIController_UseBlackboard Params;
		Params.BlackboardAsset = BlackboardAsset;
        if (BlackboardComponent != nullptr)
            Params.BlackboardComponent = *BlackboardComponent;

        this->ProcessEvent(Func, &Params);

        return Params.ReturnValue;
    }

    void OnUsingBlackBoard(class UBlackboardComponent* BlackboardComp, class UBlackboardData* BlackboardAsset) {
        static class UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("AIController", "OnUsingBlackBoard");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return;

        AIController_OnUsingBlackBoard Params;
		Params.BlackboardComp = BlackboardComp;
        Params.BlackboardAsset = BlackboardAsset;

        this->ProcessEvent(Func, &Params);
    }

    EPathFollowingRequestResult MoveToActor(class AActor* Goal, float AcceptanceRadius, bool bStopOnOverlap, bool bUsePathfinding, bool bCanStrafe, TSubclassOf<class UNavigationQueryFilter> FilterClass, bool bAllowPartialPath) {
        static class UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("AIController", "MoveToActor");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return EPathFollowingRequestResult::Failed;

        struct AIController_MoveToActor final
        {
            public:
	        class AActor*                                 Goal;                                            
	        float                                         AcceptanceRadius;                                 
	        bool                                          bStopOnOverlap;                                   
	        bool                                          bUsePathfinding;                                   
	        bool                                          bCanStrafe;                                        
	        TSubclassOf<class UNavigationQueryFilter>     FilterClass;                                       
	        bool                                          bAllowPartialPath;                              
	        EPathFollowingRequestResult                   ReturnValue;     
        };

        AIController_MoveToActor Params{};
        Params.Goal = Goal;
        Params.AcceptanceRadius = AcceptanceRadius;
        Params.bStopOnOverlap = bStopOnOverlap;
        Params.bUsePathfinding = bUsePathfinding;
        Params.bCanStrafe = bCanStrafe;
        Params.FilterClass = FilterClass;
        Params.bAllowPartialPath = bAllowPartialPath;

        this->ProcessEvent(Func, &Params);

        return Params.ReturnValue;
    }

    EPathFollowingRequestResult MoveToLocation(const struct FVector& Dest, float AcceptanceRadius, bool bStopOnOverlap, bool bUsePathfinding, bool bProjectDestinationToNavigation, bool bCanStrafe, TSubclassOf<class UNavigationQueryFilter> FilterClass, bool bAllowPartialPath) {
        static class UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("AIController", "MoveToLocation");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return EPathFollowingRequestResult::Failed;

        struct AIController_MoveToLocation final
        {
        public:
	        struct FVector                                Dest; 
	        float                                         AcceptanceRadius;                                  
	        bool                                          bStopOnOverlap;                                    
	        bool                                          bUsePathfinding;                                   
	        bool                                          bProjectDestinationToNavigation;                   
	        bool                                          bCanStrafe;                                        
	        TSubclassOf<class UNavigationQueryFilter>     FilterClass;                                       
	        bool                                          bAllowPartialPath;                                 
	        EPathFollowingRequestResult                   ReturnValue;                                       
        };

        AIController_MoveToLocation Params{};
        Params.Dest = std::move(Dest);
        Params.AcceptanceRadius = AcceptanceRadius;
        Params.bStopOnOverlap = bStopOnOverlap;
        Params.bUsePathfinding = bUsePathfinding;
        Params.bProjectDestinationToNavigation = bProjectDestinationToNavigation;
        Params.bCanStrafe = bCanStrafe;
        Params.FilterClass = FilterClass;
        Params.bAllowPartialPath = bAllowPartialPath;

        this->ProcessEvent(Func, &Params);

        return Params.ReturnValue;
    }

    void K2_SetFocalPoint(const struct FVector& FP) {
        static class UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("AIController", "K2_SetFocalPoint");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return;

        struct AIController_K2_SetFocalPoint final
        {
        public:
            struct FVector                                FP;
        };
        AIController_K2_SetFocalPoint Params;
        Params.FP = std::move(FP);

        this->ProcessEvent(Func, &Params);
    }
};

class UPrimitiveComponent : public UObject
{
public:
    DECLARE_STATIC_CLASS(UPrimitiveComponent)
    DECLARE_DEFAULT_OBJECT(UPrimitiveComponent)
};

class AFortAthenaAIBotController : public AAIController
{
public:
    DEFINE_PTR(AFortInventory, AFortAthenaAIBotController, Inventory);
    DEFINE_PTR(UFortAthenaAIBotInventoryItems, AFortAthenaAIBotController, StartupInventory);

    DEFINE_PTR(UBehaviorTree, AFortAthenaAIBotController, BehaviorTree);
    DEFINE_MEMBER(float, AFortAthenaAIBotController, Skill);

    DEFINE_PTR(UFortAthenaNpcPatrollingComponent, AFortAthenaAIBotController, CachedPatrollingComponent);
    DEFINE_MEMBER(EAlertLevel, AFortAthenaAIBotController, CurrentAlertLevel);

public:
    DefHookOg(void, OnPossessedPawnDied, AFortAthenaAIBotController*, AActor*, float, AFortPlayerControllerAthena*, AActor*, FVector, UPrimitiveComponent*, FName, FVector);

public:
    // Stuff that doesent exist in the fortnite sdk
    static void SpawnPlayerBot(int Count);
public:
    void BlueprintOnBehaviorTreeStarted() {
        static class SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortAthenaAIBotController", "BlueprintOnBehaviorTreeStarted");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return;

        this->ProcessEvent(Func, nullptr);
    }
public:
    DECLARE_STATIC_CLASS(AFortAthenaAIBotController)
    DECLARE_DEFAULT_OBJECT(AFortAthenaAIBotController)
};
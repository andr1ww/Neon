#pragma once
#include "pch.h"
#include "Neon/Runtime/Runtime.h"
#include "Engine/PlayerController/Header/PlayerController.h"
#include "FortniteGame/FortInventory/Header/FortInventory.h"
#include "FortniteGame/FortPlayerPawn/Header/FortPlayerPawn.h"
#include "FortniteGame/FortPlayerState/Header/FortPlayerState.h"
#include "FortniteGame/Common/Header/ItemDefinition.h"
#include "FortniteGame/FortServerBotManager/Header/FortServerBotManager.h"


class UBehaviorTree;
class UBrainComponent;
class UBlackboardComponent;

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
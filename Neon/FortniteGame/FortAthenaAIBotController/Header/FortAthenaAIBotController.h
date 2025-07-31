#pragma once
#include "pch.h"
#include "Neon/Runtime/Runtime.h"
#include "Engine/PlayerController/Header/PlayerController.h"
#include "FortniteGame/FortInventory/Header/FortInventory.h"
#include "FortniteGame/FortPlayerPawn/Header/FortPlayerPawn.h"
#include "FortniteGame/FortPlayerState/Header/FortPlayerState.h"
#include "FortniteGame/Common/Header/ItemDefinition.h"

class AAIController : public AController
{
private:
    struct AIController_RunBehaviorTree final
    {
    public:
        UBehaviorTree* BTAsset;
        bool ReturnValue;
    };
public:
    bool RunBehaviorTree(UBehaviorTree* BTAsset) {
        static class UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("AIController", "RunBehaviorTree");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return false;

        struct AIController_RunBehaviorTree final
        {
        public:
            UBehaviorTree* BTAsset;
            bool ReturnValue;
        } AIController_RunBehaviorTree{ BTAsset };

        SDK::StaticClassImpl("AIController")->GetClassDefaultObject()->ProcessEvent(Func, &AIController_RunBehaviorTree);

        return AIController_RunBehaviorTree.ReturnValue;
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

public:
    DefHookOg(void, OnPossessedPawnDied, AFortAthenaAIBotController*, AActor*, float, AFortPlayerControllerAthena*, AActor*, FVector, UPrimitiveComponent*, FName, FVector);
    // Stuff that doesent exist in the fortnite sdk
    static void SpawnPlayerBot(int Count);
public:
    DECLARE_STATIC_CLASS(AFortAthenaAIBotController)
    DECLARE_DEFAULT_OBJECT(AFortAthenaAIBotController)
};
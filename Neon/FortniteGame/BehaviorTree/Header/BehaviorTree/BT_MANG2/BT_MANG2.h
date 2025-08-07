#pragma once
#include "pch.h"
#include "../../BehaviorTree.h"

#include "../Tasks/Wait.h"
#include "../Tasks/SteerMovement.h"
#include "../Tasks/MoveTo.h"

#include "../Decorators/IsSet.h"
#include "../Decorators/CheckEnum.h"

#include "../Services/HandleFocusing_ScanAroundOnly.h"

#include "FortniteGame/BehaviorTree/Header/BehaviorTree/Evaluators/AvoidThreat.h"
#include "FortniteGame/BehaviorTree/Header/BehaviorTree/Evaluators/CharacterLaunched.h"

class BT_MANG2
{
public:
    static TArray<BTContext*> Bots;
public:
    static BehaviorTree* ConstructTree(BTContext Context, bool bAddToBots)
    {
        auto* Tree = new BehaviorTree();

        auto* RootSelector = new BTComposite_Selector();
        RootSelector->NodeName = "On Ground";

        {
            auto* Task = new BTTask_Wait(1.f);
            auto* Decorator = new BTDecorator_IsSet();
            Decorator->SelectedKeyName = UKismetStringLibrary::Conv_StringToName(L"AIEvaluator_Global_IsMovementBlocked");
            Task->AddDecorator(Decorator);
            RootSelector->AddChild(Task);
        }

        {
            auto* Task = new BTTask_SteerMovement(500.f, 1.5f);
            Task->AddService(new BTService_HandleFocusing_ScanAroundOnly(0.5f, 80.f));
            auto* Decorator = new BTDecorator_CheckEnum();
            Decorator->SelectedKeyName = UKismetStringLibrary::Conv_StringToName(L"AIEvaluator_CharacterLaunched_ExecutionStatus");
            Decorator->IntValue = 3;
            Decorator->Operator = EBlackboardCompareOp::GreaterThanOrEqual;
            Task->AddDecorator(Decorator);
            RootSelector->AddChild(Task);
        }

        {
            auto* Task = new BTTask_BotMoveTo();
            Task->bShouldSetFocalPoint = false;
            Task->SelectedKeyName = UKismetStringLibrary::Conv_StringToName(L"AIEvaluator_AvoidThreat_Destination");
            Task->MovementResultKey = UKismetStringLibrary::Conv_StringToName(L"AIEvaluator_AvoidThreat_ExecutionStatus"); // cheap way to stop it from executing afterwards
            auto* Decorator = new BTDecorator_CheckEnum();
            Decorator->SelectedKeyName = UKismetStringLibrary::Conv_StringToName(L"AIEvaluator_AvoidThreat_ExecutionStatus");
            Decorator->IntValue = (int)EExecutionStatus::ExecutionAllowed;
            Decorator->Operator = EBlackboardCompareOp::GreaterThanOrEqual;
            Task->AddDecorator(Decorator);
            RootSelector->AddChild(Task);
        }

        {
            auto* Service = new BTEvaluator_AvoidThreat();
            Service->RandDirOffset = 500.f;
            RootSelector->AddService(Service);
        }

        {
            auto* Service = new BTEvaluator_CharacterLaunched();
            RootSelector->AddService(Service);
        }

        Tree->RootNode = RootSelector;
        Tree->AllNodes.push_back(RootSelector);

        return Tree;
    }
};

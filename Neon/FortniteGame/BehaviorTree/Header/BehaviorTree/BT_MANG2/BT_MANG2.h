#pragma once
#include "pch.h"
#include "../../BehaviorTree.h"

#include "../Tasks/Wait.h"
#include "../Tasks/SteerMovement.h"

#include "../Decorators/IsSet.h"

#include "../Services/HandleFocusing_ScanAroundOnly.h"

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
            auto* Decorator = new BTDecorator_BlackBoard_IsSet();
            Decorator->SelectedKeyName = UKismetStringLibrary::Conv_StringToName(L"AIEvaluator_Global_IsMovementBlocked");
            Task->AddDecorator(Decorator);
            RootSelector->AddChild(Task);
        }

        {
            auto* Task = new BTTask_SteerMovement(800.f, 1.5f);
            Task->AddService(new BTService_HandleFocusing_ScanAroundOnly(0.5f, 80.f));
            RootSelector->AddChild(Task);
        }

        Tree->RootNode = RootSelector;
        Tree->AllNodes.push_back(RootSelector);

        return Tree;
    }
};

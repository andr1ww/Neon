#pragma once
#include "pch.h"
#include "../../BehaviorTree.h"

#include "../Tasks/Wait.h"
#include "../Decorators/IsSet.h"

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

        Tree->RootNode = RootSelector;
        Tree->AllNodes.push_back(RootSelector);

        return Tree;
    }
};

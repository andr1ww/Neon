#include "pch.h"
#include "../../../Header/BehaviorTree/BT_MANG2/BT_MANG2.h"
#include "FortniteGame/BehaviorTree/Header/BehaviorTree/Tasks/Wait.h"

BehaviorTree* BT_MANG2::ConstructTree(BTContext Context, bool bAddToBots)
{
    auto* Tree = new BehaviorTree();

    auto* RootSelector = new BTComposite_Selector();
    RootSelector->NodeName = "On Ground";

    {
        auto* Task = new BTTask_Wait(2.f);
        //Task->NodeName = "Test";
        RootSelector->AddChild(Task);
    }

    Tree->RootNode = RootSelector;
    Tree->AllNodes.push_back(RootSelector);

    return Tree;
}
#pragma once
#include "pch.h"
#include "../../BehaviorTree.h"

class BT_MANG2
{
public:
    static TArray<BTContext*> Bots;
public:
    static BehaviorTree* ConstructTree(BTContext Context, bool bAddToBots);
};

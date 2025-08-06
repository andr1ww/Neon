#pragma once

#include "pch.h"
#include "BehaviorTree.h"

namespace BehaviorTreeService
{
    struct BotBehavior
    {
        BehaviorTree* BehaviorTree = nullptr;
        BTContext Context = {};
    };
    inline TArray<BotBehavior> AIToTick;
    
    inline void TickAI()
    {
        if (AIToTick.Num() > 0)
        {
            for (int i = 0; i < AIToTick.Num(); i++)
            {
                if (!AIToTick.IsValidIndex(i))
                {
                    continue;
                }
                BotBehavior AI = AIToTick[i];
                if (!AI.BehaviorTree)
                {
                    UE_LOG(LogNeon, Warning, "AI->BehaviorTree is null!");
                    continue;
                }

                AI.BehaviorTree->Tick(AI.Context);
            }
        }
    }
}
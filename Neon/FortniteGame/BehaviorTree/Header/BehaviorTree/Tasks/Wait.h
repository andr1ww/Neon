#pragma once
#include "pch.h"
#include "../../BehaviorTree.h"

class BTTask_Wait : public BTNode {
public:
    float WaitTime = 0.f;
    float WorldWaitTime = 0.f;
    bool bFinishedWait = false;
public:
    BTTask_Wait(float InWaitTime) {
        WaitTime = InWaitTime;
        WorldWaitTime = UGameplayStatics::GetTimeSeconds(UWorld::GetWorld()) + InWaitTime;
    }

    EBTNodeResult ChildTask(BTContext Context) override;
};
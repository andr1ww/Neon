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

    EBTNodeResult ChildTask(BTContext Context) override
    {
        if (WaitTime == 0.f || WorldWaitTime == 0.f) {
            return EBTNodeResult::Failed;
        }
        if (UGameplayStatics::GetTimeSeconds(UWorld::GetWorld()) >= WorldWaitTime) {
            if (bFinishedWait) {
                WorldWaitTime = UGameplayStatics::GetTimeSeconds(UWorld::GetWorld()) + WaitTime;
                bFinishedWait = false;
                return EBTNodeResult::InProgress;
            }
            bFinishedWait = true;
            if (!NodeName.empty())
            {
                UE_LOG(LogNeon, Log, "BTTask_Wait Finished Wait For NodeName: '%s'", NodeName.c_str());
            }
            return EBTNodeResult::Succeeded;
        }
        return EBTNodeResult::InProgress;
    }
};
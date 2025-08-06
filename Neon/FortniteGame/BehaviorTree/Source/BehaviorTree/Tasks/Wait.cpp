#include "pch.h"
#include "../../../Header/BehaviorTree/Tasks/Wait.h"

EBTNodeResult BTTask_Wait::ChildTask(BTContext Context) {
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
#pragma once
#include "pch.h"

#include "../../BehaviorTree.h"

class BTTask_BotMoveTo : public BTNode {
public:
    float AcceptableRadius = 100.f;
    bool bAllowStrafe = true;
    bool bStopOnOverlapNeedsUpdate = false;
    bool bUsePathfinding = false;
    bool bProjectDestinationToNavigation = false;
    bool bAllowPartialPath = false;

    bool bShouldSetFocalPoint = true;
    
    FName SelectedKeyName;
    FName MovementResultKey;
public:
    EBTNodeResult ChildTask(BTContext Context) override {
        if (!Context.Controller) {
            return EBTNodeResult::Failed;
        }

        FVector Dest = Context.Controller->GetBlackboard()->GetValueAsVector(SelectedKeyName);
        if (Dest.IsZero()) return EBTNodeResult::Failed;
        
        if (bShouldSetFocalPoint)
        {
            Context.Controller->K2_SetFocalPoint(Dest);
        }
        EPathFollowingRequestResult RequestResult = Context.Controller->MoveToLocation(Dest, AcceptableRadius, bStopOnOverlapNeedsUpdate, bUsePathfinding, bProjectDestinationToNavigation, bAllowStrafe, nullptr, bAllowPartialPath);
        if (MovementResultKey.IsValid())
        {
            Context.Controller->GetBlackboard()->SetValueAsEnum(MovementResultKey, (uint8)RequestResult);
        }
        if (RequestResult == EPathFollowingRequestResult::Failed) {
            return EBTNodeResult::Failed;
        } 

        if (RequestResult == EPathFollowingRequestResult::RequestSuccessful) {
            return EBTNodeResult::InProgress;
        }

        return EBTNodeResult::Succeeded;
    }
};
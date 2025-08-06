#pragma once
#include "pch.h"

#include "../../BehaviorTree.h"

class BTTask_SteerMovement : public BTNode {
public:
    float RandDirOffset = 600.f;
    float DirectionChangeInterval = 1.f;
    float NextDirectionChangeTime = 0.f;
public:
    BTTask_SteerMovement(float Offset = 600.f, float DestChangeInterval = 1.f)
    {
        RandDirOffset = Offset;
        DirectionChangeInterval = DestChangeInterval;
    }
    
    virtual EBTNodeResult ChildTask(BTContext Context) override {
        if (!Context.Pawn || !Context.Controller) {
            return EBTNodeResult::Failed;
        }
        float CurrentTime = UGameplayStatics::GetTimeSeconds(UWorld::GetWorld());

        if (DirectionChangeInterval == 0.f || CurrentTime >= NextDirectionChangeTime)
        {
            FVector OffsetLoc = Context.Pawn->GetActorLocation();
            OffsetLoc.X += UKismetMathLibrary::RandomFloatInRange((RandDirOffset * -1.f), RandDirOffset);
            OffsetLoc.Y += UKismetMathLibrary::RandomFloatInRange((RandDirOffset * -1.f), RandDirOffset);

            EPathFollowingRequestResult Result = Context.Controller->MoveToLocation(OffsetLoc, (RandDirOffset / 10), false, false, false, true, nullptr,true);
            NextDirectionChangeTime = CurrentTime + DirectionChangeInterval;

            if (Result == EPathFollowingRequestResult::AlreadyAtGoal)
            {
                return EBTNodeResult::Succeeded;
            } else if (Result == EPathFollowingRequestResult::RequestSuccessful)
            {
                return EBTNodeResult::InProgress;
            }
        } else
        {
            return EBTNodeResult::Succeeded;
        }
        
        return EBTNodeResult::Failed;
    }
};
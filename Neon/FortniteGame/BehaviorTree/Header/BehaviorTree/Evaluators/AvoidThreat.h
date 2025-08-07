#pragma once
#include "pch.h"

#include "../../BehaviorTree.h"

class BTEvaluator_AvoidThreat : public BTService {
public:
    float RandDirOffset = 1500.f;
    float DirectionChangeInterval = 5.f;
    float NextDirectionChangeTime = 0.f;
public:
    BTEvaluator_AvoidThreat(float InDirectionChangeInterval = 5.f, float InOffset = 1500.f) {
        NodeName = "Evaluating...AvoidThreat";

        Interval = 1.f;
        DirectionChangeInterval = InDirectionChangeInterval;
        RandDirOffset = InOffset;
    }

    void TickService(BTContext Context) override {
        if (!Context.Pawn || !Context.Controller) return;

        if (Context.Controller->GetCurrentAlertLevel() != EAlertLevel::Threatened)
        {
            Context.Controller->GetBlackboard()->SetValueAsEnum(UKismetStringLibrary::Conv_StringToName(L"AIEvaluator_AvoidThreat_ExecutionStatus"), (uint8)EExecutionStatus::ExecutionDenied);
            return;
        }

        float CurrentTime = UGameplayStatics::GetTimeSeconds(UWorld::GetWorld());
        if (CurrentTime >= NextDirectionChangeTime)
        {
            FVector AvoidDirection = Context.Pawn->K2_GetActorLocation();
            AvoidDirection.X += UKismetMathLibrary::RandomFloatInRange((RandDirOffset * -1.f), RandDirOffset);
            AvoidDirection.Y += UKismetMathLibrary::RandomFloatInRange((RandDirOffset * -1.f), RandDirOffset);

            Context.Controller->GetBlackboard()->SetValueAsVector(UKismetStringLibrary::Conv_StringToName(L"AIEvaluator_AvoidThreat_Destination"), AvoidDirection);
            Context.Controller->GetBlackboard()->SetValueAsEnum(UKismetStringLibrary::Conv_StringToName(L"AIEvaluator_AvoidThreat_ExecutionStatus"), 4);
            NextDirectionChangeTime = CurrentTime + DirectionChangeInterval;
        }
    }
};
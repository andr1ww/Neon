#pragma once
#include "pch.h"

#include "../../BehaviorTree.h"

class BTEvaluator_CharacterLaunched : public BTService {
public:
    BTEvaluator_CharacterLaunched() {
        Name = "FortAthenaBTService_AIEvaluator_39";
        NodeName = "Evaluating...Character Launched";
    }

    void TickService(BTContext Context) override {
        if (!Context.Pawn || !Context.Controller) return;

        if (Context.Pawn->GetMovementComponent()) {
            if (Context.Pawn->GetMovementComponent()->IsFalling()) {
                Context.Controller->GetBlackboard()->SetValueAsEnum(UKismetStringLibrary::Conv_StringToName(L"AIEvaluator_CharacterLaunched_ExecutionStatus"), (uint8)EExecutionStatus::ExecutionAllowed);
            }
            else {
                Context.Controller->GetBlackboard()->SetValueAsEnum(UKismetStringLibrary::Conv_StringToName(L"AIEvaluator_CharacterLaunched_ExecutionStatus"), (uint8)EExecutionStatus::ExecutionDenied);
            }
        } else
        {
            UE_LOG(LogNeon, Warning, "Bot has no MovementComponent!");
        }
    }
};
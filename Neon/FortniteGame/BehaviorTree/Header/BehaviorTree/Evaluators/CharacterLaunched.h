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

        UPawnMovementComponent* MovementComp = Context.Pawn->GetMovementComponent();
        if (MovementComp) {
            if (MovementComp->IsFalling()) {
                Context.Controller->GetBlackboard()->SetValueAsEnum(UKismetStringLibrary::Conv_StringToName(L"AIEvaluator_CharacterLaunched_ExecutionStatus"), (uint8)EExecutionStatus::ExecutionAllowed);
            }
            else {
                Context.Controller->GetBlackboard()->SetValueAsEnum(UKismetStringLibrary::Conv_StringToName(L"AIEvaluator_CharacterLaunched_ExecutionStatus"), (uint8)EExecutionStatus::ExecutionDenied);
            }
        }
    }
};
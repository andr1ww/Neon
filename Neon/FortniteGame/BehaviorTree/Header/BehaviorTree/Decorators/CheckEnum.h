#pragma once
#include "pch.h"

#include "../../BehaviorTree.h"

class BTDecorator_CheckEnum : public BTDecorator {
public:
    FName SelectedKeyName;
    int IntValue = 0;
    EBlackboardCompareOp Operator = EBlackboardCompareOp::Equal;
public:
    virtual bool Evaluate(BTContext Context) override {
        if (!Context.Controller || !Context.Controller->GetBlackboard()) {
            return false;
        }

        int32 BBValue = Context.Controller->GetBlackboard()->GetValueAsEnum(SelectedKeyName);

        switch (Operator)
        {
        case EBlackboardCompareOp::Equal:               return BBValue == IntValue;
        case EBlackboardCompareOp::NotEqual:            return BBValue != IntValue;
        case EBlackboardCompareOp::LessThan:            return BBValue < IntValue;
        case EBlackboardCompareOp::LessThanOrEqual:     return BBValue <= IntValue;
        case EBlackboardCompareOp::GreaterThan:         return BBValue > IntValue;
        case EBlackboardCompareOp::GreaterThanOrEqual:  return BBValue >= IntValue;
        default:                                        return false;
        }
    }
};
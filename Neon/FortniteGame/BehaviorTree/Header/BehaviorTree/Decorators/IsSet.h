#pragma once
#include "pch.h"
#include "../../BehaviorTree.h"

class BTDecorator_IsSet : public BTDecorator {
public:
    FName SelectedKeyName;
public:
    virtual bool Evaluate(BTContext Context) override {
        if (!Context.Controller || !Context.Controller->GetBlackboard()) {
            return false;
        }

        const bool BBValue = Context.Controller->GetBlackboard()->GetValueAsBool(SelectedKeyName);
        if (BBValue) {
            return true;
        }
    }
};
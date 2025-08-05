#include "pch.h"
#include "../Header/BehaviorTree.h"

EBTNodeResult BTNode::Tick(BTContext& Context) {
    float CurrentTime = UGameplayStatics::GetTimeSeconds(UWorld::GetWorld());

    // If a decorator fails then we shouldnt execute the task
    for (BTDecorator* Decorator : Decorators) {
        if (!Decorator->Evaluate(Context)) {
            return EBTNodeResult::Failed;
        }
    }

    for (BTService* Service : Services) {
        if (Service->Interval == 0.f || CurrentTime >= Service->NextTickTime) {
            Service->TickService(Context);
            Service->NextTickTime = CurrentTime + Service->Interval;
        }
    }

    // Run the task once all of the decorators pass
    return ChildTask(Context);
}

EBTNodeResult BTComposite_Selector::Tick(BTContext Context) {
    float CurrentTime = UGameplayStatics::GetTimeSeconds(UWorld::GetWorld());

    // If a global selector decorator fails we shouldnt execute anything inside of the selector (shouldnt be on the root)
    for (BTDecorator* Decorator : Decorators) {
        if (!Decorator->Evaluate(Context)) {
            return EBTNodeResult::Failed;
        }
    }

    for (BTService* Service : Services) {
        if (Service->Interval == 0.f || CurrentTime >= Service->NextTickTime) {
            Service->TickService(Context);
            Service->NextTickTime = CurrentTime + Service->Interval;
        }
    }

    // Run all of the selectors children then if all fail then return faliure
    for (BTNode* Child : Children)
    {
        EBTNodeResult Result = Child->Tick(Context);
        if (Result == EBTNodeResult::Succeeded || Result == EBTNodeResult::InProgress)
            return Result;
    }
    return EBTNodeResult::Failed;
}
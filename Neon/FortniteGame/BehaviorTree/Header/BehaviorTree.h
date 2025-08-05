#pragma once
#include "pch.h"
#include "FortniteGame/FortPlayerController/Header/FortPlayerController.h"
#include "FortniteGame/FortPlayerPawn/Header/FortPlayerPawn.h"
#include "FortniteGame/FortPlayerState/Header/FortPlayerState.h"
#include "FortniteGame/FortServerBotManager/Header/FortServerBotManager.h"
#include "Engine/GameplayStatics/Header/GameplayStatics.h"

enum class EBlackboardCompareOp
{
    Equal,
    NotEqual,
    LessThan,
    LessThanOrEqual,
    GreaterThan,
    GreaterThanOrEqual
};

struct BTContext
{
    AFortAthenaAIBotController* Controller;
    AFortPlayerPawn* Pawn;
    AFortPlayerStateAthena* PlayerState;
};

class BTDecorator {
public:
    // All of these strings dont need to have a value its just good for runtime data
    std::string Name;
    std::string CachedDescription;
    std::string NodeName;

public:
    virtual bool Evaluate(BTContext Context) = 0;
};

class BTService {
public:
    std::string Name;
    std::string NodeName;

    float Interval = 0.f;
    float NextTickTime = 0.f;
public:
    virtual void TickService(BTContext Context) = 0;
};

class BTNode
{
private:
    std::vector<BTDecorator*> Decorators;
    std::vector<BTService*> Services;
public:
    virtual EBTNodeResult ChildTask(BTContext Context) = 0;
public:
    void AddDecorator(BTDecorator* Decorator) {
        Decorators.push_back(Decorator);
    }

    void AddService(BTService* Service) {
        Services.push_back(Service);
    }

    EBTNodeResult Tick(BTContext& Context);
};

class BTComposite_Selector
{
private:
    std::vector<BTNode*> Children;
    std::vector<BTDecorator*> Decorators;
    std::vector<BTService*> Services;

public:
    std::string Name;
	std::string NodeName;
public:
    void AddChild(BTNode* Node) {
        Children.push_back(Node);
    }

    void AddDecorator(BTDecorator* Decorator) {
        Decorators.push_back(Decorator);
    }

    void AddService(BTService* Service) {
        Services.push_back(Service);
    }

    virtual EBTNodeResult Tick(BTContext Context);
};

class BehaviorTree
{
public:
    std::string Name;

    std::vector<BTComposite_Selector*> AllNodes;
    BTComposite_Selector* RootNode = nullptr;
    UBlackboardData* BlackboardAsset = nullptr;

    BTComposite_Selector* FindSelectorByName(std::string Name) {
        for (BTComposite_Selector* Selector : AllNodes) {
            if (Selector->Name == Name || Selector->NodeName == Name) {
                return Selector;
            }
        }

		UE_LOG(LogNeon, Warning, "Selector with name '%s' not found!", Name);
        return nullptr;
    }

    void Tick(BTContext Context) {
        if (RootNode)
            RootNode->Tick(Context);
    }

    ~BehaviorTree() {
        for (auto* Node : AllNodes)
            delete Node;
    }
};
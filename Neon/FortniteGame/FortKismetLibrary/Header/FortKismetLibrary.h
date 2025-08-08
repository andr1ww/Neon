#pragma once
#include "pch.h"

#include "FortniteGame/BuildingSMActor/Header/BuildingSMActor.h"
#include "FortniteGame/FortInventory/Header/FortInventory.h"

class AFortPlayerStateAthena;

class UFortResourceItemDefinition : public UFortWorldItemDefinition
{
public:
    DECLARE_STATIC_CLASS(UFortResourceItemDefinition);
    DECLARE_DEFAULT_OBJECT(UFortResourceItemDefinition);
};

class UFortTrapItemDefinition : public UFortWorldItemDefinition
{
public:
    DECLARE_STATIC_CLASS(UFortTrapItemDefinition);
    DECLARE_DEFAULT_OBJECT(UFortTrapItemDefinition);
};

class UFortBuildingItemDefinition : public UFortWorldItemDefinition
{
public:
    DECLARE_STATIC_CLASS(UFortBuildingItemDefinition);
    DECLARE_DEFAULT_OBJECT(UFortBuildingItemDefinition);
};

class UFortAmmoItemDefinition : public UFortWorldItemDefinition
{
public:
    DECLARE_STATIC_CLASS(UFortAmmoItemDefinition);
    DECLARE_DEFAULT_OBJECT(UFortAmmoItemDefinition);
};

class UFortConsumableItemDefinition : public UFortWorldItemDefinition
{
public:
    DECLARE_STATIC_CLASS(UFortConsumableItemDefinition);
    DECLARE_DEFAULT_OBJECT(UFortConsumableItemDefinition);
};

class UFortKismetLibrary : public UObject
{
public:
    static UFortResourceItemDefinition* K2_GetResourceItemDefinition(const EFortResourceType ResourceType);
    static void UpdatePlayerCustomCharacterPartsVisualization(AFortPlayerStateAthena* PlayerState)
    {
        static SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortKismetLibrary", "UpdatePlayerCustomCharacterPartsVisualization");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return;

        static UObject* Object = nullptr;
        if (!Object) Object = SDK::StaticClassImpl("FortKismetLibrary")->GetClassDefaultObject();
    
        Object->ProcessEvent(Func, &PlayerState);
    }
public:
    DECLARE_STATIC_CLASS(UFortKismetLibrary);
    DECLARE_DEFAULT_OBJECT(UFortKismetLibrary);
};
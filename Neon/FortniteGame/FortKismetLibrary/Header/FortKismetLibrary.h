#pragma once
#include "pch.h"

#include "FortniteGame/BuildingSMActor/Header/BuildingSMActor.h"
#include "FortniteGame/FortInventory/Header/FortInventory.h"

class UFortResourceItemDefinition : public UFortWorldItemDefinition
{
    
};

class UFortKismetLibrary : public UObject
{
public:
    static UFortResourceItemDefinition* K2_GetResourceItemDefinition(const EFortResourceType ResourceType);
public:
    DECLARE_STATIC_CLASS(UFortKismetLibrary);
    DECLARE_DEFAULT_OBJECT(UFortKismetLibrary);
};
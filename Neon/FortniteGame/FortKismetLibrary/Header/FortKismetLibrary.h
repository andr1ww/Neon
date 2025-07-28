#pragma once
#include "pch.h"

#include "FortniteGame/BuildingSMActor/Header/BuildingSMActor.h"

enum class EFortResourceType : uint8
{
    Wood                                     = 0,
    Stone                                    = 1,
    Metal                                    = 2,
    Permanite                                = 3,
    GoldCurrency                             = 4,
    Ingredient                               = 5,
    None                                     = 6,
    EFortResourceType_MAX                    = 7,
};

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
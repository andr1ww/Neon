#pragma once
#include "pch.h"

#include "FortniteGame/BuildingSMActor/Header/BuildingSMActor.h"
#include "FortniteGame/FortInventory/Header/FortInventory.h"

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

class UFortWeaponRangedItemDefinition : public UFortWorldItemDefinition
{
public:
    DECLARE_STATIC_CLASS(UFortWeaponRangedItemDefinition);
    DECLARE_DEFAULT_OBJECT(UFortWeaponRangedItemDefinition);
};

class UFortKismetLibrary : public UObject
{
public:
    static UFortResourceItemDefinition* K2_GetResourceItemDefinition(const EFortResourceType ResourceType);
public:
    DECLARE_STATIC_CLASS(UFortKismetLibrary);
    DECLARE_DEFAULT_OBJECT(UFortKismetLibrary);
};
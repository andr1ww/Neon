#pragma once
#include "pch.h"

#include "Engine/Array/Header/Array.h"
#include "FortniteGame/Common/Header/ItemDefinition.h"
#include "FortniteGame/FortPlayerController/Header/FortPlayerController.h"

class AFortPlayerControllerAthena;

struct FFortItemEntry : public FFastArraySerializerItem
{
    DEFINE_MEMBER(int32, FFortItemEntry, Count);
    DEFINE_MEMBER(int32, FFortItemEntry, LoadedAmmo);
    DEFINE_MEMBER(int32, FFortItemEntry, Level);
    DEFINE_PTR(UFortItemDefinition, FFortItemEntry, ItemDefinition);
};

class UFortItem : public UObject
{
public:
    void SetOwningControllerForTemporaryItem(AFortPlayerControllerAthena* InController);
};

struct FFortItemList : public FFastArraySerializer
{
    DEFINE_MEMBER(TArray<UFortItem*>, FFortItemList, ItemInstances);
    DEFINE_MEMBER(TArray<FFortItemEntry>, FFortItemList, ReplicatedEntries);
};

class UFortWorldItem : public UFortItem
{
public:
    DEFINE_MEMBER(FFortItemEntry, UFortWorldItem, ItemEntry);
};

class AFortInventory : public AActor
{
public:
    DEFINE_MEMBER(FFortItemList, AFortInventory, Inventory);
    DEFINE_BOOL(AFortInventory, bRequiresLocalUpdate);
public:
    static void HandleInventoryLocalUpdate();
    static void Update(AFortPlayerControllerAthena* PlayerController, FFortItemEntry* Entry);
    static UObject* GiveItem(AFortPlayerControllerAthena* PlayerController, UFortItemDefinition* Def, int32 Count, int LoadedAmmo, int32 Level);
};

#pragma once
#include "pch.h"
#include <map>
#include <numeric>

#include "Engine/NetDriver/Header/NetDriver.h"
#include "FortniteGame/FortInventory/Header/FortInventory.h"
#include "FortniteGame/Common/Header/ItemDefinition.h"

class UFortWeaponItemDefinition;

struct FFortLootTierData
{
    DEFINE_MEMBER(float, FFortLootPackageData, Weight);
    DEFINE_MEMBER(FName, FFortLootTierData, LootPackage);
    DEFINE_MEMBER(int32, FFortLootTierData, LootTier);
    DEFINE_MEMBER(FName, FFortLootTierData, TierGroup);
    DEFINE_MEMBER(float, FFortLootTierData, NumLootPackageDrops);
    DEFINE_MEMBER(TArray<int32>, FFortLootTierData, LootPackageCategoryMinArray)
};

struct alignas(0x08) FTableRowBase
{
    public:
    uint8                                         Pad_0[0x8];
};

class UFortWeaponRangedItemDefinition : public UFortWeaponItemDefinition
{
public:
    DECLARE_STATIC_CLASS(UFortWeaponRangedItemDefinition);
    DECLARE_DEFAULT_OBJECT(UFortWeaponRangedItemDefinition);
};

struct FFortLootLevelData final : public FTableRowBase
{
public:
    class FName                                   category;
    int32                                         LootLevel;
    int32                                         MinItemLevel;
    int32                                         MaxItemLevel;
};

struct FFortLootPackageData
{
    DEFINE_MEMBER(float, FFortLootPackageData, Weight);
    DEFINE_MEMBER(TSoftObjectPtr<UFortItemDefinition>, FFortLootPackageData, ItemDefinition);
    DEFINE_MEMBER(FName, FFortLootPackageData, LootPackageID);
    DEFINE_MEMBER(int32, FFortLootPackageData, LootPackageCategory);
    DEFINE_MEMBER(int32, FFortLootPackageData, MinWorldLevel);
    DEFINE_MEMBER(int32, FFortLootPackageData, Count);
    DEFINE_MEMBER(int32, FFortLootPackageData, MaxWorldLevel);
    DEFINE_MEMBER(FString, FFortLootPackageData, LootPackageCall);
};

class ABuildingContainer : public AActor
{
public:
    DEFINE_MEMBER(FVector, ABuildingContainer, LootSpawnLocation_Athena);
    DEFINE_BOOL(ABuildingContainer, bAlreadySearched);
    DEFINE_MEMBER(FName, ABuildingContainer, SearchLootTierGroup);
public:
    DECLARE_STATIC_CLASS(ABuildingContainer)
    DECLARE_DEFAULT_OBJECT(ABuildingContainer)
};

class UGameFeatureData : public UDataAsset
{
public:
    DECLARE_STATIC_CLASS(UGameFeatureData)
    DECLARE_DEFAULT_OBJECT(UGameFeatureData)
};

class UFortGameFeatureData : public UGameFeatureData
{
public:
    DECLARE_STATIC_CLASS(UFortGameFeatureData)
    DECLARE_DEFAULT_OBJECT(UFortGameFeatureData)
};

struct FFortGameFeatureLootTableData final
{
public:
    DEFINE_PTR(UDataTable, FFortGameFeatureLootTableData, LootTierData);
    DEFINE_PTR(UDataTable, FFortGameFeatureLootTableData, LootPackageData);
};

enum class EFortQuickBars : uint8
{
    Primary                                  = 0,
    Secondary                                = 1,
    Creative                                 = 2,
    Max_None                                 = 3,
    EFortQuickBars_MAX                       = 4,
};

namespace FortLootPackage
{
    struct FNeonLootImproper
    {
        UFortWorldItemDefinition* ItemDefinition;
        int32 Count;
        int32 LoadedAmmo;
        
        FNeonLootImproper(UFortWorldItemDefinition* ItemDef, int32 ItemCount, int32 Ammo = 0)
            : ItemDefinition(ItemDef), Count(ItemCount), LoadedAmmo(Ammo) {}
    };

    EFortQuickBars GetQuickbar(UFortItemDefinition* ItemDefinition);
    
    template <typename T>
    static T* PickWeighted(TArray<T*>& Map, float (*RandFunc)(float), bool bCheckZero = true);

    extern TArray<FFortLootTierData*> TierDataAllGroups;
    extern TArray<FFortLootPackageData*> LPGroupsAll;
    bool IsValidPointer(void* ptr);

    void SetupLDSForPackage(TArray<FNeonLootImproper>& LootDrops, FName Package, int i, FName TierGroup, int WorldLevel, int RecursionDepth = 0);
    void SetupLootGroups(AFortGameStateAthena* GameState);
    TArray<FNeonLootImproper> PickLootDrops(FName TierGroup, int LootTier = -1, int WorldLevel = -1);
    bool SpawnLoot(ABuildingContainer* Container);
}
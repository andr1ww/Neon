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

// NumValues: 0x0007
enum class ETInteractionType : uint8
{
    IT_NoInteraction                         = 0,
    IT_Simple                                = 1,
    IT_LongPress                             = 2,
    IT_BuildingEdit                          = 3,
    IT_BuildingImprovement                   = 4,
    IT_TrapPlacement                         = 5,
    IT_MAX                                   = 6,
};

// Enum FortniteGame.EInteractionBeingAttempted
// NumValues: 0x0004
enum class EInteractionBeingAttempted : uint8
{
    FirstInteraction                         = 0,
    SecondInteraction                        = 1,
    AllInteraction                           = 2,
    EInteractionBeingAttempted_MAX           = 3,
};


struct FInteractionType final
{
public:
    ETInteractionType                             InteractionType;                                   // 0x0000(0x0001)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    EInteractionBeingAttempted                    InteractionBeingAttempted;                         // 0x0001(0x0001)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    uint8                                         Pad_2[0x2];                                        // 0x0002(0x0002)(Fixing Size After Last Property [ Dumper-7 ])
    AFortPlayerPawn*         RequestingPawn;                                    // 0x0004(0x0008)(ZeroConstructor, IsPlainOldData, NoDestructor, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    TWeakObjectPtr<class AFortPlayerController>   RequestingPlayerController;                        // 0x000C(0x0008)(ZeroConstructor, IsPlainOldData, NoDestructor, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    uint8                                         Pad_14[0x8];                                       // 0x0014(0x0008)(Fixing Size After Last Property [ Dumper-7 ])
    TWeakObjectPtr<class UPrimitiveComponent>     InteractComponent;                                 // 0x001C(0x0008)(ExportObject, ZeroConstructor, InstancedReference, IsPlainOldData, NoDestructor, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    TWeakObjectPtr<class UObject>                 OptionalObjectData;                                // 0x0024(0x0008)(ZeroConstructor, IsPlainOldData, NoDestructor, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    struct FVector                                InteractPoint;                                     // 0x002C(0x000C)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
};

class ABuildingContainer : public AActor
{
public:
    DEFINE_MEMBER(FVector, ABuildingContainer, LootSpawnLocation_Athena);
    DEFINE_BOOL(ABuildingContainer, bAlreadySearched);
    DEFINE_MEMBER(FName, ABuildingContainer, SearchLootTierGroup);
public:
    bool BP_SpawnLoot(AFortPlayerPawn* PlayerPawn) {
        static class UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("BuildingContainer", "BP_SpawnLoot");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return false;

        struct BuildingContainer_BP_SpawnLoot final
        {
        public:
            AFortPlayerPawn* PlayerPawn;
            bool ReturnValue;
        };
        BuildingContainer_BP_SpawnLoot Params;
        Params.PlayerPawn = PlayerPawn;

        this->ProcessEvent(Func, &Params);

        return Params.ReturnValue;
    }
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
    typedef TMap<struct FGameplayTag, struct FFortGameFeatureLootTableData> AnotherMap;
    DEFINE_MEMBER(AnotherMap, UFortGameFeatureData, PlaylistOverrideLootTableData);
public:
    DECLARE_STATIC_CLASS(UFortGameFeatureData)
    DECLARE_DEFAULT_OBJECT(UFortGameFeatureData)
};

struct FFortGameFeatureLootTableData final
{
public:
    DEFINE_MEMBER(TSoftObjectPtr<UDataTable>, FFortGameFeatureLootTableData, LootTierData);
    DEFINE_MEMBER(TSoftObjectPtr<UDataTable>, FFortGameFeatureLootTableData, LootPackageData);
};

enum class EFortQuickBars : uint8
{
    Primary                                  = 0,
    Secondary                                = 1,
    Creative                                 = 2,
    Max_None                                 = 3,
    EFortQuickBars_MAX                       = 4,
};

class UFortControllerComponent_Interaction : public UActorComponent
{
public:
    DECLARE_STATIC_CLASS(UFortControllerComponent_Interaction)
    DECLARE_DEFAULT_OBJECT(UFortControllerComponent_Interaction)
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

    void SetupLDSForPackage(TArray<FNeonLootImproper>& LootDrops, FName Package, int i, FName TierGroup, int WorldLevel, int RecursionDepth = 0);
    void SetupLootGroups(AFortGameStateAthena* GameState);
    TArray<FNeonLootImproper> PickLootDrops(FName TierGroup, int LootTier = -1, int WorldLevel = -1);
    bool SpawnLoot(ABuildingContainer* Container);
    void SpawnFloorLootForContainer(UBlueprintGeneratedClass* ContainerType);
    void InternalSpawnLoot(FName& TierGroup, FVector Loc, const TArray<FNeonLootImproper>& PrecomputedLoot);
    bool ServerOnAttemptInteract(ABuildingContainer* BuildingContainer, FInteractionType TYPE);
    inline void (*ServerAttemptInteractOG)(UFortControllerComponent_Interaction* Component, FFrame& Stack);   
void ServerAttemptInteract(UFortControllerComponent_Interaction* Component, FFrame& Stack);
}
#pragma once
#include "pch.h"
#include "Neon/Runtime/Runtime.h"
#include "Engine/Float/Header/Float.h"
#include "Engine/ObjectPtr/Header/ObjectPtr.h"


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

struct FGameplayTag
{
public:
    class FName                                   TagName;                                           // 0x0000(0x0004)(Edit, ZeroConstructor, EditConst, SaveGame, IsPlainOldData, NoDestructor, Protected, HasGetValueTypeHash, NativeAccessSpecifierProtected)
};

struct FSimpleCurveKey
{
    float                                              Time;                                                     // 0x0000(0x0004) (Edit, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    float                                              Value;                                                    // 0x0004(0x0004) (Edit, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
};

enum class EFortItemEntryState : uint8_t 
{
    NoneState = 0,
    NewItemCount = 1,
    ShouldShowItemToast = 2,
    DurabilityInitialized = 3,
    DoNotShowSpawnParticles = 4,
    FromRecoveredBackpack = 5,
    FromGift = 6,
    PendingUpgradeCriteriaProgress = 7,
    OwnerBuildingHandle = 8,
    FromDroppedPickup = 9,
    JustCrafted = 10,
    CraftAndSlotTarget = 11,
    GenericAttributeValueSet = 12,
    PickupInstigatorHandle = 13,
    CreativeUserPrefabHasContent = 14,
    EFortItemEntryState_MAX = 15
};

struct FFortItemEntryStateValue
{
public:
    DEFINE_MEMBER(EFortItemEntryState, FFortItemEntryStateValue, StateType);
    DEFINE_MEMBER(int, FFortItemEntryStateValue, IntValue); 
};

struct FFortBaseWeaponStats 
{
    DEFINE_MEMBER(int, FFortBaseWeaponStats, ClipSize);
};

struct FIndexedCurve
{

};

struct FRealCurve : public FIndexedCurve
{
};

struct FSimpleCurve : public FRealCurve
{
public:
    DEFINE_MEMBER(TArray<FSimpleCurveKey>, FSimpleCurve, Keys); 
};

enum class EFortRarity : uint8
{
    Common                                   = 0,
    Uncommon                                 = 1,
    Rare                                     = 2,
    Epic                                     = 3,
    Legendary                                = 4,
    Mythic                                   = 5,
    Transcendent                             = 6,
    Unattainable                             = 7,
    NumRarityValues                          = 8,
    EFortRarity_MAX                          = 9,
};

class UFortWorldItemDefinition;

class UFortItemDefinition : public UObject
{
public:
    DEFINE_MEMBER(EFortRarity, UFortItemDefinition, Rarity);
    class UFortWorldItem* CreateTemporaryItemInstanceBP(int32 Count, int32 Level)
    {
        static UFunction* Func = Runtime::StaticFindObject<UFunction>("/Script/FortniteGame.FortItemDefinition.CreateTemporaryItemInstanceBP");

        struct FortItemDefinition_CreateTemporaryItemInstanceBP final
        {
        public:
            int32                                         Count;                                             // 0x0000(0x0004)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            int32                                         Level;                                             // 0x0004(0x0004)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            UFortWorldItem* ReturnValue;                                       // 0x0008(0x0008)(Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
        };

        FortItemDefinition_CreateTemporaryItemInstanceBP Parms{};
        Parms.Count = Count;
        Parms.Level = Level;

        auto Flgs = Func->FunctionFlags();
        Func->FunctionFlags() |= 0x400;

        this->ProcessEvent(Func, &Parms);

        Func->FunctionFlags() = Flgs;

        return Parms.ReturnValue;
    }

    class UFortWorldItemDefinition* GetAmmoWorldItemDefinition_BP()
    {
        static UFunction* Func = Runtime::StaticFindObject<UFunction>("/Script/FortniteGame.FortWorldItemDefinition.GetAmmoWorldItemDefinition_BP");

        struct FortItemDefinition_CreateTemporaryItemInstanceBP final
        {
        public:
            UFortWorldItemDefinition* ReturnValue;                                       // 0x0008(0x0008)(Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
        };

        FortItemDefinition_CreateTemporaryItemInstanceBP Parms{};

        this->ProcessEvent(Func, &Parms);

        return Parms.ReturnValue;
    }

    bool IsStackable()
    {
        static SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortItemDefinition", "IsStackable");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return false;

        struct FortItemDefinition_CreateTemporaryItemInstanceBP final
        {
        public:
            bool ReturnValue;                                       // 0x0008(0x0008)(Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
        };
        
        FortItemDefinition_CreateTemporaryItemInstanceBP Params;
        
        this->ProcessEvent(Func, &Params);
        
        return Params.ReturnValue;
    }

    float GetMaxStackSize()
    {
        bool bIsScalableFloat = Fortnite_Version >= 12.00; 

        if (!bIsScalableFloat)
        {
            static auto MaxStackSize = this->Get<uintptr_t>("FortItemDefinition", "MaxStackSize");
            return static_cast<float>(MaxStackSize);
        }
    
        static auto MaxStackSize = this->Get<FScalableFloat>("FortItemDefinition", "MaxStackSize");
    
        static auto AthenaGameData = Runtime::StaticFindObject<UDataTable>("/Game/Athena/Balance/DataTables/AthenaGameData.AthenaGameData");

        auto& RowMap = AthenaGameData->GetRowMap();

        if (MaxStackSize.Curve.RowName.GetComparisonIndex() == 0)
            return MaxStackSize.Value;

        FSimpleCurve* Curve = nullptr;

        for (auto& Pair : RowMap)
        {
            if (Pair.Key.GetComparisonIndex() == MaxStackSize.Curve.RowName.GetComparisonIndex())
            {
                Curve = (FSimpleCurve*)Pair.Value;
                break;
            }
        }

        if (!Curve)
            return 1;

        return Curve->GetKeys()[0].Value;
    }
public:
    DEFINE_BOOL(FFortItemEntry, bAllowMultipleStacks);
public:
    DECLARE_STATIC_CLASS(UFortItemDefinition)
    DECLARE_DEFAULT_OBJECT(UFortItemDefinition)
};

struct FDataTableCategoryHandle final
{
public:
    DEFINE_PTR(UDataTable, FDataTableCategoryHandle, DataTable); 
    class FName                                   ColumnName;                                        // 0x0008(0x0004)(Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    class FName                                   RowContents;                                       // 0x000C(0x0004)(Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
};

class UFortWorldItemDefinition : public UFortItemDefinition
{
public:
    DEFINE_MEMBER(FDataTableCategoryHandle, UFortWorldItemDefinition, LootLevelData);
    DEFINE_MEMBER(int32, UFortWorldItemDefinition, MaxLevel);
    DEFINE_MEMBER(int32, UFortWorldItemDefinition, MinLevel);
    DEFINE_BOOL(UFortWorldItemDefinition, bCanBeDropped)
    DEFINE_BOOL(UFortWorldItemDefinition, bForceIntoOverflow)
    DEFINE_MEMBER(int, UFortWorldItemDefinition, DropCount)
    DEFINE_MEMBER(FScalableFloat, UFortWorldItemDefinition, MaxStackSize)
public:
    DECLARE_STATIC_CLASS(UFortWorldItemDefinition)
    DECLARE_DEFAULT_OBJECT(UFortWorldItemDefinition)
};

class UFortWeaponItemDefinition : public UFortWorldItemDefinition
{
public:
    DEFINE_MEMBER(FDataTableRowHandle, UFortWeaponItemDefinition, WeaponStatHandle);
    DEFINE_MEMBER(TSoftObjectPtr<UFortWorldItemDefinition>, UFortWeaponItemDefinition, AmmoData);
public:
    DECLARE_STATIC_CLASS(UFortWeaponItemDefinition)
    DECLARE_DEFAULT_OBJECT(UFortWeaponItemDefinition)
};
#pragma once
#include "pch.h"

#include "Engine/Array/Header/Array.h"
#include "Engine/Float/Header/Float.h"
#include "Engine/Guid/Header/Guid.h"
#include "FortniteGame/Common/Header/ItemDefinition.h"
#include "FortniteGame/AbilitySystemComponent/Header/AbilitySystemComponent.h"
#include "FortniteGame/FortPlayerController/Header/FortPlayerController.h"
#include "FortniteGame/FortPlayerPawn/Header/FortPlayerPawn.h"
#include "Engine/ObjectPtr/Header/ObjectPtr.h"

class AFortPlayerControllerAthena;
class AFortAthenaAIBotController;
class AFortPlayerController;
class AFortPawn;
class AFortPlayerPawn;

enum class EFortPickupSpawnSource : uint8
{
    Unset                                    = 0,
    PlayerElimination                        = 1,
    Chest                                    = 2,
    SupplyDrop                               = 3,
    AmmoBox                                  = 4,
    Drone                                    = 5,
    ItemSpawner                              = 6,
    BotElimination                           = 7,
    NPCElimination                           = 8,
    LootDrop                                 = 9,
    TossedByPlayer                           = 10,
    EFortPickupSpawnSource_MAX               = 11,
};

enum class EFortPickupSourceTypeFlag : uint8
{
    Other                                    = 0,
    Player                                   = 1,
    Destruction                              = 2,
    Container                                = 4,
    AI                                       = 8,
    Tossed                                   = 16,
    FloorLoot                                = 32,
    Fishing                                  = 64,
    EFortPickupSourceTypeFlag_MAX            = 65,
};

struct FFortItemEntry : public FFastArraySerializerItem
{
    static UClass* StaticClass()
    {
        return StaticClassImpl("FortItemEntry");
    }
    DEFINE_MEMBER(TArray<FFortItemEntryStateValue>, FFortItemEntry, StateValues);
    DEFINE_MEMBER(int32, FFortItemEntry, Count);
    DEFINE_MEMBER(int32, FFortItemEntry, LoadedAmmo);
    DEFINE_MEMBER(int32, FFortItemEntry, Level);
    DEFINE_MEMBER(FGuid, FFortItemEntry, ItemGuid);
    DEFINE_MEMBER(FGuid, FFortItemEntry, TrackerGuid);
    DEFINE_PTR(UFortItemDefinition, FFortItemEntry, ItemDefinition);
    DEFINE_MEMBER(float, FFortItemEntry, Durability);
    DEFINE_MEMBER(FGameplayAbilitySpecHandle, FFortItemEntry, GameplayAbilitySpecHandle);
};

// NumValues: 0x0004
enum class EFortPickupTossState : uint8
{
    NotTossed                                = 0,
    InProgress                               = 1,
    AtRest                                   = 2,
    EFortPickupTossState_MAX                 = 3,
};

struct FFortPickupLocationData final
{
public:
	class AFortPawn*                              PickupTarget;                                      // 0x0000(0x0008)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	class AFortPickup*                            CombineTarget;                                     // 0x0008(0x0008)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	class AFortPawn*                              ItemOwner;                                         // 0x0010(0x0008)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	struct FVector_NetQuantize10                  LootInitialPosition;                               // 0x0018(0x000C)(NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	struct FVector_NetQuantize10                  LootFinalPosition;                                 // 0x0024(0x000C)(NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	float                                         FlyTime;                                           // 0x0030(0x0004)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	struct FVector_NetQuantizeNormal              StartDirection;                                    // 0x0034(0x000C)(NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	struct FVector_NetQuantize10                  FinalTossRestLocation;                             // 0x0040(0x000C)(NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	EFortPickupTossState                          TossState;                                         // 0x004C(0x0001)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	bool                                          bPlayPickupSound;                                  // 0x004D(0x0001)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	uint8                                         Pad_4E[0x2];                                       // 0x004E(0x0002)(Fixing Size After Last Property [ Dumper-7 ])
	struct FGuid                                  PickupGuid;                                        // 0x0050(0x0010)(ZeroConstructor, IsPlainOldData, RepSkip, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPrivate)
};

class AFortPickup : public AActor
{
public:
    void OnRep_PrimaryPickupItemEntry()
    {
        static SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortPickup", "OnRep_PrimaryPickupItemEntry");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return;

        if (!this) 
            return;

        this->ProcessEvent(Func, nullptr);
    }

    void OnRep_bPickedUp()
    {
        static SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortPickup", "OnRep_bPickedUp");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return;

        if (!this) 
            return;

        this->ProcessEvent(Func, nullptr);
    }

    void OnRep_PickupLocationData()
    {
        static SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortPickup", "OnRep_PickupLocationData");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return;

        if (!this) 
            return;

        this->ProcessEvent(Func, nullptr);
    }
    
    void OnRep_TossedFromContainer()
    {
        static SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortPickup", "OnRep_TossedFromContainer");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return;

        if (!this) 
            return;

        auto Flgs = Func->FunctionFlags();
        Func->FunctionFlags() |= 0x400;
    
        this->ProcessEvent(Func, nullptr);

        Func->FunctionFlags() = Flgs;
    }


    void TossPickup(const struct FVector& FinalLocation, class AFortPawn* ItemOwner, int32 OverrideMaxStackCount, bool bToss, bool bShouldCombinePickupsWhenTossCompletes, const EFortPickupSourceTypeFlag InPickupSourceTypeFlags, const EFortPickupSpawnSource InPickupSpawnSource)
    {
        static SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortPickup", "TossPickup");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return;

        if (!this) 
            return;

        struct FortPickup_TossPickup final
        {
        public:
            struct FVector FinalLocation;
            class AFortPawn* ItemOwner;
            int32 OverrideMaxStackCount;
            bool bToss;
            bool bShouldCombinePickupsWhenTossCompletes;
            EFortPickupSourceTypeFlag InPickupSourceTypeFlags;
            EFortPickupSpawnSource InPickupSpawnSource;
        };
        FortPickup_TossPickup Params{};

        Params.FinalLocation = std::move(FinalLocation);
        Params.ItemOwner = ItemOwner;
        Params.OverrideMaxStackCount = OverrideMaxStackCount;
        Params.bToss = bToss;
        Params.bShouldCombinePickupsWhenTossCompletes = bShouldCombinePickupsWhenTossCompletes;
        Params.InPickupSourceTypeFlags = InPickupSourceTypeFlags;
        Params.InPickupSpawnSource = InPickupSpawnSource;

        auto Flgs = Func->FunctionFlags();
        Func->FunctionFlags() |= 0x400;
    
        this->ProcessEvent(Func, &Params);

        Func->FunctionFlags() = Flgs;
    }
public:
    DEFINE_BOOL(AFortPickup, bTossedFromContainer)
    DEFINE_BOOL(AFortPickup, bPickedUp);
    DEFINE_BOOL(AFortPickup, bRandomRotation);
    DEFINE_MEMBER(FFortPickupLocationData, AFortPickup, PickupLocationData);
    DEFINE_MEMBER(FFortItemEntry, AFortPickup, PrimaryPickupItemEntry);
public:
    DECLARE_STATIC_CLASS(AFortPickup)
    DECLARE_DEFAULT_OBJECT(AFortPickup)
};

class AFortPickupAthena : public AFortPickup
{
public:
    DECLARE_STATIC_CLASS(AFortPickupAthena)
    DECLARE_DEFAULT_OBJECT(AFortPickupAthena)
};

class UFortItem : public UObject
{
public:
    void SetOwningControllerForTemporaryItem(AFortPlayerControllerAthena* InController);
};

struct FFortItemList : public FFastArraySerializer
{
    DEFINE_MEMBER(TArray<UFortWorldItem*>, FFortItemList, ItemInstances);
    DEFINE_MEMBER(TArray<FFortItemEntry>, FFortItemList, ReplicatedEntries);
};

class UFortWorldItem : public UFortItem
{
public:
    DEFINE_MEMBER(FFortItemEntry, UFortWorldItem, ItemEntry);
};

class UFortGadgetItemDefinition : public UFortWorldItemDefinition
{
public:
    DECLARE_STATIC_CLASS(UFortGadgetItemDefinition)
    DECLARE_DEFAULT_OBJECT(UFortGadgetItemDefinition)
};

struct FFortRangedWeaponStats : public FFortBaseWeaponStats
{
    
};
    
class AFortInventory : public AActor
{
public:
    DEFINE_MEMBER(FFortItemList, AFortInventory, Inventory);
    DEFINE_BOOL(AFortInventory, bRequiresLocalUpdate);
public:
    void HandleInventoryLocalUpdate();
    void Update(AFortAthenaAIBotController* PlayerController, FFortItemEntry* Entry);
    void Update(AFortPlayerControllerAthena* PlayerController, FFortItemEntry* Entry);
    static UObject* GiveItem(AFortPlayerControllerAthena* PlayerController, UFortItemDefinition* Def, int32 Count, int LoadedAmmo, int32 Level);
    static UObject* GiveItem(AFortAthenaAIBotController* Controller, UFortItemDefinition* Def, int32 Count, int LoadedAmmo, int32 Level);
    static void ReplaceEntry(AFortPlayerController*, FFortItemEntry&);
    static void ReplaceEntry(AFortAthenaAIBotController* AIController, FFortItemEntry& Entry);
    static void Remove(AFortAthenaAIBotController* AIController, FGuid Guid, int AmountToRemove);
    static void Remove(AFortPlayerController* PlayerController, FGuid Guid, int AmountToRemove = -1);
    static FGuid FindGuidByDefinition(AFortPlayerControllerAthena* PC, UFortItemDefinition* ItemDef);
    static AFortPickupAthena* SpawnPickup(FVector, FFortItemEntry*, EFortPickupSourceTypeFlag SourceTypeFlag = EFortPickupSourceTypeFlag::Tossed, EFortPickupSpawnSource SpawnSource = EFortPickupSpawnSource::Unset, AFortPlayerPawn* Pawn = nullptr, int OverrideCount = -1, bool Toss = true, bool RandomRotation = true, bool bCombine = true);
    static AFortPickupAthena* SpawnPickupDirect(FVector, UFortItemDefinition*, int, int, EFortPickupSourceTypeFlag SourceTypeFlag = EFortPickupSourceTypeFlag::Tossed, EFortPickupSpawnSource SpawnSource = EFortPickupSpawnSource::Unset, AFortPlayerPawn* Pawn = nullptr, bool Toss = true);
    static AFortPickupAthena* SpawnPickup(FVector, UFortItemDefinition*, int, int, EFortPickupSourceTypeFlag SourceTypeFlag = EFortPickupSourceTypeFlag::Tossed, EFortPickupSpawnSource SpawnSource = EFortPickupSpawnSource::Unset, AFortPlayerPawn* Pawn = nullptr, bool Toss = true);
    static FFortItemEntry* MakeItemEntry(UFortItemDefinition*, int32, int32);
    static FFortRangedWeaponStats* GetStats(UFortWeaponItemDefinition*);
public:
    DECLARE_STATIC_CLASS(AFortInventory)
    DECLARE_DEFAULT_OBJECT(AFortInventory)
};

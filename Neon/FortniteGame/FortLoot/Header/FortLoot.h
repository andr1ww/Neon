#pragma once
#include "pch.h"
#include <map>
#include <numeric>

#include "Engine/NetDriver/Header/NetDriver.h"
#include "FortniteGame/FortInventory/Header/FortInventory.h"

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

namespace FortLoot
{
    struct LootItem
    {
        UFortWorldItemDefinition* ItemDefinition;
        int32 Count;
        int32 LoadedAmmo;
        
        LootItem(UFortWorldItemDefinition* ItemDef, int32 ItemCount, int32 Ammo = 0)
            : ItemDefinition(ItemDef), Count(ItemCount), LoadedAmmo(Ammo) {}
    };

    inline EFortQuickBars GetQuickbar(UFortItemDefinition* ItemDefinition)
    {
        if (!ItemDefinition) return EFortQuickBars::Max_None;
        return ItemDefinition->IsA<UFortWeaponMeleeItemDefinition>() || 
               ItemDefinition->IsA<UFortResourceItemDefinition>() || 
               ItemDefinition->IsA<UFortAmmoItemDefinition>() || 
               ItemDefinition->IsA<UFortTrapItemDefinition>() || 
               ItemDefinition->IsA<UFortBuildingItemDefinition>() || 
               ItemDefinition->IsA<UFortEditToolItemDefinition>() || 
               ((UFortWorldItemDefinition*)ItemDefinition)->GetbForceIntoOverflow() ? 
               EFortQuickBars::Secondary : EFortQuickBars::Primary;
    }
    
    template <typename T>
    static T* PickWeighted(TArray<T*>& Map, float (*RandFunc)(float), bool bCheckZero = true)
    {
        if (Map.Num() == 0) {
            return nullptr;
        }
        
        float TotalWeight = 0.0f;
        for (auto& p : Map) {
            if (p != nullptr) {
                TotalWeight += p->GetWeight();
            }
        }
        
        if (TotalWeight <= 0.0f) {
            return nullptr;
        }
        
        float RandomNumber = RandFunc(TotalWeight);

        for (auto& Element : Map)
        {
            if (!Element) {
                continue;
            }
            
            float Weight = Element->GetWeight();
            if (bCheckZero && Weight <= 0)
                continue;

            if (RandomNumber <= Weight) 
            {
                return Element;
            }

            RandomNumber -= Weight;
        }

        return nullptr;
    }

    inline TArray<FFortLootTierData*> TierDataAllGroups;
    inline TArray<FFortLootPackageData*> LPGroupsAll;

    inline bool IsValidPointer(void* ptr) {
        return ptr != nullptr && 
               ptr != (void*)0xFFFFFFFFULL && 
               ptr != (void*)0xCCCCCCCCULL && 
               ptr != (void*)0xDDDDDDDDULL &&
               (uintptr_t)ptr > 0x10000;
    }
    
    inline void SetupLDSForPackage(TArray<LootItem>& LootDrops, FName Package, int i, FName TierGroup, int WorldLevel, int RecursionDepth = 0) {
        if (RecursionDepth > 10) {
            return;
        }
        
        TArray<FFortLootPackageData*> LPGroups;
        for (auto const& Val : LPGroupsAll)
        {
            if (!Val)
                continue;

            if (Val->GetLootPackageID().GetNumber() != Package.GetNumber())
                continue;
            if (i != -1 && Val->GetLootPackageCategory() != i)
                continue;
            if (WorldLevel >= 0) {
                if (Val->GetMaxWorldLevel() >= 0 && WorldLevel > Val->GetMaxWorldLevel())
                    continue;
                if (Val->GetMinWorldLevel() >= 0 && WorldLevel < Val->GetMinWorldLevel())
                    continue;
            }

            LPGroups.Add(Val);
        }
        if (LPGroups.Num() == 0)
        {
            return;
        }

        auto LootPackage = PickWeighted(LPGroups, [](float Total) { return ((float)rand() / 32767.f) * Total; });
        if (!LootPackage)
        {
            return;
        }
        
        auto LootPackageCall = LootPackage->GetLootPackageCall();
        if (LootPackageCall.GetData().Num() > 0)
        {
            for (int j = 0; j < LootPackage->GetCount(); j++)
            {
                SetupLDSForPackage(LootDrops, UKismetStringLibrary::Conv_StringToName(LootPackageCall), 0, TierGroup, WorldLevel, RecursionDepth + 1);
            }
            return;
        }

        auto ItemDefinition = (UFortWorldItemDefinition*)LootPackage->GetItemDefinition().Get();
        if (!IsValidPointer(ItemDefinition))
        {
            return;
        }

        bool found = false;
        for (auto& LootDrop : LootDrops) {
            if (LootDrop.ItemDefinition == ItemDefinition) {
                LootDrop.Count = (int32)LootDrop.Count + LootPackage->GetCount();
                
                if (LootDrop.Count > ItemDefinition->GetMaxStackSize().Value) {
                    auto OGCount = LootDrop.Count;
                    LootDrop.Count = ItemDefinition->GetMaxStackSize().Value;

                    if (GetQuickbar(ItemDefinition) == EFortQuickBars::Secondary) {
                        int32 RemainingCount = OGCount - ItemDefinition->GetMaxStackSize().Value;
                        LootDrops.Add(LootItem(ItemDefinition, RemainingCount));
                    }
                }
                if (GetQuickbar(ItemDefinition) == EFortQuickBars::Secondary) {
                    found = true;
                }
                break;
            }
        }

        if (!found && LootPackage->GetCount() > 0) {
            LootDrops.Add(LootItem(ItemDefinition, LootPackage->GetCount()));
        }
    }

    inline void SetupLootGroups(AFortGameStateAthena* GameState)
    {
        if (!GameState) 
        {
            return;
        }

        if (LPGroupsAll.Num() > 0 && TierDataAllGroups.Num() > 0) {
            return;
        }

        static UDataTable* LootPackages = nullptr;
        static UDataTable* LootTierData = nullptr;

        if (!LootPackages || !LootTierData) {
            LootPackages = Runtime::StaticLoadObject<UDataTable>("/Game/Items/DataTables/AthenaLootPackages_Client.AthenaLootPackages_Client");
            LootTierData = Runtime::StaticLoadObject<UDataTable>("/Game/Items/DataTables/AthenaLootTierData_Client.AthenaLootTierData_Client");
            
            if (!LootPackages || !LootTierData) {
                return; 
            }
        }

        if (LootPackages)
        {
            for (TPair<FName, uint8_t*>& RowPair : LootPackages->GetRowMap())
            {
                FFortLootPackageData* Val = reinterpret_cast<FFortLootPackageData*>(RowPair.Value);
                if (IsValidPointer(Val)) {
                    LPGroupsAll.Add(Val);
                }
            }
            
            UCompositeDataTable* CompTable = Cast<UCompositeDataTable>(LootPackages);
            if (CompTable) {
                for (auto& PT : CompTable->GetParentTables()) {
                    if (PT) {
                        for (const auto& RowPair : PT->GetRowMap()) {
                            FFortLootPackageData* Val = reinterpret_cast<FFortLootPackageData*>(RowPair.Value);
                            if (IsValidPointer(Val)) {
                                LPGroupsAll.Add(Val);
                            }
                        }
                    }
                }
            }
        }
        
        if (LootTierData) {
            if (auto CompositeTable = Cast<UCompositeDataTable>(LootTierData)) {
                for (auto& ParentTable : CompositeTable->GetParentTables()) {
                    if (ParentTable) {
                        for (const auto& RowPair : ParentTable->GetRowMap()) {
                            FFortLootTierData* Val = reinterpret_cast<FFortLootTierData*>(RowPair.Value);
                            if (IsValidPointer(Val)) {
                                TierDataAllGroups.Add(Val);
                            }
                        }
                    }
                }
            }
            for (const auto& RowPair : LootTierData->GetRowMap()) {
                FFortLootTierData* Val = reinterpret_cast<FFortLootTierData*>(RowPair.Value);
                if (IsValidPointer(Val)) {
                    TierDataAllGroups.Add(Val);
                }
            }
        }
    }

    inline TArray<LootItem> PickLootDrops(FName TierGroup, int LootTier = -1, int WorldLevel = -1) {
        auto World = UWorld::GetWorld();
        if (!World) {
            return {};
        }
        
        auto GameState = World->GetGameState();
        if (!GameState) {
            return {};
        }
        
        if (WorldLevel == -1) {
            WorldLevel = GameState->GetWorldLevel();
        }
        
        SetupLootGroups(GameState);

        if (!TierDataAllGroups.Num())
        {
            return {};
        }

        TArray<FFortLootTierData*> TierDataGroups;
        for (auto const& Val : TierDataAllGroups) {
            if (!Val) {
                continue;
            }
            
            if (Val->GetTierGroup().GetNumber() == TierGroup.GetNumber() && 
                (LootTier == -1 ? true : LootTier == Val->GetLootTier()))
                TierDataGroups.Add(Val);
        }
        
        if (TierDataGroups.Num() == 0) {
            return {};
        }
        
        auto LootTierData = PickWeighted(TierDataGroups, [](float Total) { return ((float)rand() / 32767.f) * Total; });
        if (!LootTierData)
        {
            return {};
        }

        auto& CategoryMinArray = LootTierData->GetLootPackageCategoryMinArray();
        if (CategoryMinArray.Num() == 0) {
            return {};
        }

        float DropCount = 0;
        if (LootTierData->GetNumLootPackageDrops() > 0) {
            DropCount = LootTierData->GetNumLootPackageDrops() < 1 ? 1 : 
                       (float)((int32)((LootTierData->GetNumLootPackageDrops() * 2) - .5f) >> 1);

            if (LootTierData->GetNumLootPackageDrops() > 1) {
                float idk = LootTierData->GetNumLootPackageDrops() - DropCount;

                if (idk > 0.0000099999997f)
                    DropCount += idk >= ((float)rand() / 32767);
            }
        }

        float AmountOfLootDrops = 0;

        for (int i = 0; i < CategoryMinArray.Num(); ++i)
        {
            AmountOfLootDrops += CategoryMinArray[i];
        }

        int32 SumWeights = 0;

        for (int i = 0; i < CategoryMinArray.Num(); ++i)
        {
            if (CategoryMinArray[i] > 0 && CategoryMinArray[i] != 0)
                SumWeights += CategoryMinArray[i];
        }

        while (SumWeights > 0)
        {
            AmountOfLootDrops++;

            if (AmountOfLootDrops >= LootTierData->GetNumLootPackageDrops()) {
                break;
            }

            SumWeights--;
        }

        if (AmountOfLootDrops <= 0) {
            AmountOfLootDrops = 1;
        }
            
        TArray<LootItem> LootDrops;

        int32 MaxIterations = FMath::Min((int32)AmountOfLootDrops, CategoryMinArray.Num());
        
        for (int i = 0; i < MaxIterations; i++)
        {
            if (i >= CategoryMinArray.Num()) {
                break;
            }
            
            int CategoryMin = CategoryMinArray[i];
            if (CategoryMin >= 1) {
                for (int j = 0; j < CategoryMin; j++)
                {
                    SetupLDSForPackage(LootDrops, LootTierData->GetLootPackage(), i, TierGroup, WorldLevel, 0);
                }
            }
        }

        std::map<UFortWorldItemDefinition*, int32> AmmoMap;
        
   /*     for (auto& Item : LootDrops)
        {
            if (!Item.ItemDefinition)
            {
                continue;
            }
            
            if (!IsValidPointer(Item.ItemDefinition)) {
                continue;
            }
            
            if (!Item.ItemDefinition->IsA<UFortWeaponRangedItemDefinition>()) {
                continue;
            }
            
            if (Item.ItemDefinition->IsStackable()) {
                continue;
            }
            
            UFortWorldItemDefinition* AmmoDefinition = nullptr;
            if (auto Def = Item.ItemDefinition->GetAmmoWorldItemDefinition_BP()) {
                if (Def)
                {
                    AmmoDefinition = Def;
                }
            }

            if (!AmmoDefinition)
            {
                continue;
            }
            
            if (!IsValidPointer(AmmoDefinition)) {
                continue;
            }
            
            bool hasAmmo = false;
            for (auto& ExistingItem : LootDrops) {
                if (ExistingItem.ItemDefinition == AmmoDefinition) {
                    hasAmmo = true;
                    break;
                }
            }
            
            if (hasAmmo) {
                continue;
            }

            FFortLootPackageData* AmmoGroup = nullptr;
            static auto AmmoSmall = UKismetStringLibrary::Conv_StringToName(L"WorldList.AthenaAmmoSmall");
            for (auto const& Val : LPGroupsAll)
            {
                if (!IsValidPointer(Val)) continue;
                
                if (Val->GetLootPackageID().GetNumber() == AmmoSmall.GetNumber() && 
                    Val->GetItemDefinition().Get() == AmmoDefinition)
                {
                    AmmoGroup = Val;
                    break;
                }
            }

            if (AmmoGroup)
            {
                LootDrops.Add(LootItem(AmmoDefinition, AmmoGroup->GetCount()));
            }
        }
*/

        return LootDrops;
    }
        
    inline bool SpawnLoot(ABuildingContainer* Container) {
        auto World = UWorld::GetWorld();
        if (!World) {
            return false;
        }
        
        auto GameState = World->GetGameState();
        auto GameMode = World->GetAuthorityGameMode();
        if (!GameState || !GameMode || !Container || Container->GetbAlreadySearched())
        {
            return false;
        }

        FName LootTierGroupToUse = Container->GetSearchLootTierGroup();

        for (auto& [SupportTierGroup, Redirect] : GameMode->GetRedirectAthenaLootTierGroups()) {
            if (LootTierGroupToUse.GetComparisonIndex() == SupportTierGroup.GetComparisonIndex()) {
                LootTierGroupToUse = Redirect;
                break;
            }
        }

        FVector BaseLocation = Container->GetActorLocation();
        FVector OffsetLocation = BaseLocation + 
                               (Container->GetActorForwardVector() * Container->GetLootSpawnLocation_Athena().X) + 
                               (Container->GetActorRightVector() * Container->GetLootSpawnLocation_Athena().Y) + 
                               (Container->GetActorUpVector() * Container->GetLootSpawnLocation_Athena().Z);
        for (LootItem& Item : PickLootDrops(LootTierGroupToUse)) {
            if (!IsValidPointer(Item.ItemDefinition)) {
                continue;
            }
            
            AFortInventory::SpawnPickupDirect(
                OffsetLocation, 
                Item.ItemDefinition, 
                Item.Count, 
                Item.LoadedAmmo, 
                EFortPickupSourceTypeFlag::Container, 
                EFortPickupSpawnSource::Unset, 
                nullptr, 
                false
            );
        }

        Container->SetbAlreadySearched(true);
        return true;
    }
}
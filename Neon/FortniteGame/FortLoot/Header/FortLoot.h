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

// 0x0008 (0x0008 - 0x0000)
struct alignas(0x08) FTableRowBase
{
    public:
    uint8                                         Pad_0[0x8];                                        // 0x0000(0x0008)(Fixing Struct Size After Last Property [ Dumper-7 ])
};

struct FFortLootLevelData final : public FTableRowBase
{
public:
    class FName                                   category;                                          // 0x0008(0x0004)(Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    int32                                         LootLevel;                                         // 0x000C(0x0004)(Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    int32                                         MinItemLevel;                                      // 0x0010(0x0004)(Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    int32                                         MaxItemLevel;                                      // 0x0014(0x0004)(Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
};

struct FFortLootPackageData
{
    DEFINE_MEMBER(float, FFortLootPackageData, Weight);
    DEFINE_PTR(UFortItemDefinition, FFortLootPackageData, ItemDefinition);
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
    EFortQuickBars GetQuickbar(UFortItemDefinition* ItemDefinition)
    {
        if (!ItemDefinition) return EFortQuickBars::Max_None;
        return ItemDefinition->IsA<UFortWeaponMeleeItemDefinition>() || ItemDefinition->IsA<UFortResourceItemDefinition>() || ItemDefinition->IsA<UFortAmmoItemDefinition>() || ItemDefinition->IsA<UFortTrapItemDefinition>() || ItemDefinition->IsA<UFortBuildingItemDefinition>() || ItemDefinition->IsA<UFortEditToolItemDefinition>() || ((UFortWorldItemDefinition*)ItemDefinition)->GetbForceIntoOverflow() ? EFortQuickBars::Secondary : EFortQuickBars::Primary;
    }
    
    inline int GetLevel(const FDataTableCategoryHandle& CategoryHandle)
    {
        auto GameMode = UWorld::GetWorld()->GetAuthorityGameMode();
        auto GameState = UWorld::GetWorld()->GetGameState();

        if (!CategoryHandle.DataTable)
            return 0;

        if (!CategoryHandle.RowContents.GetComparisonIndex())
            return 0;

        TArray<FFortLootLevelData*> LootLevelData;

        for (auto& LootLevelDataPair : CategoryHandle.DataTable->GetRowMap())
        {
            FFortLootLevelData* Value = reinterpret_cast<FFortLootLevelData*>(LootLevelDataPair.Value);
            if (Value->category.GetComparisonIndex() != CategoryHandle.RowContents.GetComparisonIndex())
                continue;

            LootLevelData.Add(Value);
        }

        if (LootLevelData.Num() > 0)
        {
            int ind = -1;
            int ll = 0;

            for (int i = 0; i < LootLevelData.Num(); i++)
            {
                if (LootLevelData[i]->LootLevel <= GameState->GetWorldLevel() && LootLevelData[i]->LootLevel > ll)
                {
                    ll = LootLevelData[i]->LootLevel;
                    ind = i;
                }
            }

            if (ind != -1)
            {
                auto subbed = LootLevelData[ind]->MaxItemLevel - LootLevelData[ind]->MinItemLevel;

                if (subbed <= -1)
                    subbed = 0;
                else
                {
                    auto calc = (int)(((float)rand() / 32767) * (float)(subbed + 1));
                    if (calc <= subbed)
                        subbed = calc;
                }

                return subbed + LootLevelData[ind]->MinItemLevel;
            }
        }

        return 0;
    }
    
    template <typename T>
    static T* PickWeighted(TArray<T*>& Map, float (*RandFunc)(float), bool bCheckZero = true)
    {
        float TotalWeight = std::accumulate(Map.begin(), Map.end(), 0.0f, [&](float acc, T*& p) { return acc + p->GetWeight(); });
        float RandomNumber = RandFunc(TotalWeight);

        for (auto& Element : Map)
        {
            float Weight = Element->GetWeight();
            if (bCheckZero && Weight == 0)
                continue;

            if (RandomNumber <= Weight) return Element;

            RandomNumber -= Weight;
        }

        return nullptr;
    }

    inline TArray<FFortLootTierData*> TierDataAllGroups;
    inline TArray<FFortLootPackageData*> LPGroupsAll;

    inline void SetupLDSForPackage(TArray<FFortItemEntry>& LootDrops, FName Package, int i, FName TierGroup, int WorldLevel = UWorld::GetWorld()->GetGameState()->GetWorldLevel()) {
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
            return;

        auto LootPackage = PickWeighted(LPGroups, [](float Total) { return ((float)rand() / 32767.f) * Total; });
        if (!LootPackage)
            return;

        std::vector<std::string> LootPackageDropsCalls;
        LootPackage->GetLootPackageCall().ParseIntoArray(LootPackageDropsCalls, LootPackage->GetLootPackageCall());
        if (LootPackageDropsCalls.size() > 1)
        {
            for (int i = 0; i < LootPackage->GetCount(); i++)
                SetupLDSForPackage(LootDrops, UKismetStringLibrary::Conv_StringToName(LootPackage->GetLootPackageCall()), 0, TierGroup);

            return;
        }

        auto ItemDefinition = Cast<UFortWorldItemDefinition>(LootPackage->GetItemDefinition());
        if (!ItemDefinition)
            return;

        bool found = false;
        for (auto& LootDrop : LootDrops) {
            if (LootDrop.GetItemDefinition() == ItemDefinition) {
                LootDrop.GetCount() += LootPackage->GetCount();
                if (LootDrop.GetCount() > ItemDefinition->GetMaxStackSize().Value) {
                    auto OGCount = LootDrop.GetCount();
                    LootDrop.SetCount(ItemDefinition->GetMaxStackSize().Value);

                    if (GetQuickbar(LootDrop.GetItemDefinition()) == EFortQuickBars::Secondary) LootDrops.Add(*AFortInventory::MakeItemEntry(ItemDefinition, OGCount - ItemDefinition->GetMaxStackSize().Value, std::clamp(GetLevel(ItemDefinition->GetLootLevelData()), ItemDefinition->GetMinLevel(), ItemDefinition->GetMaxLevel())));
                }
                if (GetQuickbar(LootDrop.GetItemDefinition()) == EFortQuickBars::Secondary) found = true;
            }
        }

        if (!found && LootPackage->GetCount() > 0) {
            LootDrops.Add(*AFortInventory::MakeItemEntry(ItemDefinition, LootPackage->GetCount(), std::clamp(GetLevel(ItemDefinition->GetLootLevelData()), ItemDefinition->GetMinLevel(), ItemDefinition->GetMaxLevel())));
        }
    }

    inline void SetupLootGroups(AFortGameStateAthena* GameState)
    {
        if (!GameState) return;

        if (LPGroupsAll.Num() > 0 && TierDataAllGroups.Num() > 0) {
            return;
        }

        static UDataTable* LootPackages = nullptr;
        static UDataTable* LootTierData = nullptr;

        if (!LootPackages || !LootTierData) {
            static int CurrentPlaylistInfoOffset = Runtime::GetOffset(GameState, "CurrentPlaylistInfo");
            
            FPlaylistPropertyArray& CurrentPlaylistInfoPtr = *reinterpret_cast<FPlaylistPropertyArray*>(__int64(GameState) + CurrentPlaylistInfoOffset);
            
            LootPackages = CurrentPlaylistInfoPtr.GetBasePlaylist()->Get<UDataTable*>("FortPlaylist", "LootPackages");
            LootTierData = CurrentPlaylistInfoPtr.GetBasePlaylist()->Get<UDataTable*>("FortPlaylist", "LootTierData");

            if (!LootPackages || !LootTierData)
            {
                LootPackages = Runtime::StaticLoadObject<UDataTable>("/Game/Items/DataTables/AthenaLootTierData_Client.AthenaLootTierData_Client");
                LootTierData = Runtime::StaticLoadObject<UDataTable>("/Game/Items/DataTables/AthenaLootPackages_Client.AthenaLootPackages_Client");
            }
        }

        if (LootPackages)
        {
            for (const auto& RowPair : LootPackages->GetRowMap()) {
                FFortLootPackageData* Val = reinterpret_cast<FFortLootPackageData*>(RowPair.Value);
                LPGroupsAll.Add(Val);
            }
            UCompositeDataTable* CompTable = Cast<UCompositeDataTable>(LootPackages);
            if (CompTable) {
                for (auto& PT : CompTable->GetParentTables()) {
                    for (const auto& RowPair : PT->GetRowMap()) {
                        FFortLootPackageData* Val = reinterpret_cast<FFortLootPackageData*>(RowPair.Value);
                        LPGroupsAll.Add(Val);
                    }
                }
            }
        }
        if (LootTierData) {
            if (auto CompositeTable = Cast<UCompositeDataTable>(LootTierData)) {
                for (auto& ParentTable : CompositeTable->GetParentTables()) {
                    for (const auto& RowPair : ParentTable->GetRowMap()) {
                        FFortLootTierData* Val = reinterpret_cast<FFortLootTierData*>(RowPair.Value);
                        if (!Val) continue;
                        TierDataAllGroups.Add(Val);
                    }
                }
            }
            for (const auto& RowPair : LootTierData->GetRowMap()) {
                FFortLootTierData* Val = reinterpret_cast<FFortLootTierData*>(RowPair.Value);
                if (!Val) continue;
                TierDataAllGroups.Add(Val);
            }
        }

        auto GameFeatureDataArray = Runtime::GetObjectsOfClass<UFortGameFeatureData>();

        for (const auto& GameFeatureData : GameFeatureDataArray)
        {
            auto LootTableData = GameFeatureData->Get<FFortGameFeatureLootTableData>("FortGameFeatureData", "DefaultLootTableData");
            if (auto LootPackageData = LootTableData.GetLootPackageData())
            {
                for (const auto& RowPair : LootPackageData->GetRowMap()) {
                    FFortLootPackageData* Val = reinterpret_cast<FFortLootPackageData*>(RowPair.Value);
                    LPGroupsAll.Add(Val);
                }
                UCompositeDataTable* CompTable = Cast<UCompositeDataTable>(LootPackageData);
                if (CompTable) {
                    for (auto& PT : CompTable->GetParentTables()) {
                        for (const auto& RowPair : PT->GetRowMap()) {
                            FFortLootPackageData* Val = reinterpret_cast<FFortLootPackageData*>(RowPair.Value);
                            LPGroupsAll.Add(Val);
                        }
                    }
                }
            }
            if (auto LootTierDataTable = LootTableData.GetLootTierData()) {
                if (auto CompositeTable = Cast<UCompositeDataTable>(LootTierDataTable)) {
                    for (auto& ParentTable : CompositeTable->GetParentTables()) {
                        for (const auto& RowPair : ParentTable->GetRowMap()) {
                            FFortLootTierData* Val = reinterpret_cast<FFortLootTierData*>(RowPair.Value);
                            if (!Val) continue;
                            TierDataAllGroups.Add(Val);
                        }
                    }
                }
                for (const auto& RowPair : LootTierDataTable->GetRowMap()) {
                    FFortLootTierData* Val = reinterpret_cast<FFortLootTierData*>(RowPair.Value);
                    if (!Val) continue;
                    TierDataAllGroups.Add(Val);
                }
            }
        }
    }

    inline TArray<FFortItemEntry> PickLootDrops(FName TierGroup, int LootTier = -1, int WorldLevel = UWorld::GetWorld()->GetGameState()->GetWorldLevel()) {
        SetupLootGroups(UWorld::GetWorld()->GetGameState());

        if (!TierDataAllGroups.Num())
        {
            printf("NO TIER DATAT ALL GHROUPSASF\n");
        }

        TArray<FFortLootTierData*> TierDataGroups;
        for (auto const& Val : TierDataAllGroups) {
            if (Val->GetTierGroup().GetNumber() == TierGroup.GetNumber() && (LootTier == -1 ? true : LootTier == Val->GetLootTier()))
                TierDataGroups.Add(Val);
        }
        auto LootTierData = PickWeighted(TierDataGroups, [](float Total) { return ((float)rand() / 32767.f) * Total; });
        if (!LootTierData)
            return {};

        float DropCount = 0;
        if (LootTierData->GetNumLootPackageDrops() > 0) {
            DropCount = LootTierData->GetNumLootPackageDrops() < 1 ? 1 : (float)((int)((LootTierData->GetNumLootPackageDrops() * 2) - .5f) >> 1);

            if (LootTierData->GetNumLootPackageDrops() > 1) {
                float idk = LootTierData->GetNumLootPackageDrops() - DropCount;

                if (idk > 0.0000099999997f)
                    DropCount += idk >= ((float)rand() / 32767);
            }
        }

        float AmountOfLootDrops = 0;

        for (auto& Min : LootTierData->GetLootPackageCategoryMinArray())
            AmountOfLootDrops += Min;

        int SumWeights = 0;

        for (int i = 0; i < LootTierData->GetLootPackageCategoryMinArray().Num(); ++i)
            if (LootTierData->GetLootPackageCategoryMinArray()[i] > 0 && LootTierData->GetLootPackageCategoryMinArray()[i] != 0)
                SumWeights += LootTierData->GetLootPackageCategoryMinArray()[i];

        while (SumWeights > 0)
        {
            AmountOfLootDrops++;

            if (AmountOfLootDrops >= LootTierData->GetNumLootPackageDrops()) {
                AmountOfLootDrops = AmountOfLootDrops;
                break;
            }

            SumWeights--;
        }

        if (!AmountOfLootDrops)
            AmountOfLootDrops = AmountOfLootDrops;
        
        TArray<FFortItemEntry> LootDrops;

        for (int i = 0; i < AmountOfLootDrops && i < LootTierData->GetLootPackageCategoryMinArray().Num(); i++)
            for (int j = 0; j < LootTierData->GetLootPackageCategoryMinArray()[i] && LootTierData->GetLootPackageCategoryMinArray()[i] >= 1; j++)
                SetupLDSForPackage(LootDrops, LootTierData->GetLootPackage(), i, TierGroup, WorldLevel);

        std::map<UFortWorldItemDefinition*, int32> AmmoMap;
        for (auto& Item : LootDrops)
            if (Item.GetItemDefinition()->IsA<UFortWeaponRangedItemDefinition>() && !Item.GetItemDefinition()->CallFunc<bool>("FortItemDefinition", "IsStackable") && Item.GetItemDefinition()->CallFunc<UFortWorldItemDefinition*>("FortWorldItemDefinition", "GetAmmoWorldItemDefinition_BP"))
            {
                auto AmmoDefinition = Item.GetItemDefinition()->CallFunc<UFortWorldItemDefinition*>("FortWorldItemDefinition", "GetAmmoWorldItemDefinition_BP");
                int i = 0;
                FFortItemEntry* AmmoEntry = nullptr;
                for (FFortItemEntry& Entry : LootDrops)
                {
                    if (AmmoMap[AmmoDefinition] > 0 && i < AmmoMap[AmmoDefinition])
                    {
                        i++;
                        continue;
                    }
                    AmmoMap[AmmoDefinition]++;
                    if (Entry.GetItemDefinition() == AmmoDefinition)
                    {
                        AmmoEntry = &Entry;
                        break;
                    }
                }
                
                if (AmmoEntry)
                    continue;

                FFortLootPackageData* Group = nullptr;
                static auto AmmoSmall = UKismetStringLibrary::Conv_StringToName(L"WorldList.AthenaAmmoSmall");
                for (auto const& Val : LPGroupsAll)
                    if (Val->GetLootPackageID().GetNumber() == AmmoSmall.GetNumber() && Val->GetItemDefinition() == AmmoDefinition)
                    {
                        Group = Val;
                        break;
                    }

                if (Group)
                    LootDrops.Add(*AFortInventory::MakeItemEntry(AmmoDefinition, Group->GetCount(), 0));
            }

        return LootDrops;
    }

    inline bool SpawnLoot(ABuildingContainer* Container) {
        auto GameState = UWorld::GetWorld()->GetGameState();
        auto GameMode = UWorld::GetWorld()->GetAuthorityGameMode();
        if (!GameState || !GameMode || !Container || Container->GetbAlreadySearched())
            return false;

        FName LootTierGroupToUse = Container->GetSearchLootTierGroup();

        for (auto& [SupportTierGroup, Redirect] : GameMode->GetRedirectAthenaLootTierGroups()) {
            if (LootTierGroupToUse.GetComparisonIndex() == SupportTierGroup.GetComparisonIndex()) {
                LootTierGroupToUse = Redirect;
                break;
            }
        }

        FVector BaseLocation = Container->GetActorLocation();
        FVector OffsetLocation = BaseLocation + (Container->GetActorForwardVector() * Container->GetLootSpawnLocation_Athena().X) + (Container->GetActorRightVector() * Container->GetLootSpawnLocation_Athena().Y) + (Container->GetActorUpVector() * Container->GetLootSpawnLocation_Athena().Z);
        for (FFortItemEntry& Entry : PickLootDrops(LootTierGroupToUse)) {
            if (!Entry.GetItemDefinition())
                continue;
            AFortInventory::SpawnPickup(OffsetLocation, &Entry, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Unset);
        }

        Container->SetbAlreadySearched(true);
      //  Container->OnRep_bAlreadySearched();
        //Container->SearchBounceData.SearchAnimationCount++;
       // Container->BounceContainer();

        return Container;
    }
}
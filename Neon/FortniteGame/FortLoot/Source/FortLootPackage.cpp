#include "pch.h"
#include "../Header/FortLootPackage.h"

TArray<FFortLootTierData*> FortLootPackage::TierDataAllGroups;
TArray<FFortLootPackageData*> FortLootPackage::LPGroupsAll;

EFortQuickBars FortLootPackage::GetQuickbar(UFortItemDefinition* ItemDefinition)
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
T* FortLootPackage::PickWeighted(TArray<T*>& Map, float (*RandFunc)(float), bool bCheckZero)
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

void FortLootPackage::SetupLDSForPackage(TArray<FNeonLootImproper>& LootDrops, FName Package, int i, FName TierGroup, int WorldLevel, int RecursionDepth) {
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
    auto WeaponItemDefinition = Cast<UFortWeaponItemDefinition>(ItemDefinition);
    bool IsWeapon = LootPackage->GetLootPackageID().ToString().ToString().contains(".Weapon.") && WeaponItemDefinition;
    if (IsWeapon)
    {
        auto AmmoData = WeaponItemDefinition->GetAmmoData().Get();
        if (AmmoData)
        {
            auto AmmoItemDefinition = Cast<UFortWorldItemDefinition>(AmmoData);
            if (AmmoItemDefinition)
            {
                int AmmoCount = AmmoData->GetDropCount();
                if (AmmoCount > 0)
                {
                    LootDrops.Add(FNeonLootImproper(AmmoItemDefinition, AmmoCount));
                }
            }
        }
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
                    LootDrops.Add(FNeonLootImproper(ItemDefinition, RemainingCount));
                }
            }
            if (GetQuickbar(ItemDefinition) == EFortQuickBars::Secondary) {
                found = true;
            }
            break;
        }
    }

    if (!found && LootPackage->GetCount() > 0) {
        LootDrops.Add(FNeonLootImproper(ItemDefinition, LootPackage->GetCount()));
    }
}

void FortLootPackage::SetupLootGroups(AFortGameStateAthena* GameState)
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
        static int CurrentPlaylistInfoOffset = Runtime::GetOffset(GameState, "CurrentPlaylistInfo");
            
        FPlaylistPropertyArray& CurrentPlaylistInfoPtr = *reinterpret_cast<FPlaylistPropertyArray*>(__int64(GameState) + CurrentPlaylistInfoOffset);
        
        LootPackages = CurrentPlaylistInfoPtr.GetBasePlaylist()->GetLootPackages().Get(true);
        LootTierData = CurrentPlaylistInfoPtr.GetBasePlaylist()->GetLootTierData().Get(true);
        
        if (!LootPackages || !LootTierData) {
            LootPackages = Runtime::StaticLoadObject<UDataTable>("/Game/Items/DataTables/AthenaLootPackages_Client.AthenaLootPackages_Client");
            LootTierData = Runtime::StaticLoadObject<UDataTable>("/Game/Items/DataTables/AthenaLootTierData_Client.AthenaLootTierData_Client");
        }
    }

    if (LootPackages)
    {
        for (TPair<FName, uint8_t*>& RowPair : LootPackages->GetRowMap())
        {
            FFortLootPackageData* Val = reinterpret_cast<FFortLootPackageData*>(RowPair.Value);
            if (Val) {
                LPGroupsAll.Add(Val);
            }
        }
        
        UCompositeDataTable* CompTable = Cast<UCompositeDataTable>(LootPackages);
        if (CompTable) {
            for (auto& PT : CompTable->GetParentTables()) {
                if (PT) {
                    for (const auto& RowPair : PT->GetRowMap()) {
                        FFortLootPackageData* Val = reinterpret_cast<FFortLootPackageData*>(RowPair.Value);
                        if (Val) {
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
                        if (Val) {
                            TierDataAllGroups.Add(Val);
                        }
                    }
                }
            }
        }
        for (const auto& RowPair : LootTierData->GetRowMap()) {
            FFortLootTierData* Val = reinterpret_cast<FFortLootTierData*>(RowPair.Value);
            if (Val) {
                TierDataAllGroups.Add(Val);
            }
        }
    }
}

TArray<FortLootPackage::FNeonLootImproper> FortLootPackage::PickLootDrops(FName TierGroup, int LootTier, int WorldLevel) {
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
        
    TArray<FNeonLootImproper> LootDrops;

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
    
    return LootDrops;
}
    
bool FortLootPackage::SpawnLoot(ABuildingContainer* Container) {
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
    
    for (FNeonLootImproper& Item : PickLootDrops(LootTierGroupToUse)) {
        if (!Item.ItemDefinition) {
            continue;
        }

        FVector BaseLocation = Container->GetActorLocation();
        FVector OffsetLocation = BaseLocation + 
                               (Container->GetActorForwardVector() * Container->GetLootSpawnLocation_Athena().X) + 
                               (Container->GetActorRightVector() * Container->GetLootSpawnLocation_Athena().Y) + 
                               (Container->GetActorUpVector() * Container->GetLootSpawnLocation_Athena().Z);
        
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
#include "pch.h"
#include "../Header/FortLootPackage.h"

#include "Engine/GameplayStatics/Header/GameplayStatics.h"

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
    if (RecursionDepth > 3 || LootDrops.Num() > 20) {
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
        SetupLDSForPackage(LootDrops, UKismetStringLibrary::Conv_StringToName(LootPackageCall), -1, TierGroup, WorldLevel, RecursionDepth + 1);
        return;
    }

    auto ItemDefinition = (UFortWorldItemDefinition*)LootPackage->GetItemDefinition().Get();
    if (!IsValidPointer(ItemDefinition))
    {
        return;
    }

    if (LootPackage->GetCount() <= 0) {
        return;
    }

    bool found = false;
    for (auto& LootDrop : LootDrops) {
        if (LootDrop.ItemDefinition == ItemDefinition) {
            LootDrop.Count += LootPackage->GetCount();
            found = true;
            break;
        }
    }

    if (!found) {
        LootDrops.Add(FNeonLootImproper(ItemDefinition, LootPackage->GetCount()));
    }

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
                    bool ammoFound = false;
                    for (auto& LootDrop : LootDrops) {
                        if (LootDrop.ItemDefinition == AmmoItemDefinition) {
                            LootDrop.Count += AmmoCount;
                            ammoFound = true;
                            break;
                        }
                    }
                    if (!ammoFound) {
                        LootDrops.Add(FNeonLootImproper(AmmoItemDefinition, AmmoCount));
                    }
                }
            }
        }
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
    std::vector<UDataTable*> LPTables;
    std::vector<UDataTable*> LTDTables;
    
    bool bFoundOverrides = false;

    if (!LootPackages || !LootTierData)
    {
        static int CurrentPlaylistInfoOffset = Runtime::GetOffset(GameState, "CurrentPlaylistInfo");
            
        FPlaylistPropertyArray& CurrentPlaylistInfoPtr = *reinterpret_cast<FPlaylistPropertyArray*>(__int64(GameState) + CurrentPlaylistInfoOffset);

        static int LootPackagesOffset = Runtime::GetOffset(CurrentPlaylistInfoPtr.GetBasePlaylist(), "LootPackages");
        static int LootTierDataOffset = Runtime::GetOffset(CurrentPlaylistInfoPtr.GetBasePlaylist(), "LootTierData");
        static auto ErmOk = *(TSoftObjectPtr<UDataTable>*)(__int64(CurrentPlaylistInfoPtr.GetBasePlaylist()) + LootPackagesOffset);
        static auto ErmOk2 = *(TSoftObjectPtr<UDataTable>*)(__int64(CurrentPlaylistInfoPtr.GetBasePlaylist()) + LootTierDataOffset);

        LootPackages = ErmOk.Get(UCompositeDataTable::StaticClass(), true);
        LootTierData = ErmOk2.Get(UCompositeDataTable::StaticClass(), true);
        
        if (!LootPackages) {
            LootPackages = CurrentPlaylistInfoPtr.GetBasePlaylist()->GetLootPackages().Get(UDataTable::StaticClass(), true);
        }

        if (!LootTierData) {
            LootTierData = CurrentPlaylistInfoPtr.GetBasePlaylist()->GetLootTierData().Get(UDataTable::StaticClass(), true);
        }

        if (Fortnite_Version >= 12.00)
        {
            auto GameFeatureDataObjects = Runtime::GetObjectsOfClass<UFortGameFeatureData>(UFortGameFeatureData::StaticClass());
            for (auto GameFeatureData : GameFeatureDataObjects) {
                auto& PlaylistOverrideLootTableData = GameFeatureData->GetPlaylistOverrideLootTableData();
    
                auto GameplayTagContainer = CurrentPlaylistInfoPtr.GetBasePlaylist()->GetGameplayTagContainer();
    
                for (auto& Value : PlaylistOverrideLootTableData) {
                    auto OverrideTag = Value.Key;
        
                    if (OverrideTag.TagName.ToString() == "None" || !OverrideTag.TagName.IsValid()) {
                        continue;
                    }
                
                    bool bFoundMatch = false;
                    if (GameplayTagContainer.HasTag(OverrideTag)) {
                        bFoundMatch = true;
                    }
                
                    if (bFoundMatch) {
                        bFoundOverrides = true;
            
                        auto OverrideLootPackagesStr = Value.Value.GetLootPackageData().SoftObjectPtr.ObjectID.AssetPathName.ToString();
                        auto bOverrideLPIsComposite = OverrideLootPackagesStr.ToString().contains("Composite");
                        auto lpPtr = Value.Value.GetLootPackageData().Get(bOverrideLPIsComposite ? UCompositeDataTable::StaticClass() : UDataTable::StaticClass(), true);
                        if (lpPtr) {
                            LPTables.push_back(lpPtr);
                        }
            
                        auto OverrideLootTierDataStr = Value.Value.GetLootTierData().SoftObjectPtr.ObjectID.AssetPathName.ToString();
                        auto bOverrideLTDIsComposite = OverrideLootTierDataStr.ToString().contains("Composite");
                        auto ltdPtr = Value.Value.GetLootTierData().Get(bOverrideLTDIsComposite ? UCompositeDataTable::StaticClass() : UDataTable::StaticClass(), true);
                        if (ltdPtr) {
                            LTDTables.push_back(ltdPtr);
                        }
                    }
                }
            }
        }
    }

    if (!bFoundOverrides) {
        if (LootPackages) LPTables.push_back(LootPackages);
        if (LootTierData) LTDTables.push_back(LootTierData);
    }

    std::map<FName, FFortLootPackageData*> PackageGroupMap;
    for (auto LPTable : LPTables) {
        if (!LPTable) continue;
        
        if (auto CompTable = Cast<UCompositeDataTable>(LPTable)) {
            auto& ParentTables = CompTable->GetParentTables();
            for (int32 i = 0; i < ParentTables.Num(); i++) {
                auto ParentTable = ParentTables[i];
                if (ParentTable) {
                    auto& RowMap = ParentTable->GetRowMap();
                    for (const auto& RowPair : RowMap) {
                        FFortLootPackageData* Val = reinterpret_cast<FFortLootPackageData*>(RowPair.Value);
                        if (IsValidPointer(Val)) {
                            PackageGroupMap[RowPair.Key] = Val;
                        }
                    }
                }
            }
        }
        
        auto& MainRowMap = LPTable->GetRowMap();
        for (const auto& RowPair : MainRowMap) {
            FFortLootPackageData* Val = reinterpret_cast<FFortLootPackageData*>(RowPair.Value);
            if (IsValidPointer(Val)) {
                PackageGroupMap[RowPair.Key] = Val;
            }
        }
    }
    
    for (auto& [Key, Value] : PackageGroupMap) {
        LPGroupsAll.Add(Value);
    }
    
    std::map<FName, FFortLootTierData*> TierDataGroupMap;
    for (auto LTDTable : LTDTables) {
        if (!LTDTable) continue;
        
        if (auto CompTable = Cast<UCompositeDataTable>(LTDTable)) {
            auto& ParentTables = CompTable->GetParentTables();
            for (int32 i = 0; i < ParentTables.Num(); i++) {
                auto ParentTable = ParentTables[i];
                if (ParentTable) {
                    auto& RowMap = ParentTable->GetRowMap();
                    for (const auto& RowPair : RowMap) {
                        FFortLootTierData* Val = reinterpret_cast<FFortLootTierData*>(RowPair.Value);
                        if (IsValidPointer(Val)) {
                            TierDataGroupMap[RowPair.Key] = Val;
                        }
                    }
                }
            }
        }
        
        auto& MainRowMap = LTDTable->GetRowMap();
        for (const auto& RowPair : MainRowMap) {
            FFortLootTierData* Val = reinterpret_cast<FFortLootTierData*>(RowPair.Value);
            if (IsValidPointer(Val)) {
                TierDataGroupMap[RowPair.Key] = Val;
            }
        }
    }

    for (auto& [Key, Value] : TierDataGroupMap) {
        TierDataAllGroups.Add(Value);
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

struct FFortSearchBounceData final
{
public:
    struct FVector                                BounceNormal;                                      // 0x0000(0x000C)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    uint32                                        SearchAnimationCount;                              // 0x000C(0x0004)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
};

void FortLootPackage::InternalSpawnLoot(FName& TierGroup, FVector Loc, const TArray<FNeonLootImproper>& PrecomputedLoot) {
    for (auto& [SupportTierGroup, Redirect] : UWorld::GetWorld()->GetAuthorityGameMode()->GetRedirectAthenaLootTierGroups()) {
        if (TierGroup.GetComparisonIndex() == SupportTierGroup.GetComparisonIndex()) {
            TierGroup = Redirect;
            break;
        }
    }
    
    int32 SpawnedCount = 0;
    
    for (const FNeonLootImproper& Item : PrecomputedLoot) {
        if (!IsValidPointer(Item.ItemDefinition)) {
            continue;
        }

        FVector SpawnLoc = Loc;
        SpawnLoc.X += (rand() % 130 - 65);
        SpawnLoc.Y += (rand() % 130 - 65);
        
        AFortInventory::SpawnPickupDirect(
            SpawnLoc, 
            Item.ItemDefinition, 
            Item.Count, 
            Item.LoadedAmmo, 
            EFortPickupSourceTypeFlag::Container, 
            EFortPickupSpawnSource::Unset, 
            nullptr, 
            false
        );
        
        SpawnedCount++;
    }
}

void FortLootPackage::SpawnFloorLootForContainer(UBlueprintGeneratedClass* ContainerType) {
    auto Containers = UGameplayStatics::GetAllActorsOfClass(UWorld::GetWorld(), ContainerType);
    UE_LOG(LogNeon, Log, "Container Count: %d", Containers.Num());

    int32 TotalItemsSpawned = 0;

    for (auto& BuildingContainer : Containers) {
        auto Container = Cast<ABuildingContainer>(BuildingContainer);
        if (!Container) {
            continue;
        }
        
        FVector SpawnLocation = Container->K2_GetActorLocation() + 
                               Container->GetActorForwardVector() * Container->GetLootSpawnLocation_Athena().X + 
                               Container->GetActorRightVector() * Container->GetLootSpawnLocation_Athena().Y + 
                               Container->GetActorUpVector() * Container->GetLootSpawnLocation_Athena().Z;
        
        FName TierGroup = Container->GetSearchLootTierGroup();
        TArray<FNeonLootImproper> LootItems = PickLootDrops(TierGroup);
        int32 ItemsForThisContainer = LootItems.Num();
        TotalItemsSpawned += ItemsForThisContainer;
        
        InternalSpawnLoot(TierGroup, SpawnLocation, LootItems);
        BuildingContainer->K2_DestroyActor();
    }
    
    Containers.Free();
}

bool FortLootPackage::ServerOnAttemptInteract(ABuildingContainer* Container, FInteractionType Type)
{
    if (!Container) return false;

    auto Containers = UGameplayStatics::GetAllActorsOfClass(UWorld::GetWorld(), Container->GetClass());
    
    if (Container->GetbAlreadySearched())
        return true;

    ABuildingContainer* TargetContainer = nullptr;
    
    for (auto* ContainerActor : Containers)
    {
        if (ContainerActor == Container)
        {
            TargetContainer = (ABuildingContainer*)ContainerActor;
            break;
        }
    }

    if (!TargetContainer)
        TargetContainer = Container;

    auto SpawnLocation = TargetContainer->K2_GetActorLocation() + TargetContainer->GetActorRightVector() * 70.f + FVector{ 0, 0, 50 };
    UE_LOG(LogNeon, Log, "SpawnLocation: X=%f Y=%f Z=%f", SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z);
    
    FName TierGroup = TargetContainer->GetSearchLootTierGroup();
    TArray<FNeonLootImproper> LootItems = PickLootDrops(TierGroup);
    InternalSpawnLoot(TierGroup, SpawnLocation, LootItems);
    
    TargetContainer->SetbAlreadySearched(true);
    TargetContainer->CallFunc<void>("BuildingContainer", "OnRep_bAlreadySearched");
    TargetContainer->Get<FFortSearchBounceData>("BuildingContainer", "SearchBounceData").SearchAnimationCount++;
    TargetContainer->CallFunc<void>("BuildingContainer", "BounceContainer");

    return true;
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
    
    TArray<FNeonLootImproper> LootItems = PickLootDrops(LootTierGroupToUse);
    int32 SpawnedCount = 0;
    
    for (const FNeonLootImproper& Item : LootItems) {
        if (!IsValidPointer(Item.ItemDefinition)) {
            continue;
        }

        FVector Loc = Container->K2_GetActorLocation() + Container->GetActorRightVector() * 70.f + FVector{ 0, 0, 50 };
        
        AFortInventory::SpawnPickupDirect(
            Loc, 
            Item.ItemDefinition, 
            Item.Count, 
            Item.LoadedAmmo, 
            EFortPickupSourceTypeFlag::Container, 
            EFortPickupSpawnSource::Unset, 
            nullptr, 
            false
        );
        
        SpawnedCount++;
    }
    
    Container->SetbAlreadySearched(true);
    Container->CallFunc<void>("BuildingContainer", "OnRep_bAlreadySearched");
    Container->Get<FFortSearchBounceData>("BuildingContainer", "SearchBounceData").SearchAnimationCount++;
    Container->CallFunc<void>("BuildingContainer", "BounceContainer");
    
    return true;
}
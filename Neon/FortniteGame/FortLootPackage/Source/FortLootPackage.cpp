#include "pch.h"
#include "../Header/FortLootPackage.h"

#include "Engine/GameplayStatics/Header/GameplayStatics.h"
#include <FortniteGame/FortQuestManager/Header/FortQuestManager.h>

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
    static TArray<FFortLootPackageData*> LPGroupsAll;
    
    if (RecursionDepth > 3 || LootDrops.Num() > 20) return;
    
    if (LPGroupsAll.Num() == 0) {
        auto World = UWorld::GetWorld();
        if (!World || !World->GetGameState()) return;
        
        auto GameState = Cast<AFortGameStateAthena>(World->GetGameState());
        if (!GameState) return;

        static int CurrentPlaylistInfoOffset = Runtime::GetOffset(GameState, "CurrentPlaylistInfo");
        FPlaylistPropertyArray& CurrentPlaylistInfoPtr = *reinterpret_cast<FPlaylistPropertyArray*>(__int64(GameState) + CurrentPlaylistInfoOffset);

        static int LootPackagesOffset = Runtime::GetOffset(CurrentPlaylistInfoPtr.GetBasePlaylist(), "LootPackages");
        static auto ErmOk = *(TSoftObjectPtr<UDataTable>*)(__int64(CurrentPlaylistInfoPtr.GetBasePlaylist()) + LootPackagesOffset);

        UDataTable* LootPackages = ErmOk.Get(UCompositeDataTable::StaticClass(), true);
        if (!LootPackages) LootPackages = CurrentPlaylistInfoPtr.GetBasePlaylist()->GetLootPackages().Get(UDataTable::StaticClass(), true);

        if (!LootPackages)
        {
            LootPackages = Runtime::StaticLoadObject<UDataTable>("/Game/Items/DataTables/AthenaLootPackages_Client.AthenaLootPackages_Client");
        }
        
        auto AddPackages = [&](const TMap<FName, uint8*>& RowMap) {
            for (const auto& RowPair : RowMap) {
                if (auto Val = reinterpret_cast<FFortLootPackageData*>(RowPair.Value); IsValidPointer(Val)) {
                    LPGroupsAll.Add(Val);
                }
            }
        };
        
        AddPackages(LootPackages->GetRowMap());
        
        if (auto CompositeTable = Cast<UCompositeDataTable>(LootPackages)) {
            for (auto ParentTable : CompositeTable->GetParentTables()) {
                if (ParentTable) AddPackages(ParentTable->GetRowMap());
            }
        }
    }

    TArray<FFortLootPackageData*> LPGroups;
    for (auto Val : LPGroupsAll) {
        if (!Val || Val->GetLootPackageID().GetComparisonIndex() != Package.GetComparisonIndex()) continue;
        if (i != -1 && Val->GetLootPackageCategory() != i) continue;
        if (WorldLevel >= 0 && ((Val->GetMaxWorldLevel() >= 0 && WorldLevel > Val->GetMaxWorldLevel()) || 
                               (Val->GetMinWorldLevel() >= 0 && WorldLevel < Val->GetMinWorldLevel()))) continue;
        LPGroups.Add(Val);
    }
    
    if (LPGroups.Num() == 0) return;

    auto LootPackage = PickWeighted(LPGroups, [](float Total) { return ((float)rand() / 32767.f) * Total; });
    if (!LootPackage) return;
    
    auto LootPackageCall = LootPackage->GetLootPackageCall();
    if (LootPackageCall.GetData().Num() > 0) {
        for (int idx = 0; idx < LootPackage->GetCount(); idx++) {
            SetupLDSForPackage(LootDrops, UKismetStringLibrary::Conv_StringToName(LootPackageCall), 0, TierGroup, WorldLevel, RecursionDepth + 1);
        }
        return;
    }

    auto ItemDefinition = Cast<UFortWorldItemDefinition>(LootPackage->GetItemDefinition().Get());
    if (!IsValidPointer(ItemDefinition) || LootPackage->GetCount() <= 0) return;

    auto AddLoot = [&](UFortWorldItemDefinition* Item, int Count) {
        for (auto& LootDrop : LootDrops) {
            if (LootDrop.ItemDefinition == Item) {
                LootDrop.Count += Count;
                if (LootDrop.Count > Item->GetMaxStackSize().Value) {
                    int32 Overflow = LootDrop.Count - Item->GetMaxStackSize().Value;
                    LootDrop.Count = Item->GetMaxStackSize().Value;
                    if (GetQuickbar(Item) == EFortQuickBars::Secondary) {
                        LootDrops.Add(FNeonLootImproper(Item, Overflow));
                    }
                }
                return GetQuickbar(Item) == EFortQuickBars::Secondary;
            }
        }
        LootDrops.Add(FNeonLootImproper(Item, Count));
        return false;
    };

    AddLoot(ItemDefinition, LootPackage->GetCount());

    if (auto WeaponItemDefinition = Cast<UFortWeaponItemDefinition>(ItemDefinition)) {
        if (auto AmmoData = WeaponItemDefinition->GetAmmoData().Get()) {
            if (auto AmmoItemDefinition = Cast<UFortWorldItemDefinition>(AmmoData)) {
                if (int AmmoCount = AmmoData->GetDropCount(); AmmoCount > 0) {
                    AddLoot(AmmoItemDefinition, AmmoCount);
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
    
  /*  if (LPGroupsAll.Num() > 0 && TierDataAllGroups.Num() > 0) {
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

    */
}

TArray<FortLootPackage::FNeonLootImproper> FortLootPackage::PickLootDrops(FName TierGroup, int LootTier, int WorldLevel) {
    static TArray<FFortLootTierData*> TierDataAllGroups;
    
    auto World = UWorld::GetWorld();
    if (!World) {
        return {};
    }
    
    auto GameState = Cast<AFortGameStateAthena>(World->GetGameState());
    if (!GameState) {
        return {};
    }
    
    if (WorldLevel == -1) {
        WorldLevel = GameState->GetWorldLevel();
    }
    
    if (TierDataAllGroups.Num() == 0) {
        UDataTable* LootTierData = nullptr;
        static int CurrentPlaylistInfoOffset = Runtime::GetOffset(GameState, "CurrentPlaylistInfo");
            
        FPlaylistPropertyArray& CurrentPlaylistInfoPtr = *reinterpret_cast<FPlaylistPropertyArray*>(__int64(GameState) + CurrentPlaylistInfoOffset);

        static int LootTierDataOffset = Runtime::GetOffset(CurrentPlaylistInfoPtr.GetBasePlaylist(), "LootTierData");
        static auto ErmOk2 = *(TSoftObjectPtr<UDataTable>*)(__int64(CurrentPlaylistInfoPtr.GetBasePlaylist()) + LootTierDataOffset);

        LootTierData = ErmOk2.Get(UCompositeDataTable::StaticClass(), true);
        
        if (!LootTierData) {
            LootTierData = CurrentPlaylistInfoPtr.GetBasePlaylist()->GetLootTierData().Get(UDataTable::StaticClass(), true);
        }

        if (!LootTierData)
        {
            LootTierData = Runtime::StaticLoadObject<UDataTable>("/Game/Items/DataTables/AthenaLootTierData_Client.AthenaLootTierData_Client");
        }
        
        if (auto CompositeTable = Cast<UCompositeDataTable>(LootTierData)) {
            auto& ParentTables = CompositeTable->GetParentTables();
            for (auto& ParentTable : ParentTables) {
                if (ParentTable) {
                    auto& RowMap = ParentTable->GetRowMap();
                    for (const auto& RowPair : RowMap) {
                        FFortLootTierData* Val = reinterpret_cast<FFortLootTierData*>(RowPair.Value);
                        if (IsValidPointer(Val)) {
                            TierDataAllGroups.Add(Val);
                        }
                    }
                }
            }
        }
        
        auto& MainRowMap = LootTierData->GetRowMap();
        for (const auto& RowPair : MainRowMap) {
            FFortLootTierData* Val = reinterpret_cast<FFortLootTierData*>(RowPair.Value);
            if (IsValidPointer(Val)) {
                TierDataAllGroups.Add(Val);
            }
        }
    }

    TArray<FFortLootTierData*> TierDataGroups;
    for (auto const& Val : TierDataAllGroups) {
        if (!Val) continue;
        
        if (Val->GetTierGroup().GetComparisonIndex() == TierGroup.GetComparisonIndex() && 
            (LootTier == -1 ? true : LootTier == Val->GetLootTier())) {
            TierDataGroups.Add(Val);
        }
    }
    
    if (TierDataGroups.Num() == 0) {
        return {};
    }
    
    auto LootTierData = PickWeighted(TierDataGroups, [](float Total) { return ((float)rand() / 32767.f) * Total; });
    if (!LootTierData) {
        return {};
    }

    float DropCount = 0;
    if (LootTierData->GetNumLootPackageDrops() > 0) {
        DropCount = LootTierData->GetNumLootPackageDrops() < 1 ? 1 : 
                   (float)((int32)((LootTierData->GetNumLootPackageDrops() * 2) - .5f) >> 1);

        if (LootTierData->GetNumLootPackageDrops() > 1) {
            float idk = LootTierData->GetNumLootPackageDrops() - DropCount;
            if (idk > 0.0000099999997f) {
                DropCount += idk >= ((float)rand() / 32767);
            }
        }
    }

    float AmountOfLootDrops = 0;

    for (auto& Min : LootTierData->GetLootPackageCategoryMinArray()) {
        AmountOfLootDrops += Min;
    }

    int32 SumWeights = 0;

    for (int i = 0; i < LootTierData->GetLootPackageCategoryWeightArray().Num(); ++i) {
        if (LootTierData->GetLootPackageCategoryWeightArray()[i] > 0 && 
            LootTierData->GetLootPackageCategoryMaxArray()[i] != 0) {
            SumWeights += LootTierData->GetLootPackageCategoryWeightArray()[i];
        }
    }

    while (SumWeights > 0) {
        AmountOfLootDrops++;

        if (AmountOfLootDrops >= LootTierData->GetNumLootPackageDrops()) {
            break;
        }

        SumWeights--;
    }

    if (!AmountOfLootDrops) {
        AmountOfLootDrops = 1;
    }
        
    TArray<FNeonLootImproper> LootDrops;

    auto& CategoryMinArray = LootTierData->GetLootPackageCategoryMinArray();
    
    for (int i = 0; i < AmountOfLootDrops && i < CategoryMinArray.Num(); i++) {
        for (int j = 0; j < CategoryMinArray[i] && CategoryMinArray[i] >= 1; j++) {
            SetupLDSForPackage(LootDrops, LootTierData->GetLootPackage(), i, TierGroup, WorldLevel, 0);
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

    FVector SpawnLocation = Container->K2_GetActorLocation() + 
                           Container->GetActorForwardVector() * Container->GetLootSpawnLocation_Athena().X + 
                           Container->GetActorRightVector() * Container->GetLootSpawnLocation_Athena().Y + 
                           Container->GetActorUpVector() * Container->GetLootSpawnLocation_Athena().Z;
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

// should be in a diff class but wtv i put it here

void FortLootPackage::ServerAttemptInteract(UFortControllerComponent_Interaction* Component, FFrame& Stack)
{
    class AActor* ReceivingActor;
    class UPrimitiveComponent* InteractComponent;
    ETInteractionType InteractType;
    class UObject* OptionalObjectData;
    EInteractionBeingAttempted InteractionBeingAttempted;
    int32 RequestId;
    Stack.StepCompiledIn(&ReceivingActor);
    Stack.StepCompiledIn(&InteractComponent);
    Stack.StepCompiledIn(&InteractType);
    Stack.StepCompiledIn(&OptionalObjectData);
    Stack.StepCompiledIn(&InteractionBeingAttempted);
    Stack.StepCompiledIn(&RequestId);
    Stack.IncrementCode();
    
    if (ReceivingActor->IsA(ABuildingContainer::StaticClass()))
    {
        ABuildingContainer* Container = Cast<ABuildingContainer>(ReceivingActor);
        
        if (Container->GetbAlreadySearched()) {
            goto CallOriginal;
        }
        
        FName LootTierGroupToUse = Container->GetSearchLootTierGroup();
        auto World = UWorld::GetWorld();
        if (World && World->GetAuthorityGameMode()) {
            for (auto& [SupportTierGroup, Redirect] : World->GetAuthorityGameMode()->GetRedirectAthenaLootTierGroups()) {
                if (LootTierGroupToUse.GetComparisonIndex() == SupportTierGroup.GetComparisonIndex()) {
                    LootTierGroupToUse = Redirect;
                    break;
                }
            }
        }
      
        TArray<FNeonLootImproper> LootItems = PickLootDrops(LootTierGroupToUse);
        auto SpawnLocation = Container->K2_GetActorLocation() + Container->GetActorRightVector() * 70.f + FVector{ 0, 0, 50 };	

        static UFortAccoladeItemDefinition* open3 = Runtime::StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_008_SearchChests_Bronze.AccoladeId_008_SearchChests_Bronze"); // SearchCount  3
        static UFortAccoladeItemDefinition* open7 = Runtime::StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_009_SearchChests_Silver.AccoladeId_009_SearchChests_Silver");// SearchCount 7
        static UFortAccoladeItemDefinition* open12 = Runtime::StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_010_SearchChests_Gold.AccoladeId_010_SearchChests_Gold");// SearchCount 12
        static UFortAccoladeItemDefinition* openany = Runtime::StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_007_SearchChests.AccoladeId_007_SearchChests"); // SearchCount any ig
        static UFortAccoladeItemDefinition* Ammobox = Runtime::StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_011_SearchAmmoBox.AccoladeId_011_SearchAmmoBox"); // ammo box obv

        InternalSpawnLoot(LootTierGroupToUse, SpawnLocation, LootItems);
        Container->SetbAlreadySearched(true);
        Container->CallFunc<void>("BuildingContainer", "OnRep_bAlreadySearched");
        Container->Get<FFortSearchBounceData>("BuildingContainer", "SearchBounceData").SearchAnimationCount++;
        Container->CallFunc<void>("BuildingContainer", "BounceContainer");
    }

    if (ReceivingActor->IsA(AFortAthenaSupplyDrop::StaticClass()))
    {
        static FName Loot_AthenaSupplyDrop = UKismetStringLibrary::Conv_StringToName(L"Loot_AthenaSupplyDrop");
        TArray<FNeonLootImproper> LootItems = PickLootDrops(Loot_AthenaSupplyDrop);
        auto SpawnLocation = ReceivingActor->K2_GetActorLocation() + ReceivingActor->GetActorRightVector() * 70.f + FVector{ 0, 0, 50 };	

        InternalSpawnLoot(Loot_AthenaSupplyDrop, SpawnLocation, LootItems);
    }
    
CallOriginal:
    struct FortControllerComponent_Interaction_ServerAttemptInteract final
    {
    public:
        class AActor*                                 ReceivingActor;                                    // 0x0000(0x0008)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
        class UPrimitiveComponent*                    InteractComponent;                                 // 0x0008(0x0008)(Parm, ZeroConstructor, InstancedReference, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
        ETInteractionType                             InteractType;                                      // 0x0010(0x0001)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
        uint8                                         Pad_11[0x7];                                       // 0x0011(0x0007)(Fixing Size After Last Property [ Dumper-7 ])
        class UObject*                                OptionalObjectData;                                // 0x0018(0x0008)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
        EInteractionBeingAttempted                    InteractionBeingAttempted;                         // 0x0020(0x0001)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
        uint8                                         Pad_21[0x3];                                       // 0x0021(0x0003)(Fixing Size After Last Property [ Dumper-7 ])
        int32                                         RequestId;                                         // 0x0024(0x0004)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    } Params{};

    Params.ReceivingActor = ReceivingActor;
    Params.InteractComponent = InteractComponent;
    Params.InteractType = InteractType;
    Params.OptionalObjectData = OptionalObjectData;
    Params.InteractionBeingAttempted = InteractionBeingAttempted;
    Params.RequestId = RequestId;
    
    callExecOG(Component, "/Script/FortniteGame.FortControllerComponent_Interaction", ServerAttemptInteract, Params);
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
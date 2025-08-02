#include "pch.h"
#include "FortniteGame/FortInventory/Header/FortInventory.h"

#include "Engine/GameplayStatics/Header/GameplayStatics.h"
#include "FortniteGame/FortLoot/Header/FortLootPackage.h"

void AFortInventory::HandleInventoryLocalUpdate()
{
    static SDK::UFunction* Func = nullptr;

    Func = SDK::PropLibrary->GetFunctionByName("FortInventory", "HandleInventoryLocalUpdate").Func;

    auto Flgs = Func->FunctionFlags();
    Func->FunctionFlags() |= 0x400;
    
    this->ProcessEvent(Func, nullptr);

    Func->FunctionFlags() = Flgs;
}

void AFortInventory::Update(AFortPlayerControllerAthena* PlayerController, FFortItemEntry* Entry)
{
    if (!PlayerController) return;
    this->SetbRequiresLocalUpdate(true);
    this->HandleInventoryLocalUpdate();

    Entry ? this->GetInventory().MarkItemDirty(*Entry) : this->GetInventory().MarkArrayDirty();
}

void AFortInventory::Remove(AFortPlayerController* PlayerController, FGuid Guid, int AmountToRemove)
{
    if (!PlayerController) return;
    
    AFortInventory* WorldInventory = PlayerController->GetWorldInventory();
    if (!WorldInventory) return;
    
    FFortItemList& Inventory = WorldInventory->GetInventory();
    TArray<UFortWorldItem*>& ItemInstances = Inventory.GetItemInstances();
    TArray<FFortItemEntry>& ReplicatedEntries = Inventory.GetReplicatedEntries();
    
    for (int32 i = ItemInstances.Num() - 1; i >= 0; i--) {
        if (ItemInstances[i] && ItemInstances[i]->GetItemEntry().GetItemGuid() == Guid) {
            ItemInstances.Remove(i);
            break;
        }
    }
    
    static int StructSize = StaticClassImpl("FortItemEntry")->GetSize();
    for (int32 i = ReplicatedEntries.Num() - 1; i >= 0; i--) {
        auto ReplicatedEntry = (FFortItemEntry*)((uint8*)ReplicatedEntries.GetData() + (i * StructSize));
        
        if (ReplicatedEntry->GetItemGuid() == Guid) {
            UE_LOG(LogNeon, Log, "Removing replicated entry at index %d", i);
            ReplicatedEntries.Remove(i);
            break;
        }
    }

    WorldInventory->Update((AFortPlayerControllerAthena*)PlayerController, nullptr);
}

FFortRangedWeaponStats* AFortInventory::GetStats(UFortWeaponItemDefinition* Def)
{
    if (!Def || !Def->GetWeaponStatHandle().DataTable)
        return nullptr;

    if (!Def->IsA<UFortWeaponRangedItemDefinition>()) 
        return nullptr;

    auto& WeaponStatHandle = Def->GetWeaponStatHandle();
    auto Table = WeaponStatHandle.DataTable;
    auto& RowMap = Table->GetRowMap();
    
    void* Row = nullptr;
    for (auto& Pair : RowMap)
    {
        if (WeaponStatHandle.GetRowName().ToString().ToString() == Pair.Key.ToString().ToString())
        {
            Row = Pair.Value;
            break;
        }
    }
    
    return Row ? (FFortRangedWeaponStats*)Row : nullptr;
}

UObject* AFortInventory::GiveItem(AFortPlayerControllerAthena* PlayerController, UFortItemDefinition* Def, int32 Count, int LoadedAmmo, int32 Level)
{
    if (!PlayerController || !Def || Count == 0)
    {
        return nullptr;
    }

    UFortWorldItem* BP = Def->CreateTemporaryItemInstanceBP(Count, Level);
    
    if (!BP) {
        UE_LOG(LogNeon, Log, "Failed to create temporary item instance");
        return nullptr;
    }
    
    auto& ItemEntry = BP->GetItemEntry();
    ItemEntry.SetCount(Count);
    ItemEntry.SetLoadedAmmo(LoadedAmmo);
    ItemEntry.SetLevel(Level);
    ItemEntry.SetItemDefinition(Def);

    AFortInventory* WorldInventory = PlayerController->GetWorldInventory();
    FFortItemList& Inventory = WorldInventory->GetInventory();
    TArray<FFortItemEntry>& ReplicatedEntriesOffsetPtr = Inventory.GetReplicatedEntries();
    TArray<UFortWorldItem*>& ItemInstancesOffsetPtr = Inventory.GetItemInstances();
    
    static int StructSize = StaticClassImpl("FortItemEntry")->GetSize();
    ReplicatedEntriesOffsetPtr.Add(BP->GetItemEntry(), StructSize);
    ItemInstancesOffsetPtr.Add(BP);
    
    WorldInventory->Update(PlayerController, &ItemEntry);
    
    return BP;
}

UObject* AFortInventory::GiveItem(AFortAthenaAIBotController* Controller, UFortItemDefinition* Def, int32 Count, int LoadedAmmo, int32 Level)
{
    if (!Controller || !Def || Count == 0)
    {
        return nullptr;
    }

    UFortWorldItem* BP = Def->CreateTemporaryItemInstanceBP(Count, Level);

    if (!BP) {
        UE_LOG(LogNeon, Log, "Failed to create temporary item instance");
        return nullptr;
    }

    auto& ItemEntry = BP->GetItemEntry();
    ItemEntry.SetCount(Count);
    ItemEntry.SetLoadedAmmo(LoadedAmmo);
    ItemEntry.SetLevel(Level);
    ItemEntry.SetItemDefinition(Def);
    
    AFortInventory* PCInventory = Controller->GetInventory();
    if (PCInventory)
    {
        FFortItemList& Inventory = PCInventory->GetInventory();
        if (&Inventory)
        {
            TArray<FFortItemEntry>& ReplicatedEntriesOffsetPtr = Inventory.GetReplicatedEntries();
            TArray<UFortWorldItem*>& ItemInstancesOffsetPtr = Inventory.GetItemInstances();

            static int StructSize = StaticClassImpl("FortItemEntry")->GetSize();

            ReplicatedEntriesOffsetPtr.Add(BP->GetItemEntry(), StructSize);
            ItemInstancesOffsetPtr.Add(BP);
            PCInventory->Update((AFortPlayerControllerAthena*)Controller, &ItemEntry);
        }
    }
    
    return BP;
}

AFortPickupAthena* AFortInventory::SpawnPickup(FVector Loc, FFortItemEntry* Entry, EFortPickupSourceTypeFlag SourceTypeFlag, EFortPickupSpawnSource SpawnSource, AFortPlayerPawn* Pawn, int OverrideCount, bool Toss, bool RandomRotation, bool bCombine)
{
    if (!Entry || !Pawn) return nullptr;
    if (Entry->GetItemDefinition()->GetFName().ToString().ToString().find("Pickaxe") != std::string::npos) return nullptr;

    AFortPickupAthena* NewPickup = UGameplayStatics::SpawnActorOG<AFortPickupAthena>(AFortPickupAthena::StaticClass(), Loc);
    if (NewPickup != nullptr && Entry != nullptr && Entry->GetItemDefinition() != nullptr)
    {
        if (Finder->SetupPickup())
        {
            ((FVector * (*)(AFortPickup*, FFortItemEntry*, TArray<FFortItemEntry>, bool))(Finder->SetupPickup()))(NewPickup, Entry, TArray<FFortItemEntry>(), false);
        }
        
        NewPickup->SetbRandomRotation(RandomRotation);
        NewPickup->GetPrimaryPickupItemEntry().SetItemDefinition(Entry->GetItemDefinition());
        NewPickup->GetPrimaryPickupItemEntry().SetLoadedAmmo(Entry->GetLoadedAmmo());
        NewPickup->GetPrimaryPickupItemEntry().SetCount(OverrideCount != -1 ? OverrideCount : Entry->GetCount());
        NewPickup->OnRep_PrimaryPickupItemEntry();
            
        NewPickup->CallFunc<void>("FortPickup", "TossPickup", Loc, Pawn, -1, Toss, true, SourceTypeFlag, SpawnSource);
    }
    
    return NewPickup;
}

AFortPickupAthena* AFortInventory::SpawnPickupDirect(FVector Loc, UFortItemDefinition* ItemDefinition, int Count, int LoadedAmmo, EFortPickupSourceTypeFlag SourceTypeFlag, EFortPickupSpawnSource SpawnSource, AFortPlayerPawn* Pawn, bool Toss)
{
    if (!ItemDefinition) return nullptr;
 //   if (ItemDefinition->GetFName().ToString().ToString().find("Pickaxe") != std::string::npos) return nullptr;
    
    AFortPickupAthena* NewPickup = UGameplayStatics::SpawnActorOG<AFortPickupAthena>(AFortPickupAthena::StaticClass(), Loc);
    if (NewPickup != nullptr && ItemDefinition != nullptr)
    {
        NewPickup->SetbRandomRotation(true);
        NewPickup->GetPrimaryPickupItemEntry().SetItemDefinition(ItemDefinition);
        NewPickup->GetPrimaryPickupItemEntry().SetLoadedAmmo(LoadedAmmo);
        NewPickup->GetPrimaryPickupItemEntry().SetCount(Count);
        NewPickup->OnRep_PrimaryPickupItemEntry();
        
        NewPickup->CallFunc<void>("FortPickup", "TossPickup", Loc, Pawn, -1, Toss, true, SourceTypeFlag, SpawnSource);
    }
    
    return NewPickup;
}

AFortPickupAthena* AFortInventory::SpawnPickup(FVector Loc, UFortItemDefinition* ItemDefinition, int Count, int LoadedAmmo, EFortPickupSourceTypeFlag SourceTypeFlag, EFortPickupSpawnSource SpawnSource, AFortPlayerPawn* Pawn, bool Toss)
{
    FFortItemEntry* ItemEntry = MakeItemEntry(ItemDefinition, Count, 0);
    return SpawnPickup(Loc, ItemEntry, SourceTypeFlag, SpawnSource, Pawn, -1, Toss, true, true);
}

void AFortInventory::ReplaceEntry(AFortPlayerController* PlayerController, FFortItemEntry& Entry) {
    if (!PlayerController) return;
    
    AFortInventory* WorldInventory = PlayerController->GetWorldInventory();
    if (!WorldInventory) return;
    
    FFortItemList& Inventory = WorldInventory->GetInventory();
    TArray<UFortWorldItem*>& ItemInstances = Inventory.GetItemInstances();
    TArray<FFortItemEntry>& ReplicatedEntries = Inventory.GetReplicatedEntries();
    
    FGuid TargetGuid = Entry.GetItemGuid();
    
    int32 ItemIndex = -1;
    int32 ReplicatedIndex = -1;
    
    for (int32 i = 0; i < ItemInstances.Num(); i++) {
        if (ItemInstances[i] && ItemInstances[i]->GetItemEntry().GetItemGuid() == TargetGuid) {
            ItemIndex = i;
            break;
        }
    }
    
    for (int32 i = 0; i < ReplicatedEntries.Num(); i++) {
        static int StructSize = StaticClassImpl("FortItemEntry")->GetSize();
        auto ReplicatedEntry = (FFortItemEntry*)((uint8*)ReplicatedEntries.GetData() + (i * StructSize));
        
        if (ReplicatedEntry->GetItemGuid() == TargetGuid) {
            ReplicatedIndex = i;
            break;
        }
    }
    
    if (ReplicatedIndex != -1) {
        ReplicatedEntries.Remove(ReplicatedIndex);
    }
    
    if (ItemIndex != -1) {
        ItemInstances.Remove(ItemIndex);
    }
    
    if (Entry.GetItemDefinition()) {
        WorldInventory->GiveItem(
            Cast<AFortPlayerControllerAthena>(PlayerController),
            Entry.GetItemDefinition(),
            Entry.GetCount(),
            Entry.GetLoadedAmmo(),
            Entry.GetLevel()
        );
    }
}

FFortItemEntry* AFortInventory::MakeItemEntry(UFortItemDefinition* ItemDefinition, int32 Count, int32 Level) {
    int32 FortItemEntrySize = StaticClassImpl("FortItemEntry")->GetSize();
    FFortItemEntry* IE = (FFortItemEntry*)VirtualAlloc(0, FortItemEntrySize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    
    IE->MostRecentArrayReplicationKey = -1;
    IE->ReplicationID = -1;
    IE->ReplicationKey = -1;

    IE->SetItemDefinition(ItemDefinition);
    IE->SetCount(Count);
    IE->SetLoadedAmmo(/*ItemDefinition->IsA<UFortWeaponItemDefinition>() ? GetStats((UFortWeaponItemDefinition*)ItemDefinition)->ClipSize : */0);
    IE->SetDurability(1.f);
    IE->SetGameplayAbilitySpecHandle(FGameplayAbilitySpecHandle(-1));
    IE->SetLevel(Level);

    return IE;
}

void UFortItem::SetOwningControllerForTemporaryItem(AFortPlayerControllerAthena* InController)
{
    static SDK::UFunction* Func = nullptr;

    SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortItem", "SetOwningControllerForTemporaryItem");

    if (Func == nullptr)
        Func = Info.Func;
    if (!Func)
        return;

    struct FortItem_SetOwningControllerForTemporaryItem final
    {
    public:
        class AFortPlayerControllerAthena* InController;                                      // 0x0000(0x0008)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    };

    FortItem_SetOwningControllerForTemporaryItem Params{};

    Params.InController = InController;

    auto Flgs = Func->FunctionFlags();
    Func->FunctionFlags() |= 0x400;
    
    this->ProcessEvent(Func, &Params);

    Func->FunctionFlags() = Flgs;
}


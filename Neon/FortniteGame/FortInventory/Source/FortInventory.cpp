#include "pch.h"
#include "FortniteGame/FortInventory/Header/FortInventory.h"

#include "Engine/GameplayStatics/Header/GameplayStatics.h"

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
        TArray<FFortItemEntry>& ReplicatedEntriesOffsetPtr = Inventory.GetReplicatedEntries();
        TArray<UFortWorldItem*>& ItemInstancesOffsetPtr = Inventory.GetItemInstances();

        if (ReplicatedEntriesOffsetPtr.Num() > 0 && ItemInstancesOffsetPtr.Num() > 0)
        {
            static int StructSize = StaticClassImpl("FortItemEntry")->GetSize();
            
            if (BP) ReplicatedEntriesOffsetPtr.Add(ItemEntry, StructSize);
            if (BP) ItemInstancesOffsetPtr.Add(BP);
            PCInventory->Update((AFortPlayerControllerAthena*)Controller, &ItemEntry);
        }
    }
    
    return BP;
}

AFortPickupAthena* AFortInventory::SpawnPickup(FVector Loc, FFortItemEntry& Entry, EFortPickupSourceTypeFlag SourceTypeFlag, EFortPickupSpawnSource SpawnSource, AFortPlayerPawn* Pawn, int OverrideCount, bool Toss, bool RandomRotation, bool bCombine)
{
    if (OverrideCount != -1)
        printf("Count: %d\n", OverrideCount);
    AFortPickupAthena* NewPickup = UGameplayStatics::SpawnActor<AFortPickupAthena>(Loc, {});
    if (NewPickup != nullptr && Entry.GetItemDefinition() != nullptr)
    {
        NewPickup->SetbRandomRotation(RandomRotation);
        NewPickup->GetPrimaryPickupItemEntry().SetItemDefinition(Entry.GetItemDefinition());
        NewPickup->GetPrimaryPickupItemEntry().SetCount(OverrideCount != -1 ? OverrideCount : Entry.GetCount());
        NewPickup->CallFunc<void>("FortPickup", "OnRep_PrimaryPickupItemEntry");
        
        NewPickup->CallFunc<void>("FortPickup", "TossPickup", Loc, Pawn, -1, Toss, true, SourceTypeFlag, SpawnSource);
        NewPickup->Set("FortPickup", "bTossedFromContainer", SpawnSource == EFortPickupSpawnSource::Chest || SpawnSource == EFortPickupSpawnSource::AmmoBox);
        if (NewPickup->Get<bool>("FortPickup", "bTossedFromContainer")) NewPickup->CallFunc<void>("FortPickup", "OnRep_TossedFromContainer");
    }

    return NewPickup;
}

AFortPickupAthena* AFortInventory::SpawnPickup(FVector Loc, UFortItemDefinition* ItemDefinition, int Count, int LoadedAmmo, EFortPickupSourceTypeFlag SourceTypeFlag, EFortPickupSpawnSource SpawnSource, AFortPlayerPawn* Pawn, bool Toss)
{
    FFortItemEntry ItemEntry = MakeItemEntry(ItemDefinition, Count, 0);
    return SpawnPickup(Loc, ItemEntry, SourceTypeFlag, SpawnSource, Pawn, -1, Toss, true, true);
}

void AFortInventory::ReplaceEntry(AFortPlayerController* PlayerController, FFortItemEntry& Entry)
{
    if (!PlayerController) return;
    
    AFortInventory* WorldInventory = PlayerController->GetWorldInventory();
    FFortItemList& Inventory = WorldInventory->GetInventory();
    TArray<UFortWorldItem*>& ItemInstancesOffsetPtr = Inventory.GetItemInstances();
    TArray<FFortItemEntry>& ReplicatedEntriesOffsetPtr = Inventory.GetReplicatedEntries();

    UFortWorldItem* EToDelete = nullptr;
    int32 RIndex = -1;
    int32 EIndex = -1;

    for (int32 i = 0; i < ItemInstancesOffsetPtr.Num(); i++)
    {
        if (ItemInstancesOffsetPtr[i] && ItemInstancesOffsetPtr[i]->GetItemEntry().GetItemGuid() == Entry.GetItemGuid())
        {
            EToDelete = ItemInstancesOffsetPtr[i];
            EIndex = i;
            break;
        }
    }

    for (int32 i = 0; i < ReplicatedEntriesOffsetPtr.Num(); i++)
    {
        static int StructSize = StaticClassImpl("FortItemEntry")->GetSize();
        auto ReplicatedEntry = (FFortItemEntry*) ((uint8*) ReplicatedEntriesOffsetPtr.GetData() + (i * StructSize));
    
        if (ReplicatedEntry->GetItemGuid() == EToDelete->GetItemEntry().GetItemGuid())
        {
            RIndex = i;
            break;
        }
    }

    if (EToDelete && EIndex != -1)
    {
        ItemInstancesOffsetPtr.Remove(EIndex);
    }

    if (RIndex != -1)
    {
        ReplicatedEntriesOffsetPtr.Remove(RIndex);
    }

    if (Entry.GetItemDefinition())
    {
        WorldInventory->GiveItem(
            Cast<AFortPlayerControllerAthena>(PlayerController),
            Entry.GetItemDefinition(),
            Entry.GetCount(),
            Entry.GetLoadedAmmo(),
            Entry.GetLevel()
        );
    }
}

FFortItemEntry AFortInventory::MakeItemEntry(UFortItemDefinition* ItemDefinition, int32 Count, int32 Level) {
    FFortItemEntry IE{};

    IE.MostRecentArrayReplicationKey = -1;
    IE.ReplicationID = -1;
    IE.ReplicationKey = -1;

    IE.SetItemDefinition(ItemDefinition);
    IE.SetCount(Count);
    IE.SetLoadedAmmo(/*ItemDefinition->IsA<UFortWeaponItemDefinition>() ? GetStats((UFortWeaponItemDefinition*)ItemDefinition)->ClipSize : */0);
    IE.SetDurability(1.f);
    IE.SetGameplayAbilitySpecHandle(FGameplayAbilitySpecHandle(-1));
    IE.SetLevel(Level);

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


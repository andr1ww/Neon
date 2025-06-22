#include "pch.h"
#include "FortniteGame/FortInventory/Header/FortInventory.h"

void AFortInventory::Update(AFortPlayerControllerAthena* PlayerController, FFortItemEntry Entry)
{
    if (!PlayerController) return;
    AFortInventory* WorldInventory = PlayerController->GetWorldInventory();
    FFortItemList Inventory = WorldInventory->GetInventory();
    WorldInventory->Set("FortInventory", "bRequiresLocalUpdate", true);
    WorldInventory->CallFunc<void>("FortInventory", "HandleInventoryLocalUpdate");

    FFastArraySerializer InventorySerializer = Inventory;
    if (Entry.ReplicationKey)
    {
        FFastArraySerializerItem EntryRef = static_cast<FFastArraySerializerItem>(Entry);
        InventorySerializer.MarkItemDirty(EntryRef);
        InventorySerializer.MarkArrayDirty();
    }
    else
    {
        InventorySerializer.MarkArrayDirty();
    }
}

UObject* AFortInventory::GiveItem(AFortPlayerControllerAthena* PlayerController, UFortItemDefinition* Def, int Count, int LoadedAmmo, int Level)
{
    if (!PlayerController) return nullptr;
    UFortWorldItem* BP = Def->CallFunc<UFortWorldItem*>("FortItemDefinition", "CreateTemporaryItemInstanceBP", Count, Level);
    
//    BP->SetOwningControllerForTemporaryItem(PlayerController);
    BP->GetItemEntry().SetCount(Count);
    BP->GetItemEntry().SetLoadedAmmo(LoadedAmmo);
    BP->GetItemEntry().SetLevel(Level);
    BP->GetItemEntry().SetItemDefinition(Def);
    
    AFortInventory* WorldInventory = PlayerController->GetWorldInventory();
    FFortItemList Inventory = WorldInventory->GetInventory();
    
    TArray<FFortItemEntry> ReplicatedEntries = Inventory.GetReplicatedEntries();
    TArray<UFortItem*> ItemInstances = Inventory.GetItemInstances();

    ReplicatedEntries.Add(BP->GetItemEntry());
    ItemInstances.Add(BP);

    Update(PlayerController, BP->GetItemEntry());

    return BP;
}

void UFortItem::SetOwningControllerForTemporaryItem(AFortPlayerControllerAthena* InController)
{
    this->CallFunc<void>("FortItem", "SetOwningControllerForTemporaryItem", &InController);
}

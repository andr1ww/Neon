#include "pch.h"
#include "FortniteGame/FortInventory/Header/FortInventory.h"

void AFortInventory::Update(AFortPlayerControllerAthena* PlayerController, FFortItemEntry Entry)
{
    if (!PlayerController) return;
    AFortInventory* WorldInventory = PlayerController->GetWorldInventory();
    static int InventoryOffset = Runtime::GetOffset(WorldInventory, "Inventory");
    FFortItemList& InventoryPtr = *reinterpret_cast<FFortItemList*>(__int64(WorldInventory) + InventoryOffset);

    WorldInventory->SetBitfield<L"bRequiresLocalUpdate">(true);
    WorldInventory->Call<"HandleInventoryLocalUpdate">();

    InventoryPtr.MarkItemDirty(Entry);
    InventoryPtr.MarkArrayDirty();
}

UObject* AFortInventory::GiveItem(AFortPlayerControllerAthena* PlayerController, UFortItemDefinition* Def, int Count, int LoadedAmmo, int Level)
{
    if (!PlayerController) return nullptr;
    struct FortItemDefinition_CreateTemporaryItemInstanceBP final
    {
    public:
        int32                                         Count;
        int32                                         Level;
        UFortWorldItem* ReturnValue;
    } Params{
        Count, Level
    };

    Def->Call<"CreateTemporaryItemInstanceBP">(&Params);

    UFortWorldItem* BP = Params.ReturnValue;

    if (!BP) {
        return nullptr;
    }
    
    BP->SetOwningControllerForTemporaryItem(PlayerController);
    BP->GetItemEntry().SetCount(Count);
    BP->GetItemEntry().SetLoadedAmmo(LoadedAmmo);
    BP->GetItemEntry().SetLevel(Level);
    BP->GetItemEntry().SetItemDefinition(Def);
    
    AFortInventory* WorldInventory = PlayerController->GetWorldInventory();
    static int InventoryOffset = Runtime::GetOffset(WorldInventory, "Inventory");
    FFortItemList& InventoryPtr = *reinterpret_cast<FFortItemList*>(__int64(WorldInventory) + InventoryOffset);

    static int ReplicatedEntriesOffset = Runtime::GetOffsetStruct("FortItemList", "ReplicatedEntries");
    TArray<FFortItemEntry>& ReplicatedEntriesOffsetPtr = *reinterpret_cast<TArray<FFortItemEntry>*>(reinterpret_cast<__int64>(&InventoryPtr) + ReplicatedEntriesOffset);

    static int ItemInstancesOffset = Runtime::GetOffsetStruct("FortItemList", "ItemInstances");
    TArray<UFortItem*>& ItemInstancesOffsetPtr = *reinterpret_cast<TArray<UFortItem*>*>(reinterpret_cast<__int64>(&InventoryPtr) + ItemInstancesOffset);
    
    ReplicatedEntriesOffsetPtr.Add(BP->GetItemEntry());
    ItemInstancesOffsetPtr.Add(BP);

    Update(PlayerController, BP->GetItemEntry());

    return BP;
}

void UFortItem::SetOwningControllerForTemporaryItem(AFortPlayerControllerAthena* InController)
{
    this->Call<"SetOwningControllerForTemporaryItem">(InController);
}

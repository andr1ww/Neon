#include "pch.h"
#include "FortniteGame/FortInventory/Header/FortInventory.h"

void AFortInventory::HandleInventoryLocalUpdate()
{
    static SDK::UFunction* Func = nullptr;

    SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortInventory", "HandleInventoryLocalUpdate");

    if (Func == nullptr)
        Func = Info.Func;
    if (!Func)
        return;

    auto Flgs = Func->FunctionFlags();
    Func->FunctionFlags() |= 0x400;
    
    SDK::StaticClassImpl("FortInventory")->GetClassDefaultObject()->ProcessEvent(Func, nullptr);

    Func->FunctionFlags() = Flgs;
}

void AFortInventory::Update(AFortPlayerControllerAthena* PlayerController, FFortItemEntry Entry)
{
    if (!PlayerController) return;
    AFortInventory* WorldInventory = PlayerController->GetWorldInventory();
    static int InventoryOffset = Runtime::GetOffset(WorldInventory, "Inventory");
    FFortItemList& InventoryPtr = *reinterpret_cast<FFortItemList*>(__int64(WorldInventory) + InventoryOffset);

    WorldInventory->Set("FortInventory", "bRequiresLocalUpdate", true);
    WorldInventory->HandleInventoryLocalUpdate();

    InventoryPtr.MarkItemDirty(Entry);
    InventoryPtr.MarkArrayDirty();
}

UObject* AFortInventory::GiveItem(AFortPlayerControllerAthena* PlayerController, UFortItemDefinition* Def, int Count, int LoadedAmmo, int Level)
{
    if (!PlayerController) return nullptr;
    UFortWorldItem* BP = Def->CallFunc<UFortWorldItem*>("FortItemDefinition", "CreateTemporaryItemInstanceBP", Count, Level);
    if (!BP) {
        UE_LOG(LogNeon, Error, "Failed to create temporary item instance for %s", *Def->GetFName().ToString());
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
    struct FortItem_SetOwningControllerForTemporaryItem final
    {
    public:
        class AFortPlayerControllerAthena*                  InController;                                      // 0x0000(0x0008)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    };

    FortItem_SetOwningControllerForTemporaryItem Params{};

    Params.InController = InController;
    static SDK::UFunction* Func = nullptr;

    SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortItem", "SetOwningControllerForTemporaryItem");

    if (Func == nullptr)
        Func = Info.Func;
    if (!Func)
        return;

    auto Flgs = Func->FunctionFlags();
    Func->FunctionFlags() |= 0x400;
    
    SDK::StaticClassImpl("FortItem")->GetClassDefaultObject()->ProcessEvent(Func, &Params);

    Func->FunctionFlags() = Flgs;
}

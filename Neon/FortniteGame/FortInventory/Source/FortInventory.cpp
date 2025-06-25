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

void AFortInventory::Update(AFortPlayerControllerAthena* PlayerController, FFortItemEntry* Entry)
{
    if (!PlayerController) return;
    AFortInventory* WorldInventory = PlayerController->GetWorldInventory();
    FFortItemList& Inventory = WorldInventory->GetInventory();
    
    WorldInventory->SetbRequiresLocalUpdate(true);
    WorldInventory->HandleInventoryLocalUpdate();

    Inventory.MarkItemDirty(*Entry);
    Inventory.MarkArrayDirty();
}

UObject* AFortInventory::GiveItem(AFortPlayerControllerAthena* PlayerController, UFortItemDefinition* Def, int32 Count, int LoadedAmmo, int32 Level)
{
    if (!PlayerController || !Def || !Count || !Level) return nullptr;

    UFortWorldItem* BP = Def->CreateTemporaryItemInstanceBP(Count, Level);
    UE_LOG(LogNeon, Log, "BP: %s", BP->GetFName().ToString().ToString().c_str());
    
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
    

    static int ReplicatedEntriesOffset = Runtime::GetOffsetStruct("FortItemList", "ReplicatedEntries");
    TArray<FFortItemEntry>& ReplicatedEntriesOffsetPtr = *reinterpret_cast<TArray<FFortItemEntry>*>(reinterpret_cast<__int64>(&Inventory) + ReplicatedEntriesOffset);

    static int ItemInstancesOffset = Runtime::GetOffsetStruct("FortItemList", "ItemInstances");
    TArray<UFortItem*>& ItemInstancesOffsetPtr = *reinterpret_cast<TArray<UFortItem*>*>(reinterpret_cast<__int64>(&Inventory) + ItemInstancesOffset);
    
    ReplicatedEntriesOffsetPtr.Add(BP->GetItemEntry());
    ItemInstancesOffsetPtr.Add(BP);

    Update(PlayerController, &ItemEntry);

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
    
    this->ProcessEvent(Func, &Params);

    Func->FunctionFlags() = Flgs;
}

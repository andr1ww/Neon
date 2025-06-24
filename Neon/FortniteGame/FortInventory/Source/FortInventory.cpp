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
    FFortItemList Inventory = WorldInventory->GetInventory();
    
    WorldInventory->Set("FortInventory", "bRequiresLocalUpdate", true);
    WorldInventory->HandleInventoryLocalUpdate();

    Inventory.MarkItemDirty(Entry);
    Inventory.MarkArrayDirty();
}

UObject* AFortInventory::GiveItem(AFortPlayerControllerAthena* PlayerController, UFortItemDefinition* Def, int Count, int LoadedAmmo, int Level)
{
    if (!PlayerController) return nullptr;
    
    UFortWorldItem* BP = Def->CallFunc<UFortWorldItem*>("FortItemDefinition", "CreateTemporaryItemInstanceBP", Count, Level);
    if (!BP) {
        UE_LOG(LogNeon, Error, "Failed to create temporary item instance for %s", *Def->GetFName().ToString().ToString().c_str());
        return nullptr;
    }
    
    auto ItemEntry = BP->GetItemEntry();
    ItemEntry.SetCount(Count);
    ItemEntry.SetLoadedAmmo(LoadedAmmo);
    ItemEntry.SetLevel(Level);
    ItemEntry.SetItemDefinition(Def);

    AFortInventory* WorldInventory = PlayerController->GetWorldInventory();
    FFortItemList Inventory = WorldInventory->GetInventory();
    TArray<FFortItemEntry>& ReplicatedEntries = Inventory.GetReplicatedEntries();
    TArray<UFortItem*>& ItemInstances = Inventory.GetItemInstances();
    
    ReplicatedEntries.Add(ItemEntry);
    ItemInstances.Add(BP);

    Update(PlayerController, ItemEntry);

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

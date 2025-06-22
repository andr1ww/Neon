#include "pch.h"
#include "FortniteGame/FortInventory/Header/FortInventory.h"

void AFortInventory::Update(AFortPlayerControllerAthena* PlayerController, UObject* Entry)
{
    if (!PlayerController) return;
    auto WorldInventory = PlayerController->Get<UObject*>("FortPlayerController", "WorldInventory");
    auto Inventory = WorldInventory->Get<UStruct*>("FortInventory", "Inventory");
    WorldInventory->Set("FortInventory", "bRequiresLocalUpdate", true);
    WorldInventory->CallFunc<void>("FortInventory", "HandleInventoryLocalUpdate");

    if (Entry)
    {
        auto EntryPtr = reinterpret_cast<FFastArraySerializerItem*>(Entry);
        FFastArraySerializerItem& EntryRef = *EntryPtr;
        ((FFastArraySerializer*)Inventory)->MarkItemDirty(EntryRef);
        ((FFastArraySerializer*)Inventory)->MarkArrayDirty();
    } else
    {
        ((FFastArraySerializer*)Inventory)->MarkArrayDirty();
    }
}

UObject* AFortInventory::GiveItem(AFortPlayerControllerAthena* PlayerController, UObject* Def, int Count, int LoadedAmmo, int Level)
{
    if (!PlayerController || !Def) return nullptr;
}

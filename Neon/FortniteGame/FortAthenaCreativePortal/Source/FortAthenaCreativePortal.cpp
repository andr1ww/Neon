#include "pch.h"
#include "../Header/FortAthenaCreativePortal.h"

#include "Engine/GameplayStatics/Header/GameplayStatics.h"
#include "FortniteGame/FortLootPackage/Header/FortLootPackage.h"

void AFortAthenaCreativePortal::TeleportPlayerToLinkedVolume(AFortAthenaCreativePortal* Portal, FFrame& Stack)
{
    AFortPlayerPawn* PlayerPawn;
    bool bUseSpawnTags;
    Stack.StepCompiledIn(&PlayerPawn);
    Stack.StepCompiledIn(&bUseSpawnTags);
    Stack.IncrementCode();

    if (!PlayerPawn)
        return;

    auto Volume = Portal->GetLinkedVolume();
    auto Location = Volume->K2_GetActorLocation();
    Location.Z = 10000;

    PlayerPawn->K2_TeleportTo(Location, FRotator());
    PlayerPawn->BeginSkydiving(false);
    
    FFortItemList& Inventory = ((AFortPlayerControllerAthena*)PlayerPawn->GetController())->GetWorldInventory()->GetInventory();
    TArray<UFortWorldItem*>& ItemInstances = Inventory.GetItemInstances();
    
    int32 ItemIndex = -1;

    static UFortItemDefinition* Phone = Runtime::StaticLoadObject<UFortWeaponRangedItemDefinition>("/Game/Athena/Items/Weapons/Prototype/WID_CreativeTool.WID_CreativeTool");
    if (!Phone)
    {
        UE_LOG(LogNeon, Log, "Failed to load Phone item definition");
    }
    
    for (int32 i = 0; i < ItemInstances.Num(); i++) {
        if (ItemInstances[i] && ItemInstances[i]->GetItemEntry().GetItemDefinition() == Phone) {
            ItemIndex = i;
            break;
        }
    }
    
    if (ItemIndex == -1) {
        AFortInventory::GiveItem(((AFortPlayerControllerAthena*)PlayerPawn->GetController()), Phone, 1, 0, 0);
    }
}


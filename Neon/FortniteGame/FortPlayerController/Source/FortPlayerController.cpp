#include "pch.h"
#include "../Header/FortPlayerController.h"

#include "Engine/Guid/Header/Guid.h"
#include "FortniteGame/AbilitySystemComponent/Header/AbilitySystemComponent.h"
#include "Neon/Finder/Header/Finder.h"

void AFortPlayerControllerAthena::ServerAcknowledgePossession(AFortPlayerControllerAthena* PlayerController, FFrame& Stack) 
{
    APawn* PawnToAcknowledge;
    Stack.StepCompiledIn(&PawnToAcknowledge);
    Stack.IncrementCode();
    
    if (!PlayerController) return;
    PlayerController->SetAcknowledgedPawn(PawnToAcknowledge);
    PlayerController->GetPlayerState()->SetHeroType(PlayerController->GetCosmeticLoadoutPC().GetCharacter()->GetHeroDefinition());
    void* (*ApplyCharacterCustomization)(AFortPlayerStateAthena*, APawn*) = decltype(ApplyCharacterCustomization)(Finder->ApplyCharacterCustomization());
    ApplyCharacterCustomization(PlayerController->GetPlayerState(), PawnToAcknowledge);
    static UFortAbilitySet* AbilitySet = (UFortAbilitySet*)GUObjectArray.FindObject("GAS_AthenaPlayer");
    UAbilitySystemComponent::GiveAbilitySet(PlayerController->GetPlayerState()->GetAbilitySystemComponent(), AbilitySet);
}

void AFortPlayerControllerAthena::ServerExecuteInventoryItem(AFortPlayerControllerAthena* PlayerController, FFrame& Stack)
{
    FGuid ItemGuid;
    Stack.StepCompiledIn(&ItemGuid);
    Stack.IncrementCode();
    if (!PlayerController) return;

    FFortItemEntry* Entry = nullptr;
    AFortInventory* WorldInventory = PlayerController->GetWorldInventory();
    FFortItemList& Inventory = WorldInventory->GetInventory();
    TArray<FFortItemEntry>& ReplicatedEntriesOffsetPtr = Inventory.GetReplicatedEntries();
    int32 FortItemEntrySize = StaticClassImpl("FortItemEntry")->GetSize();

    for (int32 i = 0; i < ReplicatedEntriesOffsetPtr.Num(); i++)
    {
        auto Item = (FFortItemEntry*) ((uint8*) ReplicatedEntriesOffsetPtr.GetData() + (i * FortItemEntrySize));
        if (Item->GetItemGuid() == ItemGuid)
        {
            Entry = Item;
            break;
        }
    }

    if (!Entry)
    {
        UE_LOG(LogNeon, Warning, "ServerExecuteInventoryItem: Entry not found!");
        return;
    }
    
    UFortWeaponItemDefinition* ItemDefinition = Cast<UFortWeaponItemDefinition>(Entry->GetItemDefinition());
    if (Fortnite_Version.GetMajorVersion() >= 19.00) {
        PlayerController->GetMyFortPawn()->CallFunc<void>("FortPawn", "EquipWeaponDefinition", ItemDefinition, ItemGuid, Entry->GetTrackerGuid(), false);
    } else {
        PlayerController->GetMyFortPawn()->CallFunc<void>("FortPawn", "EquipWeaponDefinition", ItemDefinition, ItemGuid);
    }
}

 
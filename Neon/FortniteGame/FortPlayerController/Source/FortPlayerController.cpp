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
 //   ApplyCharacterCustomization(PlayerController->GetPlayerState(), PawnToAcknowledge);
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

void AFortPlayerControllerAthena::ServerPlayEmoteItem(AFortPlayerControllerAthena* PlayerController, FFrame& Stack)
{
    UFortMontageItemDefinitionBase* Asset;
    Stack.StepCompiledIn(&Asset);
    Stack.IncrementCode();

    UAbilitySystemComponent* AbilitySystemComponent = PlayerController->GetPlayerState()->GetAbilitySystemComponent();
    UGameplayAbility* Ability = nullptr;

    if (Cast<UAthenaSprayItemDefinition>(Asset))
    {
        static auto SprayAbility = UGAB_Spray_Generic_C::GetDefaultObj();
        Ability = Cast<UGameplayAbility>(SprayAbility);
    }
    
    if (auto Dance = Cast<UAthenaDanceItemDefinition>(Asset))
    {
        PlayerController->GetMyFortPawn()->SetbMovingEmote(Dance->GetbMovingEmote());
        PlayerController->GetMyFortPawn()->SetbMovingEmoteForwardOnly(Dance->GetbMoveForwardOnly());
        PlayerController->GetMyFortPawn()->SetEmoteWalkSpeed(Dance->GetWalkForwardSpeed());
        static auto EmoteAbility = UGAB_Emote_Generic_C::GetDefaultObj();
        Ability = Cast<UGameplayAbility>(EmoteAbility);
    }
    
    if (Ability)
    {
        int32 GameplayAbilitySpecSize = StaticClassImpl("GameplayAbilitySpec")->GetSize();
        FGameplayAbilitySpec* Spec = (FGameplayAbilitySpec*)VirtualAlloc(0, GameplayAbilitySpecSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    
        if (!Spec) return;
    
        new(Spec) FGameplayAbilitySpec();
    
        using GiveAbilityFunc = FGameplayAbilitySpecHandle(__fastcall*)(
            UAbilitySystemComponent*,
            FGameplayAbilitySpecHandle*, 
            const FGameplayAbilitySpec&,
            FGameplayEventData*
        );
        
        Spec->MostRecentArrayReplicationKey = -1;
        Spec->ReplicationID = -1;
        Spec->ReplicationKey = -1;
        Spec->GetHandle().Handle = rand();
        Spec->SetAbility(Ability);
        Spec->SetSourceObject(Asset);
        Spec->SetInputID(-1);
        Spec->SetLevel(1);
    
        FGameplayAbilitySpecHandle Handle = Spec->GetHandle();
    
        if (Finder->GiveAbilityAndActivateOnce())
        {
            ((GiveAbilityFunc)Finder->GiveAbilityAndActivateOnce())(AbilitySystemComponent, &Handle, *Spec, nullptr);
        }
    }
}

 
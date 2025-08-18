#include "pch.h"
#include "../Header/FortPlayerPawn.h"

#include "FortniteGame/FortLootPackage/Header/FortLootPackage.h"

void AFortPlayerPawn::ServerHandlePickupInfo(AFortPlayerPawn* Pawn, FFrame& Stack)
{
    AFortPickup* Pickup;
    FFortPickupRequestInfo Params;
    Stack.StepCompiledIn(&Pickup);
    Stack.StepCompiledIn(&Params);
    Stack.IncrementCode();

    if (!Pawn || !Pickup || Pickup->GetbPickedUp())
        return;

    Pawn->GetIncomingPickups().Add(Pickup);
    
    Pickup->OnRep_PickupLocationData();

    Pickup->SetbPickedUp(true);
    Pickup->OnRep_bPickedUp();
}

void AFortPlayerPawn::CompletePickupAnimation(AFortPickup* Pickup)
{
    UE_LOG(LogNeon, Log, __FUNCTION__);
    AFortPlayerPawn* Pawn = (AFortPlayerPawn*)Pickup->GetPickupLocationData().PickupTarget;
    if (!Pawn) return CompletePickupAnimationOG(Pickup);

    AFortPlayerControllerAthena* PlayerController = (AFortPlayerControllerAthena*)Pawn->GetController();
    if (!PlayerController) return CompletePickupAnimationOG(Pickup);

    AFortInventory* WorldInventory = nullptr;
    AFortAthenaAIBotController* AIController = nullptr;
    bool bIsAI = false;
    
    if (auto AI = Cast<AFortAthenaAIBotController>(PlayerController))
    {
        WorldInventory = AI->GetInventory();
        AIController = AI;
        bIsAI = true;
    }
    if (!WorldInventory)
        WorldInventory = PlayerController->GetWorldInventory();
    if (!WorldInventory)
        return;

    auto MyFortPawn = PlayerController->GetMyFortPawn();
    if (auto AI = Cast<AFortAthenaAIBotController>(PlayerController))
    {
        MyFortPawn = (AFortPlayerPawn*)AI->GetPawn();
    }

    if (!MyFortPawn) return;
    
    FFortItemList& Inventory = WorldInventory->GetInventory();
    TArray<UFortWorldItem*>& ItemInstances = Inventory.GetItemInstances();

    int ItemCount = 0;
    for (int32 i = 0; i < ItemInstances.Num(); i++) {
        if (FortLootPackage::GetQuickbar(ItemInstances[i]->GetItemEntry().GetItemDefinition()) == EFortQuickBars::Primary) 
            ++ItemCount;
    }

    int AmmoCount = 0;
    if (auto* WeaponDef = Cast<UFortWeaponItemDefinition>(Pickup->GetPrimaryPickupItemEntry().GetItemDefinition()))
    {
        if (auto Stats = AFortInventory::GetStats(WeaponDef))
        {
            AmmoCount = Stats->GetClipSize();
        }
    }

    if (ItemCount == 5 && FortLootPackage::GetQuickbar(Pickup->GetPrimaryPickupItemEntry().GetItemDefinition()) == EFortQuickBars::Primary) {
        if (MyFortPawn && MyFortPawn->GetCurrentWeapon() &&
            FortLootPackage::GetQuickbar(MyFortPawn->GetCurrentWeapon()->GetWeaponData()) == EFortQuickBars::Primary) {
        
            FGuid CurrentWeaponGuid = MyFortPawn->GetCurrentWeapon()->GetItemEntryGuid();
            FFortItemEntry* foundItemEntry = nullptr;
        
            for (int32 i = 0; i < ItemInstances.Num(); i++) {
                if (ItemInstances[i]->GetItemEntry().GetItemGuid() == CurrentWeaponGuid) {
                    foundItemEntry = &ItemInstances[i]->GetItemEntry();
                    break;
                }
            }
        
            if (foundItemEntry)
            {
                AFortInventory::SpawnPickup(MyFortPawn->GetActorLocation() + MyFortPawn->GetActorForwardVector() * 70.f + FVector(0, 0, 50), foundItemEntry->GetItemDefinition(), foundItemEntry->GetCount(), foundItemEntry->GetLoadedAmmo(), EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::Unset, MyFortPawn, true);
                
                if (bIsAI) {
                    AFortInventory::Remove(AIController, CurrentWeaponGuid, 1);
                } else {
                    AFortInventory::Remove(PlayerController, CurrentWeaponGuid, 1);
                }
                
                if (bIsAI) {
                    AFortInventory::GiveItem(AIController, Pickup->GetPrimaryPickupItemEntry().GetItemDefinition(), Pickup->GetPrimaryPickupItemEntry().GetCount(), AmmoCount * 600, Pickup->GetPrimaryPickupItemEntry().GetLevel());
                } else {
                    AFortInventory::GiveItem(PlayerController, Pickup->GetPrimaryPickupItemEntry().GetItemDefinition(), Pickup->GetPrimaryPickupItemEntry().GetCount(), AmmoCount, Pickup->GetPrimaryPickupItemEntry().GetLevel());
                }
            } else {
                AFortInventory::SpawnPickupDirect(PlayerController->GetViewTarget()->GetActorLocation(),
                    Pickup->GetPrimaryPickupItemEntry().GetItemDefinition(), Pickup->GetPrimaryPickupItemEntry().GetCount(), Pickup->GetPrimaryPickupItemEntry().GetLoadedAmmo(), EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::Unset, MyFortPawn, true);
            }
        } else {
            AFortInventory::SpawnPickupDirect(PlayerController->GetViewTarget()->GetActorLocation(),
                Pickup->GetPrimaryPickupItemEntry().GetItemDefinition(), Pickup->GetPrimaryPickupItemEntry().GetCount(), Pickup->GetPrimaryPickupItemEntry().GetLoadedAmmo(), EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::Unset, MyFortPawn, true);
        }
    } 
    else if (FortLootPackage::GetQuickbar(Pickup->GetPrimaryPickupItemEntry().GetItemDefinition()) != EFortQuickBars::Primary) {
        FFortItemEntry* existingItemEntry = nullptr;
        FGuid existingItemGuid;
        
        for (int32 i = 0; i < ItemInstances.Num(); i++) {
            if (ItemInstances[i] && ItemInstances[i]->GetItemEntry().GetItemDefinition() && 
                ItemInstances[i]->GetItemEntry().GetItemDefinition() == Pickup->GetPrimaryPickupItemEntry().GetItemDefinition()) {
                existingItemEntry = &ItemInstances[i]->GetItemEntry();
                existingItemGuid = existingItemEntry->GetItemGuid();
                break;
            }
        }
        
        if (existingItemEntry && existingItemEntry->GetItemDefinition()) {
            FFortItemEntry newEntry = *existingItemEntry;
            newEntry.SetCount(existingItemEntry->GetCount() + Pickup->GetPrimaryPickupItemEntry().GetCount());
            newEntry.SetItemGuid(existingItemGuid);
            newEntry.SetItemDefinition(existingItemEntry->GetItemDefinition()); 
            
            if (bIsAI) {
                AFortInventory::ReplaceEntry(AIController, newEntry);
            } else {
                AFortInventory::ReplaceEntry(PlayerController, newEntry);
            }
        } else {
            if (bIsAI) {
                AFortInventory::GiveItem(AIController, Pickup->GetPrimaryPickupItemEntry().GetItemDefinition(), Pickup->GetPrimaryPickupItemEntry().GetCount(), AmmoCount, Pickup->GetPrimaryPickupItemEntry().GetLevel());
            } else {
                AFortInventory::GiveItem(PlayerController, Pickup->GetPrimaryPickupItemEntry().GetItemDefinition(), Pickup->GetPrimaryPickupItemEntry().GetCount(), AmmoCount, Pickup->GetPrimaryPickupItemEntry().GetLevel());
            }
        }
    }
    else {
        if (bIsAI) {
            AFortInventory::GiveItem(AIController, Pickup->GetPrimaryPickupItemEntry().GetItemDefinition(), Pickup->GetPrimaryPickupItemEntry().GetCount(), AmmoCount, Pickup->GetPrimaryPickupItemEntry().GetLevel());
        } else {
            AFortInventory::GiveItem(PlayerController, Pickup->GetPrimaryPickupItemEntry().GetItemDefinition(), Pickup->GetPrimaryPickupItemEntry().GetCount(), AmmoCount, Pickup->GetPrimaryPickupItemEntry().GetLevel());
        }
    }
    
    return CompletePickupAnimationOG(Pickup);
}

void AFortPlayerPawn::ServerHandlePickup(AFortPlayerPawn* Pawn, FFrame& Stack)
{
    AFortPickup* Pickup;
    float InFlyTime;
    FVector InStartDirection;
    bool bPlayPickupSound;
    Stack.StepCompiledIn(&Pickup);
    Stack.StepCompiledIn(&InFlyTime);
    Stack.StepCompiledIn(&InStartDirection);
    Stack.StepCompiledIn(&bPlayPickupSound);
    Stack.IncrementCode();
    
    if (!Pawn || !Pickup || Pickup->GetbPickedUp())
        return;

    auto PlayerController = (AFortPlayerControllerAthena*)Pawn->GetController();
    
    if (!PlayerController || !Pickup->GetPrimaryPickupItemEntry().GetItemDefinition())
        return;
    
    Pawn->GetIncomingPickups().Add(Pickup);
    
    Pickup->GetPickupLocationData().bPlayPickupSound = bPlayPickupSound;
    Pickup->GetPickupLocationData().FlyTime = 0.40f;
    Pickup->GetPickupLocationData().ItemOwner = Pawn;
    Pickup->GetPickupLocationData().PickupGuid = Pickup->GetPrimaryPickupItemEntry().GetItemGuid();
    Pickup->GetPickupLocationData().PickupTarget = Pawn;
    Pickup->GetPickupLocationData().StartDirection = (FVector_NetQuantizeNormal)InStartDirection;
    Pickup->OnRep_PickupLocationData();

    Pickup->SetbPickedUp(true);
    Pickup->OnRep_bPickedUp();
//    Pickup->CallFunc<void>("Actor", "K2_DestroyActor");
}

void AFortPlayerPawn::GiveItemToInventoryOwner(UObject* Object, FFrame& Stack) {
    TScriptInterface<class IFortInventoryOwnerInterface> InventoryOwner;
    UFortWorldItemDefinition* ItemDefinition;
    int32 NumberToGive;
    bool bNotifyPlayer;
    int32 ItemLevel;
    int32 PickupInstigatorHandle;
    Stack.StepCompiledIn(&InventoryOwner);
    Stack.StepCompiledIn(&ItemDefinition);
    Stack.StepCompiledIn(&NumberToGive);
    Stack.StepCompiledIn(&bNotifyPlayer);
    Stack.StepCompiledIn(&ItemLevel);
    Stack.StepCompiledIn(&PickupInstigatorHandle);

    UE_LOG(LogNeon, Log, "GiveItemToInventoryOwner Called!");

    auto PC = (AFortPlayerControllerAthena*)InventoryOwner.ObjectPointer;
    AFortInventory::GiveItem(PC, ItemDefinition, 1, 1, 0);

	return GiveItemToInventoryOwnerOG(Object, Stack);
}


void AFortPlayerPawn::ReloadWeapon(AFortWeapon* Weapon, int32 AmmoToRemove)
{
    if (!Weapon)
        return;

    AActor* Owner = Weapon->Get<AActor*>("Actor", "Owner");
    AController* Controller = Owner ? Owner->Get<AController*>("Pawn", "Controller") : nullptr;
    AFortPlayerControllerAthena* PC = Cast<AFortPlayerControllerAthena>(Controller);
    if (!PC)
        return ReloadWeaponOG(Weapon, AmmoToRemove);

    AFortInventory* Inventory = nullptr;
    if (auto AI = Cast<AFortAthenaAIBotController>(PC))
        Inventory = AI->GetInventory();

    if (!Inventory)
        Inventory = PC->GetWorldInventory();

    if (!Inventory)
        return ReloadWeaponOG(Weapon, AmmoToRemove);

    UFortWeaponItemDefinition* WeaponData = Weapon->GetWeaponData();
    UFortWorldItemDefinition* AmmoDefinition = WeaponData ? WeaponData->GetAmmoWorldItemDefinition_BP() : nullptr;

    FFortItemList& InventoryList = Inventory->GetInventory();
    TArray<UFortWorldItem*>& ItemInstances = InventoryList.GetItemInstances();

    FGuid WeaponGuid = Weapon->GetItemEntryGuid();

    FFortItemEntry* WeaponEntry = nullptr;
    for (UFortWorldItem* Item : ItemInstances)
    {
        if (Item && Item->GetItemEntry().GetItemGuid() == WeaponGuid)
        {
            WeaponEntry = &Item->GetItemEntry();
            break;
        }
    }

    if (!WeaponEntry)
        return ReloadWeaponOG(Weapon, AmmoToRemove);

    FFortItemEntry* AmmoEntry = nullptr;
    for (UFortWorldItem* Item : ItemInstances)
    {
        if (Item && Item->GetItemEntry().GetItemDefinition() == AmmoDefinition)
        {
            AmmoEntry = &Item->GetItemEntry();
            break;
        }
    }

    if (!AmmoEntry)
        return ReloadWeaponOG(Weapon, AmmoToRemove);

    int32 NewAmmoCount = AmmoEntry->GetCount() - AmmoToRemove;
    AmmoEntry->SetCount(NewAmmoCount);

    if (NewAmmoCount <= 0)
        AFortInventory::Remove(PC, AmmoEntry->GetItemGuid());
    else
        AFortInventory::ReplaceEntry(PC, *AmmoEntry);

    WeaponEntry->SetLoadedAmmo(WeaponEntry->GetLoadedAmmo() + AmmoToRemove);
    AFortInventory::ReplaceEntry(PC, *WeaponEntry);

    return ReloadWeaponOG(Weapon, AmmoToRemove);
}

void UGA_Athena_MedConsumable_Parent_C::Athena_MedConsumable_Triggered(UGA_Athena_MedConsumable_Parent_C* Consumable)
{
    if (!Consumable || (!Consumable->GetHealsShields() && !Consumable->GetHealsHealth()) || !Consumable->GetPlayerPawn())
        return Athena_MedConsumable_TriggeredOG(Consumable);

    auto PlayerState = Cast<AFortPlayerStateAthena>(Consumable->GetPlayerPawn()->GetPlayerState());
    auto Handle = PlayerState->GetAbilitySystemComponent()->CallFunc<FGameplayEffectContextHandle>("AbilitySystemComponent", "MakeEffectContext");
    FGameplayTag Tag{};
    static auto ShieldCue = UKismetStringLibrary::Conv_StringToName(L"GameplayCue.Shield.PotionConsumed");
    static auto HealthCue = UKismetStringLibrary::Conv_StringToName(L"GameplayCue.Athena.Health.HealUsed");
    FName CueName = Consumable->GetHealsShields() ? ShieldCue : HealthCue;
    if (Consumable->GetHealsHealth() && Consumable->GetHealsShields()) {
        if (Consumable->GetPlayerPawn()->GetHealth() + Consumable->GetHealthHealAmount() <= 100) CueName = HealthCue;
    }
    Tag.TagName = CueName;
    PlayerState->GetAbilitySystemComponent()->CallFunc<void>("AbilitySystemComponent", "NetMulticast_InvokeGameplayCueAdded", Tag, FPredictionKey(), Handle);
    PlayerState->GetAbilitySystemComponent()->CallFunc<void>("AbilitySystemComponent", "NetMulticast_InvokeGameplayCueExecuted", Tag, FPredictionKey(), Handle);

    return Athena_MedConsumable_TriggeredOG(Consumable);
}

void AFortPlayerPawn::NetMulticast_Athena_BatchedDamageCues(AFortPlayerPawn* Pawn, FAthenaBatchedDamageGameplayCues_Shared SharedData, FAthenaBatchedDamageGameplayCues_NonShared NonSharedData)
{
    if (!Pawn)
        return NetMulticast_Athena_BatchedDamageCuesOG(Pawn, SharedData, NonSharedData);
    auto* Controller = Cast<AFortPlayerControllerAthena>(Pawn->GetController());
    if (!Controller)
        return NetMulticast_Athena_BatchedDamageCuesOG(Pawn, SharedData, NonSharedData);
    AFortWeapon* CurrentWeapon = Pawn->GetCurrentWeapon();
    if (!CurrentWeapon)
        return NetMulticast_Athena_BatchedDamageCuesOG(Pawn, SharedData, NonSharedData);
    AFortInventory* Inventory = nullptr;
    if (auto AI = Cast<AFortAthenaAIBotController>(Controller))
        Inventory = AI->GetInventory();
    if (!Inventory)
        Inventory = Controller->GetWorldInventory();
    if (!Inventory)
        return NetMulticast_Athena_BatchedDamageCuesOG(Pawn, SharedData, NonSharedData);
    FGuid WeaponGuid = CurrentWeapon->GetItemEntryGuid();
    FFortItemList& InventoryList = Inventory->GetInventory();
    TArray<UFortWorldItem*>& ItemInstances = InventoryList.GetItemInstances();
    FFortItemEntry* WeaponEntry = nullptr;
    for (UFortWorldItem* Item : ItemInstances)
    {
        if (Item && Item->GetItemEntry().GetItemGuid() == WeaponGuid)
        {
            WeaponEntry = &Item->GetItemEntry();
            break;
        }
    }
    if (WeaponEntry)
    {
        WeaponEntry->SetLoadedAmmo(CurrentWeapon->GetAmmoCount());
        AFortInventory::ReplaceEntry(Controller, *WeaponEntry);
    }

    return NetMulticast_Athena_BatchedDamageCuesOG(Pawn, SharedData, NonSharedData);
}

void AFortPlayerPawn::ServerSendZiplineState(AFortPlayerPawn* Pawn, FFrame& Stack)
{
    FZiplinePawnState State;
    Stack.StepCompiledIn(&State);
    Stack.IncrementCode();
    Pawn->Set("FortPlayerPawn", "ZiplineState", State);
    ((void (*)(AFortPlayerPawn*))Finder->OnRep_ZiplineState())(Pawn); 

    if (State.bJumped)
    {
        auto Velocity = Pawn->Get<UObject*>("Character", "CharacterMovement")->Get<FVector>("MovementComponent", "Velocity");
        auto VelocityX = Velocity.X * -0.5f;
        auto VelocityY = Velocity.Y * -0.5f;
        Pawn->CallFunc<void>("FortPawn", "LaunchCharacterJump", FVector{ VelocityX >= -750 ? fminf(VelocityX, 750) : -750, VelocityY >= -750 ? fminf(VelocityY, 750) : -750, 1200 }, false, false, true, true);
    }
}

void AFortPlayerPawn::ServerReviveFromDBNO(AFortPlayerPawn* Pawn, FFrame& Stack)
{
    AFortPlayerControllerAthena* EventInstigator;
    Stack.StepCompiledIn(&EventInstigator);
    Stack.IncrementCode();

    UE_LOG(LogNeon, Log, __FUNCTION__);
    
    if (!Pawn || !EventInstigator) return;

	auto PlayerState = reinterpret_cast<AFortPlayerStateAthena*>(Pawn->GetPlayerState());
	auto Controller = reinterpret_cast<AFortPlayerControllerAthena*>(Pawn->GetController());

	if (!PlayerState) return;

	static UClass* Class = UGAB_AthenaDBNO_C::StaticClass();

	if (!Class) return;

	/*FGameplayEventData EventData{};
	EventData.EventTag = Pawn->GetEventReviveTag();
	EventData.ContextHandle = PlayerState->GetAbilitySystemComponent()->MakeEffectContext();
	EventData.Instigator = EventInstigator;
	EventData.InstigatorTags = ((AFortPlayerPawnAthena*)EventInstigator->GetPawn())->GetGameplayTags();
	EventData.Target = Pawn;
	EventData.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(Pawn);
	EventData.TargetTags = Pawn->GetGameplayTags();
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Pawn, Pawn->GetEventReviveTag(), EventData);

	for (auto& Item : PlayerState->GetAbilitySystemComponent()->GetActivatableAbilities().GetItems())
	{
		if (!Item.GetAbility()) continue; 
		if (Item.GetAbility()->GetClass() == Class)
		{
			PlayerState->GetAbilitySystemComponent()->ClientCancelAbility(Item.GetHandle(), Item.ActivationInfo);
			PlayerState->GetAbilitySystemComponent()->ClientEndAbility(Item.GetHandle(), Item.ActivationInfo);
			PlayerState->GetAbilitySystemComponent()->ServerEndAbility(Item.GetHandle(), Item.ActivationInfo, Item.ActivationInfo.PredictionKeyWhenActivated);
			PlayerState->GetAbilitySystemComponent()->ServerCancelAbility(Item.GetHandle(), Item.ActivationInfo);
		}
	}
    */
	Pawn->SetbIsDBNO(false);
	Pawn->OnRep_IsDBNO();
	Pawn->SetHealth(30);
	PlayerState->SetDeathInfo({});
	PlayerState->OnRep_DeathInfo();
	Controller->CallFunc<void>("FortPlayerControllerZone", "ClientOnPawnRevived", EventInstigator);
}


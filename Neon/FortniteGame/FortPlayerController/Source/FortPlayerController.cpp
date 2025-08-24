#include "pch.h"
#include "../Header/FortPlayerController.h"

#include "Engine/GameplayStatics/Header/GameplayStatics.h"
#include "Engine/Guid/Header/Guid.h"
#include "Engine/ItemAndCount/Header/ItemAndCount.h"
#include "Engine/NetDriver/Header/NetDriver.h"
#include "FortniteGame/AbilitySystemComponent/Header/AbilitySystemComponent.h"
#include "FortniteGame/FortAthenaCreativePortal/Header/FortAthenaCreativePortal.h"
#include "FortniteGame/FortAthenaMutator/Header/FortAthenaMutator.h"
#include "FortniteGame/FortKismetLibrary/Header/FortKismetLibrary.h"
#include "FortniteGame/FortLootPackage/Header/FortLootPackage.h"
#include "FortniteGame/FortMinigameSettingsBuilding/Header/FortMinigameSettingsBuilding.h"
#include "FortniteGame/FortProjectileBase/Header/FortProjectileBase.h"
#include "FortniteGame/FortQuestManager/Header/FortQuestManager.h"
#include "Neon/Config.h"
#include "Neon/Finder/Header/Finder.h"
#include "Neon/Nexa/Nexa.h"
#include "Neon/Nexa/BroadcastMatchResults/BroadcastMatchResults.h"
#include "Neon/Nexa/Curl/Curl.h"
#include "Neon/Nexa/Echo/Echo.h"

void AFortPlayerControllerAthena::ServerAcknowledgePossession(AFortPlayerControllerAthena* PlayerController, FFrame& Stack) 
{
    APawn* PawnToAcknowledge;
    Stack.StepCompiledIn(&PawnToAcknowledge);
    Stack.IncrementCode();

    if (!PlayerController) return;
    PlayerController->SetAcknowledgedPawn(PawnToAcknowledge);
    PlayerController->GetPlayerState()->SetHeroType(PlayerController->GetCosmeticLoadoutPC().GetCharacter()->GetHeroDefinition());
    if (Fortnite_Version <= 10.40)
    {
        void* (*ApplyCharacterCustomization)(AFortPlayerStateAthena*, APawn*) = decltype(ApplyCharacterCustomization)(Finder->ApplyCharacterCustomization());
        ApplyCharacterCustomization(PlayerController->GetPlayerState(), PawnToAcknowledge);
    } else {
    	PlayerController->GetPlayerState()->Set(
"FortPlayerStateAthena", "SeasonLevelUIDisplay",
PlayerController->GetXPComponent()->Get<int32>("FortPlayerControllerAthenaXPComponent", "CurrentLevel")
);
    	PlayerController->GetPlayerState()->OnRep_SeasonLevelUIDisplay();
    	
        UFortKismetLibrary::UpdatePlayerCustomCharacterPartsVisualization(PlayerController->GetPlayerState());
    }

    if (Config::bLateGame && UWorld::GetWorld()->GetGameState()->GetGamePhase() != EAthenaGamePhase::Warmup)
    {
        PlayerController->GetMyFortPawn()->SetHealth(100);
        PlayerController->GetMyFortPawn()->SetShield(100);
    }
}

void AFortPlayerControllerAthena::ServerReadyToStartMatch(AFortPlayerControllerAthena* PlayerController, FFrame& Stack)
{
	Stack.IncrementCode();
	
	AFortGameModeAthena* GameMode = UWorld::GetWorld()->GetAuthorityGameMode();
	if (!GameMode || !PlayerController)
		return;
	AFortPlayerStateAthena* PlayerState = PlayerController->GetPlayerState();
	if (Config::bCreative)
	{
		GameMode->GetGameState()->SetGamePhase(EAthenaGamePhase::SafeZones);
		GameMode->GetGameState()->OnRep_GamePhase(EAthenaGamePhase::Warmup);
		
		AFortAthenaCreativePortal* Portal = nullptr;
		for (int i = 0; i < GameMode->GetGameState()->GetCreativePortalManager()->GetAllPortals().Num(); i++)
		{
			auto CurrentPortal = GameMode->GetGameState()->GetCreativePortalManager()->GetAllPortals()[i];
			if (!CurrentPortal->GetLinkedVolume() || CurrentPortal->GetLinkedVolume()->GetVolumeState() == EVolumeState::Ready)
				continue;
			Portal = CurrentPortal;
			UE_LOG(LogNeon, Log, "Index: %d", i);
		}

		if (!Portal)
		{
			UE_LOG(LogNeon, Log, "Failed to find portal! Report to andr1ww");
			struct
			{
		
			} Params{};
	
			callExecOG(PlayerController, "/Script/FortniteGame.FortPlayerController.ServerReadyToStartMatch", ServerReadyToStartMatch, Params);
			return;
		}

		Portal->GetLinkedVolume()->SetbShowPublishWatermark(false);
		Portal->GetLinkedVolume()->SetbNeverAllowSaving(false);
		Portal->GetLinkedVolume()->SetVolumeState(EVolumeState::Ready);
		Portal->GetLinkedVolume()->OnRep_VolumeState();
		Portal->SetOwningPlayer(PlayerState->GetUniqueId());
		Portal->OnRep_OwningPlayer();

		Portal->GetIslandInfo().CreatorName = PlayerState->GetPlayerName();
		Portal->GetIslandInfo().SupportCode = L"NexaGoated";
		Portal->GetIslandInfo().Version = 1.0f;
		Portal->OnRep_IslandInfo();

		Portal->SetbPortalOpen(true);
		Portal->OnRep_PortalOpen();

		Portal->GetPlayersReady().Add(PlayerState->GetUniqueId());
		Portal->OnRep_PlayersReady();

		Portal->SetbUserInitiatedLoad(true);
		Portal->SetbInErrorState(false);

		PlayerController->SetOwnedPortal(Portal);
		auto Comp = Portal->GetLinkedVolume()->CallFunc<UPlaysetLevelStreamComponent*>("Actor", "GetComponentByClass", UPlaysetLevelStreamComponent::StaticClass());

		static auto PlaysetDih = Runtime::StaticLoadObject<UFortPlaysetItemDefinition>("/Game/Playsets/PID_Playset_60x60_Composed.PID_Playset_60x60_Composed");
		Comp->SetPlayset(PlaysetDih);

		auto Comp2 = Portal->GetLinkedVolume()->CallFunc<UFortLevelSaveComponent*>("Actor", "GetComponentByClass", UFortLevelSaveComponent::StaticClass());

		Comp2->SetAccountIdOfOwner(PlayerState->GetUniqueId());
		Comp2->SetbIsLoaded(true);
		
		PlayerController->SetCreativePlotLinkedVolume(Portal->GetLinkedVolume());
		PlayerController->OnRep_CreativePlotLinkedVolume();

		((void (*)(UPlaysetLevelStreamComponent*)) (Finder->LoadPlayset()))(Comp);

		UGameplayStatics::SpawnActorOG(Runtime::StaticFindObject<UClass>("/Game/Athena/Items/Gameplay/MinigameSettingsControl/MinigameSettingsMachine.MinigameSettingsMachine_C"), Portal->GetLinkedVolume()->K2_GetActorLocation(), {}, Portal->GetLinkedVolume());
	}
	
	struct
	{
		
	} Params{};
	
	callExecOG(PlayerController, "/Script/FortniteGame.FortPlayerController.ServerReadyToStartMatch", ServerReadyToStartMatch, Params);
}

void AFortPlayerControllerAthena::ServerLoadingScreenDropped(AFortPlayerControllerAthena* PlayerController, FFrame& Stack)
{
    Stack.IncrementCode();
    static UFortAbilitySet* AbilitySet = nullptr;
    if (!AbilitySet)
        AbilitySet = (UFortAbilitySet*)GUObjectArray.FindObject("GAS_AthenaPlayer");

    UAbilitySystemComponent::GiveAbilitySet(
        PlayerController->GetPlayerState()->GetAbilitySystemComponent(),
        AbilitySet
    );

    AFortPlayerStateAthena* PlayerState = PlayerController->GetPlayerState();

    PlayerController->GetQuestManager(ESubGame::Athena)->InitializeQuestAbilities(PlayerController->GetPawn());

	if (Fortnite_Version >= 11.00)
	{
		PlayerController->GetXPComponent()->Set(
			"FortPlayerControllerAthenaXPComponent", "bRegisteredWithQuestManager", true
		);
		PlayerController->GetXPComponent()->OnRep_bRegisteredWithQuestManager();
	}
	
    if (Config::bEchoSessions)
    {
        std::string PlayerName = PlayerController->GetPlayerState()->GetPlayerName().ToString();

        static std::string TeamsJson = "";
        static std::chrono::steady_clock::time_point LastFetchTime = std::chrono::steady_clock::now() - std::chrono::seconds(10);

        auto Now = std::chrono::steady_clock::now();
        if (TeamsJson.empty() || std::chrono::duration_cast<std::chrono::seconds>(Now - LastFetchTime).count() >= 5)
        {
            TeamsJson = Nexa::Curl::Get("http://147.93.1.220:2087/nxa/echo/session/list/teams/" + Config::Echo::Session);
            LastFetchTime = Now;
        }

        if (!TeamsJson.empty())
        {
            try 
            {
                auto teamsArray = nlohmann::json::parse(TeamsJson);
                bool playerHasTeam = false;

                for (const auto& team : teamsArray)
                {
                    for (const auto& member : team)
                    {
                        if (member.get<std::string>() == PlayerName)
                        {
                            playerHasTeam = true;
                            break;
                        }
                    }
                    if (playerHasTeam) break;
                }

                if (!playerHasTeam)
                {
                    AFortPawn* Pawn = PlayerController->GetMyFortPawn();
                    if (Pawn)
                    {
                        Pawn->CallFunc<void>(
                            "FortPawn", "ForceKill",
                            FGameplayTag(UKismetStringLibrary::Conv_StringToName(L"DeathCause.BanHammer")),
                            Pawn->GetController(), nullptr
                        );
                    }
                    return;
                }
            }
            catch (...) {}
        }
    }

	auto GameMode = UWorld::GetWorld()->GetAuthorityGameMode();
	if (GameMode->GetCurrentPlaylistName().ToString().ToString().contains("Respawn_Op"))
	{
		AFortAthenaMutator_PerkSystemMutator* Mutator = GameMode->GetGameState()->CallFunc<AFortAthenaMutator_PerkSystemMutator*>("FortGameModeAthena", "GetMutatorByClass", GameMode, AFortAthenaMutator_PerkSystemMutator::StaticClass());
		UFortControllerComponent_PerkSystem* Comp = PlayerController->CallFunc<UFortControllerComponent_PerkSystem*>("Actor", "GetComponentByClass", UFortControllerComponent_PerkSystem::StaticClass());

    	UFortPlayerPerksItemDefinition* FactionAlter = Runtime::StaticFindObject<UFortPlayerPerksItemDefinition>("/Game/Athena/Items/PlayerPerks/PTID_SpyTeam_01.PTID_SpyTeam_01");
    	UFortPlayerPerksItemDefinition* FactionEgo = Runtime::StaticFindObject<UFortPlayerPerksItemDefinition>("/Game/Athena/Items/PlayerPerks/PTID_SpyTeam_02.PTID_SpyTeam_02");
    	
		for (auto& Ok : Comp->GetPerkSelection())
		{
			if (Comp->GetCachedFactionTag().TagName.ToString() == L"Athena.Faction.Ego")
			{
				Comp->GetSpyTechArray() = FactionEgo->GetPlayerPerkLevels()[0].PlayerPerks;
				Ok.Items = FactionEgo->GetPlayerPerkLevels()[0].PlayerPerks;
			} else
			{
				Comp->GetSpyTechArray() = FactionAlter->GetPlayerPerkLevels()[0].PlayerPerks;
				Ok.Items = FactionAlter->GetPlayerPerkLevels()[0].PlayerPerks;
			}
			
			UE_LOG(LogNeon, Log, "Ok Num: %d", Ok.Items.Num());
		}
    	
		UE_LOG(LogNeon, Log, "GetSpyTechArray Num: %d", Comp->GetSpyTechArray().Num());
		
		Comp->SetMutatorData(Mutator->GetMutatorData());
		Comp->SetRerollCount(1);
    	
		Comp->GetRoundStartCache().bDataReady = true;
		Comp->CallFunc<void>("FortControllerComponent_PerkSystem", "OnRep_PerkSelection");
		Comp->CallFunc<void>("FortControllerComponent_PerkSystem", "OnRep_SpyTechArray");
		Comp->CallFunc<void>("FortControllerComponent_PerkSystem", "OnRep_RerollCount");
	}

	struct
	{
		
	} Params{};
	
	callExecOG(PlayerController, "/Script/FortniteGame.FortPlayerController.ServerLoadingScreenDropped", ServerLoadingScreenDropped, Params);
}

void AFortPlayerControllerAthena::GetPlayerViewPoint(AFortPlayerControllerAthena* PlayerController, FVector& OutViewLocation, FRotator& OutViewRotation)
{
	if (PlayerController->GetStateName().ToString() == L"Spectating")
	{
		OutViewLocation = PlayerController->GetLastSpectatorSyncLocation();
		OutViewRotation = PlayerController->GetLastSpectatorSyncRotation();
		return;
	}

	if (PlayerController->GetMyFortPawn())
	{
		OutViewLocation = PlayerController->GetViewTarget()->K2_GetActorLocation();
		OutViewRotation = PlayerController->GetControlRotation();
		return;
	}

	return GetPlayerViewPointOG(PlayerController, OutViewLocation, OutViewRotation);
}

class UFortContextTrapItemDefinition : public UFortTrapItemDefinition
{
public:
	DECLARE_STATIC_CLASS(UFortContextTrapItemDefinition)
	DECLARE_DEFAULT_OBJECT(UFortContextTrapItemDefinition)
};

class AFortDecoTool_ContextTrap : public AFortWeapon
{
public:
	DEFINE_PTR(UFortContextTrapItemDefinition, AFortDecoTool_ContextTrap, ContextTrapItemDefinition);
public:
	DECLARE_STATIC_CLASS(AFortDecoTool_ContextTrap)
};

void AFortPlayerControllerAthena::ServerExecuteInventoryItem(AFortPlayerControllerAthena* PlayerController, FGuid ItemGuid) {
	UE_LOG(LogNeon, Log, __FUNCTION__);
    if (!PlayerController) return;
    
    AFortInventory* WorldInventory = PlayerController->GetWorldInventory();
    FFortItemList& Inventory = WorldInventory->GetInventory();
    TArray<FFortItemEntry>& ReplicatedEntries = Inventory.GetReplicatedEntries();
    
    FFortItemEntry* Entry = nullptr;
    uint8* Data = (uint8*)ReplicatedEntries.GetData();
    int32 Count = ReplicatedEntries.Num();
    static int32 FortItemEntrySize = StaticClassImpl("FortItemEntry")->GetSize();
    
    for (int32 i = 0; i < Count; ++i) {
        auto Item = (FFortItemEntry*)(Data + (i * FortItemEntrySize));
        if (Item->GetItemGuid() == ItemGuid) {
            Entry = Item;
            break;
        }
    }
    
    if (!Entry) {
    	UE_LOG(LogNeon, Log, "Failed to find entry on %s!", __FUNCTION__);
        return;
    }
	
    UFortWeaponItemDefinition* ItemDefinition = nullptr;
	static const UClass* GadgetItem = UFortGadgetItemDefinition::StaticClass();
	static const UClass* DecoItem = UFortDecoItemDefinition::StaticClass();
	
    if (Entry->GetItemDefinition()->IsA(GadgetItem))
    {
        ItemDefinition = ((UFortGadgetItemDefinition*)Entry->GetItemDefinition())->GetWeaponItemDefinition();
    } else
    {
        ItemDefinition = (UFortWeaponItemDefinition*)(Entry->GetItemDefinition());
    }

	if (auto Deco = (UFortContextTrapItemDefinition *) ItemDefinition->IsA(DecoItem)) {
		PlayerController->GetMyFortPawn()->CallFunc<void>("FortPawn", "PickUpActor", nullptr, Deco);
		PlayerController->GetMyFortPawn()->GetCurrentWeapon()->SetItemEntryGuid(ItemGuid);

		if (auto ContextTrap = (AFortDecoTool_ContextTrap*)PlayerController->GetMyFortPawn()->GetCurrentWeapon()) ContextTrap->SetContextTrapItemDefinition(Deco);
		return;
	}
    
    AFortPawn* MyFortPawn = PlayerController->GetMyFortPawn();

	if (!ItemDefinition || !MyFortPawn) return;
    
    if (Fortnite_Version.GetMajorVersion() >= 19.00) {
        MyFortPawn->EquipWeaponDefinition(ItemDefinition, ItemGuid, Entry->GetTrackerGuid(), false);
    } else {
        MyFortPawn->EquipWeaponDefinition(ItemDefinition, ItemGuid);
    }
}

void AFortPlayerControllerAthena::ServerPlayEmoteItem(AFortPlayerControllerAthena* PlayerController, FFrame& Stack)
{
    UFortMontageItemDefinitionBase* Asset;
    Stack.StepCompiledIn(&Asset);
    Stack.IncrementCode();

	if (!PlayerController->GetMyFortPawn() || !Asset) return;
	
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
    	static FGameplayAbilitySpec* StaticSpec = nullptr;
    	static bool bInitialized = false;

    	if (!bInitialized)
    	{
    		int32 GameplayAbilitySpecSize = StaticClassImpl("GameplayAbilitySpec")->GetSize();
    		StaticSpec = (FGameplayAbilitySpec*)malloc(GameplayAbilitySpecSize);
    		if (!StaticSpec)
    			return;
    		memset(StaticSpec, 0, GameplayAbilitySpecSize);
    		new (StaticSpec) FGameplayAbilitySpec();
    		bInitialized = true;
    	}

    	if (!StaticSpec)
    		return;
    
        using GiveAbilityFunc = FGameplayAbilitySpecHandle(__fastcall*)(
            UAbilitySystemComponent*,
            FGameplayAbilitySpecHandle*, 
            const FGameplayAbilitySpec&,
            FGameplayEventData*
        );
        
        StaticSpec->MostRecentArrayReplicationKey = -1;
        StaticSpec->ReplicationID = -1;
        StaticSpec->ReplicationKey = -1;
        StaticSpec->GetHandle().Handle = rand();
        StaticSpec->SetAbility(Ability);
        StaticSpec->SetSourceObject(Asset);
        StaticSpec->SetInputID(-1);
        StaticSpec->SetLevel(1);
    
        FGameplayAbilitySpecHandle Handle = StaticSpec->GetHandle();
    
        if (Finder->GiveAbilityAndActivateOnce())
        {
            ((GiveAbilityFunc)Finder->GiveAbilityAndActivateOnce())(AbilitySystemComponent, &Handle, *StaticSpec, nullptr);
        }
    }
}

void AFortPlayerControllerAthena::EnterAircraft(AFortPlayerControllerAthena* PlayerController, AFortAthenaAircraft* Aircraft)
{
	if (PlayerController && PlayerController->GetWorldInventory())
	{
		AFortInventory* WorldInventory = PlayerController->GetWorldInventory();
		FFortItemList& Inventory = WorldInventory->GetInventory();
		TArray<UFortWorldItem*>& ItemInstances = Inventory.GetItemInstances();
        
		TArray<FGuid> GuidsToRemove;
		for (UFortWorldItem* Item : ItemInstances) {
			if (Item) {
				GuidsToRemove.Add(Item->GetItemEntry().GetItemGuid());
			}
		}
        
		for (const FGuid& Guid : GuidsToRemove) {
			AFortInventory::Remove(PlayerController, Guid, -1); 
		}
        
		WorldInventory->SetbRequiresLocalUpdate(true);
		WorldInventory->HandleInventoryLocalUpdate();
	}

	if (Config::bLateGame)
	{
		PlayerController->GetMyFortPawn()->SetHealth(100);
		PlayerController->GetMyFortPawn()->SetShield(100);

		AFortInventory::GiveItem(PlayerController, UFortKismetLibrary::K2_GetResourceItemDefinition(EFortResourceType::Wood), 500, 500, 1);
		AFortInventory::GiveItem(PlayerController, UFortKismetLibrary::K2_GetResourceItemDefinition(EFortResourceType::Stone), 500, 500, 1);
		AFortInventory::GiveItem(PlayerController, UFortKismetLibrary::K2_GetResourceItemDefinition(EFortResourceType::Metal), 500, 500, 1);

		const std::map<EAmmoType, int> Ammo = {
			{EAmmoType::Assault, 250},
			{EAmmoType::Shotgun, 50},
			{EAmmoType::Submachine, 400},
			{EAmmoType::Rocket, 6},
			{EAmmoType::Sniper, 20}
		};

		for (const auto& [Type, Count] : Ammo) {
			AFortInventory::GiveItem(PlayerController, ItemAndCount::GetAmmo(Type), Count, Count, 1);
		}

		FItemAndCount Shotgun;
		do {
			Shotgun = ItemAndCount::GetShotguns();
		} while (!Shotgun.Item);

		FItemAndCount AssaultRifle;
		do {
			AssaultRifle = ItemAndCount::GetAssaultRifles();
		} while (!AssaultRifle.Item);

		FItemAndCount Sniper;
		do {
			Sniper = ItemAndCount::GetSnipers();
		} while (!Sniper.Item);

		FItemAndCount Heal;
		do {
			Heal = ItemAndCount::GetHeals();
		} while (!Heal.Item);

		FItemAndCount HealSlot2;
		do {
			HealSlot2 = ItemAndCount::GetHeals();
			if (HealSlot2.Item == Heal.Item) {
				HealSlot2 = ItemAndCount::GetHeals();
			}
		} while (!HealSlot2.Item);

		static const UClass* WeaponItemDef = UFortWeaponItemDefinition::StaticClass();

		int ShotgunClipSize = AFortInventory::GetStats((UFortWeaponItemDefinition*)Shotgun.Item)->GetClipSize();
		int AssaultRifleClipSize = AFortInventory::GetStats((UFortWeaponItemDefinition*)AssaultRifle.Item)->GetClipSize();
		int SniperClipSize = AFortInventory::GetStats((UFortWeaponItemDefinition*)Sniper.Item)->GetClipSize();
		int HealClipSize = Heal.Item->IsA(WeaponItemDef) ? AFortInventory::GetStats((UFortWeaponItemDefinition*)Heal.Item)->GetClipSize() : 0;
		int HealSlot2ClipSize = HealSlot2.Item->IsA(WeaponItemDef) ? AFortInventory::GetStats((UFortWeaponItemDefinition*)HealSlot2.Item)->GetClipSize() : 0;

		AFortInventory::GiveItem(PlayerController, AssaultRifle.Item, AssaultRifle.Count, AssaultRifleClipSize, 1);
		AFortInventory::GiveItem(PlayerController, Shotgun.Item, Shotgun.Count, ShotgunClipSize, 1);
		AFortInventory::GiveItem(PlayerController, Sniper.Item, Sniper.Count, SniperClipSize, 1);
		AFortInventory::GiveItem(PlayerController, Heal.Item, Heal.Count, HealClipSize, 1);
		AFortInventory::GiveItem(PlayerController, HealSlot2.Item, HealSlot2.Count, HealSlot2ClipSize, 1);
	}

	return EnterAircraftOG(PlayerController, Aircraft);
}

void AFortPlayerControllerAthena::ServerAttemptAircraftJump(UActorComponent* Comp, FFrame& Stack)
{
	FRotator Rotation;
	Stack.StepCompiledIn(&Rotation);
	Stack.IncrementCode();
	
	auto PlayerController = (AFortPlayerControllerAthena*)Comp->CallFunc<AActor*>("ActorComponent", "GetOwner", Comp);
	auto GameMode = UWorld::GetWorld()->GetAuthorityGameMode();
	auto GameState = UWorld::GetWorld()->GetGameState();

	if (PlayerController && GameMode)
	{
		GameMode->RestartPlayer(PlayerController);
		if (Config::bLateGame)
		{
			auto SpawnLocation = GameState->GetAircrafts()[0]->K2_GetActorLocation();
			SpawnLocation.Z -= UKismetMathLibrary::RandomIntegerInRange(370, 700);
			SpawnLocation.X += UKismetMathLibrary::RandomIntegerInRange(200, 750);
			SpawnLocation.X -= UKismetMathLibrary::RandomIntegerInRange(200, 400);

			PlayerController->GetMyFortPawn()->K2_TeleportTo(SpawnLocation, {});
		}
		
		PlayerController->Set("Controller", "ControlRotation", Rotation);

		PlayerController->GetMyFortPawn()->BeginSkydiving(true);
	}
}

void AFortPlayerControllerAthena::ServerGiveCreativeItem(AFortPlayerControllerAthena* PlayerController, FFrame& Stack)
{
	static int Size = StaticClassImpl("FortItemEntry")->GetSize();
	void* Mem = malloc(Size);
	if (!Mem || !PlayerController)
		return;

	memset(Mem, 0, Size);
	Stack.StepCompiledIn(Mem);

	auto CreativeItemPtr = reinterpret_cast<FFortItemEntry*>(Mem);
	auto ItemDef = CreativeItemPtr->GetItemDefinition();
	auto Count = CreativeItemPtr->GetCount() == 0 ? 1 : CreativeItemPtr->GetCount();
	if (!ItemDef)
	{
		UE_LOG(LogNeon, Log, "Failed to get Item Def!");
		free(Mem);
		return;
	}

	AFortInventory* WorldInventory = PlayerController->GetWorldInventory();
	TArray<UFortWorldItem*>& ItemInstances = WorldInventory->GetInventory().GetItemInstances();
            
	FFortItemEntry* ItemEntry = nullptr;
	for (UFortWorldItem* Item : ItemInstances) {
		if (Item->GetItemEntry().GetItemDefinition()->IsA(UFortResourceItemDefinition::StaticClass()) || Item->GetItemEntry().GetItemDefinition()->IsA(UFortAmmoItemDefinition::StaticClass())) {
			ItemEntry = &Item->GetItemEntry();
			break;
		}
	}

	if (ItemEntry)
	{
		ItemEntry->SetCount(ItemEntry->GetCount() + Count);
		AFortInventory::ReplaceEntry(PlayerController, *ItemEntry);
		free(Mem);
		return;
	}

	int32 LoadedAmmo = ItemDef->IsA<UFortWeaponItemDefinition>() ? AFortInventory::GetStats((UFortWeaponItemDefinition*)ItemDef)->GetClipSize() : CreativeItemPtr->GetLoadedAmmo();
	AFortInventory::GiveItem(PlayerController, ItemDef, Count, LoadedAmmo, 1);
	free(Mem);
}

void AFortPlayerControllerAthena::ServerCreateBuildingActor(AFortPlayerControllerAthena* PlayerController, FFrame& Stack)
{
	FCreateBuildingActorData CreateBuildingData;
	Stack.StepCompiledIn(&CreateBuildingData);
	Stack.IncrementCode();

	UClass* BuildingClass = nullptr;
	auto& ClassLookup = UWorld::GetWorld()->GetGameState()->GetAllPlayerBuildableClassesIndexLookup();
	for (auto& Pair : ClassLookup) {
		if (Pair.Value == CreateBuildingData.BuildingClassHandle) {
			BuildingClass = Pair.Key;
			break;
		}
	}
    
	if (!BuildingClass) {
		goto original;
	}

	{
		TArray<AActor*> ExistingBuildings;
		char PlacementResultFlag;
		static auto CantBuild = (__int64 (*)(UWorld*, UObject*, FVector, FRotator, bool, TArray<AActor*>*, char*))(Finder->CantBuild());
        
		if (CantBuild(UWorld::GetWorld(), BuildingClass, CreateBuildingData.BuildLoc, CreateBuildingData.BuildRot, CreateBuildingData.bMirrored, &ExistingBuildings, &PlacementResultFlag))
		{
			ExistingBuildings.Free();
			goto original;
		}

		for (AActor* Building : ExistingBuildings) {
			if (Building) Building->K2_DestroyActor();
		}

		static map<UClass*, EFortResourceType> ResourceMap;
            
		if (!PlayerController->GetbBuildFree())
		{
			EFortResourceType ResourceType;
			
			if (ResourceMap.contains(BuildingClass))
			{
				ResourceType = ResourceMap[BuildingClass];
			} else
			{
				ResourceMap[BuildingClass] = ((ABuildingSMActor*)BuildingClass->GetClassDefaultObject())->GetResourceType();
				ResourceType = ResourceMap[BuildingClass];
			}
			
			auto* Resource = UFortKismetLibrary::K2_GetResourceItemDefinition(ResourceType);

			AFortInventory* WorldInventory = PlayerController->GetWorldInventory();
			TArray<UFortWorldItem*>& ItemInstances = WorldInventory->GetInventory().GetItemInstances();
            
			FFortItemEntry* ItemEntry = nullptr;
			for (UFortWorldItem* Item : ItemInstances) {
				if (Item->GetItemEntry().GetItemDefinition() == Resource) {
					ItemEntry = &Item->GetItemEntry();
					break;
				}
			}

			if (!ItemEntry || ItemEntry->GetCount() < 10) {
				goto original;
			}

			ABuildingSMActor* BuildingSMActor = UGameplayStatics::SpawnActorOG<ABuildingSMActor>(
BuildingClass, CreateBuildingData.BuildLoc, CreateBuildingData.BuildRot, PlayerController);

			ItemEntry->SetCount(ItemEntry->GetCount() - 10);
			AFortInventory::ReplaceEntry(PlayerController, *ItemEntry);
            
			BuildingSMActor->SetbPlayerPlaced(true);
			BuildingSMActor->SetMirrored(CreateBuildingData.bMirrored);
			BuildingSMActor->InitializeKismetSpawnedBuildingActor(BuildingSMActor, PlayerController, true);

			if (AFortPlayerStateAthena* PlayerState = PlayerController->GetPlayerState()) {
				uint8 TeamIndex = PlayerState->GetTeamIndex();
				BuildingSMActor->Set("BuildingActor", "TeamIndex", TeamIndex);
				BuildingSMActor->Set("BuildingActor", "Team", EFortTeam(TeamIndex));
			}
		}
	}
    
	original:
		struct FortPlayerController_ServerCreateBuildingActor final
	{
	public:
		struct FCreateBuildingActorData               CreateBuildingData;                                // 0x0000(0x0038)(Parm, NoDestructor, NativeAccessSpecifierPublic)
	} Params{CreateBuildingData};

	return callExecOG(PlayerController, "/Script/FortniteGame.FortPlayerController", ServerCreateBuildingActor, Params);
}

void AFortPlayerControllerAthena::ServerBeginEditingBuildingActor(AFortPlayerControllerAthena* PlayerController, FFrame& Stack)
{
    ABuildingSMActor* BuildingSMActor;
    Stack.StepCompiledIn(&BuildingSMActor);
    Stack.IncrementCode();

    if (!PlayerController || !PlayerController->GetMyFortPawn() || !BuildingSMActor || BuildingSMActor->Get<uint8>("BuildingActor", "TeamIndex") != PlayerController->GetPlayerState()->GetTeamIndex())
        return;

    FFortItemEntry* ItemEntry = nullptr;
    AFortInventory* WorldInventory = PlayerController->GetWorldInventory();
    FFortItemList& Inventory = WorldInventory->GetInventory();
    TArray<UFortWorldItem*>& ItemInstancesOffsetPtr = Inventory.GetItemInstances();

	static UFortItemDefinition* Def = Runtime::StaticFindObject<UFortItemDefinition>("/Game/Items/Weapons/BuildingTools/EditTool.EditTool");
	
    for (int32 i = 0; i < ItemInstancesOffsetPtr.Num(); ++i)
    {
        if (ItemInstancesOffsetPtr[i]->GetItemEntry().GetItemDefinition() == Def)
        {
            ItemEntry = &ItemInstancesOffsetPtr[i]->GetItemEntry();
            break;
        }
    }

    if (!ItemEntry)
        return;

	static class UFunction* Func = nullptr;
	SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortPlayerController", "ServerExecuteInventoryItem");

	if (Func == nullptr)
		Func = Info.Func;
	if (!Func)
		return;
	
    PlayerController->ProcessEvent(Func, &ItemEntry->GetItemGuid());

//	BuildingSMActor->Set("BuildingSMActor", "EditingPlayer", PlayerController->GetPlayerState());
	auto SetEditingPlayer = (void (*)(ABuildingSMActor*, AFortPlayerState*))(Finder->SetEditingPlayer());
	SetEditingPlayer(BuildingSMActor, PlayerController->GetPlayerState());
	
    AFortWeap_EditingTool* EditingTool = (AFortWeap_EditingTool*)(PlayerController->GetMyFortPawn()->GetCurrentWeapon());
    if (EditingTool)
    {
        EditingTool->SetEditActor(BuildingSMActor);
        EditingTool->OnRep_EditActor();
    }
}

void AFortPlayerControllerAthena::ServerEditBuildingActor(AFortPlayerControllerAthena* PlayerController, FFrame& Stack)
{
    ABuildingSMActor* BuildingSMActor;
    TSubclassOf<ABuildingSMActor> NewClass;
    uint8 RotationIterations;
    bool bMirrored;
    Stack.StepCompiledIn(&BuildingSMActor);
    Stack.StepCompiledIn(&NewClass);
    Stack.StepCompiledIn(&RotationIterations);
    Stack.StepCompiledIn(&bMirrored);
    Stack.IncrementCode();

    if (!PlayerController || !BuildingSMActor || !PlayerController->GetMyFortPawn() || BuildingSMActor->Get<uint8>("BuildingActor", "TeamIndex") != PlayerController->GetPlayerState()->Get<uint8>("FortPlayerStateAthena", "TeamIndex")) return;

	auto SetEditingPlayer = (void (*)(ABuildingSMActor*, AFortPlayerState*))(Finder->SetEditingPlayer());
	SetEditingPlayer(BuildingSMActor, nullptr);
	
    static auto ReplaceBuildingActor = (ABuildingSMActor * (*)(ABuildingSMActor*, unsigned int, UObject*, unsigned int, int, bool, AFortPlayerController*))(Finder->ReplaceBuildingActor());

    ABuildingSMActor* NewBuild = ReplaceBuildingActor(BuildingSMActor, 1, NewClass, BuildingSMActor->Get<int32>("BuildingActor", "CurrentBuildingLevel"), RotationIterations, bMirrored, PlayerController);

    if (NewBuild)
    {
    	NewBuild->Set("BuildingActor", "TeamIndex", PlayerController->GetPlayerState()->GetTeamIndex());
	    NewBuild->SetbPlayerPlaced(true);
    }
}

void AFortPlayerControllerAthena::ServerEndEditingBuildingActor(AFortPlayerControllerAthena* PlayerController, FFrame& Stack)
{
    ABuildingSMActor* BuildingSMActor;
    Stack.StepCompiledIn(&BuildingSMActor);
    Stack.IncrementCode();

    if (!PlayerController || !PlayerController->GetMyFortPawn() || !BuildingSMActor || BuildingSMActor->Get<uint8>("BuildingActor", "TeamIndex") != PlayerController->GetPlayerState()->GetTeamIndex()) return;

	auto SetEditingPlayer = (void (*)(ABuildingSMActor*, AFortPlayerState*))(Finder->SetEditingPlayer());
	SetEditingPlayer(BuildingSMActor, nullptr);
	
	AFortWeap_EditingTool* EditingTool =  (AFortWeap_EditingTool*)(PlayerController->GetMyFortPawn()->GetCurrentWeapon());
	if (EditingTool)
	{
		EditingTool->SetbEditConfirmed(true);
		EditingTool->SetEditActor(nullptr);
		EditingTool->OnRep_EditActor();
	}
}

void AFortPlayerControllerAthena::ServerRepairBuildingActor(AFortPlayerControllerAthena* PlayerController, FFrame& Stack)
{
    ABuildingSMActor* BuildingSMActor;
    Stack.StepCompiledIn(&BuildingSMActor);
    Stack.IncrementCode();

    auto Price = (int32)std::floor((10.f * (1.f - BuildingSMActor->CallFunc<float>("BuildingActor", "GetHealthPercent"))) * 0.75f);
    auto Resource = UFortKismetLibrary::K2_GetResourceItemDefinition(BuildingSMActor->GetResourceType());

    FFortItemEntry* ItemEntry = nullptr;
    AFortInventory* WorldInventory = PlayerController->GetWorldInventory();
    FFortItemList& Inventory = WorldInventory->GetInventory();
    TArray<UFortWorldItem*>& ItemInstancesOffsetPtr = Inventory.GetItemInstances();

    for (int32 i = 0; i < ItemInstancesOffsetPtr.Num(); ++i)
    {
        if (ItemInstancesOffsetPtr[i]->GetItemEntry().GetItemDefinition() == Resource)
        {
            ItemEntry = &ItemInstancesOffsetPtr[i]->GetItemEntry();
            break;
        }
    }

    if (ItemEntry)
    {
        ItemEntry->SetCount(ItemEntry->GetCount() - Price);
        AFortInventory::ReplaceEntry(PlayerController, *ItemEntry);

        BuildingSMActor->CallFunc<void>("BuildingSMActor", "RepairBuilding", PlayerController, Price);
    }
}

void AFortPlayerControllerAthena::ServerDBNOReviveInterrupted(AFortPlayerControllerAthena* PlayerController, AFortPlayerPawnAthena* Pawn)
{
	ServerDBNOReviveInterruptedOG(PlayerController, Pawn);
	if (!Pawn || !PlayerController) return;

	auto PlayerState = reinterpret_cast<AFortPlayerStateAthena*>(Pawn->GetPlayerState());
	auto Controller = reinterpret_cast<AFortPlayerControllerAthena*>(Pawn->GetController());

	if (!PlayerState) return;

	static UClass* Class = UGAB_AthenaDBNO_C::StaticClass();

	if (!Class) return;
	
	auto& ActivatableAbilities = PlayerState->GetAbilitySystemComponent()->GetActivatableAbilities();
	auto& Items = ActivatableAbilities.GetItems();
	static int32 GameplayAbilitySpecSize = StaticClassImpl("GameplayAbilitySpec")->GetSize();
    
	for (int i = 0; i < Items.Num(); i++)
	{
		auto& Spec = *reinterpret_cast<FGameplayAbilitySpec*>(__int64(Items.GetData()) + (i * GameplayAbilitySpecSize));

		if (!Spec.GetAbility() || !Spec.GetAbility()->IsA(Class))
			continue;
		PlayerState->GetAbilitySystemComponent()->CallFunc<void>("AbilitySystemComponent", "ClientCancelAbility", Spec.GetHandle(), Spec.GetActivationInfo());
		PlayerState->GetAbilitySystemComponent()->CallFunc<void>("AbilitySystemComponent", "ClientEndAbility", Spec.GetHandle(), Spec.GetActivationInfo());
		PlayerState->GetAbilitySystemComponent()->CallFunc<void>("AbilitySystemComponent", "ServerEndAbility", Spec.GetHandle(), Spec.GetActivationInfo(), nullptr);
			
		break;
	}

	Controller->GetMyFortPawn()->SetbIsDying(false);
	Controller->GetMyFortPawn()->SetbPlayedDying(false);
	
	Controller->GetMyFortPawn()->SetbIsDBNO(false);
	Controller->GetMyFortPawn()->OnRep_IsDBNO();
	Controller->GetMyFortPawn()->SetHealth(30);
	PlayerState->SetDeathInfo({});
	PlayerState->OnRep_DeathInfo();
	Controller->GetMyFortPawn()->CallFunc<void>("FortPlayerPawn", "ReviveFromDBNO");
	Controller->CallFunc<void>("FortPlayerControllerZone", "ClientOnPawnRevived", PlayerController);

	Controller->GetMyFortPawn()->Set("FortPlayerPawnAthena", "DBNORevivalStacking", 0);
}

void AFortPlayerControllerAthena::ClientOnPawnDied(AFortPlayerControllerAthena* PlayerController, FFortPlayerDeathReport& DeathReport)
{
	if (!PlayerController)
		return ClientOnPawnDiedOG(PlayerController, DeathReport);

	auto GameMode = UWorld::GetWorld()->GetAuthorityGameMode();
	auto GameState = UWorld::GetWorld()->GetGameState();
	auto PlayerState = PlayerController->GetPlayerState();
	auto KillerPlayerState = DeathReport.GetKillerPlayerState();
	auto KillerPawn = DeathReport.GetKillerPawn();
	auto VictimPawn = PlayerController->GetMyFortPawn();
	bool bRespawningAllowed = GameState && PlayerState ? GameState->IsRespawningAllowed(PlayerState) : false;
	if (bRespawningAllowed) return ClientOnPawnDiedOG(PlayerController, DeathReport);
	
	FVector DeathLocation = VictimPawn ? VictimPawn->K2_GetActorLocation() : FVector(0,0,0);

	if (!KillerPlayerState && VictimPawn)
		KillerPlayerState = ((AFortPlayerControllerAthena*)VictimPawn->GetController())->GetPlayerState();

	FDeathInfo* DeathInfo = &PlayerState->GetDeathInfo(); 
	auto MatchReport = PlayerController->GetMatchReport();
	FAthenaRewardResult* RewardResult = MatchReport ? &MatchReport->GetEndOfMatchResults() : nullptr;
	FAthenaMatchStats* MatchStats = MatchReport ? &MatchReport->GetMatchStats() : nullptr;
	FAthenaMatchTeamStats* TeamStats = MatchReport ? &MatchReport->GetTeamStats() : nullptr;
	
	auto DeathTags = DeathReport.GetTags();
	EDeathCause DeathCause = PlayerState->ToDeathCause(DeathTags, false);

	PlayerState->Set("FortPlayerState", "PawnDeathLocation", DeathLocation);
	
	if (DeathInfo) {
		DeathInfo->SetbDBNO(false);
		DeathInfo->SetDeathLocation(DeathLocation);
		DeathInfo->SetDeathTags(DeathTags);
		DeathInfo->SetDeathCause(DeathCause);
		DeathInfo->SetDowner(nullptr);
		DeathInfo->SetFinisherOrDowner((AActor*)KillerPlayerState);

		if (VictimPawn) {
			DeathInfo->GetDistance() = (DeathCause != EDeathCause::FallDamage) 
			? KillerPawn ? KillerPawn->GetDistanceTo(VictimPawn) : 0.0f
			: VictimPawn->Get<float>("FortPlayerPawnAthena", "LastFallDistance");
		}

		DeathInfo->SetbInitialized(true);
		PlayerState->SetDeathInfo(*DeathInfo);
	}

	static auto Wood = Runtime::StaticFindObject<UFortWorldItemDefinition>("/Game/Items/ResourcePickups/WoodItemData.WoodItemData");
	static auto Stone = Runtime::StaticFindObject<UFortWorldItemDefinition>("/Game/Items/ResourcePickups/StoneItemData.StoneItemData");
	static auto Metal = Runtime::StaticFindObject<UFortWorldItemDefinition>("/Game/Items/ResourcePickups/MetalItemData.MetalItemData");
   
	auto WorldInventory = PlayerController->GetWorldInventory();
	if (!bRespawningAllowed && WorldInventory && VictimPawn) {
		static const UClass* WeaponClass = UFortWeaponRangedItemDefinition::StaticClass();
		static const UClass* ConsumableClass = UFortConsumableItemDefinition::StaticClass();
		static const UClass* AmmoClass = UFortAmmoItemDefinition::StaticClass();
		static const UClass* MeleeClass = UFortWeaponMeleeItemDefinition::StaticClass();
   
		bool bFoundMats = false;
   
		for (const auto& entry : WorldInventory->GetInventory().GetItemInstances()) {
			auto ItemDef = entry->GetItemEntry().GetItemDefinition();
        
			if (ItemDef->IsA(MeleeClass)) continue;
        
			int Count = entry->GetItemEntry().GetCount();
			int LoadedAmmo = entry->GetItemEntry().GetLoadedAmmo();
        
			if (ItemDef == Wood || ItemDef == Stone || ItemDef == Metal) {
				bFoundMats = true;
				AFortInventory::SpawnPickup(DeathLocation, ItemDef, Count, LoadedAmmo,
					EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::PlayerElimination, VictimPawn, true);
			}
			
			if (ItemDef->IsA(WeaponClass) || ItemDef->IsA(ConsumableClass) || ItemDef->IsA(AmmoClass)) {
				AFortInventory::SpawnPickup(DeathLocation, ItemDef, Count, LoadedAmmo,
					EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::PlayerElimination, VictimPawn, true);
			}
		}
   
		if (!bFoundMats) {
			AFortInventory::SpawnPickup(DeathLocation, Wood, 50, 0,
				EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::PlayerElimination, VictimPawn, true);
			AFortInventory::SpawnPickup(DeathLocation, Stone, 50, 0,
				EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::PlayerElimination, VictimPawn, true);
			AFortInventory::SpawnPickup(DeathLocation, Metal, 50, 0,
				EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::PlayerElimination, VictimPawn, true);
		}
	}
	
	if (!KillerPlayerState) KillerPlayerState = PlayerState;
	if (!KillerPawn) KillerPawn = VictimPawn;

	static const UClass* AI = AFortAthenaAIBotController::StaticClass();
	
	if (KillerPlayerState && KillerPawn && KillerPawn->GetController() && KillerPawn->GetController() != PlayerController && !KillerPawn->GetController()->IsA(AI))
	{
		int32 KillerScore = KillerPlayerState->GetKillScore() + 1;
		int32 TeamScore = KillerPlayerState->GetTeamKillScore() + 1;
		
		KillerPlayerState->SetKillScore(KillerScore);
		KillerPlayerState->SetTeamKillScore(TeamScore);
		KillerPlayerState->ClientReportTeamKill(TeamScore);
		KillerPlayerState->ClientReportKill(PlayerState);

		auto Team = KillerPlayerState->GetPlayerTeam();
		if (Team) {
			for (int32 i = 0; i < Team->GetTeamMembers().Num(); ++i) {
				auto MemberPlayerState = Team->GetTeamMembers()[i]->GetPlayerState();
				if (MemberPlayerState != KillerPlayerState) {
					MemberPlayerState->SetTeamKillScore(TeamScore);
					KillerPlayerState->ClientReportTeamKill(TeamScore);
				}
			}
		}
   	
		if (auto CPlayerController = (AFortPlayerControllerAthena*)KillerPawn->GetController()) {
			if (CPlayerController->GetMyFortPawn() && MatchStats) {
				static UFortAccoladeItemDefinition* Bronze = Runtime::StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_014_Elimination_Bronze.AccoladeId_014_Elimination_Bronze");
				static UFortAccoladeItemDefinition* Silver = Runtime::StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_015_Elimination_Silver.AccoladeId_015_Elimination_Silver");
				static UFortAccoladeItemDefinition* Gold = Runtime::StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_016_Elimination_Gold.AccoladeId_016_Elimination_Gold");
				static UFortAccoladeItemDefinition* Elim = Runtime::StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_012_Elimination.AccoladeId_012_Elimination");
				
				UFortQuestManager::GiveAccolade(CPlayerController, Elim);
				
				if (KillerScore == 1) {
					UFortQuestManager::GiveAccolade(CPlayerController, Bronze);
				}
				else if (KillerScore == 4) {
					UFortQuestManager::GiveAccolade(CPlayerController, Silver);
				}
				else if (KillerScore == 8) {
					UFortQuestManager::GiveAccolade(CPlayerController, Gold);
				}

				MatchStats->Stats[3] = KillerScore;
				MatchReport->SetMatchStats(*MatchStats);
				PlayerController->ClientSendTeamStatsForPlayer(*TeamStats);
			}
		}
	}

	bool bRebooting = false;
	if (!KillerPawn->GetController()->IsA(AI))
	{
		auto PlayerTeam = PlayerState->GetPlayerTeam();

		if (PlayerTeam && PlayerTeam->GetTeamMembers().Num() > 1) {
			bRebooting = true;
			for (auto& MemberController : PlayerTeam->GetTeamMembers()) {
				if (MemberController && MemberController != PlayerController) {
					if (!MemberController->GetbMarkedAlive()) {
						bRebooting = false;
						break;
					}
				}
			}
		}
	}
	
	static int DamageCauserOffset = Runtime::GetOffsetStruct("FortPlayerDeathReport", "DamageCauser");
	AActor* DamageCauser = *(AActor**)((char*)&DeathReport + DamageCauserOffset);
	UFortWeaponItemDefinition* ItemDef = nullptr;

	static const UClass* FortProjectileBase = AFortProjectileBase::StaticClass();
	if (DamageCauser)
	{
		if (DamageCauser->IsA(FortProjectileBase))
		{
			auto Owner = (AFortWeapon*)(DamageCauser->GetOwner());
			ItemDef = Owner->IsValidLowLevel() ? Owner->GetWeaponData() : nullptr; 
		}

		if (auto WeaponDef = (AFortWeapon*)(DamageCauser))
		{
			ItemDef = WeaponDef->GetWeaponData();
		}
	}
		
	int32 AliveCount = GameMode->GetAlivePlayers().Num() + GameMode->GetAliveBots().Num();
	
	if (!KillerPawn->GetController()->IsA(AI))
	{
		if (GameState->GetPlayersLeft() <= 25)
		{
			int Score = (GameState->GetPlayersLeft() == 1) ? 30 : 
						(GameState->GetPlayersLeft() == 2) ? 25 :
						(GameState->GetPlayersLeft() <= 5) ? 15 :
						(GameState->GetPlayersLeft() <= 25) ? 10 : 5;
			
			for (int i = 0; i < GameMode->GetAlivePlayers().Num(); i++)
			{
				GameMode->GetAlivePlayers()[i]->ClientReportTournamentPlacementPointsScored(GameState->GetPlayersLeft(), Score);
			}
		}
	}

	if (!bRespawningAllowed)
	{
		((void (*)(AFortGameModeAthena*, AFortPlayerController*, AFortPlayerStateAthena*, AFortPawn*, UFortWeaponItemDefinition*, EDeathCause, char, bool))(Finder->RemoveFromAlivePlayers()))
	  (GameMode, PlayerController, KillerPlayerState, KillerPawn, ItemDef, DeathCause, 0, false);
	
		if (bRebooting) {
			PlayerController->SetbMarkedAlive(false);
			return ClientOnPawnDiedOG(PlayerController, DeathReport);
		}

		PlayerController->SetbMarkedAlive(false);
	
		if (!bRebooting) {
			for (auto& MemberController : PlayerState->GetPlayerTeam()->GetTeamMembers()) {
				if (MemberController != PlayerController && !MemberController->GetbMarkedAlive()) {
					auto MemberPlayerState = MemberController->GetPlayerState();
					auto MemberMatchReport = MemberController->GetMatchReport();
					
					if (MemberMatchReport && MemberPlayerState) {
						MemberPlayerState->SetPlace(AliveCount);
						
						auto MemberMatchStats = &MemberMatchReport->GetMatchStats();
						auto MemberTeamStats = &MemberMatchReport->GetTeamStats();
						auto MemberRewardResult = &MemberMatchReport->GetEndOfMatchResults();
						
						if (MemberPlayerState->GetKillScore() && MemberPlayerState->GetSquadId() && MemberMatchStats) {
							MemberMatchStats->Stats[3] = MemberPlayerState->GetKillScore();
							MemberMatchStats->Stats[8] = MemberPlayerState->GetSquadId();
							MemberMatchReport->SetMatchStats(*MemberMatchStats);
							MemberController->ClientSendMatchStatsForPlayer(*MemberMatchStats);
						}
						
						if (MemberTeamStats) {
							MemberTeamStats->SetPlace(AliveCount);
							MemberTeamStats->SetTotalPlayers(AliveCount);
							MemberMatchReport->SetTeamStats(*MemberTeamStats);
							MemberController->ClientSendTeamStatsForPlayer(*MemberTeamStats);
						}
						
						int32 MemberXP = MemberController->GetXPComponent()->GetTotalXpEarned();
						MemberRewardResult->SetTotalBookXpGained(MemberXP);
						MemberRewardResult->SetTotalSeasonXpGained(MemberXP);
						MemberMatchReport->SetEndOfMatchResults(*MemberRewardResult);
					}
				}
			}
		}

		if (MatchReport && RewardResult) {
			int32 TotalXP = Fortnite_Version >= 11.10 ? PlayerController->GetXPComponent()->GetTotalXpEarned() : 0;
			RewardResult->SetTotalBookXpGained(TotalXP);
			RewardResult->SetTotalSeasonXpGained(TotalXP);
			MatchReport->SetEndOfMatchResults(*RewardResult);

			int32 PlayerPlace = AliveCount;
			PlayerState->SetPlace(PlayerPlace);

			if (PlayerState->GetKillScore() && PlayerState->GetSquadId() && MatchStats) {
				MatchStats->Stats[3] = PlayerState->GetKillScore();
				MatchStats->Stats[8] = PlayerState->GetSquadId();
				MatchReport->SetMatchStats(*MatchStats);
			}

			if (TeamStats) {
				TeamStats->SetPlace(PlayerPlace);
				TeamStats->SetTotalPlayers(PlayerPlace);
				MatchReport->SetTeamStats(*TeamStats);
			}
		}
	}
   	
	AFortPlayerControllerAthena* LastAliveController = nullptr;
		
	for (int32 i = 0; i < GameMode->GetAlivePlayers().Num(); ++i) {
		if (GameMode->GetAlivePlayers()[i] && GameMode->GetAlivePlayers()[i] != PlayerController && GameMode->GetAlivePlayers()[i]->GetMyFortPawn()) {
			LastAliveController = GameMode->GetAlivePlayers()[i];
			break;
		}
	}

	if (KillerPawn && KillerPawn != VictimPawn && !KillerPawn->GetController()->IsA(AI)) {
		auto* AbilitySystem = KillerPlayerState->GetAbilitySystemComponent();
		auto Handle = AbilitySystem->MakeEffectContext();
		FGameplayTag Tag;
		static auto Cue = UKismetStringLibrary::Conv_StringToName(L"GameplayCue.Shield.PotionConsumed");
		Tag.TagName = Cue;
		AbilitySystem->CallFunc<void>("AbilitySystemComponent", "NetMulticast_InvokeGameplayCueAdded", Tag, FPredictionKey(), Handle);

		auto Pawn = ((AFortPlayerControllerAthena*)KillerPlayerState->GetOwner())->GetMyFortPawn();
		if (Pawn) {
			auto Health = Pawn->GetHealth();
			auto Shield = Pawn->GetShield();

			if (Health >= 100.0f) {
				Shield = Shield + 50.0f;
			} else if (Health + 50.0f > 100.0f) {
				float HealthOverflow = (Health + 50.0f) - 100.0f;
				Health = 100.0f;
				Shield = Shield + HealthOverflow;
			} else {
				Health += 50.0f;
			}
    
			Pawn->SetHealth(Health);
			Pawn->SetShield(Shield);
		}
	}

	if (Config::bEchoSessions)
	{
		if (!bRebooting) {
			for (auto& MemberController : PlayerState->GetPlayerTeam()->GetTeamMembers()) {
				if (!IsValidPointer(MemberController))
					continue;
				if (MemberController != PlayerController && !MemberController->GetbMarkedAlive()) {
					auto MemberPlayerState = MemberController->GetPlayerState();
					Nexa::BroadcastMatchResults(MemberController, MemberPlayerState, Nexa::GetState().PlaylistData, GameMode);
				}
			}
		}

		Nexa::BroadcastMatchResults(PlayerController, PlayerState, Nexa::GetState().PlaylistData, GameMode);
	}

	int32 TotalAlive = GameMode->GetAlivePlayers().Num() + GameMode->GetAliveBots().Num();
	if (TotalAlive == 1 && LastAliveController) {
		AFortPlayerStateAthena* WinnerPlayerState = LastAliveController->GetPlayerState();
		AFortPlayerPawn* WinnerPawn = LastAliveController->GetMyFortPawn();
       
		if (WinnerPlayerState && WinnerPawn) {
			WinnerPlayerState->SetPlace(1);
				
			auto WinnerMatchReport = LastAliveController->GetMatchReport();
			if (WinnerMatchReport && RewardResult) {
				int32 WinnerXP = LastAliveController->GetXPComponent()->GetTotalXpEarned();
				auto WinnerRewardResult = &WinnerMatchReport->GetEndOfMatchResults();
				WinnerRewardResult->SetTotalBookXpGained(WinnerXP);
				WinnerRewardResult->SetTotalSeasonXpGained(WinnerXP);
				WinnerMatchReport->SetEndOfMatchResults(*WinnerRewardResult);
				LastAliveController->ClientSendEndBattleRoyaleMatchForPlayer(true, *WinnerRewardResult);

				auto WinnerMatchStats = &WinnerMatchReport->GetMatchStats();
				auto WinnerTeamStats = &WinnerMatchReport->GetTeamStats();

				WinnerMatchStats->Stats[3] = WinnerPlayerState->GetKillScore();
				WinnerMatchStats->Stats[8] = WinnerPlayerState->GetSquadId();
				WinnerMatchReport->SetMatchStats(*WinnerMatchStats);
				LastAliveController->ClientSendMatchStatsForPlayer(*WinnerMatchStats);

				int32 TotalPlayers = TotalAlive + 1;
				WinnerTeamStats->SetPlace(1);
				WinnerTeamStats->SetTotalPlayers(TotalPlayers);
				WinnerMatchReport->SetTeamStats(*WinnerTeamStats);
				LastAliveController->ClientSendTeamStatsForPlayer(*WinnerTeamStats);
			}

			uint8 WinningTeamIndex = WinnerPlayerState->GetTeamIndex();
			GameState->Set("FortGameStateAthena", "WinningTeam", WinningTeamIndex);
			GameState->Set("FortGameStateAthena", "WinningPlayerState", WinnerPlayerState);
		}
	}
	
	PlayerState->OnRep_DeathInfo();
	KillerPlayerState->OnRep_KillScore();
	KillerPlayerState->OnRep_TeamKillScore();
	PlayerState->OnRep_Place();
	GameState->OnRep_WinningTeam();
	GameState->OnRep_WinningPlayerState();
	
	ClientOnPawnDiedOG(PlayerController, DeathReport);
}

void AFortPlayerControllerAthena::ServerCheat(AFortPlayerControllerAthena* PlayerController, FFrame& Stack)
{
	FString Msg;
	Stack.StepCompiledIn(&Msg);
	Stack.IncrementCode();

	UE_LOG(LogNeon, Log, "Wow");
}

void AFortPlayerControllerAthena::ServerAttemptInventoryDrop(AFortPlayerControllerAthena* PlayerController, FFrame& Stack)
{
	FGuid ItemGuid;
	int32 Count;
	Stack.StepCompiledIn(&ItemGuid);
	Stack.StepCompiledIn(&Count);
	Stack.IncrementCode();
	
	if (!PlayerController || !PlayerController->GetPawn())
	{
		return;
	}

	AFortInventory* WorldInventory = PlayerController->GetWorldInventory();
	FFortItemList& Inventory = WorldInventory->GetInventory();
	TArray<UFortWorldItem*>& ItemInstances = Inventory.GetItemInstances();
	FFortItemEntry* Entry = nullptr;
    
	for (int32 i = 0; i < ItemInstances.Num(); ++i) {
		if (ItemInstances[i]->GetItemEntry().GetItemGuid() == ItemGuid) {
			Entry = &ItemInstances[i]->GetItemEntry();
			break;
		}
	}

	if (!Entry || (Entry->GetCount() - Count) < 0)
	{
		return;
	}
	
	Entry->SetCount(Entry->GetCount() - Count);
	AFortInventory::SpawnPickup(PlayerController->GetPawn()->K2_GetActorLocation() + PlayerController->GetPawn()->GetActorForwardVector() * 70.f + FVector(0, 0, 50), Entry, EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::Unset, PlayerController->GetMyFortPawn(), Count);
	if (Entry->GetCount() == 0)
		AFortInventory::Remove(PlayerController, ItemGuid);
	else
		AFortInventory::ReplaceEntry(PlayerController, *Entry);

	/*static const auto ShieldsDefinition = Runtime::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Consumables/Shields/Athena_Shields.Athena_Shields");
	static const auto MedkitDefinition = Runtime::StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Consumables/Medkit/Athena_Medkit.Athena_Medkit");
	if (Entry->GetItemDefinition() == ShieldsDefinition || Entry->GetItemDefinition() == MedkitDefinition) {
		if (PlayerController->GetMyFortPawn()) {
			PlayerController->GetMyFortPawn()->SetbIsEmoteLeader(true);
			PlayerController->GetMyFortPawn()->OnRep_IsEmoteLeader();
		}
	}*/
}

int32 AFortPlayerControllerAthena::K2_RemoveItemFromPlayerByGuid(UObject* Context, FFrame& Stack)
{
    AFortPlayerControllerAthena* PlayerController;
    FGuid ItemGuid;
    int32 AmountToRemove;
    bool bForceRemoval;
    Stack.StepCompiledIn(&PlayerController);
    Stack.StepCompiledIn(&ItemGuid);
    Stack.StepCompiledIn(&AmountToRemove);
    Stack.StepCompiledIn(&bForceRemoval);
	Stack.IncrementCode();

    AFortInventory::Remove(PlayerController, ItemGuid, 1);

	struct FortKismetLibrary_K2_RemoveItemFromPlayerByGuid final
	{
	public:
		class AFortPlayerController*                  PlayerController;                                  // 0x0000(0x0008)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		struct FGuid                                  ItemGuid;                                          // 0x0008(0x0010)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		int32                                         AmountToRemove;                                    // 0x0018(0x0004)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		bool                                          bForceRemoval;                                     // 0x001C(0x0001)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		uint8                                         Pad_1D[0x3];                                       // 0x001D(0x0003)(Fixing Size After Last Property [ Dumper-7 ])
		int32                                         ReturnValue;                                       // 0x0020(0x0004)(Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		uint8                                         Pad_24[0x4];                                       // 0x0024(0x0004)(Fixing Struct Size After Last Property [ Dumper-7 ])
	} Params;
	
	Params.PlayerController = PlayerController;
	Params.ItemGuid = std::move(ItemGuid);
	Params.AmountToRemove = AmountToRemove;
	Params.bForceRemoval = bForceRemoval;
	
	callExecOG(Context, "/Script/FortniteGame.FortKismetLibrary.K2_RemoveItemFromPlayerByGuid", K2_RemoveItemFromPlayerByGuid, Params);
	return -1;
}

int32 AFortPlayerControllerAthena::K2_RemoveItemFromPlayer(AFortPlayerControllerAthena* PC, UFortWorldItemDefinition* ItemDefinition, int32 AmountToRemove, bool bForceRemoval) {
    if (!PC || !ItemDefinition) {
		return AmountToRemove;
    }

    UE_LOG(LogNeon, Log, "K2_RemoveItemFromPlayer Called: %s %s", std::to_string(AmountToRemove), ItemDefinition->GetFName().ToString().ToString().c_str());

    FGuid ItemGuid = AFortInventory::FindGuidByDefinition(PC, ItemDefinition);
    if (ItemGuid.A != 0) {
        AFortInventory::Remove(PC, ItemGuid, INT_MAX);
        PC->GetWorldInventory()->Update(PC, nullptr);
		UE_LOG(LogNeon, Log, "K2_RemoveItemFromPlayer: Removed %s %s", std::to_string(AmountToRemove), ItemDefinition->GetFName().ToString().ToString().c_str());
    }

    return AmountToRemove;
    return K2_RemoveItemFromPlayerOG(PC, ItemDefinition, AmountToRemove, bForceRemoval);
}



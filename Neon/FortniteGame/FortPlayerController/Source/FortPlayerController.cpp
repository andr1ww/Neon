#include "pch.h"
#include "../Header/FortPlayerController.h"

#include "Engine/GameplayStatics/Header/GameplayStatics.h"
#include "Engine/Guid/Header/Guid.h"
#include "Engine/ItemAndCount/Header/ItemAndCount.h"
#include "Engine/NetDriver/Header/NetDriver.h"
#include "FortniteGame/AbilitySystemComponent/Header/AbilitySystemComponent.h"
#include "FortniteGame/FortAthenaMutator/Header/FortAthenaMutator.h"
#include "FortniteGame/FortKismetLibrary/Header/FortKismetLibrary.h"
#include "FortniteGame/FortLootPackage/Header/FortLootPackage.h"
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
        UFortKismetLibrary::UpdatePlayerCustomCharacterPartsVisualization(PlayerController->GetPlayerState());
    }

    if (Config::bLateGame && UWorld::GetWorld()->GetGameState()->GetGamePhase() != EAthenaGamePhase::Warmup)
    {
        PlayerController->GetMyFortPawn()->SetHealth(100);
        PlayerController->GetMyFortPawn()->SetShield(100);
    }
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
		PlayerState->Set(
			"FortPlayerStateAthena", "SeasonLevelUIDisplay",
			PlayerController->GetXPComponent()->Get<int32>("FortPlayerControllerAthenaXPComponent", "CurrentLevel")
		);
		PlayerState->OnRep_SeasonLevelUIDisplay();

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
}

void AFortPlayerControllerAthena::ServerExecuteInventoryItem(AFortPlayerControllerAthena* PlayerController, FFrame& Stack) {
    FGuid ItemGuid;
    Stack.StepCompiledIn(&ItemGuid);
    Stack.IncrementCode();
    
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
        return;
    }
	
    UFortWeaponItemDefinition* ItemDefinition = nullptr;
    if (Entry->GetItemDefinition()->IsA<UFortGadgetItemDefinition>())
    {
        ItemDefinition = Cast<UFortGadgetItemDefinition>(Entry->GetItemDefinition())->GetWeaponItemDefinition();
    } else
    {
        ItemDefinition = Cast<UFortWeaponItemDefinition>(Entry->GetItemDefinition());
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

		int ShotgunClipSize = AFortInventory::GetStats((UFortWeaponItemDefinition*)Shotgun.Item)->GetClipSize();
		int AssaultRifleClipSize = AFortInventory::GetStats((UFortWeaponItemDefinition*)AssaultRifle.Item)->GetClipSize();
		int SniperClipSize = AFortInventory::GetStats((UFortWeaponItemDefinition*)Sniper.Item)->GetClipSize();
		int HealClipSize = Heal.Item->IsA<UFortWeaponItemDefinition>() ? AFortInventory::GetStats((UFortWeaponItemDefinition*)Heal.Item)->GetClipSize() : 0;
		int HealSlot2ClipSize = HealSlot2.Item->IsA<UFortWeaponItemDefinition>() ? AFortInventory::GetStats((UFortWeaponItemDefinition*)HealSlot2.Item)->GetClipSize() : 0;

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

void AFortPlayerControllerAthena::ServerCreateBuildingActor(AFortPlayerControllerAthena* PlayerController, FFrame& Stack) {
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
        return;
    }

    TArray<AActor*> ExistingBuildings;
    char PlacementResultFlag;
    static auto CantBuild = (__int64 (*)(UWorld*, UObject*, FVector, FRotator, bool, TArray<AActor*>*, char*))(Finder->CantBuild());
    
    if (CantBuild(UWorld::GetWorld(), BuildingClass, CreateBuildingData.BuildLoc, CreateBuildingData.BuildRot, CreateBuildingData.bMirrored, &ExistingBuildings, &PlacementResultFlag)) {
    	ExistingBuildings.Free();
    	return;
    }

    for (AActor* Building : ExistingBuildings) {
        if (Building) Building->K2_DestroyActor();
    }

    ABuildingSMActor* BuildingSMActor = UGameplayStatics::SpawnActorOG<ABuildingSMActor>(
        BuildingClass, CreateBuildingData.BuildLoc, CreateBuildingData.BuildRot, PlayerController);
        
    if (BuildingSMActor) {
    	if (!PlayerController->Get<bool>("FortPlayerController", "bBuildFree")) {
    		auto* Resource = UFortKismetLibrary::K2_GetResourceItemDefinition(BuildingSMActor->GetResourceType());

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
    			return;
    		}

    		ItemEntry->SetCount(ItemEntry->GetCount() - 10);
    		AFortInventory::ReplaceEntry(PlayerController, *ItemEntry);
    	}
    	
        BuildingSMActor->SetbPlayerPlaced(true);
        BuildingSMActor->InitializeKismetSpawnedBuildingActor(BuildingSMActor, PlayerController, true);

        if (AFortPlayerStateAthena* PlayerState = PlayerController->GetPlayerState()) {
            uint8 TeamIndex = PlayerState->GetTeamIndex();
            BuildingSMActor->Set("BuildingActor", "TeamIndex", TeamIndex);
            BuildingSMActor->Set("BuildingActor", "Team", EFortTeam(TeamIndex));
        }

        PlayerController->Set("FortPlayerControllerAthena", "BuildingsCreated",
            PlayerController->Get<int32>("FortPlayerControllerAthena", "BuildingsCreated") + 1);
    }
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

    for (int32 i = 0; i < ItemInstancesOffsetPtr.Num(); ++i)
    {
        if (ItemInstancesOffsetPtr[i]->GetItemEntry().GetItemDefinition()->IsA<UFortEditToolItemDefinition>())
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

	BuildingSMActor->Set("BuildingSMActor", "EditingPlayer", PlayerController->GetPlayerState());

    AFortWeap_EditingTool* EditingTool = Cast<AFortWeap_EditingTool>(PlayerController->GetMyFortPawn()->GetCurrentWeapon());
    if (EditingTool)
    {
        EditingTool->Set("FortWeap_EditingTool", "EditActor", BuildingSMActor);
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

    if (!PlayerController || !BuildingSMActor  || !BuildingSMActor->IsA<ABuildingSMActor>() || BuildingSMActor->Get<uint8>("BuildingActor", "TeamIndex") != PlayerController->GetPlayerState()->Get<uint8>("FortPlayerStateAthena", "TeamIndex")) return;

    BuildingSMActor->Set("BuildingSMActor", "EditingPlayer", nullptr);

    static auto ReplaceBuildingActor = (ABuildingSMActor * (*)(ABuildingSMActor*, unsigned int, UObject*, unsigned int, int, bool, AFortPlayerController*))(Finder->ReplaceBuildingActor());

    ABuildingSMActor* NewBuild = ReplaceBuildingActor(BuildingSMActor, 1, NewClass, BuildingSMActor->Get<int32>("BuildingActor", "CurrentBuildingLevel"), RotationIterations, bMirrored, PlayerController);

    if (NewBuild) NewBuild->SetbPlayerPlaced(true);
}

void AFortPlayerControllerAthena::ServerEndEditingBuildingActor(AFortPlayerControllerAthena* PlayerController, FFrame& Stack)
{
    ABuildingSMActor* BuildingSMActor;
    Stack.StepCompiledIn(&BuildingSMActor);
    Stack.IncrementCode();

    if (!PlayerController || !BuildingSMActor || !BuildingSMActor || !BuildingSMActor->IsA<ABuildingSMActor>() || BuildingSMActor->Get<uint8>("BuildingActor", "TeamIndex") != PlayerController->GetPlayerState()->GetTeamIndex()) return;

	BuildingSMActor->Set("BuildingSMActor", "EditingPlayer", nullptr);
	
    PlayerController->Set("FortPlayerControllerAthena", "BuildingsEdited", 
    PlayerController->Get<int32>("FortPlayerControllerAthena", "BuildingsEdited") + 1);
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
	UE_LOG(LogNeon, Log, __FUNCTION__);
	if (!Pawn || !PlayerController) return;

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
	Controller->CallFunc<void>("FortPlayerControllerZone", "ClientOnPawnRevived", PlayerController);
}

void AFortPlayerControllerAthena::ClientOnPawnDied(AFortPlayerControllerAthena* PlayerController, FFortPlayerDeathReport& DeathReport)
{
	UE_LOG(LogNeon, Log, __FUNCTION__);
	if (!PlayerController)
		return ClientOnPawnDiedOG(PlayerController, DeathReport);

	auto GameMode = UWorld::GetWorld()->GetAuthorityGameMode();
	auto GameState = UWorld::GetWorld()->GetGameState();
	auto PlayerState = PlayerController->GetPlayerState();
	auto KillerPlayerState = DeathReport.GetKillerPlayerState();
	auto KillerPawn = DeathReport.GetKillerPawn();
	auto VictimPawn = PlayerController->GetMyFortPawn();

	FVector DeathLocation = VictimPawn ? VictimPawn->GetActorLocation() : FVector(0,0,0);

	if (!KillerPlayerState && VictimPawn)
		KillerPlayerState = ((AFortPlayerControllerAthena*)VictimPawn->GetController())->GetPlayerState();

	FDeathInfo* DeathInfo = &PlayerState->GetDeathInfo(); 
	auto MatchReport = PlayerController->GetMatchReport();
	FAthenaRewardResult* RewardResult = MatchReport ? &MatchReport->GetEndOfMatchResults() : nullptr;
	FAthenaMatchStats* MatchStats = MatchReport ? &MatchReport->GetMatchStats() : nullptr;
	FAthenaMatchTeamStats* TeamStats = MatchReport ? &MatchReport->GetTeamStats() : nullptr;

	bool bIsDBNO = VictimPawn && VictimPawn->IsDBNO();
 
	auto DeathTags = DeathReport.GetTags();
	EDeathCause DeathCause = PlayerState->ToDeathCause(DeathTags, bIsDBNO);

	PlayerState->Set("FortPlayerState", "PawnDeathLocation", DeathLocation);
	
	if (DeathInfo) {
		FGameplayTagContainer CopyTags;

		for (int i = 0; i < DeathTags.GameplayTags.Num(); ++i)
		{
			CopyTags.GameplayTags.Add(DeathTags.GameplayTags[i]);
		}

		for (int i = 0; i < DeathTags.ParentTags.Num(); ++i)
		{
			CopyTags.ParentTags.Add(DeathTags.ParentTags[i]);
		}
		
		DeathInfo->SetbDBNO(bIsDBNO);
		DeathInfo->SetDeathLocation(DeathLocation);
		DeathInfo->SetDeathTags(CopyTags);
		DeathInfo->SetDeathCause(DeathCause);
		DeathInfo->SetDowner(bIsDBNO ? (AActor*)KillerPlayerState : nullptr);
		DeathInfo->SetFinisherOrDowner((AActor*)KillerPlayerState);

		if (VictimPawn) {
			DeathInfo->GetDistance() = (DeathCause != EDeathCause::FallDamage) 
			? KillerPawn ? KillerPawn->GetDistanceTo(VictimPawn) : 0.0f
			: VictimPawn->Get<float>("FortPlayerPawnAthena", "LastFallDistance");
		}

		DeathInfo->SetbInitialized(true);
		PlayerState->SetDeathInfo(*DeathInfo);
		PlayerState->OnRep_DeathInfo();
	}
   
	auto WorldInventory = PlayerController->GetWorldInventory();
	if (WorldInventory && VictimPawn) {
		static const UClass* ResourceClass = UFortResourceItemDefinition::StaticClass();
		static const UClass* WeaponClass = UFortWeaponRangedItemDefinition::StaticClass();
		static const UClass* ConsumableClass = UFortConsumableItemDefinition::StaticClass();
		static const UClass* AmmoClass = UFortAmmoItemDefinition::StaticClass();
		static const UClass* MeleeClass = UFortWeaponMeleeItemDefinition::StaticClass();
   
		auto Location = VictimPawn->GetActorLocation();
		bool bFoundMats = false;
   
		const auto& ItemInstances = WorldInventory->GetInventory().GetItemInstances();
   
		for (const auto& entry : ItemInstances) {
			auto ItemDef = entry->GetItemEntry().GetItemDefinition();
        
			if (ItemDef->IsA(MeleeClass)) continue;
        
			int Count = entry->GetItemEntry().GetCount();
			int LoadedAmmo = entry->GetItemEntry().GetLoadedAmmo();
        
			if (ItemDef->IsA(ResourceClass)) {
				bFoundMats = true;
				AFortInventory::SpawnPickupDirect(Location, ItemDef, Count, LoadedAmmo,
					EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::PlayerElimination, VictimPawn, true);
			}
			else if (ItemDef->IsA(WeaponClass)) {
				AFortInventory::SpawnPickupDirect(Location, ItemDef, Count, LoadedAmmo,
					EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::PlayerElimination, VictimPawn, true);
			}
			else if (ItemDef->IsA(ConsumableClass)) {
				AFortInventory::SpawnPickupDirect(Location, ItemDef, Count, LoadedAmmo,
					EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::PlayerElimination, VictimPawn, true);
			}
			else if (ItemDef->IsA(AmmoClass)) {
				AFortInventory::SpawnPickupDirect(Location, ItemDef, Count, LoadedAmmo,
					EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::PlayerElimination, VictimPawn, true);
			}
		}
   
		if (!bFoundMats) {
			AFortInventory::SpawnPickupDirect(Location, UFortKismetLibrary::K2_GetResourceItemDefinition(EFortResourceType::Wood), 50, 0,
				EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::PlayerElimination, VictimPawn, true);
			AFortInventory::SpawnPickupDirect(Location, UFortKismetLibrary::K2_GetResourceItemDefinition(EFortResourceType::Stone), 50, 0,
				EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::PlayerElimination, VictimPawn, true);
			AFortInventory::SpawnPickupDirect(Location, UFortKismetLibrary::K2_GetResourceItemDefinition(EFortResourceType::Metal), 50, 0,
				EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::PlayerElimination, VictimPawn, true);
		}
	}
	
	if (!KillerPlayerState) KillerPlayerState = PlayerState;
	if (!KillerPawn) KillerPawn = VictimPawn;

	if (KillerPlayerState && KillerPawn && KillerPawn->GetController() && KillerPawn->GetController() != PlayerController && !KillerPawn->GetController()->IsA<AFortAthenaAIBotController>())
	{
		int32 KillerScore = KillerPlayerState->GetKillScore() + 1;
		int32 TeamScore = KillerPlayerState->GetTeamKillScore() + 1;
		
		KillerPlayerState->SetKillScore(KillerScore);
		KillerPlayerState->OnRep_KillScore();
		KillerPlayerState->SetTeamKillScore(TeamScore);
		KillerPlayerState->OnRep_TeamKillScore();
		KillerPlayerState->ClientReportTeamKill(TeamScore);

		auto Team = KillerPlayerState->GetPlayerTeam();
		if (Team) {
			const auto& TeamMembers = Team->GetTeamMembers();
			for (int32 i = 0; i < TeamMembers.Num(); ++i) {
				auto MemberPlayerState = TeamMembers[i]->GetPlayerState();
				if (MemberPlayerState != KillerPlayerState) {
					int32 MemberTeamScore = MemberPlayerState->GetTeamKillScore() + 1;
					MemberPlayerState->SetTeamKillScore(MemberTeamScore);
					MemberPlayerState->OnRep_TeamKillScore();
					MemberPlayerState->ClientReportTeamKill(MemberTeamScore);
				}
			}
		}
   	
		KillerPlayerState->ClientReportKill(PlayerState);
		if (auto CPlayerController = (AFortPlayerControllerAthena*)KillerPawn->GetController()) {
			if (CPlayerController->GetMyFortPawn() && MatchStats) {
				int32 CurrentKills = KillerScore;

				static UFortAccoladeItemDefinition* Bronze = Runtime::StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_014_Elimination_Bronze.AccoladeId_014_Elimination_Bronze");
				static UFortAccoladeItemDefinition* Silver = Runtime::StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_015_Elimination_Silver.AccoladeId_015_Elimination_Silver");
				static UFortAccoladeItemDefinition* Gold = Runtime::StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_016_Elimination_Gold.AccoladeId_016_Elimination_Gold");

				if (CurrentKills == 1) {
					UFortQuestManager::GiveAccolade(CPlayerController, Bronze);
				}
				else if (CurrentKills == 4) {
					UFortQuestManager::GiveAccolade(CPlayerController, Silver);
				}
				else if (CurrentKills == 8) {
					UFortQuestManager::GiveAccolade(CPlayerController, Gold);
				}

				MatchStats->Stats[3] = CurrentKills;
				MatchReport->SetMatchStats(*MatchStats);
			}
		}
	}

	bool bRebooting = PlayerState->GetPlayerTeam()->GetTeamMembers().Num() > 1;
	if (bRebooting && DeathInfo->GetbDBNO()) for (auto& Member : PlayerState->GetPlayerTeam()->GetTeamMembers()) {
		auto MemberController = (AFortPlayerControllerAthena*)Member;
		if (MemberController != PlayerController && !MemberController->GetbMarkedAlive()) bRebooting = false;
	}

	static int DamageCauserOffset = Runtime::GetOffsetStruct("FortPlayerDeathReport", "DamageCauser");
	AActor* DamageCauser = *(AActor**)((char*)&DeathReport + DamageCauserOffset);
	UFortWeaponItemDefinition* ItemDef = nullptr;
		
	if (DamageCauser)
	{
		if (DamageCauser->IsA<AFortProjectileBase>())
		{
			auto Owner = Cast<AFortWeapon>(DamageCauser->GetOwner());
			ItemDef = Owner->IsValidLowLevel() ? Owner->GetWeaponData() : nullptr; 
		}

		if (auto WeaponDef = Cast<AFortWeapon>(DamageCauser))
		{
			ItemDef = WeaponDef->GetWeaponData();
		}
	}
		
	int32 AliveCount = GameMode->GetAlivePlayers().Num() + GameMode->GetAliveBots().Num();
	std::vector<std::pair<int, int>> PlacementScores = {
		{1, 30},
		{2, 25},
		{5, 15},
		{25, 10},
		{50, 5},
	};
	
	for (const auto& entry : PlacementScores)
	{
		if (entry.first == GameState->GetPlayersLeft())
		{
			for (int i = 0; i < GameMode->GetAlivePlayers().Num(); i++)
			{
				GameMode->GetAlivePlayers()[i]->ClientReportTournamentPlacementPointsScored(GameState->GetPlayersLeft(), entry.second);
			}
			break;
		}
	}
	
	if (bRebooting) {
		((void (*)(AFortGameModeAthena*, AFortPlayerController*, AFortPlayerStateAthena*, AFortPawn*, UFortWeaponItemDefinition*, EDeathCause, char, bool))(Finder->RemoveFromAlivePlayers()))
		  (GameMode, PlayerController, KillerPlayerState, KillerPawn, ItemDef, DeathCause, 0, false);
		PlayerController->SetbMarkedAlive(false);
		return ClientOnPawnDiedOG(PlayerController, DeathReport);
	}
	
	((void (*)(AFortGameModeAthena*, AFortPlayerController*, AFortPlayerStateAthena*, AFortPawn*, UFortWeaponItemDefinition*, EDeathCause, char, bool))(Finder->RemoveFromAlivePlayers()))
	  (GameMode, PlayerController, KillerPlayerState, KillerPawn, ItemDef, DeathCause, 0, false);

	PlayerController->SetbMarkedAlive(false);
	
	if (!bRebooting) {
		for (auto& Member : PlayerState->GetPlayerTeam()->GetTeamMembers()) {
			auto MemberController = (AFortPlayerControllerAthena*)Member;
			if (MemberController != PlayerController && !MemberController->GetbMarkedAlive()) {
				auto MemberPlayerState = MemberController->GetPlayerState();
				auto MemberMatchReport = MemberController->GetMatchReport();
					
				if (MemberMatchReport && MemberPlayerState) {
					MemberPlayerState->SetPlace(AliveCount);
					MemberPlayerState->OnRep_Place();
						
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
					MemberController->ClientSendEndBattleRoyaleMatchForPlayer(true, *MemberRewardResult);
				}
			}
		}
	}

	if (MatchReport && RewardResult) {
		int32 TotalXP = PlayerController->GetXPComponent()->GetTotalXpEarned();
		RewardResult->SetTotalBookXpGained(TotalXP);
		RewardResult->SetTotalSeasonXpGained(TotalXP);
		MatchReport->SetEndOfMatchResults(*RewardResult);
		PlayerController->ClientSendEndBattleRoyaleMatchForPlayer(true, *RewardResult);

		int32 PlayerPlace = AliveCount;
		PlayerState->SetPlace(PlayerPlace);
		PlayerState->OnRep_Place();

		if (PlayerState->GetKillScore() && PlayerState->GetSquadId() && MatchStats) {
			MatchStats->Stats[3] = PlayerState->GetKillScore();
			MatchStats->Stats[8] = PlayerState->GetSquadId();
			MatchReport->SetMatchStats(*MatchStats);
			PlayerController->ClientSendMatchStatsForPlayer(*MatchStats);
		}

		if (TeamStats) {
			TeamStats->SetPlace(PlayerPlace);
			TeamStats->SetTotalPlayers(PlayerPlace);
			MatchReport->SetTeamStats(*TeamStats);
			PlayerController->ClientSendTeamStatsForPlayer(*TeamStats);
		}
	}
   	
	AFortPlayerControllerAthena* LastAliveController = nullptr;
	const auto& AlivePlayers = GameMode->GetAlivePlayers();
		
	for (int32 i = 0; i < AlivePlayers.Num(); ++i) {
		const auto& AlivePC = AlivePlayers[i];
		if (AlivePC && AlivePC != PlayerController && AlivePC->GetMyFortPawn() && !AlivePC->GetMyFortPawn()->IsDBNO()) {
			LastAliveController = AlivePC;
		}
	}

	if (Config::bLateGame && KillerPawn && KillerPawn != PlayerController->GetMyFortPawn()) {
		auto* AbilitySystem = PlayerState->GetAbilitySystemComponent();
		auto Handle = AbilitySystem->CallFunc<FGameplayEffectContextHandle>("AbilitySystemComponent", "MakeEffectContext");
		FGameplayTag Tag;
		static auto Cue = UKismetStringLibrary::Conv_StringToName(L"GameplayCue.Shield.PotionConsumed");
		Tag.TagName = Cue;
		AbilitySystem->CallFunc<void>("AbilitySystemComponent", "NetMulticast_InvokeGameplayCueAdded", Tag, FPredictionKey(), Handle);
		AbilitySystem->CallFunc<void>("AbilitySystemComponent", "NetMulticast_InvokeGameplayCueExecuted", Tag, FPredictionKey(), Handle);

		auto Pawn = Cast<AFortPlayerControllerAthena>(KillerPlayerState->GetOwner())->GetMyFortPawn();
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

	int32 TotalAlive = GameMode->GetAlivePlayers().Num() + GameMode->GetAliveBots().Num();
	if (TotalAlive == 1 && LastAliveController) {
		AFortPlayerStateAthena* WinnerPlayerState = LastAliveController->GetPlayerState();
		AFortPlayerPawn* WinnerPawn = LastAliveController->GetMyFortPawn();
       
		if (WinnerPlayerState && WinnerPawn) {
			WinnerPlayerState->SetPlace(1);
			WinnerPlayerState->OnRep_Place();
				
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

			uint8 WinningTeamIndex = WinnerPlayerState->Get<uint8>("FortPlayerStateAthena", "TeamIndex");
			GameState->Set("FortGameStateAthena", "WinningTeam", WinningTeamIndex);
			GameState->OnRep_WinningTeam();
			GameState->Set("FortGameStateAthena", "WinningPlayerState", WinnerPlayerState);
			GameState->OnRep_WinningPlayerState();
		}
	}

	if (Config::bEchoSessions)
	{
		if (!bRebooting) {
			for (auto& Member : PlayerState->GetPlayerTeam()->GetTeamMembers()) {
				auto MemberController = (AFortPlayerControllerAthena*)Member;
				if (MemberController != PlayerController && !MemberController->GetbMarkedAlive()) {
					auto MemberPlayerState = MemberController->GetPlayerState();
					Nexa::BroadcastMatchResults(MemberController, MemberPlayerState, Nexa::GetState().PlaylistData, GameMode);
					std::thread t([=]() {
						Nexa::Echo::LowerEchoSessionCount();
					});
					t.detach();
				}
			}
		}

		Nexa::BroadcastMatchResults(PlayerController, PlayerState, Nexa::GetState().PlaylistData, GameMode);
		
		std::thread t([]() {
			Nexa::Echo::LowerEchoSessionCount();
		});
		t.detach();
	}
   
	ClientOnPawnDiedOG(PlayerController, DeathReport);
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
	AFortInventory::SpawnPickup(PlayerController->GetPawn()->GetActorLocation() + PlayerController->GetPawn()->GetActorForwardVector() * 70.f + FVector(0, 0, 50), Entry, EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::Unset, PlayerController->GetMyFortPawn(), Count);
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

    UE_LOG(LogNeon, Log, "K2_RemoveItemFromPlayerByGuid Called!");
    AFortInventory::Remove(PlayerController, ItemGuid, 1);

    return K2_RemoveItemFromPlayerByGuidOG(Context, Stack);
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



#include "pch.h"
#include "../Header/FortPlayerController.h"

#include "Engine/GameplayStatics/Header/GameplayStatics.h"
#include "Engine/Guid/Header/Guid.h"
#include "Engine/NetDriver/Header/NetDriver.h"
#include "FortniteGame/AbilitySystemComponent/Header/AbilitySystemComponent.h"
#include "FortniteGame/FortKismetLibrary/Header/FortKismetLibrary.h"
#include "Neon/Finder/Header/Finder.h"

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
    } else
    {
        UFortKismetLibrary::GetDefaultObj()->CallFunc<void>("FortKismetLibrary", "UpdatePlayerCustomCharacterPartsVisualization", PlayerController->GetPlayerState());    
    }
    
    static UFortAbilitySet* AbilitySet = (UFortAbilitySet*)GUObjectArray.FindObject("GAS_AthenaPlayer");
    UAbilitySystemComponent::GiveAbilitySet(PlayerController->GetPlayerState()->GetAbilitySystemComponent(), AbilitySet);
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
        ItemDefinition = Cast<UFortGadgetItemDefinition>(Entry->GetItemDefinition())->CallFunc<UFortWeaponItemDefinition*>("FortGadgetItemDefinition", "GetWeaponItemDefinition");
    } else
    {
        ItemDefinition = Cast<UFortWeaponItemDefinition>(Entry->GetItemDefinition());
    }
    
    AFortPawn* MyFortPawn = PlayerController->GetMyFortPawn();
    
    if (Fortnite_Version.GetMajorVersion() >= 19.00) {
        MyFortPawn->CallFunc<void>("FortPawn", "EquipWeaponDefinition", ItemDefinition, ItemGuid, Entry->GetTrackerGuid(), false);
    } else {
        MyFortPawn->CallFunc<void>("FortPawn", "EquipWeaponDefinition", ItemDefinition, ItemGuid);
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

void AFortPlayerControllerAthena::ServerAttemptAircraftJump(UActorComponent* Comp, FFrame& Stack)
{
    FRotator Rotation;
    Stack.StepCompiledIn(&Rotation);
    Stack.IncrementCode();
    
    auto PlayerController = (AFortPlayerControllerAthena*)Comp->CallFunc<AActor*>("ActorComponent", "GetOwner", Comp);
    auto GameMode = UWorld::GetWorld()->GetAuthorityGameMode();

    if (PlayerController && GameMode)
    {
        GameMode->CallFunc<void>("GameModeBase", "RestartPlayer", PlayerController);
        PlayerController->Set("Controller", "ControlRotation", Rotation);

        PlayerController->GetMyFortPawn()->CallFunc<void>("FortPlayerPawn", "BeginSkydiving", true);
    }
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

    if (!BuildingClass)
    {
        return;
    }

    if (!BuildingClass->GetClassDefaultObject()->IsA<ABuildingActor>())
    {
        return;
    }

    TArray<AActor*> ExistingBuildings;
    char PlacementResultFlag;
    static auto CantBuild = (__int64 (*)(UWorld*, UObject*, FVector, FRotator, bool, TArray<AActor*>*, char*))(Finder->CantBuild());
    bool bCanBuild = !CantBuild(UWorld::GetWorld(), BuildingClass, CreateBuildingData.BuildLoc, 
                 CreateBuildingData.BuildRot, CreateBuildingData.bMirrored, 
                 &ExistingBuildings, &PlacementResultFlag);
        
    if (!bCanBuild)
    {
        return;
    }
    
    if (!PlayerController->Get<bool>("FortPlayerController", "bBuildFree"))
    {
        auto* Resource = UFortKismetLibrary::K2_GetResourceItemDefinition(
            ((ABuildingSMActor*)BuildingClass->GetClassDefaultObject())->GetResourceType());
        
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
        
        if (!ItemEntry || ItemEntry->GetCount() < 10)
            return;
           
        if (ItemEntry->GetCount() > 0)
        {
            ItemEntry->SetCount(ItemEntry->GetCount() - 10);
            AFortInventory::ReplaceEntry(PlayerController, *ItemEntry);
        }
    }
    
    for (auto* Building : ExistingBuildings) {
        if (Building) Building->CallFunc<void>("Actor", "K2_DestroyActor");
    }
    
    auto BuildingActor = UGameplayStatics::SpawnActorOG<ABuildingSMActor>(BuildingClass, CreateBuildingData.BuildLoc, 
                                             CreateBuildingData.BuildRot, PlayerController);
   
    if (ABuildingSMActor* BuildingSMActor = BuildingActor) {
        BuildingSMActor->SetbPlayerPlaced(true);
        BuildingSMActor->CallFunc<void>("BuildingActor", "InitializeKismetSpawnedBuildingActor", BuildingSMActor, PlayerController, true);
        
        if (AFortPlayerStateAthena* PlayerState = PlayerController->GetPlayerState()) {
            BuildingSMActor->Set("BuildingActor", "TeamIndex", PlayerState->Get<uint8>("FortPlayerStateAthena", "TeamIndex"));
            BuildingSMActor->Set("BuildingActor", "Team", EFortTeam(BuildingSMActor->Get<uint8>("BuildingActor", "TeamIndex")));
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

    if (!PlayerController || !PlayerController->GetMyFortPawn() || !BuildingSMActor || BuildingSMActor->Get<uint8>("BuildingActor", "TeamIndex") != PlayerController->GetPlayerState()->Get<uint8>("FortPlayerStateAthena", "TeamIndex"))
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
    
    PlayerController->CallFunc<void>("FortPlayerController", "ServerExecuteInventoryItem",ItemEntry->GetItemGuid());

    AFortWeap_EditingTool* EditingTool = Cast<AFortWeap_EditingTool>(PlayerController->GetMyFortPawn()->Get<AFortWeapon*>("FortPawn", "CurrentWeapon"));
    if (EditingTool)
    {
        EditingTool->Set("FortWeap_EditingTool", "EditActor", BuildingSMActor);
        EditingTool->CallFunc<void>("FortWeap_EditingTool", "OnRep_EditActor");
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

    if (!PlayerController || !BuildingSMActor || !BuildingSMActor || !BuildingSMActor->IsA<ABuildingSMActor>() || BuildingSMActor->Get<uint8>("BuildingActor", "TeamIndex") != PlayerController->GetPlayerState()->Get<uint8>("FortPlayerStateAthena", "TeamIndex")) return;

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

    if (!PlayerController || !BuildingSMActor || !BuildingSMActor || !BuildingSMActor->IsA<ABuildingSMActor>() || BuildingSMActor->Get<uint8>("BuildingActor", "TeamIndex") != PlayerController->GetPlayerState()->Get<uint8>("FortPlayerStateAthena", "TeamIndex")) return;
    
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

void AFortPlayerControllerAthena::ClientOnPawnDied(AFortPlayerControllerAthena* PlayerController, FFortPlayerDeathReport& DeathReport)
{
    auto GameMode = UWorld::GetWorld()->GetAuthorityGameMode();
    auto GameState = UWorld::GetWorld()->GetGameState();
    auto PlayerState = PlayerController->GetPlayerState();

    if (!PlayerController)
        return ClientOnPawnDiedOG(PlayerController, DeathReport);

    auto KillerPlayerState = DeathReport.GetKillerPlayerState();
    auto KillerPawn = DeathReport.GetKillerPawn().Get();
    auto VictimPawn = PlayerController->GetMyFortPawn();

    FVector DeathLocation = VictimPawn ? VictimPawn->GetActorLocation() : FVector{0,0,0};

    if (!KillerPlayerState && VictimPawn)
        KillerPlayerState = ((AFortPlayerControllerAthena*)VictimPawn->GetController())->GetPlayerState();

	static int32 FDeathInfoSize = StaticClassImpl("DeathInfo")->GetSize();
	FDeathInfo* DeathInfo = (FDeathInfo*)VirtualAlloc(0, FDeathInfoSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	
    PlayerState->Set("FortPlayerState", "PawnDeathLocation", DeathLocation);
    DeathInfo->SetbDBNO(false);
    DeathInfo->SetDeathLocation(DeathLocation);
    DeathInfo->SetDeathTags(DeathReport.Tags);
    DeathInfo->SetDeathCause(PlayerState->CallFunc<EDeathCause>("FortPlayerStateAthena", "ToDeathCause", DeathReport.Tags, false));
    DeathInfo->SetDowner((AActor*)KillerPlayerState);
    DeathInfo->SetFinisherOrDowner((AActor*)KillerPlayerState);
    EDeathCause CachedDeathCause = DeathInfo->GetDeathCause();

    if (VictimPawn) {
        DeathInfo->GetDistance() = (CachedDeathCause != EDeathCause::FallDamage) 
            ? (KillerPawn && KillerPawn->GetClass()->GetFunction("GetDistanceTo") ? KillerPawn->CallFunc<float>("Actor", "GetDistanceTo", VictimPawn) : 0.0f)
            : VictimPawn->Get<float>("FortPlayerPawnAthena", "LastFallDistance");
    }

 /*   DeathInfo->SetbInitialized(true);
    PlayerState->CallFunc<void>("FortPlayerStateAthena", "OnRep_DeathInfo");
	
	if (PlayerController->GetWorldInventory() && PlayerController->GetMyFortPawn()) {
		static UClass* Types[] = {
			UFortResourceItemDefinition::StaticClass(),
			UFortWeaponRangedItemDefinition::StaticClass(),
			UFortConsumableItemDefinition::StaticClass(),
			UFortAmmoItemDefinition::StaticClass()
		};
		
		auto Location = PlayerController->GetMyFortPawn()->GetActorLocation();
		bool bFoundMats = false;
		
		for (auto& entry : PlayerController->GetWorldInventory()->GetInventory().GetReplicatedEntries()) {
			auto ItemDef = entry.GetItemDefinition();
			if (ItemDef->IsA<UFortWeaponMeleeItemDefinition>()) continue;
			
			bool bAllowedType = false;
			for (auto Type : Types) {
				if (ItemDef->IsA(Type)) {
					bAllowedType = true;
					if (!bFoundMats && Type == UFortResourceItemDefinition::StaticClass())
						bFoundMats = true;
					break;
				}
			}
			
			if (bAllowedType) {
				AFortInventory::SpawnPickup(Location, entry, EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::PlayerElimination, PlayerController->GetMyFortPawn());
			}
		}
		
		if (!bFoundMats) {
			static auto Wood = Runtime::StaticFindObject<UFortWorldItemDefinition>("/Game/Items/ResourcePickups/WoodItemData.WoodItemData");
			static auto Stone = Runtime::StaticFindObject<UFortWorldItemDefinition>("/Game/Items/ResourcePickups/StoneItemData.StoneItemData");
			static auto Metal = Runtime::StaticFindObject<UFortWorldItemDefinition>("/Game/Items/ResourcePickups/MetalItemData.MetalItemData");
			
			AFortInventory::SpawnPickup(Location, AFortInventory::MakeItemEntry(Wood, 50, 0), EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::PlayerElimination, PlayerController->GetMyFortPawn());
			AFortInventory::SpawnPickup(Location, AFortInventory::MakeItemEntry(Stone, 50, 0), EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::PlayerElimination, PlayerController->GetMyFortPawn());
			AFortInventory::SpawnPickup(Location, AFortInventory::MakeItemEntry(Metal, 50, 0), EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::PlayerElimination, PlayerController->GetMyFortPawn());
		}
	}

	if (!KillerPlayerState) KillerPlayerState = PlayerState;
	if (!KillerPawn) KillerPawn = PlayerController->GetMyFortPawn();

	if (KillerPlayerState && KillerPawn && KillerPawn->GetController() && KillerPawn->GetController() != PlayerController) {
		KillerPlayerState->SetKillScore(KillerPlayerState->GetKillScore()++);
		KillerPlayerState->CallFunc<void>("FortPlayerStateAthena", "OnRep_KillScore");
		KillerPlayerState->SetTeamKillScore(KillerPlayerState->GetTeamKillScore()++);;
		KillerPlayerState->CallFunc<void>("FortPlayerStateAthena", "OnRep_TeamKillScore");
		KillerPlayerState->CallFunc<void>("FortPlayerStateAthena", "ClientReportTeamKill", KillerPlayerState->GetTeamKillScore());
		
		for (auto Member : ((AFortPlayerStateAthena*)KillerPlayerState)->Get<TWeakObjectPtr<AFortTeamInfo>>("FortPlayerState", "PlayerTeam")->GetTeamMembers()) {
			auto MemberPlayerState = Member->GetPlayerState();
			if (MemberPlayerState != KillerPlayerState) {
				MemberPlayerState->SetTeamKillScore(MemberPlayerState->GetTeamKillScore()++);
				MemberPlayerState->CallFunc<void>("FortPlayerStateAthena", "OnRep_TeamKillScore");
				MemberPlayerState->CallFunc<void>("FortPlayerStateAthena", "ClientReportTeamKill", MemberPlayerState->GetTeamKillScore());
			}
		}
		
		KillerPlayerState->CallFunc<void>("FortPlayerStateAthena", "ClientReportKill", KillerPlayerState);
		if (auto CPlayerController = (AFortPlayerControllerAthena*)KillerPawn->GetController()) {
			if (CPlayerController->GetMyFortPawn())
				CPlayerController->GetMatchReport()->MatchStats.Stats[3] = KillerPlayerState->GetKillScore();
		}
		
		/*if (KillerPawn && KillerPawn != PlayerController->GetMyFortPawn()) {
			auto Handle = KillerPlayerState->GetAbilitySystemComponent()->CallFunc<FGameplayEffectContextHandle>("AbilitySystemComponent", "MakeEffectContext");
			FGameplayTag Tag;
			static auto Cue = UKismetStringLibrary::GetDefaultObj()->CallFunc<FName>("KismetStringLibrary","Conv_StringToName", L"GameplayCue.Shield.PotionConsumed");
			Tag.TagName = Cue;
			KillerPlayerState->GetAbilitySystemComponent()->NetMulticast_InvokeGameplayCueAdded(Tag, FPredictionKey(), Handle);
			KillerPlayerState->GetAbilitySystemComponent()->NetMulticast_InvokeGameplayCueExecuted(Tag, FPredictionKey(), Handle);

			if (KillerPawn) {
				auto Health = KillerPawn->GetHealth();
				auto Shield = KillerPawn->GetShield();

				if (Health >= 100.0f) {
					Shield = Shield + 50.0f;
				} else if (Health + 50.0f > 100.0f) {
					float HealthOverflow = (Health + 50.0f) - 100.0f;
					Health = 100.0f;
					Shield = Shield + HealthOverflow;
				} else {
					Health += 50.0f;
				}
    
				KillerPawn->SetHealth(Health);
				KillerPawn->SetShield(Shield);
			}
		}
	}

	static std::unordered_map<AFortPlayerController*, AFortPlayerController*> SpectatorMap;
	
	if ((PlayerController->GetMyFortPawn() ? !PlayerController->GetMyFortPawn()->CallFunc<bool>("FortPawn", "IsDBNO") : true))
	{
		((void (*)(AFortGameModeAthena*, AFortPlayerController*, AFortPlayerStateAthena*, AFortPawn*, UFortWeaponItemDefinition*, EDeathCause, char))(IMAGEBASE + 0x8495BE0))
			(GameMode, PlayerController, KillerPlayerState, KillerPawn, 
			 DeathReport.DamageCauser.ObjectIndex ? Cast<AFortWeapon>(DeathReport.DamageCauser.Get())->GetWeaponData() : nullptr, 
			 CachedDeathCause, 0);

		FAthenaRewardResult KResult = PlayerController->GetMyFortPawn()->EndOfMatchResults;

		if (PlayerController->GetMatchReport())
		{
			KResult.TotalBookXpGained = PlayerController->XPComponent->TotalXpEarned;
			KResult.TotalSeasonXpGained = PlayerController->XPComponent->TotalXpEarned;
			PlayerController->GetMatchReport()->EndOfMatchResults = KResult;
			PlayerController->ClientSendEndBattleRoyaleMatchForPlayer(true, PlayerController->GetMatchReport()->EndOfMatchResults);

			PlayerState->SetPlace(GameState->PlayersLeft + 1);
			PlayerState->CallFunc<void>("FortGameStateAthena", "OnRep_Place");

			FAthenaMatchStats& Stats = PlayerController->GetMatchReport()->MatchStats;
			FAthenaMatchTeamStats& TeamStats = PlayerController->GetMatchReport()->TeamStats;

			if (PlayerState && PlayerState->GetKillScore() && PlayerState->SquadId) {
				Stats.Stats[3] = PlayerState->GetKillScore();
				Stats.Stats[8] = PlayerState->SquadId;
				PlayerController->ClientSendMatchStatsForPlayer(Stats);
			}

			TeamStats.Place = PlayerState->GetPlace();
			TeamStats.TotalPlayers = GameState->TotalPlayers;
			PlayerController->ClientSendTeamStatsForPlayer(TeamStats);
		}
		
		int AlivePlayersCount = 0;
		AFortPlayerControllerAthena* LastAliveController = nullptr;

		for (auto& AlivePC : GameMode->GetAlivePlayers()) {
			if (AlivePC && AlivePC != PlayerController && AlivePC->GetMyFortPawn() && !AlivePC->GetMyFortPawn()->CallFunc<bool>("FortPawn", "IsDBNO")) {
				AlivePlayersCount++;
				LastAliveController = AlivePC;
			}
		}

		if (AlivePlayersCount == 1 && LastAliveController) {
			AFortPlayerStateAthena* WinnerPlayerState = LastAliveController->GetPlayerState();
			AFortPlayerPawn* WinnerPawn = LastAliveController->GetMyFortPawn();
        
			if (WinnerPlayerState && WinnerPawn) {
				WinnerPlayerState->SetPlace( 1);
				WinnerPlayerState->CallFunc<void>("FortGameStateAthena", "OnRep_Place");

				auto WinnerWeapon = DeathReport.DamageCauser.ObjectIndex ? Cast<AFortWeapon>(DeathReport.DamageCauser.Get())->GetWeaponData() : nullptr;
            
				LastAliveController->PlayWinEffects(WinnerPawn, WinnerWeapon, CachedDeathCause, false);
				LastAliveController->ClientNotifyWon(WinnerPawn, WinnerWeapon, CachedDeathCause);
				LastAliveController->ClientNotifyTeamWon(WinnerPawn, WinnerWeapon, CachedDeathCause);

				if (PlayerController->GetMatchReport())
				{
					KResult.TotalBookXpGained = LastAliveController->XPComponent->TotalXpEarned;
					KResult.TotalSeasonXpGained = LastAliveController->XPComponent->TotalXpEarned;
					LastAliveController->GetMatchReport()->EndOfMatchResults = KResult;
				}
				LastAliveController->ClientSendEndBattleRoyaleMatchForPlayer(true, LastAliveController->GetMatchReport()->EndOfMatchResults);

				FAthenaMatchStats& WinnerStats = LastAliveController->GetMatchReport()->MatchStats;
				FAthenaMatchTeamStats& WinnerTeamStats = LastAliveController->GetMatchReport()->TeamStats;

				WinnerStats.Stats[3] = WinnerPlayerState->GetKillScore();
				WinnerStats.Stats[8] = WinnerPlayerState->SquadId;
				LastAliveController->ClientSendMatchStatsForPlayer(WinnerStats);

				WinnerTeamStats.Place = WinnerPlayerState->Place;
				WinnerTeamStats.TotalPlayers = GameState->TotalPlayers;
				LastAliveController->ClientSendTeamStatsForPlayer(WinnerTeamStats);

				GameState->WinningTeam = WinnerPlayerState->Get<uint8>("FortPlayerStateAthena", "TeamIndex");
				GameState->OnRep_WinningTeam();
				GameState->WinningPlayerState = WinnerPlayerState;
				GameState->OnRep_WinningPlayerState();
			}
		}
	}
	
	ClientOnPawnDiedOG(PlayerController, DeathReport); */
}

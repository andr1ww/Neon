#include "pch.h"
#include "../Header/FortPlayerController.h"

#include "Engine/GameplayStatics/Header/GameplayStatics.h"
#include "Engine/Guid/Header/Guid.h"
#include "Engine/NetDriver/Header/NetDriver.h"
#include "FortniteGame/AbilitySystemComponent/Header/AbilitySystemComponent.h"
#include "FortniteGame/FortKismetLibrary/Header/FortKismetLibrary.h"
#include "FortniteGame/FortLoot/Header/FortLootPackage.h"
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
}

void AFortPlayerControllerAthena::ServerLoadingScreenDropped(AFortPlayerControllerAthena* PlayerController, FFrame& Stack)
{
	Stack.IncrementCode();
	static UFortAbilitySet* AbilitySet = nullptr;
	if (!AbilitySet) AbilitySet = (UFortAbilitySet*)GUObjectArray.FindObject("GAS_AthenaPlayer");
	UAbilitySystemComponent::GiveAbilitySet(PlayerController->GetPlayerState()->GetAbilitySystemComponent(), AbilitySet);
	AFortGameStateAthena* GameState = UWorld::GetWorld()->GetGameState();
	AFortPlayerStateAthena* PlayerState = PlayerController->GetPlayerState();
	
	PlayerState->Set("FortPlayerStateAthena", "SquadId", PlayerState->Get<uint8>("FortPlayerStateAthena", "TeamIndex") - 3);
	PlayerState->CallFunc<void>("FortPlayerStateAthena", "OnRep_SquadId");

	static const int32 FGameMemberInfoSize = StaticClassImpl("GameMemberInfo")->GetSize();
	FGameMemberInfo* Member = (FGameMemberInfo*)malloc(FGameMemberInfoSize);
	memset(Member, 0, FGameMemberInfoSize);
	Member->MostRecentArrayReplicationKey = -1;
	Member->ReplicationID = -1;
	Member->ReplicationKey = -1;
	Member->TeamIndex = PlayerState->Get<uint8>("FortPlayerStateAthena", "TeamIndex");
	Member->SquadId = PlayerState->Get<uint8>("FortPlayerStateAthena", "SquadId");
	Member->MemberUniqueId = PlayerState->Get<FUniqueNetIdRepl>("PlayerState", "UniqueId");

	GameState->GetGameMemberInfoArray().GetMembers().Add(*Member);
	GameState->GetGameMemberInfoArray().MarkItemDirty(*Member);
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

	if (!PlayerController->GetMyFortPawn()) return;
	
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
        GameMode->RestartPlayer(PlayerController);
        PlayerController->Set("Controller", "ControlRotation", Rotation);

        PlayerController->GetMyFortPawn()->BeginSkydiving(true);
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

	static FDeathInfo* DeathInfo = nullptr;
	static FAthenaRewardResult* RewardResult = nullptr;
	static FAthenaMatchStats* MatchStats = nullptr;
	static FAthenaMatchTeamStats* TeamStats = nullptr;
	
	if (!DeathInfo) {
		static const int32 FDeathInfoSize = StaticClassImpl("DeathInfo")->GetSize();
		static const int32 FAthenaRewardResultSize = StaticClassImpl("AthenaRewardResult")->GetSize();
		static const int32 FAthenaMatchStatsSize = StaticClassImpl("AthenaMatchStats")->GetSize();
		static const int32 FAthenaMatchTeamStatsSize = StaticClassImpl("AthenaMatchTeamStats")->GetSize();
		
		DeathInfo = (FDeathInfo*)malloc(FDeathInfoSize);
		RewardResult = (FAthenaRewardResult*)malloc(FAthenaRewardResultSize);
		MatchStats = (FAthenaMatchStats*)malloc(FAthenaMatchStatsSize);
		TeamStats = (FAthenaMatchTeamStats*)malloc(FAthenaMatchTeamStatsSize);
	}
	
	memset(DeathInfo, 0, sizeof(FDeathInfo));
	memset(RewardResult, 0, sizeof(FAthenaRewardResult));
	memset(MatchStats, 0, sizeof(FAthenaMatchStats));
	memset(TeamStats, 0, sizeof(FAthenaMatchTeamStats));
	
	auto DeathTags = DeathReport.GetTags();
	EDeathCause DeathCause = PlayerState->CallFunc<EDeathCause>("FortPlayerStateAthena", "ToDeathCause", DeathTags, false);

	PlayerState->Set("FortPlayerState", "PawnDeathLocation", DeathLocation);
	DeathInfo->SetbDBNO(false);
	DeathInfo->SetDeathLocation(DeathLocation);
	DeathInfo->SetDeathTags(DeathTags);
	DeathInfo->SetDeathCause(DeathCause);
	DeathInfo->SetDowner((AActor*)KillerPlayerState);
	DeathInfo->SetFinisherOrDowner((AActor*)KillerPlayerState);

	if (VictimPawn) {
		DeathInfo->GetDistance() = (DeathCause != EDeathCause::FallDamage) 
			? (KillerPawn && KillerPawn->GetClass()->GetFunction("GetDistanceTo") ? KillerPawn->CallFunc<float>("Actor", "GetDistanceTo", VictimPawn) : 0.0f)
			: VictimPawn->Get<float>("FortPlayerPawnAthena", "LastFallDistance");
	}

	DeathInfo->SetbInitialized(true);
	PlayerState->SetDeathInfo(*DeathInfo);
	PlayerState->OnRep_DeathInfo();
   
	auto WorldInventory = PlayerController->GetWorldInventory();
	if (WorldInventory && VictimPawn) {
		static const UClass* Types[] = {
			UFortResourceItemDefinition::StaticClass(),
			UFortWeaponRangedItemDefinition::StaticClass(),
			UFortConsumableItemDefinition::StaticClass(),
			UFortAmmoItemDefinition::StaticClass()
		};
   	
		auto Location = VictimPawn->GetActorLocation();
		bool bFoundMats = false;
		
		const auto& ItemInstances = WorldInventory->GetInventory().GetItemInstances();
		for (int32 i = 0; i < ItemInstances.Num(); ++i) {
			const auto& entry = ItemInstances[i];
			auto ItemDef = entry->GetItemEntry().GetItemDefinition();
			if (ItemDef->IsA<UFortWeaponMeleeItemDefinition>()) continue;
   		
			for (int32 j = 0; j < 4; ++j) {
				if (ItemDef->IsA(Types[j])) {
					if (!bFoundMats && Types[j] == UFortResourceItemDefinition::StaticClass())
						bFoundMats = true;
					AFortInventory::SpawnPickup(Location, &entry->GetItemEntry(), EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::PlayerElimination, VictimPawn);
					break;
				}
			}
		}
   	
		if (!bFoundMats) {
			static auto Wood = Runtime::StaticFindObject<UFortWorldItemDefinition>("/Game/Items/ResourcePickups/WoodItemData.WoodItemData");
			static auto Stone = Runtime::StaticFindObject<UFortWorldItemDefinition>("/Game/Items/ResourcePickups/StoneItemData.StoneItemData");
			static auto Metal = Runtime::StaticFindObject<UFortWorldItemDefinition>("/Game/Items/ResourcePickups/MetalItemData.MetalItemData");
   		
			AFortInventory::SpawnPickup(Location, AFortInventory::MakeItemEntry(Wood, 50, 0), EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::PlayerElimination, VictimPawn);
			AFortInventory::SpawnPickup(Location, AFortInventory::MakeItemEntry(Stone, 50, 0), EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::PlayerElimination, VictimPawn);
			AFortInventory::SpawnPickup(Location, AFortInventory::MakeItemEntry(Metal, 50, 0), EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::PlayerElimination, VictimPawn);
		}
	}

	if (!KillerPlayerState) KillerPlayerState = PlayerState;
	if (!KillerPawn) KillerPawn = VictimPawn;

	if (KillerPlayerState && KillerPawn && KillerPawn->GetController() && KillerPawn->GetController() != PlayerController) {
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
   	
		KillerPlayerState->ClientReportKill(KillerPlayerState);
		if (auto CPlayerController = (AFortPlayerControllerAthena*)KillerPawn->GetController()) {
			if (CPlayerController->GetMyFortPawn()) {
				MatchStats->Stats[3] = KillerScore;
				CPlayerController->GetMatchReport()->SetMatchStats(*MatchStats);
			}
		}
	}
   
	bool bIsDBNO = VictimPawn && VictimPawn->IsDBNO();
	if (!bIsDBNO) {
		static int DamageCauserOffset = Runtime::GetOffsetStruct("FortPlayerDeathReport", "DamageCauser");
		AActor* DamageCauser = *(AActor**)((char*)&DeathReport + DamageCauserOffset);
		UFortWeaponItemDefinition* ItemDef = nullptr;
		
		if (DamageCauser)
		{
			if (DamageCauser->IsA<AFortProjectileBase>())
			{
				auto Owner = Cast<AFortWeapon>(DamageCauser->CallFunc<AActor*>("Actor", "GetOwner"));
				ItemDef = Owner->IsValidLowLevel() ? Owner->GetWeaponData() : nullptr; 
			}

			if (auto WeaponDef = Cast<AFortWeapon>(DamageCauser))
			{
				ItemDef = WeaponDef->GetWeaponData();
			}
		}
		
		((void (*)(AFortGameModeAthena*, AFortPlayerController*, AFortPlayerStateAthena*, AFortPawn*, UFortWeaponItemDefinition*, EDeathCause, char, bool))(Finder->RemoveFromAlivePlayers()))
		  (GameMode, PlayerController, KillerPlayerState, KillerPawn, ItemDef, DeathCause, 0, false);

		auto MatchReport = PlayerController->GetMatchReport();
		if (MatchReport) {
			*RewardResult = MatchReport->GetEndOfMatchResults();
			int32 TotalXP = PlayerController->GetXPComponent()->Get<int32>("FortPlayerControllerAthenaXPComponent", "TotalXpEarned");
			RewardResult->SetTotalBookXpGained(TotalXP);
			RewardResult->SetTotalSeasonXpGained(TotalXP);
			MatchReport->SetEndOfMatchResults(*RewardResult);
			PlayerController->ClientSendEndBattleRoyaleMatchForPlayer(true, MatchReport->GetEndOfMatchResults());

			int32 AliveCount = GameMode->GetAlivePlayers().Num() + GameMode->GetAliveBots().Num();
			int32 PlayerPlace = AliveCount + 1;
			PlayerState->SetPlace(PlayerPlace);
			PlayerState->OnRep_Place();

			*MatchStats = MatchReport->GetMatchStats();
			*TeamStats = MatchReport->GetTeamStats();

			if (PlayerState->GetKillScore() && PlayerState->GetSquadId()) {
				MatchStats->Stats[3] = PlayerState->GetKillScore();
				MatchStats->Stats[8] = PlayerState->GetSquadId();
				MatchReport->SetMatchStats(*MatchStats);
				PlayerController->ClientSendMatchStatsForPlayer(*MatchStats);
			}

			TeamStats->SetPlace(PlayerPlace);
			TeamStats->SetTotalPlayers(PlayerPlace);
			MatchReport->SetTeamStats(*TeamStats);
			PlayerController->ClientSendTeamStatsForPlayer(*TeamStats);
		}
   	
		AFortPlayerControllerAthena* LastAliveController = nullptr;
		const auto& AlivePlayers = GameMode->GetAlivePlayers();
		
		for (int32 i = 0; i < AlivePlayers.Num(); ++i) {
			const auto& AlivePC = AlivePlayers[i];
			if (AlivePC && AlivePC != PlayerController && AlivePC->GetMyFortPawn() && !AlivePC->GetMyFortPawn()->IsDBNO()) {
				LastAliveController = AlivePC;
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
				if (WinnerMatchReport) {
					int32 WinnerXP = LastAliveController->GetXPComponent()->Get<int32>("FortPlayerControllerAthenaXPComponent", "TotalXpEarned");
					RewardResult->SetTotalBookXpGained(WinnerXP);
					RewardResult->SetTotalSeasonXpGained(WinnerXP);
					WinnerMatchReport->SetEndOfMatchResults(*RewardResult);
					LastAliveController->ClientSendEndBattleRoyaleMatchForPlayer(true, WinnerMatchReport->GetEndOfMatchResults());

					*MatchStats = WinnerMatchReport->GetMatchStats();
					*TeamStats = WinnerMatchReport->GetTeamStats();

					MatchStats->Stats[3] = WinnerPlayerState->GetKillScore();
					MatchStats->Stats[8] = WinnerPlayerState->GetSquadId();
					WinnerMatchReport->SetMatchStats(*MatchStats);
					LastAliveController->ClientSendMatchStatsForPlayer(*MatchStats);

					int32 TotalPlayers = TotalAlive + 1;
					TeamStats->SetPlace(1);
					TeamStats->SetTotalPlayers(TotalPlayers);
					WinnerMatchReport->SetTeamStats(*TeamStats);
					LastAliveController->ClientSendTeamStatsForPlayer(*TeamStats);
				}

				uint8 WinningTeamIndex = WinnerPlayerState->Get<uint8>("FortPlayerStateAthena", "TeamIndex");
				GameState->Set("FortGameStateAthena", "WinningTeam", WinningTeamIndex);
				GameState->OnRep_WinningTeam();
				GameState->Set("FortGameStateAthena", "WinningPlayerState", WinnerPlayerState);
				GameState->OnRep_WinningPlayerState();
			}
		}
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
		return;

	AFortInventory* WorldInventory = PlayerController->GetWorldInventory();
	FFortItemList& Inventory = WorldInventory->GetInventory();
	TArray<FFortItemEntry>& ReplicatedEntries = Inventory.GetReplicatedEntries();
    
	FFortItemEntry* Entry = nullptr;
	uint8* Data = (uint8*)ReplicatedEntries.GetData();
	static int32 FortItemEntrySize = StaticClassImpl("FortItemEntry")->GetSize();
    
	for (int32 i = 0; i < Count; ++i) {
		auto Item = (FFortItemEntry*)(Data + (i * FortItemEntrySize));
		if (Item->GetItemGuid() == ItemGuid) {
			Entry = Item;
			break;
		}
	}

	if (!Entry || (Entry->GetCount() - Count) < 0)
		return;
	Entry->SetCount(Entry->GetCount() - Count);
	AFortInventory::SpawnPickup(PlayerController->GetPawn()->GetActorLocation() + PlayerController->GetPawn()->GetActorForwardVector() * 70.f + FVector(0, 0, 50), Entry, EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::Unset, PlayerController->GetMyFortPawn(), Count);
	if (Entry->GetCount() == 0)
		AFortInventory::Remove(PlayerController, ItemGuid);
	else
		AFortInventory::ReplaceEntry(PlayerController, *Entry);
}

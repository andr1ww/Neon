#include "pch.h"
#include "../Header/FortAthenaAIBotController.h"

#include "Engine/GameplayStatics/Header/GameplayStatics.h"
#include "Engine/Kismet/Header/Kismet.h"
#include "Engine/NetDriver/Header/NetDriver.h"
#include "FortniteGame/FortLootPackage/Header/FortLootPackage.h"
#include "FortniteGame/FortServerBotManager/Header/FortServerBotManager.h"
#include "Neon/TickService/FortAthenaAI/Names.h"
#include "Neon/TickService/FortAthenaAI/Header/FortAthenaAI.h"
#include "FortniteGame/FortQuestManager/Header/FortQuestManager.h"

void AFortAthenaAIBotController::SpawnPlayerBot(int Count) {
	static std::vector<UAthenaCharacterItemDefinition*> Characters = std::vector<UAthenaCharacterItemDefinition*>();
	static std::vector<UAthenaPickaxeItemDefinition*> Pickaxes = std::vector<UAthenaPickaxeItemDefinition*>();

	static UBehaviorTree* BehaviorTree = Runtime::StaticLoadObject<UBehaviorTree>("/Game/Athena/AI/Phoebe/BehaviorTrees/BT_Phoebe.BT_Phoebe");
	static UBehaviorTree* BehaviorTreeGamePlay = Runtime::StaticLoadObject<UBehaviorTree>("/Game/Athena/AI/Phoebe/BehaviorTrees/BT_Phoebe_Gameplay.BT_Phoebe_Gameplay");
	static UBehaviorTree* BehaviorTreeLoot = Runtime::StaticLoadObject<UBehaviorTree>("/Game/Athena/AI/Phoebe/BehaviorTrees/BT_Phoebe_Loot.BT_Phoebe_Loot");
	static UBehaviorTree* BehaviorTreeStorm = Runtime::StaticLoadObject<UBehaviorTree>("/Game/Athena/AI/Phoebe/BehaviorTrees/BT_Phoebe_Storm.BT_Phoebe_Storm");
	static UBehaviorTree* BehaviorTreeUnStuck = Runtime::StaticLoadObject<UBehaviorTree>("/Game/Athena/AI/Phoebe/BehaviorTrees/BT_Phoebe_UnstuckNew.BT_Phoebe_UnstuckNew");

	if (Characters.size() == 0)
	{
		Characters = Runtime::GetObjectsOfClass<UAthenaCharacterItemDefinition>();
		Pickaxes = Runtime::GetObjectsOfClass<UAthenaPickaxeItemDefinition>();
	}
	
	static auto BotBP = Runtime::StaticLoadObject<UClass>("/Game/Athena/AI/Phoebe/BP_PlayerPawn_Athena_Phoebe.BP_PlayerPawn_Athena_Phoebe_C");
	if (!BotBP) {
		return;
	}

	auto GameMode = UWorld::GetWorld()->GetAuthorityGameMode();

	static TArray<AActor*> PlayerStarts = UGameplayStatics::GetAllActorsOfClass(UWorld::GetWorld(), AFortPlayerStartWarmup::StaticClass());
	if (PlayerStarts.Num() == 0) {
		return;
	}

	static TArray<AActor*> Spawns = PlayerStarts;
	
	for (int i = 0; i < Count; i++) {
		int RandomIndex = rand() % Spawns.Num();
		AActor* BotSpawn = Spawns[RandomIndex];
		Spawns.Remove(RandomIndex);
		FVector Loc = BotSpawn->GetActorLocation();
		Loc.Z += 250;
		
	//	AFortPlayerPawn* Pawn = UGameplayStatics::SpawnActorOG<AFortPlayerPawnAthena>(BotBP, Loc, {});
		AFortPlayerPawn* Pawn = GameMode->GetServerBotManager()->GetCachedBotMutator()->SpawnBot(BotBP, BotSpawn, Loc, {}, false);
		AFortAthenaAIBotController* Controller = (AFortAthenaAIBotController*)Pawn->GetController();

	//	GameMode->GetAlivePlayers().Add(Cast<AFortPlayerControllerAthena>(Controller));
	//	++GameState->GetPlayersLeft();
	//	GameState->OnRep_PlayersLeft();
		
		if (Characters.size() != 0)
		{
			UAthenaCharacterItemDefinition* CID = nullptr;
			bool Found = false;
    
			while (!Found && Characters.size() > 0) {
				int randomIndex = rand() % Characters.size();
				CID = Characters[randomIndex];
        
				if (CID && IsValidPointer(CID) && IsValidPointer(CID->GetHeroDefinition())) {
					Found = true;
					TArray<TSoftObjectPtr<UFortHeroSpecialization>> Specializations = CID->GetHeroDefinition()->GetSpecializations();
					for (TSoftObjectPtr<UFortHeroSpecialization> Spec : Specializations)
					{
						if (Spec.IsValid())
						{
							auto SpecDef = Spec.Get(UFortHeroSpecialization::StaticClass(), true);
							for (int32 j = 0; j < SpecDef->GetCharacterParts().Num(); j++)
							{
								UCustomCharacterPart* Part = SpecDef->GetCharacterParts()[j].Get(UCustomCharacterPart::StaticClass(), true); 
								if (Part) {
									Pawn->CallFunc<void>("FortPlayerPawn", "ServerChoosePart", Part->GetCharacterPartType(), Part);
								}
							}
						}
					}
				} else {
					Characters.erase(Characters.begin() + randomIndex);
				}
			}
		}
		
		if (!Controller->GetInventory()) {
			Controller->SetInventory(UGameplayStatics::SpawnActorOG<AFortInventory>(AFortInventory::StaticClass(), {}, {}, Controller));
		}

		auto& StartingItemsArray = GameMode->GetStartingItems();
		static int32 FItemAndCountSize = StaticClassImpl("ItemAndCount")->GetSize();
		for (int i = 0; i < StartingItemsArray.Num(); i++)
		{
			auto Item = (FItemAndCount*) ((uint8*) StartingItemsArray.GetData() + (i * FItemAndCountSize));
        
			if (!Item) {
				UE_LOG(LogNeon, Fatal, "StartingItem is null in SpawnDefaultPawnFor!");
				break;
			}
    
	//		AFortInventory::GiveItem(Controller, Item->GetItem(), Item->GetCount(), 1, 1);
		}

		static UFortWeaponMeleeItemDefinition* PickDef = Runtime::StaticLoadObject<UFortWeaponMeleeItemDefinition>("/Game/Athena/Items/Weapons/WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01");
		bool Valid = false;
		if (Pickaxes.size() > 0) {
			while (!Valid) {
				auto Pickaxe = Pickaxes[rand() % Pickaxes.size()];
				if (IsValidPointer(Pickaxe))
				{
					PickDef = Pickaxe->GetWeaponDefinition();
					Valid = true;
				}
			}
		}
		
		if (PickDef) {
			UFortWorldItem* Item = (UFortWorldItem*)AFortInventory::GiveItem(Controller, PickDef, 1, 1, 0);
			if (Item) {
				Pawn->EquipWeaponDefinition((UFortWeaponItemDefinition*)Item->GetItemEntry().GetItemDefinition(), Item->GetItemEntry().GetItemGuid(), Item->GetItemEntry().GetTrackerGuid(), false);
				//Pawn->CallFunc<void>("FortPawn", "EquipWeaponDefinition", Item->GetItemEntry().GetItemDefinition(), Item->GetItemEntry(), false);
			}
		}

		for (size_t i = 0; i < Controller->GetDigestedBotSkillSets().Num(); i++)
		{
			if (Controller->GetDigestedBotSkillSets()[i]->IsA(UFortAthenaAIBotAimingDigestedSkillSet::StaticClass()))
			{
				Controller->Set("FortAthenaAIBotController", "CacheAimingDigestedSkillSet", (UFortAthenaAIBotAimingDigestedSkillSet*)Controller->GetDigestedBotSkillSets()[i]);
			}

			if (Controller->GetDigestedBotSkillSets()[i]->IsA(UFortAthenaAIBotHarvestDigestedSkillSet::StaticClass()))
			{
				Controller->Set("FortAthenaAIBotController", "CacheHarvestDigestedSkillSet", (UFortAthenaAIBotHarvestDigestedSkillSet*)Controller->GetDigestedBotSkillSets()[i]);
			}
	
			if (Controller->GetDigestedBotSkillSets()[i]->IsA(UFortAthenaAIBotInventoryDigestedSkillSet::StaticClass()))
			{
				Controller->Set("FortAthenaAIBotController", "CacheInventoryDigestedSkillSet", (UFortAthenaAIBotInventoryDigestedSkillSet*)Controller->GetDigestedBotSkillSets()[i]);
			}

			if (Controller->GetDigestedBotSkillSets()[i]->IsA(UFortAthenaAIBotLootingDigestedSkillSet::StaticClass()))
			{
				Controller->Set("FortAthenaAIBotController", "CacheLootingSkillSet", (UFortAthenaAIBotLootingDigestedSkillSet*)Controller->GetDigestedBotSkillSets()[i]);
			}

			if (Controller->GetDigestedBotSkillSets()[i]->IsA(UFortAthenaAIBotMovementDigestedSkillSet::StaticClass()))
			{
				Controller->Set("FortAthenaAIBotController", "CacheMovementSkillSet", (UFortAthenaAIBotMovementDigestedSkillSet*)Controller->GetDigestedBotSkillSets()[i]);
			}

			if (Controller->GetDigestedBotSkillSets()[i]->IsA(UFortAthenaAIBotPerceptionDigestedSkillSet::StaticClass()))
			{
				Controller->Set("FortAthenaAIBotController", "CachePerceptionDigestedSkillSet", (UFortAthenaAIBotPerceptionDigestedSkillSet*)Controller->GetDigestedBotSkillSets()[i]);
			}

			if (Controller->GetDigestedBotSkillSets()[i]->IsA(UFortAthenaAIBotPlayStyleDigestedSkillSet::StaticClass()))
			{
				Controller->Set("FortAthenaAIBotController", "CachePlayStyleSkillSet", (UFortAthenaAIBotPlayStyleDigestedSkillSet*)Controller->GetDigestedBotSkillSets()[i]);
			}
		}

		if (BehaviorTree)
		{
			UBlackboardComponent* Blackboard = Controller->GetBlackboard();
			
			Controller->UseBlackboard(Controller->GetBehaviorTree()->GetBlackboardAsset(), &Blackboard);
			Controller->OnUsingBlackBoard(Blackboard, Controller->GetBehaviorTree()->GetBlackboardAsset());

			UFortServerBotManagerAthena::RunBehaviorTree(Controller, BehaviorTree);
			UFortServerBotManagerAthena::RunBehaviorTree(Controller, BehaviorTreeGamePlay);
			UFortServerBotManagerAthena::RunBehaviorTree(Controller, BehaviorTreeLoot);
			UFortServerBotManagerAthena::RunBehaviorTree(Controller, BehaviorTreeStorm);
			UFortServerBotManagerAthena::RunBehaviorTree(Controller, BehaviorTreeUnStuck);

			if (auto System = UWorld::GetWorld()->GetNavigationSystem())
			{
				if (auto Nav = System->GetMainNavData())
				{
					Controller->GetPathFollowingComponent()->SetMyNavData(Nav);
					Controller->GetPathFollowingComponent()->CallFunc<void>("PathFollowingComponent", "OnNavDataRegistered", Nav);
					Controller->GetPathFollowingComponent()->CallFunc<void>("ActorComponent", "Activate", true);
					Controller->GetPathFollowingComponent()->CallFunc<void>("ActorComponent", "SetActivate", true, true);
					Controller->GetPathFollowingComponent()->CallFunc<void>("ActorComponent", "OnRep_IsActive");
				}
			}
		}
		
		if (Names.size() != 0) {
			std::string Name = Names[rand() % Names.size()];
			Names.erase(std::find(Names.begin(), Names.end(), Name));

			std::wstring wideString(MultiByteToWideChar(CP_UTF8, 0, Name.c_str(), Name.length(), nullptr, 0), 0);
			MultiByteToWideChar(CP_UTF8, 0, Name.c_str(), Name.length(), &wideString[0], wideString.length());

			GameMode->CallFunc<void>("GameModeBase", "ChangeName", Controller, FString(wideString.c_str()), true);
			Controller->GetPlayerState()->CallFunc<void>("PlayerState", "OnRep_PlayerName");
		}
		
		TickService::FortAthenaAIService::AddToService(Controller, Pawn);
	}
}

void AFortAthenaAIBotController::OnPossessedPawnDied(AFortAthenaAIBotController* Controller, AActor* DamagedActor, float Damage, AFortPlayerControllerAthena* InstigatedBy, AActor* DamageCauser, FVector HitLocation, UPrimitiveComponent* HitComp, FName Bone, FVector Momentum)
{
	if (Controller->GetPawn() && InstigatedBy)
	{
		AFortPlayerPawn* Pawn = (AFortPlayerPawn*)Controller->GetPawn();
		TickService::FortAthenaAIService::RemoveFromService(Controller);

		AFortPlayerStateAthena* PlayerState = (AFortPlayerStateAthena*)Controller->GetPlayerState();
    	
		FDeathInfo* DeathInfo = &PlayerState->GetDeathInfo(); 
		EDeathCause DeathCause = PlayerState->ToDeathCause(Pawn->GetDeathTags(), false);
    	
		if (DeathInfo) {
			DeathInfo->SetbDBNO(false);
			DeathInfo->SetDeathLocation(Pawn->K2_GetActorLocation());
			DeathInfo->SetDeathTags(Pawn->GetDeathTags());
			DeathInfo->SetDeathCause(DeathCause);
			DeathInfo->SetDowner( nullptr);
			DeathInfo->SetFinisherOrDowner(InstigatedBy->GetPlayerState());

			if (Pawn && InstigatedBy->IsA<AFortPlayerControllerAthena>()) {
				DeathInfo->GetDistance() = (DeathCause != EDeathCause::FallDamage) 
					? (InstigatedBy->GetMyFortPawn() && InstigatedBy->GetMyFortPawn()->GetClass()->GetFunction("GetDistanceTo") ? InstigatedBy->GetMyFortPawn()->GetDistanceTo(Pawn) : 0.0f)
					: Pawn->Get<float>("FortPlayerPawnAthena", "LastFallDistance");
			} else if (Pawn && InstigatedBy->IsA<AFortAthenaAIBotController>())
			{
				AFortAthenaAIBotController* Controller = (AFortAthenaAIBotController*)InstigatedBy;
				DeathInfo->GetDistance() = (DeathCause != EDeathCause::FallDamage) 
					? (Controller->GetPawn() && Controller->GetPawn()->GetClass()->GetFunction("GetDistanceTo") ? Controller->GetPawn()->GetDistanceTo(Pawn) : 0.0f)
					: Pawn->Get<float>("FortPlayerPawnAthena", "LastFallDistance");
			}

			DeathInfo->SetbInitialized(true);
			PlayerState->SetDeathInfo(*DeathInfo);
			PlayerState->OnRep_DeathInfo();
		}

		AFortPlayerStateAthena* KillerPlayerState = InstigatedBy->GetPlayerState();
   	
		if (Controller->GetFName().ToString().ToString().contains("Phoebe"))
		{
			KillerPlayerState->ClientReportKill(PlayerState);

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

			if (InstigatedBy->IsA<AFortPlayerControllerAthena>() && InstigatedBy->GetMyFortPawn())
			{
				int32 CurrentKills = KillerScore;

				static UFortAccoladeItemDefinition* Bronze = Runtime::StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_014_Elimination_Bronze.AccoladeId_014_Elimination_Bronze");
				static UFortAccoladeItemDefinition* Silver = Runtime::StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_015_Elimination_Silver.AccoladeId_015_Elimination_Silver");
				static UFortAccoladeItemDefinition* Gold = Runtime::StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_016_Elimination_Gold.AccoladeId_016_Elimination_Gold");

				static UFortAccoladeItemDefinition* Elim = Runtime::StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_012_Elimination.AccoladeId_012_Elimination");
				UFortQuestManager::GiveAccolade(InstigatedBy, Elim);

				if (CurrentKills == 1) {
					UFortQuestManager::GiveAccolade(InstigatedBy, Bronze);
				}
				else if (CurrentKills == 4) {
					UFortQuestManager::GiveAccolade(InstigatedBy, Silver);
				}
				else if (CurrentKills == 8) {
					UFortQuestManager::GiveAccolade(InstigatedBy, Gold);
				}
			}
		}
    	
		AFortInventory* ControllerInventory = Controller->GetInventory();
        
		if (ControllerInventory != nullptr)
		{
			FFortItemList& Inventory = ControllerInventory->GetInventory();
			TArray<UFortWorldItem*>& ItemInstancesOffsetPtr = Inventory.GetItemInstances();
			for (int32 i = 0; i < ItemInstancesOffsetPtr.Num(); ++i)
			{
				UFortWorldItemDefinition* WorldItem = (UFortWorldItemDefinition*)ItemInstancesOffsetPtr[i]->GetItemEntry().GetItemDefinition();
				if (WorldItem && WorldItem->GetbCanBeDropped()) {
					AFortInventory::SpawnPickupDirect(
						Pawn->K2_GetActorLocation(),
						WorldItem,
						ItemInstancesOffsetPtr[i]->GetItemEntry().GetCount(),
						0,
						EFortPickupSourceTypeFlag::Tossed,
						EFortPickupSpawnSource::Unset,
						InstigatedBy->IsA<AFortAthenaAIBotController>() ? (AFortPlayerPawnAthena*)((AFortAthenaAIBotController*)InstigatedBy)->GetPawn() : InstigatedBy->GetMyFortPawn()
					);
				}
			}
		}
	}

	FBotInventory::RemoveInventory(Controller);

	return OnPossessedPawnDiedOG(Controller, DamagedActor, Damage, InstigatedBy, DamageCauser, HitLocation, HitComp, Bone, Momentum);
}
#include "pch.h"
#include "../Header/FortAthenaAIBotController.h"

#include "Engine/GameplayStatics/Header/GameplayStatics.h"
#include "Engine/Kismet/Header/Kismet.h"
#include "Engine/NetDriver/Header/NetDriver.h"
#include "FortniteGame/FortLootPackage/Header/FortLootPackage.h"
#include "FortniteGame/FortServerBotManager/Header/FortServerBotManager.h"

void AFortAthenaAIBotController::SpawnPlayerBot(int Count) {
	static std::vector<UAthenaCharacterItemDefinition*> Characters = std::vector<UAthenaCharacterItemDefinition*>();
	static std::vector<UAthenaPickaxeItemDefinition*> Pickaxes = std::vector<UAthenaPickaxeItemDefinition*>();

	static UBehaviorTree* BehaviorTree = Runtime::StaticLoadObject<UBehaviorTree>("/Game/Athena/AI/Phoebe/BehaviorTrees/BT_Phoebe.BT_Phoebe");

	if (Characters.size() == 0)
	{
		Characters = Runtime::GetObjectsOfClass<UAthenaCharacterItemDefinition>();
		Pickaxes = Runtime::GetObjectsOfClass<UAthenaPickaxeItemDefinition>();
	}
	
	static auto BotBP = Runtime::StaticLoadObject<UClass>("/Game/Athena/AI/Phoebe/BP_PlayerPawn_Athena_Phoebe.BP_PlayerPawn_Athena_Phoebe_C");
	if (!BotBP) {
		return;
	}

	static TArray<AActor*> PlayerStarts = UGameplayStatics::GetAllActorsOfClass(UWorld::GetWorld(), AFortPlayerStartWarmup::StaticClass());
	if (PlayerStarts.Num() == 0) {
		return;
	}

	auto GameMode = UWorld::GetWorld()->GetAuthorityGameMode();

	TArray<AActor*> Spawns = PlayerStarts;
	
	for (int i = 0; i < Count; i++) {
		int RandomIndex = rand() % Spawns.Num();
		AActor* BotSpawn = Spawns[RandomIndex];
		Spawns.Remove(RandomIndex);
		
		AFortPlayerPawn* Pawn = GameMode->GetServerBotManager()->GetCachedBotMutator()->SpawnBot(BotBP, BotSpawn, BotSpawn->GetActorLocation(), {}, false);
		AFortAthenaAIBotController* PC = (AFortAthenaAIBotController*)Pawn->GetController();

		if (Characters.size() != 0)
		{
			UAthenaCharacterItemDefinition* CID = Characters[rand() % (Characters.size() - 1)];
			if (CID && FortLootPackage::IsValidPointer(CID)) {
				if (!FortLootPackage::IsValidPointer(CID->GetHeroDefinition())) continue;
				TArray<TSoftObjectPtr<UFortHeroSpecialization>> Specializations = CID->GetHeroDefinition()->GetSpecializations();
				if (Specializations.Num() == 0) continue;
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
			}
		}

		if (!PC->GetInventory()) {
			PC->SetInventory(UGameplayStatics::SpawnActorOG<AFortInventory>(AFortInventory::StaticClass(), {}, {}, PC));
		}

		auto& StartingItemsArray = GameMode->GetStartingItems();
		int32 FItemAndCountSize = StaticClassImpl("ItemAndCount")->GetSize();
		for (int i = 0; i < StartingItemsArray.Num(); i++)
		{
			auto Item = (FItemAndCount*) ((uint8*) StartingItemsArray.GetData() + (i * FItemAndCountSize));
        
			if (!Item) {
				UE_LOG(LogNeon, Fatal, "StartingItem is null in SpawnDefaultPawnFor!");
				break;
			}
    
	//		AFortInventory::GiveItem(PC, Item->GetItem(), Item->GetCount(), 1, 1);
		}

		static UFortWeaponMeleeItemDefinition* PickDef = Runtime::StaticLoadObject<UFortWeaponMeleeItemDefinition>("/Game/Athena/Items/Weapons/WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01");
		/*if (Pickaxes.size() != 0) {
			auto Pickaxe = (UFortWeaponMeleeItemDefinition*)Pickaxes[rand() % (Pickaxes.size() - 1)];
			if (FortLootPackage::IsValidPointer(Pickaxe))
			{
				PickDef = Pickaxe;
			}
		}*/
		
		if (PickDef) {
			UFortWorldItem* Item = (UFortWorldItem*)AFortInventory::GiveItem(PC, PickDef, 1, 1, 0);
			if (Item) {
				Pawn->EquipWeaponDefinition((UFortWeaponItemDefinition*)Item->GetItemEntry().GetItemDefinition(), Item->GetItemEntry().GetItemGuid(), Item->GetItemEntry().GetTrackerGuid(), false);
				//Pawn->CallFunc<void>("FortPawn", "EquipWeaponDefinition", Item->GetItemEntry().GetItemDefinition(), Item->GetItemEntry(), false);
			}
		}

		for (size_t i = 0; i < PC->GetDigestedBotSkillSets().Num(); i++)
		{
			if (PC->GetDigestedBotSkillSets()[i]->IsA(UFortAthenaAIBotAimingDigestedSkillSet::StaticClass()))
			{
				PC->Set("FortAthenaAIBotController", "CacheAimingDigestedSkillSet", (UFortAthenaAIBotAimingDigestedSkillSet*)PC->GetDigestedBotSkillSets()[i]);
			}

			if (PC->GetDigestedBotSkillSets()[i]->IsA(UFortAthenaAIBotHarvestDigestedSkillSet::StaticClass()))
			{
				PC->Set("FortAthenaAIBotController", "CacheHarvestDigestedSkillSet", (UFortAthenaAIBotHarvestDigestedSkillSet*)PC->GetDigestedBotSkillSets()[i]);
			}
	
			if (PC->GetDigestedBotSkillSets()[i]->IsA(UFortAthenaAIBotInventoryDigestedSkillSet::StaticClass()))
			{
				PC->Set("FortAthenaAIBotController", "CacheInventoryDigestedSkillSet", (UFortAthenaAIBotInventoryDigestedSkillSet*)PC->GetDigestedBotSkillSets()[i]);
			}

			if (PC->GetDigestedBotSkillSets()[i]->IsA(UFortAthenaAIBotLootingDigestedSkillSet::StaticClass()))
			{
				PC->Set("FortAthenaAIBotController", "CacheLootingSkillSet", (UFortAthenaAIBotLootingDigestedSkillSet*)PC->GetDigestedBotSkillSets()[i]);
			}

			if (PC->GetDigestedBotSkillSets()[i]->IsA(UFortAthenaAIBotMovementDigestedSkillSet::StaticClass()))
			{
				PC->Set("FortAthenaAIBotController", "CacheMovementSkillSet", (UFortAthenaAIBotMovementDigestedSkillSet*)PC->GetDigestedBotSkillSets()[i]);
			}

			if (PC->GetDigestedBotSkillSets()[i]->IsA(UFortAthenaAIBotPerceptionDigestedSkillSet::StaticClass()))
			{
				PC->Set("FortAthenaAIBotController", "CachePerceptionDigestedSkillSet", (UFortAthenaAIBotPerceptionDigestedSkillSet*)PC->GetDigestedBotSkillSets()[i]);
			}

			if (PC->GetDigestedBotSkillSets()[i]->IsA(UFortAthenaAIBotPlayStyleDigestedSkillSet::StaticClass()))
			{
				PC->Set("FortAthenaAIBotController", "CachePlayStyleSkillSet", (UFortAthenaAIBotPlayStyleDigestedSkillSet*)PC->GetDigestedBotSkillSets()[i]);
			}
		}

		if (BehaviorTree)
		{
			UBlackboardComponent* Blackboard = PC->GetBlackboard();
			PC->UseBlackboard(PC->GetBehaviorTree()->GetBlackboardAsset(), &Blackboard);
			PC->OnUsingBlackBoard(Blackboard, PC->GetBehaviorTree()->GetBlackboardAsset());
			
			UFortServerBotManagerAthena::RunBehaviorTree(PC, BehaviorTree);
			PC->GetBlackboard()->SetValueAsEnum(UKismetStringLibrary::Conv_StringToName(L"AIEvaluator_Global_GamePhaseStep"), (uint8)EAthenaGamePhaseStep::Warmup);
			PC->GetBlackboard()->SetValueAsEnum(UKismetStringLibrary::Conv_StringToName(L"AIEvaluator_Global_GamePhase"), (uint8)EAthenaGamePhase::Warmup);
		}
	}
}

void AFortAthenaAIBotController::OnPossessedPawnDied(AFortAthenaAIBotController* Controller, AActor* DamagedActor, float Damage, AFortPlayerControllerAthena* InstigatedBy, AActor* DamageCauser, FVector HitLocation, UPrimitiveComponent* HitComp, FName Bone, FVector Momentum)
{
    if (Controller->GetPawn() && InstigatedBy)
    {
        AFortInventory* PCInventory = Controller->GetInventory();
        
        if (PCInventory != nullptr)
        {
            FFortItemList& Inventory = PCInventory->GetInventory();
            TArray<UFortWorldItem*>& ItemInstancesOffsetPtr = Inventory.GetItemInstances();
            for (int32 i = 0; i < ItemInstancesOffsetPtr.Num(); ++i)
            {
				UFortWorldItemDefinition* WorldItem = (UFortWorldItemDefinition*)ItemInstancesOffsetPtr[i]->GetItemEntry().GetItemDefinition();
				if (WorldItem && WorldItem->GetbCanBeDropped()) {
					AFortInventory::SpawnPickupDirect(
						Controller->GetActorLocation(),
						WorldItem,
						ItemInstancesOffsetPtr[i]->GetItemEntry().GetCount(),
						0,
						EFortPickupSourceTypeFlag::Tossed,
						EFortPickupSpawnSource::Unset,
						InstigatedBy->GetMyFortPawn()
					);
				}
            }
        }
        else
        {
        	UFortAthenaAIBotInventoryItems* StartupInventory = FBotInventory::GetInventory(Controller);
        	if (StartupInventory != nullptr)
            {
                for (int32 i = 0; i < StartupInventory->GetItems().Num(); i++) 
                {
                    UFortItemDefinition* ItemDef = StartupInventory->GetItems()[i];
                    if (ItemDef)
                    {
                        AFortInventory::SpawnPickup(
                            Controller->GetActorLocation(), 
                            ItemDef, 
                            1, 
                            30, 
                            EFortPickupSourceTypeFlag::Tossed, 
                            EFortPickupSpawnSource::Unset,
                            InstigatedBy->GetMyFortPawn()
                        );
                    }
                }
            }
        }
    }

	FBotInventory::RemoveInventory(Controller);

	return OnPossessedPawnDiedOG(Controller, DamagedActor, Damage, InstigatedBy, DamageCauser, HitLocation, HitComp, Bone, Momentum);
}
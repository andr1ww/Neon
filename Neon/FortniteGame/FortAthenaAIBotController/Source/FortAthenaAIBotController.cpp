#include "pch.h"
#include "../Header/FortAthenaAIBotController.h"

#include "Engine/GameplayStatics/Header/GameplayStatics.h"
#include "Engine/Kismet/Header/Kismet.h"
#include "Engine/NetDriver/Header/NetDriver.h"
#include "FortniteGame/FortLoot/Header/FortLootPackage.h"

void AFortAthenaAIBotController::SpawnPlayerBot(int Count) {
	static std::vector<UAthenaCharacterItemDefinition*> Characters = std::vector<UAthenaCharacterItemDefinition*>();
	static std::vector<UAthenaPickaxeItemDefinition*> Pickaxes = std::vector<UAthenaPickaxeItemDefinition*>();

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
		if (Pickaxes.size() != 0) {
			auto Pickaxe = (UFortWeaponMeleeItemDefinition*)Pickaxes[rand() % (Pickaxes.size() - 1)];
			if (FortLootPackage::IsValidPointer(Pickaxe))
			{
				PickDef = Pickaxe;
			}
		}
		
		if (PickDef) {
	//		UFortWorldItem* Item = (UFortWorldItem*)AFortInventory::GiveItem(PC, PickDef, 1, 1, 0);
	//		if (Item) {
	//			Pawn->EquipWeaponDefinition((UFortWeaponItemDefinition*)Item->GetItemEntry().GetItemDefinition(), Item->GetItemEntry().GetItemGuid());
	//		}
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
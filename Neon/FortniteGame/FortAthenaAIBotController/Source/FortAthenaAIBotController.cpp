#include "pch.h"
#include "../Header/FortAthenaAIBotController.h"

#include "Engine/GameplayStatics/Header/GameplayStatics.h"
#include "Engine/Kismet/Header/Kismet.h"
#include "Engine/NetDriver/Header/NetDriver.h"

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
			/*	if (CID) {
			UFortHeroSpecialization* Spec = Runtime::StaticLoadObject<UFortHeroSpecialization>(UKismetStringLibrary::GetDefaultObj()->CallFunc<FString>("KismetStringLibrary", "Conv_NameToString", CID->GetHeroDefinition()->GetSpecializations()[i].SoftObjectPtr.ObjectID.AssetPathName).ToString());
				if (Spec)
				{
					for (int32 i = 0; i < Spec->GetCharacterParts().Num(); i++)
					{
						UCustomCharacterPart* Part = Runtime::StaticLoadObject<UCustomCharacterPart>(UKismetStringLibrary::GetDefaultObj()->CallFunc<FString>("KismetStringLibrary", "Conv_NameToString", Spec->GetCharacterParts()[i].SoftObjectPtr.ObjectID.AssetPathName).ToString());
						if (Part) {
							Pawn->CallFunc<void>("FortPlayerPawn", "ServerChoosePart", Part->GetCharacterPartType(), Part);
						}
					}
				}
			} */
		}

		if (!PC->GetInventory()) {
			PC->SetInventory(UGameplayStatics::SpawnActor<AFortInventory>({}, {}, Pawn));
		}

		static UFortWeaponMeleeItemDefinition* PickDef = Runtime::StaticLoadObject<UFortWeaponMeleeItemDefinition>("/Game/Athena/Items/Weapons/WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01");
		if (Pickaxes.size() != 0) {
	//		PickDef = (UFortWeaponMeleeItemDefinition*)Pickaxes[rand() % (Pickaxes.Num() - 1)];
		}
		if (PickDef) {
			UFortWorldItem* Item = (UFortWorldItem*)AFortInventory::GiveItem(PC, PickDef, 1, 1, 0);
			if (Item) {
				Pawn->CallFunc<void>("FortPawn", "EquipWeaponDefinition", Item->GetItemEntry().GetItemDefinition(), Item->GetItemEntry().GetItemGuid(), false);
			}
		}
	}
}

void AFortAthenaAIBotController::OnPossessedPawnDied(AFortAthenaAIBotController* Controller, AActor* DamagedActor, float Damage, AFortPlayerControllerAthena* InstigatedBy, AActor* DamageCauser, FVector HitLocation, UPrimitiveComponent* HitComp, FName Bone, FVector Momentum)
{
	if (Controller->GetPawn())
	{
		AFortInventory* PCInventory = Controller->GetInventory();
		FFortItemList& Inventory = PCInventory->GetInventory();
		if (&Inventory != nullptr)
		{
			TArray<FFortItemEntry>& ReplicatedEntriesOffsetPtr = Inventory.GetReplicatedEntries();
			for (int32 i = 0; i < ReplicatedEntriesOffsetPtr.Num(); ++i)
			{
				AFortInventory::SpawnPickup(
					Controller->GetActorLocation(), 
					ReplicatedEntriesOffsetPtr[i].GetItemDefinition(), 
					ReplicatedEntriesOffsetPtr[i].GetCount(), 
					0,
					EFortPickupSourceTypeFlag::Tossed, 
					EFortPickupSpawnSource::Unset,
					InstigatedBy->GetMyFortPawn()
				);
			}
		}
	}

	return OnPossessedPawnDiedOG(Controller, DamagedActor, Damage, InstigatedBy, DamageCauser, HitLocation, HitComp, Bone, Momentum);
}

#include "pch.h"
#include "../Header/FortAthenaAIBotController.h"

#include "Engine/GameplayStatics/Header/GameplayStatics.h"
#include "Engine/Guid/Header/Guid.h"
#include "Engine/NetDriver/Header/NetDriver.h"
#include "FortniteGame/AbilitySystemComponent/Header/AbilitySystemComponent.h"
#include "FortniteGame/FortKismetLibrary/Header/FortKismetLibrary.h"
#include "Neon/Finder/Header/Finder.h"

void AFortAthenaAIBotController::SpawnPlayerBot(int Count) {
	static UBehaviorTree* PhoebeBehaviorTree = Runtime::StaticLoadObject<UBehaviorTree>("/Game/Athena/AI/Phoebe/BehaviorTrees/BT_Phoebe.BT_Phoebe");
	static UClass* PhoebePawnClass = Runtime::StaticLoadObject<UClass>("/Game/Athena/AI/Phoebe/BP_PlayerPawn_Athena_Phoebe.BP_PlayerPawn_Athena_Phoebe_C");
	if (!PhoebeBehaviorTree || !PhoebePawnClass) {
		UE_LOG(LogNeon, Warning, "AFortAthenaAIBotController::SpawnPlayerBot: Unable to spawn bot because BehaviorTree or PawnClass was not found!");
		return;
	}

	static TArray<AActor*> PlayerStarts = UGameplayStatics::GetAllActorsOfClass(UWorld::GetWorld(), AFortPlayerStartWarmup::StaticClass());
	if (PlayerStarts.Num() == 0) {
		UE_LOG(LogNeon, Warning, "AFortAthenaAIBotController::SpawnPlayerBot: Unable to spawn bot because there is no PlayerStarts!");
		return;
	}

	auto GameMode = (AFortGameModeAthena*)UWorld::GetWorld()->GetAuthorityGameMode();
	AFortGameStateAthena* GameState = (AFortGameStateAthena*)UWorld::GetWorld()->GetGameState();

	for (int i = 0; i < Count; i++) {
		UFortAthenaAIBotCustomizationData* BotCustomizationData = (UFortAthenaAIBotCustomizationData*)UGameplayStatics::SpawnObject(UFortAthenaAIBotCustomizationData::StaticClass(), GameMode);
		BotCustomizationData->SetBehaviorTree(PhoebeBehaviorTree);
		BotCustomizationData->SetPawnClass(PhoebePawnClass);
		BotCustomizationData->SetStartupInventory((UFortAthenaAIBotInventoryItems*)UGameplayStatics::SpawnObject(UFortAthenaAIBotInventoryItems::StaticClass(), GameMode));
		BotCustomizationData->SetCharacterCustomization((UFortAthenaAIBotCharacterCustomization*)UGameplayStatics::SpawnObject(UFortAthenaAIBotCharacterCustomization::StaticClass(), GameMode));

		FFortAthenaAIBotRunTimeCustomizationData BotRunTimeCustomizationData;

		AActor* BotSpawn = PlayerStarts[rand() % (PlayerStarts.Num() - 1)];
		GameMode->GetServerBotManager()->SpawnBot(GameMode->GetServerBotManager(), BotSpawn->GetActorLocation(), {}, BotCustomizationData, BotRunTimeCustomizationData);
	}
}
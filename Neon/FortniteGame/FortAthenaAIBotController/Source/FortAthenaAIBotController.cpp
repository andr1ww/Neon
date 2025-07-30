#include "pch.h"
#include "../Header/FortAthenaAIBotController.h"

#include "Engine/GameplayStatics/Header/GameplayStatics.h"
#include "Engine/NetDriver/Header/NetDriver.h"

void AFortAthenaAIBotController::SpawnPlayerBot(int Count) {
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
		
		GameMode->GetServerBotManager()->GetCachedBotMutator()->SpawnBot(BotBP, BotSpawn, BotSpawn->GetActorLocation(), {}, false);
	}
}
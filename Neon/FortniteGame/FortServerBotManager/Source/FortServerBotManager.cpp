#include "pch.h"
#include "../Header/FortServerBotManager.h"

#include "Engine/GameplayStatics/Header/GameplayStatics.h"

AFortPlayerPawn* UFortServerBotManagerAthena::SpawnBot(UFortServerBotManagerAthena* BotManager, FVector SpawnLoc, FRotator SpawnRot, UFortAthenaAIBotCustomizationData* BotData, FFortAthenaAIBotRunTimeCustomizationData& RuntimeBotData)
{
    if (BotData->GetFName().ToString().ToString().contains("MANG_POI_Yacht"))
    {
        BotData = Runtime::StaticLoadObject<UFortAthenaAIBotCustomizationData>("/Game/Athena/AI/MANG/BotData/BotData_MANG_POI_HDP.BotData_MANG_POI_HDP");
    }

    if (BotData->GetCharacterCustomization()->GetCustomizationLoadout().GetCharacter()->GetFName().ToString().ToString() == "CID_556_Athena_Commando_F_RebirthDefaultA")
    {
        BotData->GetCharacterCustomization()->GetCustomizationLoadout().SetCharacter(Runtime::StaticLoadObject<UAthenaCharacterItemDefinition>("/Game/Athena/Items/Cosmetics/Characters/CID_NPC_Athena_Commando_M_HenchmanGood.CID_NPC_Athena_Commando_M_HenchmanGood"));
    }

    AActor* SpawnLocator = UGameplayStatics::SpawnActor<ADefaultPawn>(SpawnLoc, SpawnRot);
    AFortPlayerPawn* Ret = BotMutator->SpawnBot(BotData->GetPawnClass(), SpawnLocator, SpawnLoc, SpawnRot, true);

    return Ret;
}

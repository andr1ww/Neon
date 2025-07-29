#include "pch.h"
#include "../Header/FortServerBotManager.h"

#include "Engine/GameplayStatics/Header/GameplayStatics.h"
#include "Engine/Kismet/Header/Kismet.h"

AFortPlayerPawn* UFortServerBotManagerAthena::SpawnBot(UFortServerBotManagerAthena* BotManager, FVector SpawnLoc, FRotator SpawnRot, UFortAthenaAIBotCustomizationData* BotData, FFortAthenaAIBotRunTimeCustomizationData& RuntimeBotData)
{
    if (Fortnite_Version.GetMajorVersion() == 12 && Fortnite_Version >= 12.41) {
        if (BotData->GetFName().ToString().ToString().contains("MANG_POI_Yacht"))
        {
            BotData = Runtime::StaticLoadObject<UFortAthenaAIBotCustomizationData>("/Game/Athena/AI/MANG/BotData/BotData_MANG_POI_HDP.BotData_MANG_POI_HDP");
        }
    }

    if (BotData->GetCharacterCustomization()->GetCustomizationLoadout().GetCharacter()->GetFName().ToString().ToString() == "CID_556_Athena_Commando_F_RebirthDefaultA")
    {
        std::string Tag = RuntimeBotData.PredefinedCosmeticSetTag.TagName.ToString().ToString();
        if (Tag == "Athena.Faction.Alter") {
            BotData->GetCharacterCustomization()->GetCustomizationLoadout().SetCharacter(Runtime::StaticLoadObject<UAthenaCharacterItemDefinition>("/Game/Athena/Items/Cosmetics/Characters/CID_NPC_Athena_Commando_M_HenchmanBad.CID_NPC_Athena_Commando_M_HenchmanBad"));
        }
        else if (Tag == "Athena.Faction.Ego") {
            BotData->GetCharacterCustomization()->GetCustomizationLoadout().SetCharacter(Runtime::StaticLoadObject<UAthenaCharacterItemDefinition>("/Game/Athena/Items/Cosmetics/Characters/CID_NPC_Athena_Commando_M_HenchmanGood.CID_NPC_Athena_Commando_M_HenchmanGood"));
        }
    }

    AFortPlayerPawn* Ret = SpawnBotOG(BotManager, SpawnLoc, SpawnRot, BotData, RuntimeBotData);
    if (Ret)
    {
        AFortAthenaAIBotController* Controller = (AFortAthenaAIBotController*)Ret->GetController();

        Controller->Set("FortAthenaAIBotController", "CosmeticLoadoutBC",BotData->GetCharacterCustomization()->GetCustomizationLoadout());
        for (int32 i = 0; i < BotData->GetCharacterCustomization()->GetCustomizationLoadout().GetCharacter()->GetHeroDefinition()->GetSpecializations().Num(); i++)
        {
            UFortHeroSpecialization* Spec = Runtime::StaticLoadObject<UFortHeroSpecialization>(UKismetStringLibrary::GetDefaultObj()->CallFunc<FString>("KismetStringLibrary", "Conv_NameToString", BotData->GetCharacterCustomization()->GetCustomizationLoadout().GetCharacter()->GetHeroDefinition()->GetSpecializations()[i].SoftObjectPtr.ObjectID.AssetPathName).ToString());

            if (Spec)
            {
                for (int32 i = 0; i < Spec->GetCharacterParts().Num(); i++)
                {
                    UCustomCharacterPart* Part = Runtime::StaticLoadObject<UCustomCharacterPart>(UKismetStringLibrary::GetDefaultObj()->CallFunc<FString>("KismetStringLibrary", "Conv_NameToString", Spec->GetCharacterParts()[i].SoftObjectPtr.ObjectID.AssetPathName).ToString());
                    Ret->CallFunc<void>("FortPlayerPawn", "ServerChoosePart", Part->GetCharacterPartType(), Part);
                }
            }
        }
        Ret->SetCosmeticLoadout(&BotData->GetCharacterCustomization()->GetCustomizationLoadout());
        Ret->CallFunc<void>("FortPlayerPawn", "OnRep_CosmeticLoadout");
    
        if (!Controller->GetInventory()) {
            Controller->SetInventory(UGameplayStatics::SpawnActor<AFortInventory>({}, {}, Ret));
        }

        for (int32 i = 0; i < BotData->GetStartupInventory()->GetItems().Num(); i++) {
            
        }

        return Ret;
    }
}

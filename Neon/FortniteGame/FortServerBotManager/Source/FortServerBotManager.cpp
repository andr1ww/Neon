#include "pch.h"
#include "../Header/FortServerBotManager.h"

#include "Engine/GameplayStatics/Header/GameplayStatics.h"
#include "Engine/Kismet/Header/Kismet.h"

AFortPlayerPawn* UFortServerBotManagerAthena::SpawnBot(UFortServerBotManagerAthena* BotManager, FVector SpawnLoc, FRotator SpawnRot, UFortAthenaAIBotCustomizationData* BotData, FFortAthenaAIBotRunTimeCustomizationData& RuntimeBotData)
{
    static TArray<UAthenaCharacterItemDefinition*> Characters = TArray<UAthenaCharacterItemDefinition*>();
    static TArray<UAthenaPickaxeItemDefinition*> Pickaxes = TArray<UAthenaPickaxeItemDefinition*>();
    UAthenaCharacterItemDefinition* RandomCharacter = nullptr;
    UAthenaPickaxeItemDefinition* RandomPickaxe = nullptr;
    if (Characters.Num() == 0)
    {
        Characters = Runtime::GetObjectsOfClass<UAthenaCharacterItemDefinition>();
        Pickaxes = Runtime::GetObjectsOfClass<UAthenaPickaxeItemDefinition>();
    }
    if (Characters.Num() > 0 && Pickaxes.Num() > 0)
    {
        int32 RandomCharacterIndex = rand() % Characters.Num() - 1;
        RandomCharacter = Characters[RandomCharacterIndex];
        int32 RandomPickaxeIndex = rand() % Pickaxes.Num() - 1;
        RandomPickaxe = Pickaxes[RandomPickaxeIndex];
    }

    static void (*BotManagerSetup)(__int64 BotManaager, __int64 Pawn, __int64 BehaviorTree, __int64 a4, DWORD* SkillLevel, __int64 idk, __int64 StartupInventory, __int64 BotNameSettings, __int64 idk_1, BYTE* CanRespawnOnDeath, unsigned __int8 BitFieldDataThing, BYTE* CustomSquadId, FFortAthenaAIBotRunTimeCustomizationData InRuntimeBotData) = decltype(BotManagerSetup)(Finder->BotManagerSetup());
    
    AFortPlayerPawn* Ret = SpawnBotOG(BotManager, SpawnLoc, SpawnRot, BotData, RuntimeBotData);
    if (Ret)
    {
        AFortAthenaAIBotController* Controller = (AFortAthenaAIBotController*)Ret->GetController();

        if (BotData->GetCharacterCustomization()) {
            if (&BotData->GetCharacterCustomization()->GetCustomizationLoadout()) {
                if (Fortnite_Version.GetMajorVersion() == 12 && Fortnite_Version >= 12.41) {
                    if (BotData->GetFName().ToString().ToString().contains("MANG_POI_Yacht"))
                    {
                        BotData = Runtime::StaticLoadObject<UFortAthenaAIBotCustomizationData>("/Game/Athena/AI/MANG/BotData/BotData_MANG_POI_HDP.BotData_MANG_POI_HDP");
                    }
                }

                if (BotData->GetCharacterCustomization()->GetCustomizationLoadout().GetCharacter()->GetFName().ToString().ToString() == "CID_556_Athena_Commando_F_RebirthDefaultA")
                {
                    if (!Controller->GetInventory()) {
                        Controller->SetInventory(UGameplayStatics::SpawnActor<AFortInventory>({}, {}, Ret));
                    }

                    if (BotData->GetStartupInventory())
                    {
                        for (int32 i = 0; i < BotData->GetStartupInventory()->GetItems().Num(); i++) {
                            UFortItemDefinition* ItemDef = BotData->GetStartupInventory()->GetItems()[i];
                            if (!ItemDef) continue;
                            UFortWorldItem* Item = (UFortWorldItem*)AFortInventory::GiveItem(Controller, ItemDef, 1, 30, 0);
                            if (ItemDef->IsA(UFortWeaponItemDefinition::StaticClass())) {
                                Ret->CallFunc<void>("FortPawn", "EquipWeaponDefinition", Item->GetItemEntry().GetItemDefinition(), Item->GetItemEntry(), false);
                            }
                        }
                    }
                    
                    std::string Tag = RuntimeBotData.PredefinedCosmeticSetTag.TagName.ToString().ToString();
                    if (Tag == "Athena.Faction.Alter") {
                        BotData->GetCharacterCustomization()->GetCustomizationLoadout().SetCharacter(Runtime::StaticLoadObject<UAthenaCharacterItemDefinition>("/Game/Athena/Items/Cosmetics/Characters/CID_NPC_Athena_Commando_M_HenchmanBad.CID_NPC_Athena_Commando_M_HenchmanBad"));
                    }
                    else if (Tag == "Athena.Faction.Ego") {
                        BotData->GetCharacterCustomization()->GetCustomizationLoadout().SetCharacter(Runtime::StaticLoadObject<UAthenaCharacterItemDefinition>("/Game/Athena/Items/Cosmetics/Characters/CID_NPC_Athena_Commando_M_HenchmanGood.CID_NPC_Athena_Commando_M_HenchmanGood"));
                    }

                    DWORD CustomSquadId = RuntimeBotData.CustomSquadId;
                    BYTE TrueByte = 1;
                    BYTE FalseByte = 0;
                    BotManagerSetup(__int64(BotManager), __int64(Ret), __int64(BotData->GetBehaviorTree()), 0, &CustomSquadId, 0, __int64(BotData->GetStartupInventory()), __int64(BotData->GetBotNameSettings()), 0, &FalseByte, 0, &TrueByte, RuntimeBotData);
                } else
                {
                        
                        if (RandomCharacter) {
                  //          BotData->GetCharacterCustomization()->GetCustomizationLoadout().SetCharacter(RandomCharacter);
                        }
                        
                        if (RandomPickaxe) {
                            BotData->GetCharacterCustomization()->GetCustomizationLoadout().SetPickaxe(RandomPickaxe);
                        }
                    
                    if (!Controller->GetInventory()) {
                        Controller->SetInventory(UGameplayStatics::SpawnActor<AFortInventory>({}, {}, Ret));
                    }

                    auto& StartingItemsArray = BotManager->GetCachedGameMode()->GetStartingItems();
                    int32 FItemAndCountSize = StaticClassImpl("ItemAndCount")->GetSize();
                    for (int i = 0; i < StartingItemsArray.Num(); i++)
                    {
                        auto Item = (FItemAndCount*) ((uint8*) StartingItemsArray.GetData() + (i * FItemAndCountSize));
        
                        if (!Item) {
                            UE_LOG(LogNeon, Fatal, "StartingItem is null in SpawnDefaultPawnFor!");
                            return Ret;
                        }
    
                        AFortInventory::GiveItem(Controller, Item->GetItem(), Item->GetCount(), 1, 1);
                    }

             //       AFortInventory::GiveItem(Controller, RandomPickaxe->GetWeaponDefinition(), 1, 0, 1); 
                }

                Controller->Set("FortAthenaAIBotController", "CosmeticLoadoutBC", BotData->GetCharacterCustomization()->GetCustomizationLoadout());
                if (BotData->GetCharacterCustomization()->GetCustomizationLoadout().GetCharacter()->GetHeroDefinition())
                {
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
                }
                
                Ret->SetCosmeticLoadout(&BotData->GetCharacterCustomization()->GetCustomizationLoadout());
                Ret->CallFunc<void>("FortPlayerPawn", "OnRep_CosmeticLoadout");
            }
        }

        return Ret;
    }
}

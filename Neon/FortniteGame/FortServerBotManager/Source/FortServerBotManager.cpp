#include "pch.h"
#include "../Header/FortServerBotManager.h"
#include "Engine/GameplayStatics/Header/GameplayStatics.h"
#include "Engine/Kismet/Header/Kismet.h"

void StartTree(UBehaviorTreeComponent* BTComp, UBehaviorTree* BTAsset, EBTExecutionMode::Type Mode = EBTExecutionMode::Looped)
{
    if (!BTComp || !BTAsset)
        return;

    BTComp->SetDefaultBehaviorTreeAsset(BTAsset);

    BTComp->StartLogic();
}

bool RunBehaviorTree(AFortAthenaAIBotController* PC, UBehaviorTree* BTAsset)
{
    if (!BTAsset || !PC) {
        UE_LOG(LogNeon, Error, "RunBehaviorTree: BTAsset or PC is null!");
		return false;
    }

    bool bSuccess = true;

    UBlackboardComponent* BlackboardComp = PC->GetBlackboard();
    if (BTAsset->GetBlackboardAsset() && !BlackboardComp)
    {
       bSuccess = PC->UseBlackboard(BTAsset->GetBlackboardAsset(), &BlackboardComp);
       if (bSuccess) {
           PC->OnUsingBlackBoard(BlackboardComp, BTAsset->GetBlackboardAsset());
       }
    } 

    if (bSuccess)
    {
        UBehaviorTreeComponent* BTComp = (UBehaviorTreeComponent*)PC->GetBrainComponent();
        if (!BTComp)
        {
            BTComp = (UBehaviorTreeComponent*)UGameplayStatics::SpawnObject(UBehaviorTreeComponent::StaticClass(), PC);
            ((void (*)(UObject * Component, UObject * World))(Finder->RegisterComponentWithWorld()))(BTComp, UWorld::GetWorld());
        }
        PC->SetBrainComponent(BTComp);
        StartTree(BTComp, BTAsset, EBTExecutionMode::Looped);
    } else
    {
        UE_LOG(LogNeon, Log, "Fuck this ");
    }

    return bSuccess;
}

AFortPlayerPawn* UFortServerBotManagerAthena::SpawnBot(UFortServerBotManagerAthena* BotManager, FVector SpawnLoc, FRotator SpawnRot, UFortAthenaAIBotCustomizationData* BotData, FFortAthenaAIBotRunTimeCustomizationData& RuntimeBotData)
{
    if (Fortnite_Version == 13.40 || Fortnite_Version == 12.41)
    {
        if (__int64(_ReturnAddress()) == Finder->SpawnBotRet())
            return SpawnBotOG(BotManager, SpawnLoc, SpawnRot, BotData, RuntimeBotData);
    } else if (__int64(_ReturnAddress()) - IMAGEBASE == 0x1aaa8df)
    {
        return SpawnBotOG(BotManager, SpawnLoc, SpawnRot, BotData, RuntimeBotData);
    }
    
    static void (*BotManagerSetup)(__int64 BotManaager, __int64 Pawn, __int64 BehaviorTree, __int64 a4, DWORD* SkillLevel, __int64 idk, __int64 StartupInventory, __int64 BotNameSettings, __int64 idk_1, BYTE* CanRespawnOnDeath, unsigned __int8 BitFieldDataThing, BYTE* CustomSquadId, FFortAthenaAIBotRunTimeCustomizationData InRuntimeBotData) = decltype(BotManagerSetup)(Finder->BotManagerSetup());
   
    AActor *SpawnLocator = UGameplayStatics::SpawnActorOG<ADefaultPawn>(ADefaultPawn::StaticClass(), SpawnLoc, SpawnRot);
    AFortPlayerPawn* Ret = BotManager->GetCachedBotMutator()->SpawnBot(BotData->GetPawnClass(), SpawnLocator, SpawnLoc, SpawnRot, true);

 //   Ret->CallFunc<void>("Actor", "K2_TeleportTo", SpawnLoc, SpawnRot);
    
    if (Ret)
    {
        AFortAthenaAIBotController* Controller = (AFortAthenaAIBotController*)Ret->GetController();

        if (BotData->GetStartupInventory())
        {
            FBotInventory::SetInventory(Controller, BotData->GetStartupInventory());
        }

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
                    std::string Tag = RuntimeBotData.PredefinedCosmeticSetTag.TagName.ToString().ToString();
                    if (Tag == "Athena.Faction.Alter") {
                        BotData->GetCharacterCustomization()->GetCustomizationLoadout().SetCharacter(Runtime::StaticLoadObject<UAthenaCharacterItemDefinition>("/Game/Athena/Items/Cosmetics/Characters/CID_NPC_Athena_Commando_M_HenchmanBad.CID_NPC_Athena_Commando_M_HenchmanBad"));
                    }
                    else if (Tag == "Athena.Faction.Ego") {
                        BotData->GetCharacterCustomization()->GetCustomizationLoadout().SetCharacter(Runtime::StaticLoadObject<UAthenaCharacterItemDefinition>("/Game/Athena/Items/Cosmetics/Characters/CID_NPC_Athena_Commando_M_HenchmanGood.CID_NPC_Athena_Commando_M_HenchmanGood"));
                    }
                } else
                {
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

        if (BotData->GetFName().ToString().ToString().contains("MANG"))
        {
            DWORD CustomSquadId = RuntimeBotData.CustomSquadId;
            BYTE TrueByte = 1;
            BYTE FalseByte = 0;
            BotManagerSetup(__int64(BotManager), __int64(Ret), __int64(BotData->GetBehaviorTree()), 0, &CustomSquadId, 0, __int64(BotData->GetStartupInventory()), __int64(BotData->GetBotNameSettings()), 0, &FalseByte, 0, &TrueByte, RuntimeBotData);
            if (!Controller->GetInventory()) {
                Controller->SetInventory(UGameplayStatics::SpawnActorOG<AFortInventory>(AFortInventory::StaticClass(), {}, {}, Ret));
            }

            if (BotData->GetStartupInventory())
            {
                for (int32 i = 0; i < BotData->GetStartupInventory()->GetItems().Num(); i++) {
                    UFortItemDefinition* ItemDef = BotData->GetStartupInventory()->GetItems()[i];
                    if (!ItemDef) continue;
                    UFortWorldItem* Item = (UFortWorldItem*)AFortInventory::GiveItem(Controller, ItemDef, 1, 30, 0);
                    if (ItemDef->IsA(UFortWeaponItemDefinition::StaticClass()) && ((UFortWorldItemDefinition*)ItemDef)->GetbCanBeDropped()) {
                        Ret->CallFunc<void>("FortPawn", "EquipWeaponDefinition", Item->GetItemEntry().GetItemDefinition(), Item->GetItemEntry(), false);
                    }
                }
            }
        }

        Controller->GetPathFollowingComponent()->SetMyNavData(UWorld::GetWorld()->GetNavigationSystem()->GetMainNavData());
        Controller->GetPathFollowingComponent()->CallFunc<void>("PathFollowingComponent", "OnNavDataRegistered", UWorld::GetWorld()->GetNavigationSystem()->GetMainNavData());
        Controller->GetPathFollowingComponent()->CallFunc<void>("ActorComponent", "Activate", true);
        Controller->GetPathFollowingComponent()->CallFunc<void>("ActorComponent", "SetActivate", true, true);
        Controller->GetPathFollowingComponent()->CallFunc<void>("ActorComponent", "OnRep_IsActive");
        UBlackboardComponent* Blackboard = Controller->GetBlackboard();
        Controller->UseBlackboard(Controller->GetBehaviorTree()->GetBlackboardAsset(), &Blackboard);
        Controller->OnUsingBlackBoard(Blackboard, Controller->GetBehaviorTree()->GetBlackboardAsset());
        
        bool bRanBehaviorTree = false;
        if (BotData->GetBehaviorTree()) {
			Controller->SetBehaviorTree(BotData->GetBehaviorTree());
            if (RunBehaviorTree(Controller, BotData->GetBehaviorTree())) {
                Controller->BlueprintOnBehaviorTreeStarted();
				bRanBehaviorTree = true;

                Controller->GetBlackboard()->SetValueAsEnum(UKismetStringLibrary::Conv_StringToName(L"AIEvaluator_Global_GamePhaseStep"), 6);
                Controller->GetBlackboard()->SetValueAsEnum(UKismetStringLibrary::Conv_StringToName(L"AIEvaluator_Global_GamePhase"), (uint8)EAthenaGamePhase::SafeZones);
                Controller->GetBlackboard()->SetValueAsBool(UKismetStringLibrary::Conv_StringToName(L"AIEvaluator_Patrolling_ShouldMove"), true);

                Controller->GetBlackboard()->SetValueAsBool(UKismetStringLibrary::Conv_StringToName(L"AIEvaluator_Global_IsMovementBlocked"), false);
                Controller->GetBlackboard()->SetValueAsEnum(UKismetStringLibrary::Conv_StringToName(L"AIEvaluator_AvoidThreat_ExecutionStatus"), (uint8)EExecutionStatus::ExecutionAllowed);
                Controller->GetBlackboard()->SetValueAsEnum(UKismetStringLibrary::Conv_StringToName(L"AIEvaluator_Leash_ExecutionStatus"), (uint8)EExecutionStatus::ExecutionAllowed);
                Controller->GetBlackboard()->SetValueAsEnum(UKismetStringLibrary::Conv_StringToName(L"AIEvaluator_Patrolling_ExecutionStatus"), (uint8)EExecutionStatus::ExecutionAllowed);
                Controller->GetBlackboard()->SetValueAsEnum(UKismetStringLibrary::Conv_StringToName(L"AIEvaluator_DynamicBlueprint_ExecutionStatus"), (uint8)EExecutionStatus::ExecutionAllowed);
                Controller->GetBlackboard()->SetValueAsEnum(UKismetStringLibrary::Conv_StringToName(L"AIEvaluator_CharacterLaunched_ExecutionStatus"), (uint8)EExecutionStatus::ExecutionAllowed);

                Controller->GetBrainComponent()->RestartLogic();
            }
            else {
				UE_LOG(LogNeon, Warning, "Bot %s Failed to RunBehaviorTree %s!", Ret->GetFName().ToString().ToString().c_str(), BotData->GetBehaviorTree()->GetFName().ToString().ToString().c_str());
				RunBehaviorTree(Controller, BotData->GetBehaviorTree());
            }
        }

        if (!bRanBehaviorTree) {
            // start manual tickingb
        }
        
        SpawnLocator->CallFunc<void>("Actor", "K2_DestroyActor");
        
        return Ret;
    }
}

void UFortServerBotManagerAthena::InitializeForWorld(UNavigationSystemV1* NavSystem, UWorld World, uint8 Mode)
{
    UE_LOG(LogNeon, Log, "Fuck");
    NavSystem->SetbAutoCreateNavigationData(true);
    return InitializeForWorldOG(NavSystem, World, Mode);
}

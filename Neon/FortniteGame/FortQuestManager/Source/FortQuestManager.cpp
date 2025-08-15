#include "pch.h"
#include "../Header/FortQuestManager.h"

#include "Engine/GameplayStatics/Header/GameplayStatics.h"
#include "FortniteGame/FortPlayerController/Header/FortPlayerController.h"

void UFortQuestManager::SendComplexCustomStatEvent(UFortQuestManager* QuestManager, UObject* TargetObj, FGameplayTagContainer& SourceTags, FGameplayTagContainer& TargetTags, bool* QuestActive, bool* QuestCompleted, int32 Count)
{
	SendStatEvent(QuestManager, TargetObj, SourceTags, TargetTags, nullptr, nullptr, 1, EFortQuestObjectiveStatEvent::ComplexCustom);
    return SendComplexCustomStatEventOG(QuestManager, TargetObj, SourceTags, TargetTags, QuestActive, QuestCompleted, Count);
}

void SendObjectiveStat(AFortPlayerControllerAthena* PlayerController, const FName& BackendName, UFortQuestItemDefinition* QuestDefinition, int32 Count)
{
	if (!PlayerController) {
		return;
	}

	for (auto& UpdatedObjectiveStat : PlayerController->GetUpdatedObjectiveStats())
	{
		if (UpdatedObjectiveStat.BackendName.GetComparisonIndex() == BackendName.GetComparisonIndex())
		{
			UpdatedObjectiveStat.Quest = QuestDefinition;
			UpdatedObjectiveStat.CurrentStage++;
			UpdatedObjectiveStat.StatDelta = Count;
			UpdatedObjectiveStat.StatValue = QuestDefinition->GetObjectiveCompletionCount();
			PlayerController->CallFunc<void>("FortPlayerController","OnRep_UpdatedObjectiveStats" );
			return;
		}
	}
	
	static FFortUpdatedObjectiveStat* NewUpdatedObjectiveStat = nullptr;
	static int FFortUpdatedObjectiveStatSize = 0;
	if (!NewUpdatedObjectiveStat)
	{
		FFortUpdatedObjectiveStatSize = StaticClassImpl("FortUpdatedObjectiveStat")->GetSize();
		NewUpdatedObjectiveStat = (FFortUpdatedObjectiveStat*)malloc(FFortUpdatedObjectiveStatSize);
		memset(NewUpdatedObjectiveStat, 0, sizeof(FFortUpdatedObjectiveStat));
	}

	if (NewUpdatedObjectiveStat)
	{
		NewUpdatedObjectiveStat->BackendName = BackendName;
		NewUpdatedObjectiveStat->Quest = QuestDefinition;
		NewUpdatedObjectiveStat->CurrentStage++;
		NewUpdatedObjectiveStat->StatDelta = Count;
		NewUpdatedObjectiveStat->StatValue = QuestDefinition->GetObjectiveCompletionCount();

		PlayerController->GetUpdatedObjectiveStats().Add(*NewUpdatedObjectiveStat, FFortUpdatedObjectiveStatSize);
	}

	PlayerController->CallFunc<void>("FortPlayerController","OnRep_UpdatedObjectiveStats" );
}

static void ProgressQuest(AFortPlayerControllerAthena* PlayerController, UFortQuestManager* QuestManager, UFortQuestItem* QuestItem, UFortQuestItemDefinition* QuestDefinition, FFortMcpQuestObjectiveInfo* Obj, int32 PlayerControllerount)
{
	static std::unordered_map<AFortPlayerControllerAthena*, std::vector<FFortMcpQuestObjectiveInfo>> ObjCompArray;
	auto Count = QuestManager->GetObjectiveCompletionCount(QuestDefinition, Obj->GetBackendName());
	
	Count++;
	
	QuestDefinition->SetObjectiveCompletionCount(Count); 
	
	bool thisObjectiveCompleted = (Count >= Obj->GetCount());
	bool allObjsCompleted = false;
	if (thisObjectiveCompleted && QuestDefinition->GetObjectives().Num() == 1)
	{
		allObjsCompleted = true;
	}
	
	if (thisObjectiveCompleted && QuestDefinition->GetObjectives().Num() > 1)
	{
		bool alreadyExists = false;
		for (auto& ObjComp : ObjCompArray[PlayerController])
		{
			if (ObjComp.GetBackendName().GetComparisonIndex() == Obj->GetBackendName().GetComparisonIndex())
			{
				alreadyExists = true;
				break;
			}
		}
		if (!alreadyExists)
		{
			ObjCompArray[PlayerController].push_back(*Obj);
		}
		
		int32 totalObjectives = QuestDefinition->GetObjectives().Num();
		
		auto CompletionCount = 0;
		for (auto& QuestObj : QuestDefinition->GetObjectives())
		{
			bool Found = false;
			for (auto& ObjComp : ObjCompArray[PlayerController])
			{
			//	if (QuestObj.GetBackendName().ToString().ToString() == ObjComp.GetBackendName().ToString().ToString())
			//	{
			//		Found = true;
			//		break;
			//	}
			}
			if (Found)
			{
				CompletionCount++;
			}
		}
		
		allObjsCompleted = (CompletionCount == totalObjectives);
	}
	
	auto PlayerState = PlayerController->GetPlayerState();
	if (PlayerState)
	{
		for (const auto& TeamMember : PlayerState->GetPlayerTeam()->GetTeamMembers())
		{
			auto TeamMemberPlayerController = (AFortPlayerControllerAthena*)TeamMember;
			if (TeamMemberPlayerController->IsA<AFortAthenaAIBotController>())
				continue;

			SendObjectiveStat(TeamMemberPlayerController, Obj->GetBackendName(), QuestDefinition, Count);

			if (TeamMemberPlayerController == PlayerController)
			{
				int32 DeltaChange = Count; 
				QuestManager->CallFunc<void>("FortQuestManager", "SelfCompletedUpdatedQuest", TeamMemberPlayerController, QuestDefinition, Obj->GetBackendName(), Count,
					DeltaChange, PlayerState, thisObjectiveCompleted, allObjsCompleted);
			}
			else
			{
				int32 DeltaChange = Count; 
				QuestManager->CallFunc<void>("FortQuestManager", "AssistedPlayerUpdatedQuest", TeamMemberPlayerController, QuestDefinition, Obj->GetBackendName(), Count,
					DeltaChange, PlayerState, thisObjectiveCompleted, allObjsCompleted);
			}
			
			QuestManager->CallFunc<void>("FortQuestManager", "HandleQuestUpdated", 
				TeamMemberPlayerController, 
				QuestDefinition, 
				Obj->GetBackendName(), 
				Count, 
				1,
				TeamMemberPlayerController == PlayerController ? nullptr : PlayerState, 
				thisObjectiveCompleted, 
				allObjsCompleted);
		}
	}
	
	if (allObjsCompleted) 
	{
		ObjCompArray[PlayerController].clear();
		
		int32 XPPlayerControllerCount = 0;
		if (auto RewardsTable = QuestDefinition->GetRewardsTable())
		{
			static auto Name = UKismetStringLibrary::Conv_StringToName(L"Default");
			auto& RowMap = RewardsTable->GetRowMap();
			
			for (const auto& [RowName, RowPtr] : RowMap)
			{
				if (RowName.GetComparisonIndex() == Name.GetComparisonIndex())
				{
					auto DefaultRow = (FFortQuestRewardTableRow*)RowPtr;
					XPPlayerControllerCount = DefaultRow->GetQuantity();
					UE_LOG(LogNeon, Log, "XPPlayerControllerCount: %d", XPPlayerControllerCount);
					break;
				}
			}
		}
		
		if (XPPlayerControllerCount) 
		{
			UE_LOG(LogNeon, Log, "Granting XP");
			FXPEventEntry QuestEntry{};
			
			QuestEntry.EventXpValue = XPPlayerControllerCount;
			QuestEntry.QuestDef = QuestDefinition;
			QuestEntry.Time = UGameplayStatics::GetTimeSeconds(UWorld::GetWorld());
			QuestEntry.TotalXpEarnedInMatch = PlayerController->GetXPComponent()->Get<int32>("FortPlayerControllerAthenaXPComponent", "TotalXpEarned") + XPPlayerControllerCount;
			QuestEntry.SimulatedXpEvent = UKismetStringLibrary::Conv_StringToText(FString(L"Objective completed"));
			
			int32 CurrentChallengeXp = PlayerController->GetXPComponent()->Get<int32>("FortPlayerControllerAthenaXPComponent", "ChallengeXp");
			int32 CurrentTotalXp = PlayerController->GetXPComponent()->Get<int32>("FortPlayerControllerAthenaXPComponent", "TotalXpEarned");
			
			PlayerController->GetXPComponent()->Set("FortPlayerControllerAthenaXPComponent", "ChallengeXp", CurrentChallengeXp + XPPlayerControllerCount);
			PlayerController->GetXPComponent()->Set("FortPlayerControllerAthenaXPComponent", "TotalXpEarned", CurrentTotalXp + XPPlayerControllerCount);
			
			PlayerController->GetXPComponent()->CallFunc<void>("FortPlayerControllerAthenaXPComponent", "OnXpUpdated", 
				PlayerController->GetXPComponent()->Get<int32>("FortPlayerControllerAthenaXPComponent", "CombatXp"),
				PlayerController->GetXPComponent()->Get<int32>("FortPlayerControllerAthenaXPComponent", "SurvivalXp"),
				PlayerController->GetXPComponent()->Get<int32>("FortPlayerControllerAthenaXPComponent", "MedalBonusXP"),
				PlayerController->GetXPComponent()->Get<int32>("FortPlayerControllerAthenaXPComponent", "ChallengeXp"),
				PlayerController->GetXPComponent()->Get<int32>("FortPlayerControllerAthenaXPComponent", "MatchXp"),
				PlayerController->GetXPComponent()->Get<int32>("FortPlayerControllerAthenaXPComponent", "TotalXpEarned"));
				
			int64 CurrentProfileVer = PlayerController->GetXPComponent()->Get<int64>("FortPlayerControllerAthenaXPComponent", "InMatchProfileVer");
			PlayerController->GetXPComponent()->Set("FortPlayerControllerAthenaXPComponent", "InMatchProfileVer", CurrentProfileVer + 1);
			
			PlayerController->GetXPComponent()->CallFunc<void>("FortPlayerControllerAthenaXPComponent", "OnInMatchProfileUpdate", CurrentProfileVer + 1);
			PlayerController->GetXPComponent()->CallFunc<void>("FortPlayerControllerAthenaXPComponent", "OnProfileUpdated");
			PlayerController->GetXPComponent()->GetWaitingQuestXp().Add(QuestEntry);
	//		PlayerController->GetXPComponent()->CallFunc<void>("FortPlayerControllerAthenaXPComponent", "HighPrioXPEvent", QuestEntry);
		}
		
	//	QuestManager->CallFunc<void>("FortQuestManager", "ClaimQuestReward", QuestItem);
	} 
}

void UFortQuestManager::SendStatEvent(UFortQuestManager* QuestManager, UObject* TargetObj, FGameplayTagContainer& SourceTags, FGameplayTagContainer& TargetTags, bool* QuestActive, bool* QuestCompleted, int32 Count, EFortQuestObjectiveStatEvent StatEvent)
{
	auto Controller = (AFortPlayerControllerAthena*)QuestManager->GetPlayerControllerBP();

	if (!Controller) return;

	FGameplayTagContainer PSourceTags;
	FGameplayTagContainer ContextTags;
	
	QuestManager->GetSourceAndContextTags(&PSourceTags, &ContextTags);

	static int CurrentPlaylistInfoOffset = Runtime::GetOffset(UWorld::GetWorld()->GetGameState(), "CurrentPlaylistInfo");
            
	FPlaylistPropertyArray& CurrentPlaylistInfoPtr = *reinterpret_cast<FPlaylistPropertyArray*>(__int64(UWorld::GetWorld()->GetGameState()) + CurrentPlaylistInfoOffset);
	
	ContextTags.AppendTags(CurrentPlaylistInfoPtr.GetBasePlaylist()->GetGameplayTagContainer());
	SourceTags.AppendTags(PSourceTags);

	auto& CurrentQuests = QuestManager->GetCurrentQuests();
	for (int i = 0; i < CurrentQuests.Num(); i++)
	{
		if (!CurrentQuests.IsValidIndex(i) || !CurrentQuests[i])
			continue;
			
		auto CurrentQuest = CurrentQuests[i];
		
		if (CurrentQuest->HasCompletedQuest())
			continue;

		auto QuestDef = CurrentQuest->GetQuestDefinitionBP();

		if (!QuestDef)
			continue;

		if (QuestManager->HasCompletedQuest(QuestDef))
			continue;
				
		auto& Objectives = QuestDef->GetObjectives();
		static int32 FFortMcpQuestObjectiveInfoSize = StaticClassImpl("FortMcpQuestObjectiveInfo")->GetSize();
		for (int j = 0; j < Objectives.Num(); j++)
		{
			if (!Objectives.IsValidIndex(j))
				continue;
				
			FFortMcpQuestObjectiveInfo* Objective = (FFortMcpQuestObjectiveInfo*)((uint8*)Objectives.GetData() + (j * FFortMcpQuestObjectiveInfoSize));
			
			if (QuestManager->HasCompletedObjectiveWithName(QuestDef, Objective->GetBackendName()) ||
					QuestManager->HasCompletedObjective(QuestDef, Objective->GetObjectiveStatHandle()) ||
					CurrentQuest->HasCompletedObjectiveWithName(Objective->GetBackendName()) ||
					CurrentQuest->HasCompletedObjective(Objective->GetObjectiveStatHandle()))
			{
				continue;
			}
				
					
			auto StatTable = Objective->GetObjectiveStatHandle().DataTable;
			auto& StatRowName = Objective->GetObjectiveStatHandle().GetRowName();

			if (!StatTable || !StatRowName.IsValid())
			{
				auto& InlineStats = Objective->GetInlineObjectiveStats();
				static int32 FFortQuestObjectiveStatSize = StaticClassImpl("FortQuestObjectiveStat")->GetSize();
				for (int k = 0; k < InlineStats.Num(); k++)
				{
					if (!InlineStats.IsValidIndex(k))
						continue;
						
					FFortQuestObjectiveStat* ObjectiveStat = (FFortQuestObjectiveStat*)((uint8*)InlineStats.GetData() + (k * FFortQuestObjectiveStatSize));
					
					if (ObjectiveStat->GetType() != StatEvent)
						continue;
					bool bFoundQuest = true; 

					auto& TagConditions = ObjectiveStat->GetTagConditions();
					static int32 FInlineObjectiveStatTagCheckEntrySize = StaticClassImpl("InlineObjectiveStatTagCheckEntry")->GetSize();
					for (int l = 0; l < TagConditions.Num(); l++)
					{
						if (!TagConditions.IsValidIndex(l))
							continue;
							
						FInlineObjectiveStatTagCheckEntry* TagCondition = (FInlineObjectiveStatTagCheckEntry*)((uint8*)TagConditions.GetData() + (l * FInlineObjectiveStatTagCheckEntrySize));
						
						if (!TagCondition->GetRequire() || !bFoundQuest)
							continue;

						switch (TagCondition->GetType())
						{
						case EInlineObjectiveStatTagCheckEntryType::Target:
							{
								if (!ObjectiveStat->GetbHsaInclusiveTargetTags())
									break;

								if (!TargetTags.HasTag(TagCondition->GetTag()))
								{
									bFoundQuest = false;
								}
								
								break;
							}
						case EInlineObjectiveStatTagCheckEntryType::Source:
							{
								if (!ObjectiveStat->GetbHasInclusiveSourceTags())
									break;

								if (!SourceTags.HasTag(TagCondition->GetTag()))
								{
									bFoundQuest = false;
								}

								break;
							}
						case EInlineObjectiveStatTagCheckEntryType::Context:
							{
								if (!ObjectiveStat->GetbHasInclusiveContextTags())
									break;

								if (!ContextTags.HasTag(TagCondition->GetTag()))
								{
									bFoundQuest = false;
								}

								break;
							}
						case EInlineObjectiveStatTagCheckEntryType::EInlineObjectiveStatTagCheckEntryType_MAX:
							{
								break;
							}
						default:
							break;
						}
					}

					if (ObjectiveStat->GetType() != StatEvent)
					{
						bFoundQuest = false;
					}

					if (bFoundQuest)
					{
						ProgressQuest(Controller, QuestManager, CurrentQuest, QuestDef, Objective, Objective->GetCount() + 1);
						UE_LOG(LogNeon, Log, "BackendName: %s", Objective->GetBackendName().ToString().ToString().c_str());
					}
				}
			}
			else if (StatTable && StatRowName.GetComparisonIndex())
			{
				auto& RowMap = StatTable->GetRowMap<>();

				for (const auto& [RowName, RowPtr] : RowMap)
				{
					if (RowName.GetComparisonIndex() == StatRowName.GetComparisonIndex())
					{
						auto Row = (FFortQuestObjectiveStatTableRow*)RowPtr;

						if (Row->GetType() != StatEvent)
							continue;

						if (!TargetTags.HasAll(Row->GetTargetTagContainer()))
							continue;

						if (!SourceTags.HasAll(Row->GetSourceTagContainer()))
							continue;

						if (!ContextTags.HasAll(Row->GetContextTagContainer()))
							continue;
						ProgressQuest(Controller, QuestManager, CurrentQuest, QuestDef, Objective, Objective->GetCount() + 1);
						UE_LOG(LogNeon, Log, "BackendName: %s", Objective->GetBackendName().ToString().ToString().c_str());
					}
				}
			}
		}
	}
}

void UFortQuestManager::SendStatEventWithTags(UFortQuestManager* QuestManager, EFortQuestObjectiveStatEvent Type, UObject* TargetObject, FGameplayTagContainer& TargetTags, FGameplayTagContainer& SourceTags, FGameplayTagContainer& ContextTags, int Count)
{
	auto Controller = (AFortPlayerControllerAthena*)QuestManager->GetPlayerControllerBP();

	if (!Controller) return;

	QuestManager = Controller->CallFunc<UFortQuestManager*>("FortPlayerController", "GetQuestManager", 1);

	static int CurrentPlaylistInfoOffset = Runtime::GetOffset(UWorld::GetWorld()->GetGameState(), "CurrentPlaylistInfo");
            
	FPlaylistPropertyArray& CurrentPlaylistInfoPtr = *reinterpret_cast<FPlaylistPropertyArray*>(__int64(UWorld::GetWorld()->GetGameState()) + CurrentPlaylistInfoOffset);
	
	ContextTags.AppendTags(CurrentPlaylistInfoPtr.GetBasePlaylist()->GetGameplayTagContainer());
	SourceTags.AppendTags(SourceTags);

	auto& CurrentQuests = QuestManager->GetCurrentQuests();
	for (int i = 0; i < CurrentQuests.Num(); i++)
	{
		if (!CurrentQuests.IsValidIndex(i) || !CurrentQuests[i])
			continue;
			
		auto CurrentQuest = CurrentQuests[i];
		
		if (CurrentQuest->HasCompletedQuest())
			continue;

		auto QuestDef = CurrentQuest->GetQuestDefinitionBP();

		if (!QuestDef)
			continue;

		if (QuestManager->HasCompletedQuest(QuestDef))
			continue;
				
		auto& Objectives = QuestDef->GetObjectives();
		static int32 FFortMcpQuestObjectiveInfoSize = StaticClassImpl("FortMcpQuestObjectiveInfo")->GetSize();
		for (int j = 0; j < Objectives.Num(); j++)
		{
			if (!Objectives.IsValidIndex(j))
				continue;
				
			FFortMcpQuestObjectiveInfo* Objective = (FFortMcpQuestObjectiveInfo*)((uint8*)Objectives.GetData() + (j * FFortMcpQuestObjectiveInfoSize));
			
			if (QuestManager->HasCompletedObjectiveWithName(QuestDef, Objective->GetBackendName()) ||
					QuestManager->HasCompletedObjective(QuestDef, Objective->GetObjectiveStatHandle()) ||
					CurrentQuest->HasCompletedObjectiveWithName(Objective->GetBackendName()) ||
					CurrentQuest->HasCompletedObjective(Objective->GetObjectiveStatHandle()))
			{
				continue;
			}
				
					
			auto StatTable = Objective->GetObjectiveStatHandle().DataTable;
			auto& StatRowName = Objective->GetObjectiveStatHandle().GetRowName();

			if (!StatTable || !StatRowName.IsValid())
			{
				auto& InlineStats = Objective->GetInlineObjectiveStats();
				static int32 FFortQuestObjectiveStatSize = StaticClassImpl("FortQuestObjectiveStat")->GetSize();
				for (int k = 0; k < InlineStats.Num(); k++)
				{
					if (!InlineStats.IsValidIndex(k))
						continue;
						
					FFortQuestObjectiveStat* ObjectiveStat = (FFortQuestObjectiveStat*)((uint8*)InlineStats.GetData() + (k * FFortQuestObjectiveStatSize));
					
					if (ObjectiveStat->GetType() != Type)
						continue;
					bool bFoundQuest = true; 

					auto& TagConditions = ObjectiveStat->GetTagConditions();
					static int32 FInlineObjectiveStatTagCheckEntrySize = StaticClassImpl("InlineObjectiveStatTagCheckEntry")->GetSize();
					for (int l = 0; l < TagConditions.Num(); l++)
					{
						if (!TagConditions.IsValidIndex(l))
							continue;
							
						FInlineObjectiveStatTagCheckEntry* TagCondition = (FInlineObjectiveStatTagCheckEntry*)((uint8*)TagConditions.GetData() + (l * FInlineObjectiveStatTagCheckEntrySize));
						
						if (!TagCondition->GetRequire() || !bFoundQuest)
							continue;

						switch (TagCondition->GetType())
						{
						case EInlineObjectiveStatTagCheckEntryType::Target:
							{
								if (!ObjectiveStat->GetbHsaInclusiveTargetTags())
									break;

								if (!TargetTags.HasTag(TagCondition->GetTag()))
								{
									bFoundQuest = false;
								}
								
								break;
							}
						case EInlineObjectiveStatTagCheckEntryType::Source:
							{
								if (!ObjectiveStat->GetbHasInclusiveSourceTags())
									break;

								if (!SourceTags.HasTag(TagCondition->GetTag()))
								{
									bFoundQuest = false;
								}

								break;
							}
						case EInlineObjectiveStatTagCheckEntryType::Context:
							{
								if (!ObjectiveStat->GetbHasInclusiveContextTags())
									break;

								if (!ContextTags.HasTag(TagCondition->GetTag()))
								{
									bFoundQuest = false;
								}

								break;
							}
						case EInlineObjectiveStatTagCheckEntryType::EInlineObjectiveStatTagCheckEntryType_MAX:
							{
								break;
							}
						default:
							break;
						}
					}

					if (ObjectiveStat->GetType() != Type)
					{
						bFoundQuest = false;
					}

					if (bFoundQuest)
					{
						ProgressQuest(Controller, QuestManager, CurrentQuest, QuestDef, Objective, Objective->GetCount() + 1);
						UE_LOG(LogNeon, Log, "BackendName: %s", Objective->GetBackendName().ToString().ToString().c_str());
					}
				}
			}
			else if (StatTable && StatRowName.GetComparisonIndex())
			{
				auto& RowMap = StatTable->GetRowMap<>();

				for (const auto& [RowName, RowPtr] : RowMap)
				{
					if (RowName.GetComparisonIndex() == StatRowName.GetComparisonIndex())
					{
						auto Row = (FFortQuestObjectiveStatTableRow*)RowPtr;

						if (Row->GetType() != Type)
							continue;

						if (!TargetTags.HasAll(Row->GetTargetTagContainer()))
							continue;

						if (!SourceTags.HasAll(Row->GetSourceTagContainer()))
							continue;

						if (!ContextTags.HasAll(Row->GetContextTagContainer()))
							continue;
						ProgressQuest(Controller, QuestManager, CurrentQuest, QuestDef, Objective, Objective->GetCount() + 1);
						UE_LOG(LogNeon, Log, "BackendName: %s", Objective->GetBackendName().ToString().ToString().c_str());
					}
				}
			}
		}
	}
	return SendStatEventWithTagsOG(QuestManager, Type, TargetObject, TargetTags, SourceTags, ContextTags, Count);
}

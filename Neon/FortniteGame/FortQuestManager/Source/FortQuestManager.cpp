#include "pch.h"
#include "../Header/FortQuestManager.h"

#include "FortniteGame/FortPlayerController/Header/FortPlayerController.h"

void UFortQuestManager::SendComplexCustomStatEvent(UFortQuestManager* QuestManager, UObject* TargetObj, FGameplayTagContainer& SourceTags, FGameplayTagContainer& TargetTags, bool* QuestActive, bool* QuestCompleted, int32 Count)
{ // dont think this is the right way to do this but wtv? - Andrew
	UE_LOG(LogNeon, Log, "NAV");
	SendStatEvent(QuestManager, TargetObj, SourceTags, TargetTags, nullptr, nullptr, 1, EFortQuestObjectiveStatEvent::ComplexCustom);
    return SendComplexCustomStatEventOG(QuestManager, TargetObj, SourceTags, TargetTags, QuestActive, QuestCompleted, Count);
}

void UFortQuestManager::SendStatEvent(UFortQuestManager* QuestManager, UObject* TargetObj, FGameplayTagContainer& SourceTags, FGameplayTagContainer& TargetTags, bool* QuestActive, bool* QuestCompleted, int32 Count, EFortQuestObjectiveStatEvent StatEvent)
{
	auto Controller = (AFortPlayerControllerAthena*)QuestManager->GetPlayerControllerBP();

	if (!Controller) return;

	FGameplayTagContainer PSourceTags; // p for player if ur retarded tech
	FGameplayTagContainer ContextTags;

	QuestManager->GetSourceAndContextTags(&PSourceTags, &ContextTags);

	static int CurrentPlaylistInfoOffset = Runtime::GetOffset(UWorld::GetWorld()->GetGameState(), "CurrentPlaylistInfo");
            
	FPlaylistPropertyArray& CurrentPlaylistInfoPtr = *reinterpret_cast<FPlaylistPropertyArray*>(__int64(UWorld::GetWorld()->GetGameState()) + CurrentPlaylistInfoOffset);
	
	ContextTags.AppendTags(CurrentPlaylistInfoPtr.GetBasePlaylist()->GetGameplayTagContainer());
	SourceTags.AppendTags(PSourceTags);

	for (auto& CurrentQuest : QuestManager->GetCurrentQuests())
	{
		if (CurrentQuest->HasCompletedQuest())
			continue;

		auto QuestDef = CurrentQuest->GetQuestDefinitionBP();

		if (!QuestDef)
			continue;

		if (QuestManager->HasCompletedQuest(QuestDef))
			continue;
				
		for (auto& Objective : QuestDef->GetObjectives())
		{
			if (QuestManager->HasCompletedObjectiveWithName(QuestDef, Objective.GetBackendName()) ||
				QuestManager->HasCompletedObjective(QuestDef, Objective.GetObjectiveStatHandle()) ||
				CurrentQuest->HasCompletedObjectiveWithName(Objective.GetBackendName()) ||
				CurrentQuest->HasCompletedObjective(Objective.GetObjectiveStatHandle()))
			{
				continue;
			}
					
			auto StatTable = Objective.GetObjectiveStatHandle().DataTable;
			auto& StatRowName = Objective.GetObjectiveStatHandle().GetRowName();

			if (!StatTable || !StatRowName.IsValid())
			{
				for (auto& ObjectiveStat : Objective.GetInlineObjectiveStats())
				{
					if (ObjectiveStat.GetType() != StatEvent)
						continue;
					bool bFoundCorrectQuest = true; 

					for (auto& TagCondition : ObjectiveStat.GetTagConditions())
					{
						if (!TagCondition.GetRequire() || !bFoundCorrectQuest)
							continue;

						switch (TagCondition.GetType())
						{
						case EInlineObjectiveStatTagCheckEntryType::Target:
							{
								if (!ObjectiveStat.GetbHsaInclusiveTargetTags())
									break;

								if (!TargetTags.HasTag(TagCondition.GetTag()))
									bFoundCorrectQuest = false;

								break;
							}
						case EInlineObjectiveStatTagCheckEntryType::Source:
							{
								if (!ObjectiveStat.GetbHasInclusiveSourceTags())
									break;

								if (!SourceTags.HasTag(TagCondition.GetTag()))
									bFoundCorrectQuest = false;

								break;
							}
						case EInlineObjectiveStatTagCheckEntryType::Context:
							{
								if (!ObjectiveStat.GetbHasInclusiveContextTags())
									break;

								if (!ContextTags.HasTag(TagCondition.GetTag()))
									bFoundCorrectQuest = false;

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

					if (ObjectiveStat.GetType() != StatEvent)
						bFoundCorrectQuest = false;

					if (bFoundCorrectQuest)
					{
						UE_LOG(LogNeon, Log, "BackendName: %s", Objective.GetBackendName().ToString().ToString().c_str());
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

						UE_LOG(LogNeon, Log, "BackendName: %s", Objective.GetBackendName().ToString().ToString().c_str());
					}
				}
			}
		}
	}
}
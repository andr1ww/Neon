#include "pch.h"
#include "../Header/FortQuestManager.h"

#include "Engine/GameplayStatics/Header/GameplayStatics.h"
#include "FortniteGame/FortPlayerController/Header/FortPlayerController.h"

void UFortQuestManager::SendComplexCustomStatEvent(UFortQuestManager* QuestManager, UObject* TargetObj, FGameplayTagContainer& SourceTags, FGameplayTagContainer& TargetTags, bool* QuestActive, bool* QuestCompleted, int32 Count)
{
	FGameplayTagContainer ContextTags;
	SendStatEvent(QuestManager, TargetObj, SourceTags, TargetTags, nullptr, nullptr, 1, EFortQuestObjectiveStatEvent::ComplexCustom, ContextTags);
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
			PlayerController->OnRep_UpdatedObjectiveStats();
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

	PlayerController->OnRep_UpdatedObjectiveStats();
}

static std::unordered_map<AFortPlayerControllerAthena*, std::vector<FNexaBroadcastQuestProgress>> PlayerNexaBroadcastQuestProgress;

std::vector<FNexaBroadcastQuestProgress> UFortQuestManager::GetNexaBroadcastQuestProgress(AFortPlayerControllerAthena* PlayerController) {
    if (!PlayerController) {
        return {};
    }
    
    return PlayerNexaBroadcastQuestProgress[PlayerController];
}

static void ProgressQuest(AFortPlayerControllerAthena* PlayerController, UFortQuestManager* QuestManager, UFortQuestItem* QuestItem, UFortQuestItemDefinition* QuestDefinition, FFortMcpQuestObjectiveInfo* Obj, int32 IncrementCount)
{
    static std::unordered_map<AFortPlayerControllerAthena*, std::vector<FFortMcpQuestObjectiveInfo>> ObjCompArray;
    auto Count = QuestManager->GetObjectiveCompletionCount(QuestDefinition, Obj->GetBackendName());

    int32 NewCount = Count + IncrementCount;
    
    QuestDefinition->SetObjectiveCompletionCount(NewCount);
    
    std::string BackendName = std::string(Obj->GetBackendName().ToString().ToString());
    
	bool found = false;
	for (auto& entry : PlayerNexaBroadcastQuestProgress[PlayerController]) {
		if (entry.BackendName == BackendName) {
			entry.Count = NewCount;
			found = true;
			UE_LOG(LogNeon, Log, "Updated NexaBroadcastQuestProgress: %s, Count: %d", BackendName.c_str(), NewCount);
			break;
		}
	}
    
	if (!found) {
		FNexaBroadcastQuestProgress newEntry;
		newEntry.BackendName = BackendName;
		newEntry.Count = NewCount;
		PlayerNexaBroadcastQuestProgress[PlayerController].push_back(newEntry);
		UE_LOG(LogNeon, Log, "Added NexaBroadcastQuestProgress: %s, Count: %d", BackendName.c_str(), NewCount);
	}
    
    bool thisObjectiveCompleted = (NewCount >= Obj->GetCount()); 
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
                if (QuestObj.GetBackendName().GetComparisonIndex() == ObjComp.GetBackendName().GetComparisonIndex())
                {
                    Found = true;
                    break;
                }
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

            SendObjectiveStat(TeamMemberPlayerController, Obj->GetBackendName(), QuestDefinition, NewCount);

            if (TeamMemberPlayerController == PlayerController)
            {
                QuestManager->CallFunc<void>("FortQuestManager", "SelfCompletedUpdatedQuest",
                    TeamMemberPlayerController, QuestDefinition, Obj->GetBackendName(), NewCount,
                    IncrementCount, PlayerState, thisObjectiveCompleted, allObjsCompleted);
            }
            else
            {
                QuestManager->CallFunc<void>("FortQuestManager", "AssistedPlayerUpdatedQuest",
                    TeamMemberPlayerController, QuestDefinition, Obj->GetBackendName(), NewCount,
                    IncrementCount, PlayerState, thisObjectiveCompleted, allObjsCompleted);
            }

            QuestManager->CallFunc<void>("FortQuestManager", "HandleQuestUpdated",
                TeamMemberPlayerController,
                QuestDefinition,
                Obj->GetBackendName(),
                NewCount,
                IncrementCount,
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
                    break;
                }
            }
        }

        if (XPPlayerControllerCount > 0)
        {
            for (const auto& TeamMember : PlayerState->GetPlayerTeam()->GetTeamMembers())
            {
                auto TeamMemberPlayerController = (AFortPlayerControllerAthena*)TeamMember;
                if (!TeamMemberPlayerController || TeamMemberPlayerController->IsA<AFortAthenaAIBotController>())
                    continue;

                auto* XPComp = TeamMemberPlayerController->GetXPComponent();
                if (!XPComp) continue;

                FXPEventInfo XpEventInfo{};
                XpEventInfo.EventXpValue = XPPlayerControllerCount;
                XpEventInfo.RestedValuePortion = XPPlayerControllerCount;
                XpEventInfo.RestedXPRemaining = XPPlayerControllerCount;
                XpEventInfo.TotalXpEarnedInMatch = XPComp->GetTotalXpEarned() + XPPlayerControllerCount;
                XpEventInfo.Priority = EXPEventPriorityType::XPBarOnly;
                XpEventInfo.SimulatedText = UKismetStringLibrary::Conv_StringToText(L"Objective Completed");
                XpEventInfo.EventName = UKismetStringLibrary::Conv_StringToName(L"Objective Completed");
                XpEventInfo.SeasonBoostValuePortion = 20;

                XPComp->SetChallengeXp(XPComp->GetChallengeXp() + XPPlayerControllerCount);
                XPComp->SetMatchXp(XPComp->GetMatchXp() + XPPlayerControllerCount);
                XPComp->SetTotalXpEarned(XPComp->GetTotalXpEarned() + XPPlayerControllerCount);

                XPComp->OnXPEvent(XpEventInfo);
            }
        }
    }
}

void UFortQuestManager::SendStatEvent(UFortQuestManager* QuestManager, UObject* TargetObj, FGameplayTagContainer& SourceTags, FGameplayTagContainer& TargetTags, bool* QuestActive, bool* QuestCompleted, int32 Count, EFortQuestObjectiveStatEvent StatEvent, FGameplayTagContainer& ContextTags)
{
	auto Controller = (AFortPlayerControllerAthena*)QuestManager->GetPlayerControllerBP();

	if (!Controller) return;

	FGameplayTagContainer PSourceTags;
	FGameplayTagContainer PContextTags;
	
	QuestManager->GetSourceAndContextTags(&PSourceTags, &ContextTags);

	static int CurrentPlaylistInfoOffset = Runtime::GetOffset(UWorld::GetWorld()->GetGameState(), "CurrentPlaylistInfo");
            
	FPlaylistPropertyArray& CurrentPlaylistInfoPtr = *reinterpret_cast<FPlaylistPropertyArray*>(__int64(UWorld::GetWorld()->GetGameState()) + CurrentPlaylistInfoOffset);

	ContextTags.AppendTags(PContextTags);
	ContextTags.AppendTags(CurrentPlaylistInfoPtr.GetBasePlaylist()->GetGameplayTagContainer());
	SourceTags.AppendTags(PSourceTags);
	ContextTags.AppendTags(ContextTags);

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
								if (!ObjectiveStat->GetbHasInclusiveTargetTags())
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

					if (bFoundQuest)
					{
						ProgressQuest(Controller, QuestManager, CurrentQuest, QuestDef, Objective, Count); 
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

						ProgressQuest(Controller, QuestManager, CurrentQuest, QuestDef, Objective, Count);
					}
				}
			}
		}
	}
}

void UFortQuestManager::GiveAccolade(AFortPlayerControllerAthena* PlayerController, UFortAccoladeItemDefinition* Accolade)
{
	if (!PlayerController || !Accolade || Fortnite_Version <= 10.40) 
	{
		return;
	}

	float XpValue = 0;
	TMap<FName, uint8*>& RowMap = *reinterpret_cast<TMap<FName, uint8*>*>(__int64(Accolade->GetXpRewardAmount().Curve.CurveTable) + 0x30);

	for (auto& Pair : RowMap)
	{
		if (Pair.Key.ToString().ToString() == Accolade->GetXpRewardAmount().Curve.RowName.ToString().ToString())
		{
			FSimpleCurve* Curve = (FSimpleCurve*)Pair.Value;
			XpValue = Curve->GetKeys()[0].Value;
		}
	}
	
	FAthenaAccolades NewAccolade{};
    NewAccolade.AccoladeDef = Accolade;
    NewAccolade.Count = 1;
    NewAccolade.TemplateId = Accolade->GetFName().ToString();

	static SDK::UFunction* Func = nullptr;
	SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("KismetSystemLibrary", "GetPrimaryAssetIdFromObject");

	if (Func == nullptr)
		Func = Info.Func;
	if (!Func)
		return;

	struct
	{
		UObject* Object;
		FPrimaryAssetId ReturnValue;
	}UKismetSystemLibrary_GetPrimaryAssetIdFromObject_Params{ Accolade };

	static UObject* DefaultObject = nullptr;
	if (!DefaultObject) DefaultObject = SDK::StaticClassImpl("KismetSystemLibrary")->GetClassDefaultObject();
	DefaultObject->ProcessEvent(Func, &UKismetSystemLibrary_GetPrimaryAssetIdFromObject_Params);
	
    FXPEventInfo XpEventInfo{};
    XpEventInfo.Accolade = UKismetSystemLibrary_GetPrimaryAssetIdFromObject_Params.ReturnValue;
    XpEventInfo.EventXpValue = XpValue;
    XpEventInfo.RestedValuePortion = XpValue;
    XpEventInfo.RestedXPRemaining = XpValue;
    XpEventInfo.TotalXpEarnedInMatch = PlayerController->GetXPComponent()->GetTotalXpEarned() + XpValue;
    XpEventInfo.Priority = Accolade->GetPriority();
    XpEventInfo.SimulatedText = Accolade->CallFunc<FText>("FortItemDefinition", "GetShortDescription");
    XpEventInfo.EventName = Accolade->GetFName();
    XpEventInfo.SeasonBoostValuePortion = 20;

    PlayerController->GetXPComponent()->SetMatchXp(PlayerController->GetXPComponent()->GetMatchXp() + XpValue);
    PlayerController->GetXPComponent()->SetTotalXpEarned(PlayerController->GetXPComponent()->GetTotalXpEarned() + XpValue);

	PlayerController->GetXPComponent()->GetMedalsEarned().Add(Accolade);
	PlayerController->GetXPComponent()->GetPlayerAccolades().Add(NewAccolade);

    PlayerController->GetXPComponent()->OnXPEvent(XpEventInfo);
}

void UFortQuestManager::SendStatEventWithTags(UFortQuestManager* QuestManager, EFortQuestObjectiveStatEvent Type, UObject* TargetObject, FGameplayTagContainer& TargetTags, FGameplayTagContainer& SourceTags, FGameplayTagContainer& ContextTags, int Count)
{
	auto Controller = (AFortPlayerControllerAthena*)QuestManager->GetPlayerControllerBP();
	if (!Controller) return;
	QuestManager = Controller->GetQuestManager(ESubGame::Athena);
	SendStatEvent(QuestManager, TargetObject, SourceTags, TargetTags, nullptr, nullptr, Count, Type, ContextTags);
	return SendStatEventWithTagsOG(QuestManager, Type, TargetObject, TargetTags, SourceTags, ContextTags, Count);
}
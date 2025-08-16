#pragma once
#include "pch.h"

#include "FortniteGame/Common/Header/Tags.h"
#include "FortniteGame/FortLootPackage/Header/FortLootPackage.h"
#include "FortniteGame/FortPlayerController/Header/FortPlayerController.h"

enum class EFortQuestObjectiveStatEvent : uint8
{
	Kill                                     = 0,
	TeamKill                                 = 1,
	KillContribution                         = 2,
	Damage                                   = 3,
	SquadDamage                              = 4,
	Visit                                    = 5,
	VisitDiscoverPOI                         = 6,
	Land                                     = 7,
	Emote                                    = 8,
	Spray                                    = 9,
	Toy                                      = 10,
	Build                                    = 11,
	BuildingEdit                             = 12,
	BuildingRepair                           = 13,
	BuildingUpgrade                          = 14,
	PlaceTrap                                = 15,
	Complete                                 = 16,
	Craft                                    = 17,
	Collect                                  = 18,
	Win                                      = 19,
	Interact                                 = 20,
	TeamInteract                             = 21,
	Destroy                                  = 22,
	Ability                                  = 23,
	WaveComplete                             = 24,
	Custom                                   = 25,
	ComplexCustom                            = 26,
	Client                                   = 27,
	AthenaRank                               = 28,
	AthenaOutlive                            = 29,
	RevivePlayer                             = 30,
	Heal                                     = 31,
	EarnVehicleTrickPoints                   = 32,
	VehicleAirTime                           = 33,
	TimeElapsed                              = 34,
	Death                                    = 35,
	AthenaMarker                             = 36,
	PlacementUpdate                          = 37,
	StormPhase                               = 38,
	DistanceTraveled                         = 39,
	DownOrElim                               = 40,
	Accolade                                 = 41,
	TakeDamage                               = 42,
	NumGameplayEvents                        = 43,
	Acquire                                  = 44,
	Consume                                  = 45,
	OpenCardPack                             = 46,
	PurchaseCardPack                         = 47,
	Convert                                  = 48,
	Upgrade                                  = 49,
	UpgradeRarity                            = 50,
	QuestComplete                            = 51,
	AssignWorker                             = 52,
	CollectExpedition                        = 53,
	CollectSuccessfulExpedition              = 54,
	LevelUpCollectionBook                    = 55,
	LevelUpAthenaSeason                      = 56,
	LevelUpBattlePass                        = 57,
	GainAthenaSeasonXp                       = 58,
	HasItem                                  = 59,
	HasAccumulatedItem                       = 60,
	SlotInCollection                         = 61,
	AlterationRespec                         = 62,
	AlterationUpgrade                        = 63,
	HasCompletedQuest                        = 64,
	HasAssignedWorker                        = 65,
	HasUpgraded                              = 66,
	HasConverted                             = 67,
	HasUpgradedRarity                        = 68,
	HasLeveledUpCollectionBook               = 69,
	SlotHeroInLoadout                        = 70,
	HasLeveledUpAthenaSeason                 = 71,
	HasLeveledUpBattlePass                   = 72,
	HasGainedAthenaSeasonXp                  = 73,
	MinigameDynamicEvent                     = 74,
	MinigameComplete                         = 75,
	MinigameDeath                            = 76,
	MinigameAssist                           = 77,
	Max_None                                 = 78,
	EFortQuestObjectiveStatEvent_MAX         = 79,
};

enum class EInlineObjectiveStatTagCheckEntryType : uint8
{
	Target                                   = 0,
	Source                                   = 1,
	Context                                  = 2,
	EInlineObjectiveStatTagCheckEntryType_MAX = 3,
};

struct FFortQuestObjectiveStatXPTableRow final : public FTableRowBase
{
	DEFINE_MEMBER(EFortQuestObjectiveStatEvent, FFortQuestObjectiveStatXPTableRow, Type);
	DEFINE_MEMBER(FGameplayTagContainer, FFortQuestObjectiveStatXPTableRow, SourceTags);
	DEFINE_MEMBER(FGameplayTagContainer, FFortQuestObjectiveStatXPTableRow, TargetTags);
	DEFINE_MEMBER(FGameplayTagContainer, FFortQuestObjectiveStatXPTableRow, ContextTags);
};

struct FInlineObjectiveStatTagCheckEntry final
{
	DEFINE_MEMBER(EInlineObjectiveStatTagCheckEntryType, FInlineObjectiveStatTagCheckEntry, Type);
	DEFINE_BOOL(FInlineObjectiveStatTagCheckEntry, Require);
	DEFINE_MEMBER(FGameplayTag, FInlineObjectiveStatTagCheckEntry, Tag);
};

struct FFortQuestObjectiveStatTableRow final : public FTableRowBase
{
	DEFINE_MEMBER(FGameplayTagContainer, FFortQuestObjectiveStatTableRow, TargetTagContainer);
	DEFINE_MEMBER(FGameplayTagContainer, FFortQuestObjectiveStatTableRow, SourceTagContainer);
	DEFINE_MEMBER(FGameplayTagContainer, FFortQuestObjectiveStatTableRow, ContextTagContainer);
	DEFINE_MEMBER(EFortQuestObjectiveStatEvent, FFortQuestObjectiveStatTableRow, Type);
};

enum class EFortSimulatedXPSize : uint8
{
	EFortSimulatedXPSize__None = 0,
	EFortSimulatedXPSize__VerySmall = 1,
	EFortSimulatedXPSize__Small = 2,
	EFortSimulatedXPSize__Medium = 3,
	EFortSimulatedXPSize__Large = 4,
	EFortSimulatedXPSize__VeryLarge = 5,
	EFortSimulatedXPSize__EFortSimulatedXPSize_MAX = 6,
};

struct FFortQuestObjectiveStat
{
	DEFINE_MEMBER(TArray<struct FInlineObjectiveStatTagCheckEntry>, FFortQuestObjectiveStat, TagConditions);
	DEFINE_MEMBER(EFortQuestObjectiveStatEvent, FFortQuestObjectiveStat, Type);
	DEFINE_BOOL(FFortQuestObjectiveStat, bHasInclusiveTargetTags);
	DEFINE_BOOL(FFortQuestObjectiveStat, bHasInclusiveSourceTags);
	DEFINE_BOOL(FFortQuestObjectiveStat, bHasInclusiveContextTags);
};

struct FFortQuestRewardTableRow final : public FTableRowBase
{
public:
	DEFINE_MEMBER(int32, FFortQuestRewardTableRow, Quantity);
	class FString                                 QuestTemplateId;                                   // 0x0008(0x0010)(Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	class FName                                   TemplateId;                                        // 0x0018(0x0008)(Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	bool                                          Hidden;                                            // 0x0024(0x0001)(Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	bool                                          Feature;                                           // 0x0025(0x0001)(Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	bool                                          Selectable;                                        // 0x0026(0x0001)(Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	uint8                                         Pad_201E[0x1];                                     // 0x0027(0x0001)(Fixing Struct Size After Last Property [ Dumper-7 ])
};

struct FFortMcpQuestObjectiveInfo final
{
	DEFINE_MEMBER(int32, FFortMcpQuestObjectiveInfo, Count);
	DEFINE_MEMBER(TArray<struct FFortQuestObjectiveStat> , FFortMcpQuestObjectiveInfo, InlineObjectiveStats);
	DEFINE_MEMBER(FName, FFortMcpQuestObjectiveInfo, BackendName);
	DEFINE_MEMBER(FDataTableRowHandle, FFortMcpQuestObjectiveInfo, ObjectiveStatHandle);
};

class UFortQuestItemDefinition : public UObject
{
public:
	DEFINE_PTR(UDataTable, UFortQuestItemDefinition, RewardsTable);
	DEFINE_MEMBER(int32, UFortQuestItemDefinition, ObjectiveCompletionCount);
	DEFINE_MEMBER(TArray<struct FFortMcpQuestObjectiveInfo>,UFortQuestItemDefinition,     Objectives);
public:
	DECLARE_DEFAULT_OBJECT(UFortQuestItemDefinition)
	DECLARE_STATIC_CLASS(UFortQuestItemDefinition)
};

class UFortQuestItem : public UObject
{
public:
	
	inline bool HasCompletedObjective(const FDataTableRowHandle& ObjectiveStatHandle)
	{
		static SDK::UFunction* Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortQuestItem", "HasCompletedObjective");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return false;

		struct FortQuestItem_HasCompletedObjective final
		{
		public:
			struct FDataTableRowHandle                    ObjectiveStatHandle;                               // 0x0008(0x0010)(Parm, NoDestructor, NativeAccessSpecifierPublic)
			bool                                          ReturnValue;                                       // 0x0018(0x0001)(Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			uint8                                         Pad_3A75[0x7];                                     // 0x0019(0x0007)(Fixing Struct Size After Last Property [ Dumper-7 ])
		} Params{};

		Params.ObjectiveStatHandle = ObjectiveStatHandle;
    
		this->ProcessEvent(Func, &Params);

		return Params.ReturnValue;
	}
	
	inline bool HasCompletedObjectiveWithName(class FName BackendName)
	{
		static SDK::UFunction* Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortQuestItem", "HasCompletedObjectiveWithName");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return false;

		struct FortQuestItem_HasCompletedObjectiveWithName final
		{
		public:
			class FName                                   BackendName;                                       // 0x0000(0x0008)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			bool                                          ReturnValue;                                       // 0x0008(0x0001)(Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			uint8                                         Pad_3A57[0x3];                                     // 0x0009(0x0003)(Fixing Struct Size After Last Property [ Dumper-7 ])
		} Params{};

		Params.BackendName = BackendName;
    
		this->ProcessEvent(Func, &Params);

		return Params.ReturnValue;
	}
	
	inline bool HasCompletedQuest()
	{
		static SDK::UFunction* Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortQuestItem", "HasCompletedQuest");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return false;

		struct FortQuestManager_HasCompletedQuest final
		{
		public:
			bool                                          ReturnValue;                                   
		} Params{};
    
		this->ProcessEvent(Func, &Params);

		return Params.ReturnValue;
	}

	inline UFortQuestItemDefinition* GetQuestDefinitionBP()
	{
		static SDK::UFunction* Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortQuestItem", "GetQuestDefinitionBP");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return nullptr;

		struct GetQuestDefinitionBP final
		{
		public:
			class UFortQuestItemDefinition*                  ReturnValue;                                       // 0x0000(0x0008)(Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		} Params{  };
    
		this->ProcessEvent(Func, &Params);

		return Params.ReturnValue;
	}
public:
	DECLARE_STATIC_CLASS(UFortQuestItem)
	DECLARE_DEFAULT_OBJECT(UFortQuestItem)
};

class UFortQuestManager final : public UObject
{
public:
	inline AFortPlayerController* GetPlayerControllerBP()
	{
		static SDK::UFunction* Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortQuestManager", "GetPlayerControllerBP");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return nullptr;

		struct FortQuestManager_GetPlayerControllerBP final
		{
		public:
			class AFortPlayerController*                  ReturnValue;                                       // 0x0000(0x0008)(Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		} Params{  };
    
		this->ProcessEvent(Func, &Params);

		return Params.ReturnValue;
	}

	inline int32 GetObjectiveCompletionCount(const class UFortQuestItemDefinition* Definition, class FName BackendName)
	{
		static SDK::UFunction* Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortQuestManager", "GetObjectiveCompletionCount");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return 0;

		struct FortQuestManager_GetObjectiveCompletionCount final
		{
		public:
			const class UFortQuestItemDefinition*         Definition;                                        // 0x0000(0x0008)(ConstParm, Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			class FName                                   BackendName;                                       // 0x0008(0x0008)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			int32                                         ReturnValue;                                       // 0x0010(0x0004)(Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			uint8                                         Pad_3A73[0x4];                                     // 0x0014(0x0004)(Fixing Struct Size After Last Property [ Dumper-7 ])
		} Params{  };

		Params.Definition = Definition;
		Params.BackendName = BackendName;
    
		this->ProcessEvent(Func, &Params);

		return Params.ReturnValue;
	}

	inline bool HasCompletedObjective(const class UFortQuestItemDefinition* Definition, const FDataTableRowHandle& ObjectiveStatHandle)
	{
		static SDK::UFunction* Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortQuestManager", "HasCompletedObjective");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return false;

		struct FortQuestManager_HasCompletedObjective final
		{
		public:
			const class UFortQuestItemDefinition*         Definition;                                        // 0x0000(0x0008)(ConstParm, Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			struct FDataTableRowHandle                    ObjectiveStatHandle;                               // 0x0008(0x0010)(Parm, NoDestructor, NativeAccessSpecifierPublic)
			bool                                          ReturnValue;                                       // 0x0018(0x0001)(Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			uint8                                         Pad_3A75[0x7];                                     // 0x0019(0x0007)(Fixing Struct Size After Last Property [ Dumper-7 ])
		} Params{};

		Params.Definition = Definition;
		Params.ObjectiveStatHandle = ObjectiveStatHandle;
    
		this->ProcessEvent(Func, &Params);

		return Params.ReturnValue;
	}

	void InitializeQuestAbilities(APawn* Pawn)
	{
		static SDK::UFunction* Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortQuestManager", "InitializeQuestAbilities");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return;

		struct FortQuestManager_InitializeQuestAbilities final
		{
		public:
			class APawn*                                  PlayerPawn;                                        // 0x0000(0x0008)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		} Params{Pawn};
    
		this->ProcessEvent(Func, &Params);
	}

	inline bool HasCompletedQuest(const class UFortQuestItemDefinition* Definition)
	{
		static SDK::UFunction* Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortQuestManager", "HasCompletedQuest");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return false;

		struct FortQuestManager_HasCompletedQuest final
		{
		public:
			const class UFortQuestItemDefinition*         Definition;                                        // 0x0000(0x0008)(ConstParm, Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			bool                                          ReturnValue;                                       // 0x0010(0x0001)(Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			uint8                                         Pad_3A76[0x7];                                     // 0x0011(0x0007)(Fixing Struct Size After Last Property [ Dumper-7 ])
		} Params{};

		Params.Definition = Definition;
    
		this->ProcessEvent(Func, &Params);

		return Params.ReturnValue;
	}

	inline bool HasCompletedObjectiveWithName(const class UFortQuestItemDefinition* Definition, class FName BackendName)
	{
		static SDK::UFunction* Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortQuestManager", "HasCompletedObjectiveWithName");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return false;

		struct FortQuestManager_HasCompletedObjectiveWithName final
		{
		public:
			const class UFortQuestItemDefinition*         Definition;                                        // 0x0000(0x0008)(ConstParm, Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			class FName                                   BackendName;                                       // 0x0008(0x0008)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			bool                                          ReturnValue;                                       // 0x0010(0x0001)(Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			uint8                                         Pad_3A76[0x7];                                     // 0x0011(0x0007)(Fixing Struct Size After Last Property [ Dumper-7 ])
		} Params{};

		Params.Definition = Definition;
		Params.BackendName = BackendName;
    
		this->ProcessEvent(Func, &Params);

		return Params.ReturnValue;
	}

	inline void GetSourceAndContextTags(struct FGameplayTagContainer* OutSourceTags, struct FGameplayTagContainer* OutContextTags)
	{
		static SDK::UFunction* Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortQuestManager", "GetSourceAndContextTags");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return;

		struct FortQuestManager_GetSourceAndContextTags final
		{
		public:
			struct FGameplayTagContainer                  OutSourceTags;                                     // 0x0000(0x0020)(Parm, OutParm, NativeAccessSpecifierPublic)
			struct FGameplayTagContainer                  OutContextTags;                                    // 0x0020(0x0020)(Parm, OutParm, NativeAccessSpecifierPublic)
		} Params{  };
    
		this->ProcessEvent(Func, &Params);

		*OutContextTags = Params.OutContextTags;
		*OutSourceTags = Params.OutSourceTags;
	}
	
    static void SendStatEvent(UFortQuestManager* QuestManager, UObject* TargetObj, FGameplayTagContainer& SourceTags, FGameplayTagContainer& TargetTags, bool* QuestActive, bool* QuestCompleted, int32 Count, EFortQuestObjectiveStatEvent StatEvent);
    DefHookOg(void, SendComplexCustomStatEvent, UFortQuestManager* QuestManager, UObject* TargetObj, FGameplayTagContainer& SourceTags, FGameplayTagContainer& TargetTags, bool* QuestActive, bool* QuestCompleted, int32 Count);
	DefHookOg(void, SendStatEventWithTags, 
		UFortQuestManager*,
		EFortQuestObjectiveStatEvent Type,
		UObject *TargetObject,
	 FGameplayTagContainer& TargetTags,
		 FGameplayTagContainer& SourceTags,
		 FGameplayTagContainer& ContextTags,
		int Count);
public:
	DEFINE_MEMBER(TArray<UFortQuestItem*>, UFortQuestManager, CurrentQuests); 
	DECLARE_STATIC_CLASS(UFortQuestManager)
    DECLARE_DEFAULT_OBJECT(UFortQuestManager)
};

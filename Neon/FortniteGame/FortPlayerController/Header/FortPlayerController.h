#pragma once
#include "pch.h"
#include "Neon/Runtime/Runtime.h"
#include "Engine/PlayerController/Header/PlayerController.h"
#include "FortniteGame/BuildingSMActor/Header/BuildingSMActor.h"
#include "FortniteGame/FortInventory/Header/FortInventory.h"
#include "FortniteGame/FortPlayerPawn/Header/FortPlayerPawn.h"
#include "FortniteGame/FortPlayerState/Header/FortPlayerState.h"
#include "FortniteGame/Common/Header/ItemDefinition.h"

class AFortInventory;
class UFortWeaponItemDefinition;
class AFortAthenaAircraft;
class AFortAthenaCreativePortal;

class UAthenaCharacterItemDefinition : public UObject
{
public:
	DEFINE_PTR(UFortHeroType, UAthenaCharacterItemDefinition, HeroDefinition);

public:
	DECLARE_STATIC_CLASS(UAthenaCharacterItemDefinition)
	DECLARE_DEFAULT_OBJECT(UAthenaCharacterItemDefinition)
};

class UFortWeaponMeleeItemDefinition : public UFortWeaponItemDefinition
{
public:
	DECLARE_STATIC_CLASS(UFortWeaponMeleeItemDefinition)
	DECLARE_DEFAULT_OBJECT(UFortWeaponMeleeItemDefinition)
};

class UAthenaPickaxeItemDefinition : public UObject
{
public:
	DEFINE_PTR(UFortWeaponMeleeItemDefinition, UAthenaPickaxeItemDefinition, WeaponDefinition);

public:
	DECLARE_STATIC_CLASS(UAthenaPickaxeItemDefinition)
	DECLARE_DEFAULT_OBJECT(UAthenaPickaxeItemDefinition)
};

struct FFortAthenaLoadout final
{
public:
	DEFINE_PTR(UAthenaCharacterItemDefinition, FFortAthenaLoadout, Character);
	DEFINE_PTR(UAthenaPickaxeItemDefinition, FFortAthenaLoadout, Pickaxe);
};

struct FFortUpdatedObjectiveStat final
{
public:
	class UFortQuestItemDefinition *Quest; // 0x0000(0x0008)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	class FName BackendName;			   // 0x0008(0x0008)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	int32 StatValue;					   // 0x0010(0x0004)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	int32 ShadowStatValue;				   // 0x0014(0x0004)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	int32 StatDelta;					   // 0x0018(0x0004)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	int32 CurrentStage;					   // 0x001C(0x0004)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
};

enum class ESubGame : uint8
{
	Campaign                                 = 0,
	Athena                                   = 1,
	Invalid                                  = 2,
	Count                                    = 2,
	ESubGame_MAX                             = 3,
};

class AFortPlayerController : public APlayerController
{
public:
	DEFINE_MEMBER(TArray<struct FFortUpdatedObjectiveStat>, AFortPlayerController, UpdatedObjectiveStats);
	DEFINE_PTR(AFortInventory, AFortPlayerController, WorldInventory);
	DEFINE_MEMBER(FFortAthenaLoadout, AFortPlayerController, CosmeticLoadoutPC);
	DEFINE_PTR(AFortPlayerPawn, AFortPlayerController, MyFortPawn);

public:
	void ClientReportDamagedResourceBuilding(class ABuildingSMActor *BuildingSMActor, EFortResourceType PotentialResourceType, int32 PotentialResourceCount, bool bDestroyed, bool bJustHitWeakspot)
	{
		static class UFunction *Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortPlayerController", "ClientReportDamagedResourceBuilding");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return;

		struct FortPlayerController_ClientReportDamagedResourceBuilding final
		{
		public:
			class ABuildingSMActor *BuildingSMActor;
			EFortResourceType PotentialResourceType;
			int32 PotentialResourceCount;
			bool bDestroyed;
			bool bJustHitWeakspot;
		};
		FortPlayerController_ClientReportDamagedResourceBuilding Params;
		Params.BuildingSMActor = BuildingSMActor;
		Params.PotentialResourceType = PotentialResourceType;
		Params.PotentialResourceCount = PotentialResourceCount;
		Params.bDestroyed = bDestroyed;
		Params.bJustHitWeakspot = bJustHitWeakspot;

		this->ProcessEvent(Func, &Params);
	}

	void OnRep_UpdatedObjectiveStats()
	{
		static class UFunction *Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortPlayerController", "OnRep_UpdatedObjectiveStats");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return

		this->ProcessEvent(Func, nullptr);
	}

	bool IsInAircraft()
	{
		static class UFunction *Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortPlayerController", "IsInAircraft");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return false;

		struct Ok final
		{
		public:
			bool ReturnValue;
		} Params{};

		this->ProcessEvent(Func, &Params);

		return Params.ReturnValue;
	}

	class UFortQuestManager* GetQuestManager(ESubGame SubGame)
	{
		static class UFunction *Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortPlayerController", "GetQuestManager");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return nullptr;

		struct FortPlayerController_GetQuestManager final
		{
		public:
			ESubGame SubGame;						  // 0x0000(0x0001)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			uint8 Pad_1[0x7];					  // 0x0001(0x0007)(Fixing Size After Last Property [ Dumper-7 ])
			class UFortQuestManager *ReturnValue; // 0x0008(0x0008)(Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		} Params{};

		Params.SubGame = SubGame;

		this->ProcessEvent(Func, &Params);

		return Params.ReturnValue;
	}
};

class UBlueprintGeneratedClass : public UClass
{
};

class UFortMontageItemDefinitionBase : public UObject
{
};

class UFortAthenaAIBotInventoryItems final : public UObject
{
public:
	DEFINE_MEMBER(TArray<UFortItemDefinition *>, UFortAthenaAIBotInventoryItems, Items);
};

class UAthenaDanceItemDefinition : public UFortMontageItemDefinitionBase
{
public:
	DEFINE_BOOL(UAthenaDanceItemDefinition, bMovingEmote);
	DEFINE_BOOL(UAthenaDanceItemDefinition, bMoveForwardOnly);
	DEFINE_BOOL(UAthenaDanceItemDefinition, bMoveFollowingOnly);
	DEFINE_MEMBER(float, UAthenaDanceItemDefinition, WalkForwardSpeed);
	DEFINE_PTR(UClass, UAthenaDanceItemDefinition, CustomDanceAbility);
public:
	DECLARE_STATIC_CLASS(UAthenaDanceItemDefinition)
	DECLARE_DEFAULT_OBJECT(UAthenaDanceItemDefinition)
};

class UAthenaEmojiItemDefinition : public UAthenaDanceItemDefinition
{
public:
	DECLARE_STATIC_CLASS(UAthenaEmojiItemDefinition)
	DECLARE_DEFAULT_OBJECT(UAthenaEmojiItemDefinition)
};

class UAthenaSprayItemDefinition : public UAthenaDanceItemDefinition
{
};

class UGAB_Emote_Generic_C : public UGameplayAbility
{
public:
	DECLARE_DEFAULT_OBJECT(UGAB_Emote_Generic_C);
	DECLARE_STATIC_CLASS(UGAB_Emote_Generic_C);
};

struct FAthenaRewardResult final
{
public:
	DEFINE_MEMBER(int32, FAthenaRewardResult, LevelsGained);
	DEFINE_MEMBER(int32, FAthenaRewardResult, BookLevelsGained);
	DEFINE_MEMBER(int32, FAthenaRewardResult, TotalSeasonXpGained);
	DEFINE_MEMBER(int32, FAthenaRewardResult, TotalBookXpGained);
	DEFINE_MEMBER(int32, FAthenaRewardResult, PrePenaltySeasonXpGained);
	uint8 Pad_14[0x4];
	DEFINE_MEMBER(float, FAthenaRewardResult, AntiAddictionMultiplier);
	uint8 Pad_3C[0x4];
};

struct FAthenaMatchStats final
{
public:
	DEFINE_MEMBER(FString, FAthenaMatchStats, StatBucket);
	DEFINE_MEMBER(FString, FAthenaMatchStats, MatchID);
	DEFINE_MEMBER(FString, FAthenaMatchStats, MatchEndTime);
	DEFINE_MEMBER(FString, FAthenaMatchStats, MatchPlatform);
	int32 Stats[0x14];
	DEFINE_BOOL(FAthenaMatchStats, bIsValid);
	uint8 Pad_D5[0x3];
	DEFINE_MEMBER(FString, FAthenaMatchStats, FactionTag);
};

struct FAthenaMatchTeamStats final
{
public:
	DEFINE_MEMBER(int32, FAthenaMatchTeamStats, Place);
	DEFINE_MEMBER(int32, FAthenaMatchTeamStats, TotalPlayers);
};

class UAthenaPlayerMatchReport : public UObject
{
public:
	DEFINE_MEMBER(FAthenaRewardResult, UAthenaPlayerMatchReport, EndOfMatchResults); // 0x0000(0x0038)(BlueprintVisible, BlueprintReadOnly, Transient, NativeAccessSpecifierPublic)
	DEFINE_MEMBER(FAthenaMatchTeamStats, UAthenaPlayerMatchReport, TeamStats);		 // 0x0000(0x0028)(BlueprintVisible, BlueprintReadOnly, Transient, NativeAccessSpecifierPublic)
	DEFINE_MEMBER(FAthenaMatchStats, UAthenaPlayerMatchReport, MatchStats);			 // 0x0028(0x00A0)(BlueprintVisible, BlueprintReadOnly, Transient, NativeAccessSpecifierPublic)
public:
	DECLARE_STATIC_CLASS(UAthenaPlayerMatchReport);
	DECLARE_DEFAULT_OBJECT(UAthenaPlayerMatchReport);
};

class UGAB_Spray_Generic_C : public UGAB_Emote_Generic_C
{
public:
	DECLARE_DEFAULT_OBJECT(UGAB_Spray_Generic_C);
	DECLARE_STATIC_CLASS(UGAB_Spray_Generic_C);
};

class UActorComponent : public UObject
{
public:
	struct ActorComponent_Activate final
	{
	public:
		bool bReset;
	};

	struct ActorComponent_SetActive final
	{
	public:
		bool bNewActive;
		bool bReset;
	};

public:
	AActor *GetOwner(UActorComponent* This)
	{
		static SDK::UFunction *Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("ActorComponent", "GetOwner");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return nullptr;
		struct
		{
			UActorComponent *Comp;
			AActor *ReturnValue;
		} GetOwnerParams{};

		GetOwnerParams.Comp = This;

		this->ProcessEvent(Func, &GetOwnerParams);

		return GetOwnerParams.ReturnValue;
	}

	void Activate(bool bReset)
	{
		static SDK::UFunction *Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("ActorComponent", "Activate");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return;

		ActorComponent_Activate Params;
		Params.bReset = bReset;

		this->ProcessEvent(Func, &Params);
	}

	void SetActive(bool bNewActive, bool bReset)
	{
		static SDK::UFunction *Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("ActorComponent", "SetActive");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return;

		ActorComponent_SetActive Params;
		Params.bNewActive = bNewActive;
		Params.bReset = bReset;

		this->ProcessEvent(Func, &Params);
	}

	void OnRep_IsActive()
	{
		static SDK::UFunction *Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("ActorComponent", "OnRep_IsActive");
		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return;

		this->ProcessEvent(Func, nullptr);
	}
};

enum class EFortTeam : uint8
{
	Spectator = 0,
	HumanCampaign = 1,
	Monster = 2,
	HumanPvP_Team1 = 3,
	HumanPvP_Team2 = 4,
	MAX = 255,
};

class UFortEditToolItemDefinition : public UObject
{
public:
	DECLARE_STATIC_CLASS(UFortEditToolItemDefinition)
	DECLARE_DEFAULT_OBJECT(UFortEditToolItemDefinition)
};

class AFortWeap_EditingTool : public UObject
{
public:
	void OnRep_EditActor()
	{
		static SDK::UFunction *Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortWeap_EditingTool", "OnRep_EditActor");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return;

		this->ProcessEvent(Func, nullptr);
	}

public:
	DECLARE_STATIC_CLASS(AFortWeap_EditingTool)
	DECLARE_DEFAULT_OBJECT(AFortWeap_EditingTool)
};

struct FFortPlayerDeathReport final
{
public:
	DEFINE_MEMBER(float, FFortPlayerDeathReport, ServerTimeForRespawn);
	DEFINE_MEMBER(float, FFortPlayerDeathReport, ServerTimeForResurrect);
	DEFINE_MEMBER(float, FFortPlayerDeathReport, LethalDamage);
	uint8 Pad_C[0x4];
	DEFINE_PTR(AFortPlayerStateAthena, FFortPlayerDeathReport, KillerPlayerState);
	DEFINE_PTR(AFortPlayerPawn, FFortPlayerDeathReport, KillerPawn);
	DEFINE_MEMBER(float, FFortPlayerDeathReport, KillerHealthPercent);
	DEFINE_MEMBER(float, FFortPlayerDeathReport, KillerShieldPercent);
	DEFINE_MEMBER(float, FFortPlayerDeathReport, KillerOvershieldPercent);
	uint8 Pad_2C[0x4];
	DEFINE_PTR(UFortWeaponItemDefinition, FFortPlayerDeathReport, KillerWeapon);
	DEFINE_PTR(AActor, FFortPlayerDeathReport, DamageCauser);
	uint8 bDroppedBackpack : 1;
	uint8 bNotifyUI : 1;
	uint8 Pad_41[0x7];
	DEFINE_MEMBER(FGameplayTagContainer, FFortPlayerDeathReport, Tags);
	DEFINE_MEMBER(FVector, FFortPlayerDeathReport, ViewLocationAtTimeOfDeath);
	DEFINE_MEMBER(FRotator, FFortPlayerDeathReport, ViewRotationAtTimeOfDeath);
};

struct FPrimaryAssetType final
{
public:
	class FName Name; // 0x0000(0x0008)(Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
};
// ScriptStruct CoreUObject.PrimaryAssetId
// 0x0010 (0x0010 - 0x0000)
struct FPrimaryAssetId final
{
public:
	struct FPrimaryAssetType PrimaryAssetType; // 0x0000(0x0008)(Edit, BlueprintVisible, ZeroConstructor, SaveGame, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	class FName PrimaryAssetName;			   // 0x0008(0x0008)(Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
};

struct FXPEventEntry : public FFastArraySerializerItem
{
public:
	uint8 Pad_4665[0x4];					  // Fixing Size After Last Property  [ Dumper-7 ]
	class FText SimulatedXpEvent;			  // 0x10(0x18)(NativeAccessSpecifierPublic)
	class UFortQuestItemDefinition *QuestDef; // 0x28(0x8)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	struct FPrimaryAssetId Accolade;		  // 0x30(0x10)(ZeroConstructor, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	float Time;								  // 0x40(0x4)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	int32 EventXpValue;						  // 0x44(0x4)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	int32 TotalXpEarnedInMatch;				  // 0x48(0x4)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	uint8 Pad_4666[0x4];					  // Fixing Size Of Struct [ Dumper-7 ]
};

// Enum FortniteGame.EXPEventPriorityType
// NumValues: 0x0005
enum class EXPEventPriorityType : uint8
{
	NearReticle = 0,
	XPBarOnly = 1,
	TopCenter = 2,
	Feed = 3,
	EXPEventPriorityType_MAX = 4,
};

struct FXPEventInfo final
{
public:
	class FName EventName;					  // 0x0000(0x0008)(Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	class FText SimulatedText;				  // 0x0008(0x0018)(Edit, BlueprintVisible, BlueprintReadOnly, NativeAccessSpecifierPublic)
	class UFortQuestItemDefinition *QuestDef; // 0x0020(0x0008)(Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	EXPEventPriorityType Priority;			  // 0x0028(0x0001)(Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	uint8 Pad_29[0x3];						  // 0x0029(0x0003)(Fixing Size After Last Property [ Dumper-7 ])
	int32 EventXpValue;						  // 0x002C(0x0004)(Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	int32 TotalXpEarnedInMatch;				  // 0x0030(0x0004)(Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	struct FPrimaryAssetId Accolade;		  // 0x0034(0x0010)(Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	int32 RestedValuePortion;				  // 0x0044(0x0004)(Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	int32 SeasonBoostValuePortion;			  // 0x0048(0x0004)(Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	int32 RestedXPRemaining;				  // 0x004C(0x0004)(Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
};

struct FPlaylistData final
{
public:
	class FName                                   PlaylistName;                                      // 0x0000(0x0008)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	class FString                                 TournamentId;                                      // 0x0008(0x0010)(ZeroConstructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	class FString                                 EventWindowId;                                     // 0x0018(0x0010)(ZeroConstructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	class FString                                 RegionId;                                          // 0x0028(0x0010)(ZeroConstructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
};

struct FAthenaAccolades final
{
public:
	class UFortAccoladeItemDefinition*            AccoladeDef;                                       // 0x0000(0x0008)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	class FString                                 TemplateId;                                        // 0x0008(0x0010)(ZeroConstructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	int32                                         Count;                                             // 0x0018(0x0004)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	uint8                                         Pad_1C[0x4];                                       // 0x001C(0x0004)(Fixing Struct Size After Last Property [ Dumper-7 ])
};

class UFortPlayerControllerAthenaXPComponent : public UObject
{
public:
	void OnXPEvent(const struct FXPEventInfo &Param_HighPrioXPEvent)
	{
		static SDK::UFunction *Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortPlayerControllerAthenaXPComponent", "OnXPEvent");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return;

		struct FortPlayerControllerAthenaXPComponent_HighPrioXPEvent final
		{
		public:
			FXPEventInfo XPEvent; // 0x0000(0x0050)(Parm, NativeAccessSpecifierPublic)
		} Params{};

		Params.XPEvent = Param_HighPrioXPEvent;

		this->ProcessEvent(Func, &Params);
	}

	void HighPrioXPEvent(const struct FXPEventEntry &Param_HighPrioXPEvent)
	{
		static SDK::UFunction *Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortPlayerControllerAthenaXPComponent", "HighPrioXPEvent");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return;

		struct
		{
			struct FXPEventEntry HighPrioXPEvent;
		} params{Param_HighPrioXPEvent};

		this->ProcessEvent(Func, &params);
	}

	void OnRep_bRegisteredWithQuestManager()
	{
		static SDK::UFunction *Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortPlayerControllerAthenaXPComponent", "OnRep_bRegisteredWithQuestManager");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return;

		this->ProcessEvent(Func, nullptr);
	}

public:
	DEFINE_MEMBER(TArray<struct FAthenaAccolades> , UFortPlayerControllerAthenaXPComponent, PlayerAccolades);
	DEFINE_MEMBER(TArray<class UFortAccoladeItemDefinition*>, UFortPlayerControllerAthenaXPComponent, MedalsEarned);
	DEFINE_MEMBER(int32, UFortPlayerControllerAthenaXPComponent, CombatXp);
	DEFINE_MEMBER(int32, UFortPlayerControllerAthenaXPComponent, SurvivalXp);
	DEFINE_MEMBER(int32, UFortPlayerControllerAthenaXPComponent, MedalBonusXP);
	DEFINE_MEMBER(int32, UFortPlayerControllerAthenaXPComponent, ChallengeXp);
	DEFINE_MEMBER(int32, UFortPlayerControllerAthenaXPComponent, MatchXp);
	DEFINE_MEMBER(int32, UFortPlayerControllerAthenaXPComponent, TotalXpEarned);
	DEFINE_MEMBER(int64, UFortPlayerControllerAthenaXPComponent, InMatchProfileVer);
	DEFINE_MEMBER(int32, UFortPlayerControllerAthenaXPComponent, RestXP);
	DEFINE_MEMBER(TArray<struct FXPEventEntry>, UFortPlayerControllerAthenaXPComponent, WaitingQuestXp);
	DECLARE_STATIC_CLASS(UFortPlayerControllerAthenaXPComponent)
	DECLARE_DEFAULT_OBJECT(UFortPlayerControllerAthenaXPComponent)
};

class AFortProjectileBase : public UObject
{
public:
	DECLARE_STATIC_CLASS(AFortProjectileBase)
	DECLARE_DEFAULT_OBJECT(AFortProjectileBase)
};

class UAthenaResurrectionComponent : public UActorComponent
{
public:
	DEFINE_MEMBER(TWeakObjectPtr<AActor>, UAthenaResurrectionComponent, ResurrectionLocation);
	DEFINE_MEMBER(int, UAthenaResurrectionComponent, ClosestSpawnMachineIndex);
public:
	DECLARE_STATIC_CLASS(UAthenaResurrectionComponent)
	DECLARE_DEFAULT_OBJECT(UAthenaResurrectionComponent)
};

class AFortPlayerControllerAthena : public AFortPlayerController
{
public:
	DEFINE_PTR(UAthenaResurrectionComponent, AFortPlayerControllerAthena, ResurrectionComponent);
	DEFINE_BOOL(AFortPlayerControllerAthena, bMarkedAlive)
	DEFINE_PTR(UFortPlayerControllerAthenaXPComponent, AFortPlayerControllerAthena, XPComponent);
	DEFINE_PTR(UAthenaPlayerMatchReport, AFortPlayerControllerAthena, MatchReport);
	DEFINE_PTR(AFortPlayerStateAthena, AFortPlayerControllerAthena, PlayerState);
	DEFINE_BOOL(AFortPlayerControllerAthena, bHasServerFinishedLoading);
	DEFINE_PTR(AFortAthenaCreativePortal, AFortPlayerControllerAthena, OwnedPortal);
public:
	DefHookOg(void, EnterAircraft, AFortPlayerControllerAthena *PlayerController, AFortAthenaAircraft *Aircraft);
	static void ServerAttemptAircraftJump(UActorComponent *Comp, FFrame &Stack);
	static void ServerLoadingScreenDropped(AFortPlayerControllerAthena *PlayerController, FFrame &Stack);
	static void ServerAcknowledgePossession(AFortPlayerControllerAthena *PlayerController, FFrame &Stack);
	static void ServerExecuteInventoryItem(AFortPlayerControllerAthena *PlayerController, FFrame &Stack);
	static void ServerPlayEmoteItem(AFortPlayerControllerAthena *PlayerController, FFrame &Stack);
	static void ServerCreateBuildingActor(AFortPlayerControllerAthena *PlayerController, FFrame &Stack);
	static void ServerBeginEditingBuildingActor(AFortPlayerControllerAthena *PlayerController, FFrame &Stack);
	static void ServerEditBuildingActor(AFortPlayerControllerAthena *PlayerController, FFrame &Stack);
	static void ServerEndEditingBuildingActor(AFortPlayerControllerAthena *PlayerController, FFrame &Stack);
	static void ServerRepairBuildingActor(AFortPlayerControllerAthena *PlayerController, FFrame &Stack);
	static void ServerAttemptInventoryDrop(AFortPlayerControllerAthena *PlayerController, FFrame &Stack);
	DefHookOg(void, ClientOnPawnDied, AFortPlayerControllerAthena *PlayerController, FFortPlayerDeathReport &DeathReport);
	DefHookOg(void, ServerDBNOReviveInterrupted, AFortPlayerControllerAthena* PlayerController, class AFortPlayerPawnAthena* Pawn);

	DefHookOg(int32, K2_RemoveItemFromPlayerByGuid, UObject *, FFrame &);
	DefHookOg(int32, K2_RemoveItemFromPlayer, AFortPlayerControllerAthena *, UFortWorldItemDefinition *, int32, bool);

public:
	DECLARE_STATIC_CLASS(AFortPlayerControllerAthena)
	DECLARE_DEFAULT_OBJECT(AFortPlayerControllerAthena)
public:
	void ClientSendEndBattleRoyaleMatchForPlayer(bool bSuccess, const FAthenaRewardResult Result)
	{
		/*   static SDK::UFunction* Func = nullptr;
		   SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortPlayerControllerAthena", "ClientSendEndBattleRoyaleMatchForPlayer");

		   if (Func == nullptr)
			   Func = Info.Func;
		   if (!Func)
			   return;

		   struct FortPlayerControllerAthena_ClientSendEndBattleRoyaleMatchForPlayer final
		   {
		   public:
			   bool                                          bSuccess;                                          // 0x0000(0x0001)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			   uint8                                         Pad_2B47[0x7];                                     // 0x0001(0x0007)(Fixing Size After Last Property [ Dumper-7 ])
			   struct FAthenaRewardResult                    Result;                                            // 0x0008(0x0040)(ConstParm, Parm, ReferenceParm, NativeAccessSpecifierPublic)
		   } FortPlayerControllerAthena_ClientSendEndBattleRoyaleMatchForPlayer_Params{ };

		   FortPlayerControllerAthena_ClientSendEndBattleRoyaleMatchForPlayer_Params.bSuccess = bSuccess;
		   FortPlayerControllerAthena_ClientSendEndBattleRoyaleMatchForPlayer_Params.Result = Result;

		   this->ProcessEvent(Func, &FortPlayerControllerAthena_ClientSendEndBattleRoyaleMatchForPlayer_Params);
   */
		this->CallFunc<void>("FortPlayerControllerAthena", "ClientSendEndBattleRoyaleMatchForPlayer", bSuccess, Result);
	}

	void ClientSendMatchStatsForPlayer(const struct FAthenaMatchStats &Result)
	{
		static SDK::UFunction *Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortPlayerControllerAthena", "ClientSendEndBattleRoyaleMatchForPlayer");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return;

		struct FortPlayerControllerAthena_ClientSendMatchStatsForPlayer final
		{
		public:
			struct FAthenaMatchStats TeamStats; // 0x0000(0x0008)(ConstParm, Parm, ReferenceParm, NoDestructor, NativeAccessSpecifierPublic)
		} FortPlayerControllerAthena_ClientSendMatchStatsForPlayer_Params{Result};

		this->ProcessEvent(Func, &FortPlayerControllerAthena_ClientSendMatchStatsForPlayer_Params);
	}

	void ClientReportTournamentPlacementPointsScored(int32 Placement, int32 PointsEarned)
	{
		static SDK::UFunction *Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortPlayerControllerAthena", "ClientReportTournamentPlacementPointsScored");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return;

		struct FortPlayerControllerAthena_ClientReportTournamentPlacementPointsScored final
		{
		public:
			int32                                         Placement;                                         // 0x0000(0x0004)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			int32                                         PointsEarned;                                      // 0x0004(0x0004)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		} FortPlayerControllerAthena_ClientSendMatchStatsForPlayer_Params{Placement, PointsEarned};

		this->ProcessEvent(Func, &FortPlayerControllerAthena_ClientSendMatchStatsForPlayer_Params);
	}

	void ClientSendTeamStatsForPlayer(const struct FAthenaMatchTeamStats &Result)
	{
		static SDK::UFunction *Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortPlayerControllerAthena", "ClientSendTeamStatsForPlayer");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return;

		struct FortPlayerControllerAthena_ClientSendTeamStatsForPlayer final
		{
		public:
			struct FAthenaMatchTeamStats TeamStats; // 0x0000(0x0008)(ConstParm, Parm, ReferenceParm, NoDestructor, NativeAccessSpecifierPublic)
		} FortPlayerControllerAthena_ClientSendTeamStatsForPlayer_Params{Result};

		this->ProcessEvent(Func, &FortPlayerControllerAthena_ClientSendTeamStatsForPlayer_Params);
	}
};
#pragma once
#include "pch.h"

#include "FortniteGame/AbilitySystemComponent/Header/AbilitySystemComponent.h"
#include "Engine/ObjectPtr/Header/ObjectPtr.h"
#include "FortniteGame/BuildingSMActor/Header/BuildingSMActor.h"
#include "FortniteGame/Common/Header/Tags.h"
#include "FortniteGame/FortPlayerController/Header/FortPlayerController.h"

enum class EFortCustomPartType : uint8
{
    Head                                     = 0,
    Body                                     = 1,
    Hat                                      = 2,
    Backpack                                 = 3,
    Charm                                    = 4,
    Face                                     = 5,
    NumTypes                                 = 6,
    EFortCustomPartType_MAX                  = 7,
};

class UCustomCharacterPart : public UObject
{
public:
	DECLARE_STATIC_CLASS(UCustomCharacterPart)
	DECLARE_DEFAULT_OBJECT(UCustomCharacterPart)
public:
    DEFINE_MEMBER(EFortCustomPartType, UCustomCharacterPart, CharacterPartType)
};

class UFortHeroSpecialization : public UObject
{
public:
    DEFINE_MEMBER(TArray<TSoftObjectPtr<UCustomCharacterPart>>, UFortHeroSpecialization, CharacterParts);
public:
	DECLARE_STATIC_CLASS(UFortHeroSpecialization)
	DECLARE_DEFAULT_OBJECT(UFortHeroSpecialization)
};

class UFortHeroType : public UObject
{
public:
    DEFINE_MEMBER(TArray<TSoftObjectPtr<UFortHeroSpecialization>>, UFortHeroType, Specializations);
public:
	DECLARE_STATIC_CLASS(UFortHeroType)
	DECLARE_DEFAULT_OBJECT(UFortHeroType)
};

class AFortTeamInfo : public UObject
{
public:
	DEFINE_MEMBER(TArray<AFortPlayerControllerAthena*>, AFortTeamInfo, TeamMembers); // 0x0280(0x0010)(ZeroConstructor, Protected, UObjectWrapper, NativeAccessSpecifierProtected)
};

class AFortPlayerState : public APlayerState
{
public:
	DEFINE_PTR(AFortTeamInfo, AFortPlayerState, PlayerTeam); 
};

class AFortPlayerStateZone : public AFortPlayerState
{
public:
	DECLARE_STATIC_CLASS(AFortPlayerStateZone)
	DECLARE_DEFAULT_OBJECT(AFortPlayerStateZone)
};

enum class EDeathCause : uint8
{
	OutsideSafeZone                          = 0,
	FallDamage                               = 1,
	Pistol                                   = 2,
	Shotgun                                  = 3,
	Rifle                                    = 4,
	SMG                                      = 5,
	Sniper                                   = 6,
	SniperNoScope                            = 7,
	Melee                                    = 8,
	InfinityBlade                            = 9,
	Grenade                                  = 10,
	C4                                       = 11,
	GrenadeLauncher                          = 12,
	RocketLauncher                           = 13,
	Minigun                                  = 14,
	Bow                                      = 15,
	Trap                                     = 16,
	DBNOTimeout                              = 17,
	Banhammer                                = 18,
	RemovedFromGame                          = 19,
	MassiveMelee                             = 20,
	MassiveDiveBomb                          = 21,
	MassiveRanged                            = 22,
	Vehicle                                  = 23,
	ShoppingCart                             = 24,
	ATK                                      = 25,
	QuadCrasher                              = 26,
	Biplane                                  = 27,
	BiplaneGun                               = 28,
	LMG                                      = 29,
	GasGrenade                               = 30,
	InstantEnvironmental                     = 31,
	InstantEnvironmentalFellOutOfWorld       = 32,
	InstantEnvironmentalUnderLandscape       = 33,
	Turret                                   = 34,
	ShipCannon                               = 35,
	Cube                                     = 36,
	Balloon                                  = 37,
	StormSurge                               = 38,
	Lava                                     = 39,
	BasicFiend                               = 40,
	EliteFiend                               = 41,
	RangedFiend                              = 42,
	BasicBrute                               = 43,
	EliteBrute                               = 44,
	MegaBrute                                = 45,
	SilentSwitchingToSpectate                = 46,
	LoggedOut                                = 47,
	TeamSwitchSuicide                        = 48,
	WonMatch                                 = 49,
	Unspecified                              = 50,
	EDeathCause_MAX                          = 51,
};

struct FDeathInfo final
{
public:
	DEFINE_PTR(AActor, FDeathInfo, FinisherOrDowner); // 0x0000(0x0008)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	DEFINE_PTR(AActor, FDeathInfo, Downer); // 0x0000(0x0008)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	DEFINE_MEMBER(bool, FDeathInfo, bDBNO); // 0x0010(0x0001)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	DEFINE_MEMBER(EDeathCause, FDeathInfo, DeathCause); // 0x0011(0x0001)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	DEFINE_MEMBER(FGameplayTagContainer, FDeathInfo, DeathTags); // 0x0012(0x0020)(BlueprintVisible, BlueprintReadOnly, NativeAccessSpecifierPublic)
	uint8                                         DeathClassSlot;                                    // 0x0012(0x0001)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	uint8                                         Pad_1B19[0x1];                                     // 0x0013(0x0001)(Fixing Size After Last Property [ Dumper-7 ])
	DEFINE_MEMBER(float, FDeathInfo, Distance);
	float                                         Distance;                                          // 0x0014(0x0004)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	DEFINE_MEMBER(FVector, FDeathInfo, DeathLocation); // 0x0018(0x000C)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	DEFINE_BOOL(FDeathInfo, bInitialized)
	uint8                                         Pad_1B1A[0x3];                                     // 0x0025(0x0003)(Fixing Size After Last Property [ Dumper-7 ])
	uint8                                         Pad_1B1B[0x8];                                     // 0x0048(0x0008)(Fixing Struct Size After Last Property [ Dumper-7 ])
};

class AFortPlayerStateAthena : public AFortPlayerStateZone
{
public:
	DEFINE_MEMBER(int32, AFortPlayerStateAthena, SquadId);
	DEFINE_MEMBER(int32, AFortPlayerStateAthena, Place);
	DEFINE_MEMBER(int32, AFortPlayerStateAthena, KillScore);
	DEFINE_MEMBER(int32, AFortPlayerStateAthena, TeamKillScore);
    DEFINE_MEMBER(FDeathInfo, AFortPlayerStateAthena, DeathInfo);
    DEFINE_PTR(UFortHeroType, AFortPlayerStateAthena, HeroType);
    DEFINE_PTR(UAbilitySystemComponent, AFortPlayerStateAthena, AbilitySystemComponent);

public:
	void OnRep_DeathInfo()
	{
		static SDK::UFunction* Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortPlayerStateAthena", "OnRep_DeathInfo");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return;
		
		this->ProcessEvent(Func, nullptr);
	}

	void OnRep_SeasonLevelUIDisplay()
	{
		static SDK::UFunction* Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortPlayerStateAthena", "OnRep_SeasonLevelUIDisplay");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return;
		
		this->ProcessEvent(Func, nullptr);
	}

	void OnRep_SquadId()
	{
		static SDK::UFunction* Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortPlayerStateAthena", "OnRep_SquadId");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return;
		
		this->ProcessEvent(Func, nullptr);
	}

	void OnRep_TeamKillScore()
	{
		static SDK::UFunction* Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortPlayerStateAthena", "OnRep_TeamKillScore");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return;
		
		this->ProcessEvent(Func, nullptr);
	}

	void OnRep_KillScore()
	{
		static SDK::UFunction* Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortPlayerStateAthena", "OnRep_KillScore");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return;
		
		this->ProcessEvent(Func, nullptr);
	}

	void OnRep_Place()
	{
		static SDK::UFunction* Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortPlayerStateAthena", "OnRep_Place");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return;
		
		this->ProcessEvent(Func, nullptr);
	}

	void ClientReportKill(const AFortPlayerStateAthena* Player)
	{
		static SDK::UFunction* Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortPlayerStateAthena", "ClientReportKill");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return;

		struct FortPlayerStateAthena_ClientReportKill final
		{
		public:
			const class AFortPlayerStateAthena*           Player;                                            // 0x0000(0x0008)(ConstParm, Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		} Params{Player};
		
		this->ProcessEvent(Func, &Params);
	}

	void ClientReportTeamKill(int32 TeamKills)
	{
		static SDK::UFunction* Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortPlayerStateAthena", "ClientReportTeamKill");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return;

		struct FortPlayerStateAthena_ClientReportTeamKill final
		{
		public:
			int32                                         TeamKills;                                         // 0x0000(0x0004)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		} Params{TeamKills};
		
		this->ProcessEvent(Func, &Params);
	}
public:
	DECLARE_STATIC_CLASS(AFortPlayerStateAthena)
	DECLARE_DEFAULT_OBJECT(AFortPlayerStateAthena)
};
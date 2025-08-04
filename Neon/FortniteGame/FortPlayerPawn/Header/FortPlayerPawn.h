#pragma once
#include "pch.h"
#include "FortniteGame/FortPlayerController/Header/FortPlayerController.h"

struct FFortAthenaLoadout;
class AFortWeapon;

class ACharacter : public APawn
{
};

class AFGF_Character : public ACharacter
{
    
};

class AFortPawn : public AFGF_Character
{
public:
	struct FortPawn_PawnStartFire final
	{
	public:
		uint8 FireModeNum;
	};

	struct FortPawn_PawnStopFire final
	{
	public:
		uint8 FireModeNum;
	};
public:
	bool IsDBNO()
	{
		static SDK::UFunction* Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortPawn", "IsDBNO");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return false;

		struct FortPawn_IsDBNO final
		{
		public:
			bool                                          ReturnValue;                                       // 0x0000(0x0001)(Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		} Params{};
		
		this->ProcessEvent(Func, &Params);

		return Params.ReturnValue;
	}

	void PawnStartFire(uint8 FireModeNum)
	{
		static SDK::UFunction* Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortPawn", "PawnStartFire");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return;

		FortPawn_PawnStartFire Params;
		Params.FireModeNum = FireModeNum;
		
		this->ProcessEvent(Func, &Params);
	}

	void PawnStopFire(uint8 FireModeNum)
	{
		static SDK::UFunction* Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortPawn", "PawnStopFire");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return;

		FortPawn_PawnStopFire Params;
		Params.FireModeNum = FireModeNum;
		
		this->ProcessEvent(Func, &Params);
	}

	// Using the newer version of the func so newer versions dont die
	AFortWeapon* EquipWeaponDefinition(const UFortWeaponItemDefinition* WeaponData, const FGuid& ItemEntryGuid, const FGuid& TrackerGuid, bool bDisableEquipAnimation)
	{
		static SDK::UFunction* Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortPawn", "EquipWeaponDefinition");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return nullptr;

		struct FortPawn_EquipWeaponDefinition final
		{
		public:
			const UFortWeaponItemDefinition* WeaponData;
			FGuid ItemEntryGuid;
			FGuid TrackerGuid;
			bool bDisableEquipAnimation;
			AFortWeapon* ReturnValue;
		};
		FortPawn_EquipWeaponDefinition Params;
		Params.WeaponData = WeaponData;
		Params.ItemEntryGuid = ItemEntryGuid;
		Params.TrackerGuid = TrackerGuid;
		Params.bDisableEquipAnimation = bDisableEquipAnimation;
		
		this->ProcessEvent(Func, &Params);

		return Params.ReturnValue;
	}
public:
	DEFINE_PTR(AFortWeapon, AFortPawn, CurrentWeapon);
    DEFINE_BOOL(AFortPawn, bMovingEmote);
    DEFINE_BOOL(AFortPawn, bMovingEmoteForwardOnly);
    DEFINE_BOOL(AFortPawn, bMovingEmoteFollowingOnly);
    DEFINE_MEMBER(float, AFortPawn, EmoteWalkSpeed);
public:
	DECLARE_DEFAULT_OBJECT(AFortPawn);
	DECLARE_STATIC_CLASS(AFortPawn);
};

struct FFortPickupRequestInfo final
{
public:
	struct FGuid                                  SwapWithItem;                                      // 0x0000(0x0010)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	float                                         FlyTime;                                           // 0x0010(0x0004)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	uint8                                         Pad_14[0x4];                                       // 0x0014(0x0004)(Fixing Size After Last Property [ Dumper-7 ])
	struct FVector                                Direction;                                         // 0x0018(0x0018)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	uint8                                         bPlayPickupSound : 1;                              // 0x0030(0x0001)(BitIndex: 0x00, PropSize: 0x0001 (NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic))
	uint8                                         bIsAutoPickup : 1;                                 // 0x0030(0x0001)(BitIndex: 0x01, PropSize: 0x0001 (NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic))
	uint8                                         bUseRequestedSwap : 1;                             // 0x0030(0x0001)(BitIndex: 0x02, PropSize: 0x0001 (NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic))
	uint8                                         bTrySwapWithWeapon : 1;                            // 0x0030(0x0001)(BitIndex: 0x03, PropSize: 0x0001 (NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic))
	uint8                                         bIsVisualOnlyPickup : 1;                           // 0x0030(0x0001)(BitIndex: 0x04, PropSize: 0x0001 (NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic))
	uint8                                         Pad_31[0x7];                                       // 0x0031(0x0007)(Fixing Struct Size After Last Property [ Dumper-7 ])
};


class AFortPlayerPawn : public AFortPawn
{
public:
	DEFINE_MEMBER(TArray<class AFortPickup*>, AFortPlayerPawn, IncomingPickups); 
    DEFINE_PTR(FFortAthenaLoadout, AFortPlayerPawn, CosmeticLoadout);
public:
	void BeginSkydiving(bool NewValue)
	{
		static SDK::UFunction* Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortPlayerPawn", "BeginSkydiving");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return;
    
		struct { bool bStartSkydiving; } BeginSkydivingParams{ NewValue };
	
		this->ProcessEvent(Func, &BeginSkydivingParams);
	}

	void OnRep_CosmeticLoadout()
	{
		static SDK::UFunction* Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortPlayerPawn", "OnRep_CosmeticLoadout");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return;
		
		this->ProcessEvent(Func, nullptr);
	}
public:
    static void ServerHandlePickupInfo(AFortPlayerPawn* Pawn, FFrame& Stack);
	static void ServerHandlePickup(AFortPlayerPawn* Pawn, FFrame& Stack);
	DefHookOg(void, CompletePickupAnimation, AFortPickup* Pickup);

	DefHookOg(void, GiveItemToInventoryOwner, UObject*, FFrame&);
};

class ADefaultPawn : public APawn
{
public:
	DECLARE_DEFAULT_OBJECT(ADefaultPawn)
	DECLARE_STATIC_CLASS(ADefaultPawn)
};
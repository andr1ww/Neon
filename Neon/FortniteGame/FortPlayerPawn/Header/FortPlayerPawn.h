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
	DEFINE_BOOL(AFortPawn, bIsDBNO)
	DEFINE_MEMBER(FGameplayTagContainer, AFortPawn, GameplayTags);
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

	void SetHealth(float Health)
	{
		static SDK::UFunction* Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortPawn", "SetHealth");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return;

		struct Erm final
		{
		public:
			float Health;
		} Params;
		Params.Health = Health;
		
		this->ProcessEvent(Func, &Params);
	}

	void SetShield(float Shield)
	{
		static SDK::UFunction* Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortPawn", "SetShield");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return;

		struct Erm final
		{
		public:
			float Shield;
		} Params;
		Params.Shield = Shield;
		
		this->ProcessEvent(Func, &Params);
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

	inline float GetHealth()
	{
		static SDK::UFunction* Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortPawn", "GetHealth");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return 0;

		struct Erm final
		{
		public:
			float ReturnValue;
		} Params;
		
		this->ProcessEvent(Func, &Params);

		return Params.ReturnValue;
	}

	inline float GetShield()
	{
		static SDK::UFunction* Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortPawn", "GetShield");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return 0;

		struct Erm final
		{
		public:
			float ReturnValue;
		} Params;
		
		this->ProcessEvent(Func, &Params);

		return Params.ReturnValue;
	}

	
	void OnRep_IsDBNO()
	{
		static SDK::UFunction* Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortPawn", "OnRep_IsDBNO");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return;
		
		this->ProcessEvent(Func, nullptr);
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

	AFortWeapon* EquipWeaponDefinition(const UFortWeaponItemDefinition* WeaponData, const FGuid& ItemEntryGuid)
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
			AFortWeapon* ReturnValue;
		};
		FortPawn_EquipWeaponDefinition Params;
		Params.WeaponData = WeaponData;
		Params.ItemEntryGuid = ItemEntryGuid;
		
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

struct FAthenaBatchedDamageGameplayCues_Shared final
{
public:
	struct FVector_NetQuantize10                  Location;                                          // 0x0000(0x0018)(NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	struct FVector_NetQuantizeNormal              Normal;                                            // 0x0018(0x0018)(NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	float                                         Magnitude;                                         // 0x0030(0x0004)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	bool                                          bWeaponActivate;                                   // 0x0034(0x0001)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	bool                                          bIsFatal;                                          // 0x0035(0x0001)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	bool                                          bIsCritical;                                       // 0x0036(0x0001)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	bool                                          bIsShield;                                         // 0x0037(0x0001)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	bool                                          bIsShieldDestroyed;                                // 0x0038(0x0001)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	bool                                          bIsShieldApplied;                                  // 0x0039(0x0001)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	bool                                          bIsBallistic;                                      // 0x003A(0x0001)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	bool                                          bIsBeam;                                           // 0x003B(0x0001)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	uint8                                         Pad_3C[0x4];                                       // 0x003C(0x0004)(Fixing Size After Last Property [ Dumper-7 ])
	struct FVector_NetQuantize10                  NonPlayerLocation;                                 // 0x0040(0x0018)(RepSkip, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	struct FVector_NetQuantizeNormal              NonPlayerNormal;                                   // 0x0058(0x0018)(RepSkip, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	float                                         NonPlayerMagnitude;                                // 0x0070(0x0004)(ZeroConstructor, IsPlainOldData, RepSkip, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	bool                                          NonPlayerbIsFatal;                                 // 0x0074(0x0001)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	bool                                          NonPlayerbIsCritical;                              // 0x0075(0x0001)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	bool                                          bIsValid;                                          // 0x0076(0x0001)(ZeroConstructor, IsPlainOldData, RepSkip, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	uint8                                         Pad_77[0x1];                                       // 0x0077(0x0001)(Fixing Struct Size After Last Property [ Dumper-7 ])
};

struct FAthenaBatchedDamageGameplayCues_NonShared final
{
public:
	class AActor* HitActor;                                          // 0x0000(0x0008)(ZeroConstructor, NoDestructor, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	class AActor* NonPlayerHitActor;                                 // 0x0008(0x0008)(ZeroConstructor, NoDestructor, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic)
};

struct FTimerHandle
{
	FTimerHandle()
		: Handle(0)
	{
	}

	bool IsValid() const
	{
		return Handle != 0;
	}

	void Invalidate()
	{
		Handle = 0;
	}

	bool operator==(const FTimerHandle& Other) const
	{
		return Handle == Other.Handle;
	}

	bool operator!=(const FTimerHandle& Other) const
	{
		return Handle != Other.Handle;
	}

	static const uint32 IndexBits = 24;
	static const uint32 SerialNumberBits = 40;

	static_assert(IndexBits + SerialNumberBits == 64, "The space for the timer index and serial number should total 64 bits");

	static const int32  MaxIndex = (int32)1 << IndexBits;
	static const uint64 MaxSerialNumber = (uint64)1 << SerialNumberBits;

	void SetIndexAndSerialNumber(int32 Index, uint64 SerialNumber)
	{
		Handle = (SerialNumber << IndexBits) | (uint64)(uint32)Index;
	}

	FORCEINLINE int32 GetIndex() const
	{
		return (int32)(Handle & (uint64)(MaxIndex - 1));
	}

	FORCEINLINE uint64 GetSerialNumber() const
	{
		return Handle >> IndexBits;
	}

	uint64 Handle;
};

struct FZiplinePawnState final
{
public:
	TWeakObjectPtr<class AFortAthenaZiplineBase>  Zipline;                                           // 0x0000(0x0008)(ZeroConstructor, IsPlainOldData, NoDestructor, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	TWeakObjectPtr<class AFortAthenaZiplineBase>  PreviousZipline;                                   // 0x0008(0x0008)(ZeroConstructor, IsPlainOldData, RepSkip, NoDestructor, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	TWeakObjectPtr<class UPrimitiveComponent>     InteractComponent;                                 // 0x0010(0x0008)(ExportObject, ZeroConstructor, InstancedReference, IsPlainOldData, NoDestructor, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	bool                                          bIsZiplining;                                      // 0x0018(0x0001)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	bool                                          bJumped;                                           // 0x0019(0x0001)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	bool                                          bReachedEnd;                                       // 0x001A(0x0001)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	uint8                                         Pad_1B[0x1];                                       // 0x001B(0x0001)(Fixing Size After Last Property [ Dumper-7 ])
	int32                                         AuthoritativeValue;                                // 0x001C(0x0004)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	struct FVector                                SocketOffset;                                      // 0x0020(0x0018)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	float                                         TimeZipliningBegan;                                // 0x0038(0x0004)(ZeroConstructor, IsPlainOldData, RepSkip, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	float                                         TimeZipliningEndedFromJump;                        // 0x003C(0x0004)(ZeroConstructor, IsPlainOldData, RepSkip, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	struct FTimerHandle                           RemoveZiplineFromIgnoreWhenMovingTimerHandle;      // 0x0040(0x0008)(RepSkip, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	uint8                                         Pad_48[0x18];                                      // 0x0048(0x0018)(Fixing Struct Size After Last Property [ Dumper-7 ])
};

class UGAB_AthenaDBNO_C : public UObject
{
public:
	DECLARE_STATIC_CLASS(UGAB_AthenaDBNO_C)
	DECLARE_DEFAULT_OBJECT(UGAB_AthenaDBNO_C)
};

class UFortGameplayAbility : public UObject
{
public:
	DECLARE_STATIC_CLASS(UFortGameplayAbility)
	DECLARE_DEFAULT_OBJECT(UFortGameplayAbility)
};

class AFortPlayerPawn : public AFortPawn
{
public:
	DEFINE_BOOL(AFortPlayerPawn, bIsSkydiving);
	DEFINE_MEMBER(FGameplayTag, AFortPlayerPawn, EventReviveTag)
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
	static void ServerSendZiplineState(AFortPlayerPawn* Pawn, FFrame& Stack);
	
	DefHookOg(void, ServerReviveFromDBNO, AFortPlayerPawn* Pawn, FFrame& Stack);
	DefHookOg(void, ReloadWeapon, AFortWeapon* Weapon, int AmmoToRemove);
	DefHookOg(void, CompletePickupAnimation, AFortPickup* Pickup);
	DefHookOg(void, GiveItemToInventoryOwner, UObject*, FFrame&);
	DefHookOg(void, NetMulticast_Athena_BatchedDamageCues, AFortPlayerPawn*, FAthenaBatchedDamageGameplayCues_Shared, FAthenaBatchedDamageGameplayCues_NonShared);
};

class UGA_Athena_MedConsumable_Parent_C : public UFortGameplayAbility
{
public:
	DEFINE_PTR(AFortPlayerPawn, UGA_Athena_MedConsumable_Parent_C, PlayerPawn);
	DEFINE_BOOL(UGA_Athena_MedConsumable_Parent_C, HealsHealth)
	DEFINE_BOOL(UGA_Athena_MedConsumable_Parent_C, HealsShields)
	DEFINE_MEMBER(float, UGA_Athena_MedConsumable_Parent_C, HealthHealAmount)
public:
	DefHookOg(void, Athena_MedConsumable_Triggered, UGA_Athena_MedConsumable_Parent_C* Consumable);
	DECLARE_STATIC_CLASS(UGA_Athena_MedConsumable_Parent_C)
	DECLARE_DEFAULT_OBJECT(UGA_Athena_MedConsumable_Parent_C)
};

class AFortPlayerPawnAthena : public AFortPlayerPawn
{
public:
	DECLARE_STATIC_CLASS(AFortPlayerPawnAthena)
	DECLARE_DEFAULT_OBJECT(AFortPlayerPawnAthena)
};

class ADefaultPawn : public APawn
{
public:
	DECLARE_DEFAULT_OBJECT(ADefaultPawn)
	DECLARE_STATIC_CLASS(ADefaultPawn)
};
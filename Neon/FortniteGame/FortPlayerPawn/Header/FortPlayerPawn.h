#pragma once
#include "pch.h"
#include "FortniteGame/FortPlayerController/Header/FortPlayerController.h"

struct FFortAthenaLoadout;

class ACharacter : public APawn
{
};

class AFGF_Character : public ACharacter
{
    
};

class AFortPawn : public AFGF_Character
{
public:
    DEFINE_BOOL(AFortPawn, bMovingEmote);
    DEFINE_BOOL(AFortPawn, bMovingEmoteForwardOnly);
    DEFINE_BOOL(AFortPawn, bMovingEmoteFollowingOnly);
    DEFINE_MEMBER(float, AFortPawn, EmoteWalkSpeed);
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
public:
    static void ServerHandlePickupInfo(AFortPlayerPawn* Pawn, FFrame& Stack);
	static void ServerHandlePickup(AFortPlayerPawn* Pawn, FFrame& Stack);
	DefHookOg(void, CompletePickupAnimation, AFortPickup* Pickup);
};

class ADefaultPawn : public APawn
{
    
};
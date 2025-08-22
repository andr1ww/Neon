#pragma once
#include "pch.h"

#include "Engine/Transform/Header/Transform.h"

struct FHitResult final
{
public:
	uint8_t Padding[0x88]; // 0xe8 on LWC builds
};

class USceneComponent : public UObject
{
public:
	void K2_SetWorldLocationAndRotation(const struct FVector& NewLocation, const struct FRotator& NewRotation, bool bSweep, struct FHitResult* SweepHitResult, bool bTeleport)
	{
		static SDK::UFunction* Func = nullptr;
		SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("SceneComponent", "K2_SetWorldLocationAndRotation");

		if (Func == nullptr)
			Func = Info.Func;
		if (!Func)
			return;

		struct SceneComponent_K2_SetWorldLocationAndRotation final
		{
		public:
			struct FVector                                NewLocation;                                       // 0x0000(0x000C)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			struct FRotator                               NewRotation;                                       // 0x000C(0x000C)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, NativeAccessSpecifierPublic)
			bool                                          bSweep;                                            // 0x0018(0x0001)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			uint8                                         Pad_19[0x3];                                       // 0x0019(0x0003)(Fixing Size After Last Property [ Dumper-7 ])
			struct FHitResult                             SweepHitResult;                                    // 0x001C(0x008C)(Parm, OutParm, IsPlainOldData, NoDestructor, ContainsInstancedReference, NativeAccessSpecifierPublic)
			bool                                          bTeleport;                                         // 0x00A8(0x0001)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			uint8                                         Pad_A9[0x3];                                       // 0x00A9(0x0003)(Fixing Struct Size After Last Property [ Dumper-7 ])
		};
		
		SceneComponent_K2_SetWorldLocationAndRotation Params;
		Params.NewLocation = std::move(NewLocation);
		Params.NewRotation = std::move(NewRotation);
		Params.bSweep = bSweep;
		Params.bTeleport = bTeleport;
    
		this->ProcessEvent(Func, &Params);
	}
public:
    DEFINE_PTR(USceneComponent, USceneComponent, RootComponent);
    DECLARE_DEFAULT_OBJECT(USceneComponent)
    DECLARE_STATIC_CLASS(USceneComponent)
};

// NumValues: 0x0003
enum class ERotatorQuantization : uint8
{
	ByteComponents                           = 0,
	ShortComponents                          = 1,
	ERotatorQuantization_MAX                 = 2,
};

// Enum Engine.EVectorQuantization
// NumValues: 0x0004
enum class EVectorQuantization : uint8
{
	RoundWholeNumber                         = 0,
	RoundOneDecimal                          = 1,
	RoundTwoDecimals                         = 2,
	EVectorQuantization_MAX                  = 3,
};

struct FRepMovement final
{
public:
	struct FVector                                LinearVelocity;                                    // 0x0000(0x000C)(ZeroConstructor, Transient, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	struct FVector                                AngularVelocity;                                   // 0x000C(0x000C)(ZeroConstructor, Transient, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	struct FVector                                Location;                                          // 0x0018(0x000C)(ZeroConstructor, Transient, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	struct FRotator                               Rotation;                                          // 0x0024(0x000C)(ZeroConstructor, Transient, IsPlainOldData, NoDestructor, NativeAccessSpecifierPublic)
	uint8                                         bSimulatedPhysicSleep : 1;                         // 0x0030(0x0001)(BitIndex: 0x00, PropSize: 0x0001 (Transient, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic))
	uint8                                         bRepPhysics : 1;                                   // 0x0030(0x0001)(BitIndex: 0x01, PropSize: 0x0001 (Transient, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic))
	EVectorQuantization                           LocationQuantizationLevel;                         // 0x0031(0x0001)(Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData, NoDestructor, AdvancedDisplay, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	EVectorQuantization                           VelocityQuantizationLevel;                         // 0x0032(0x0001)(Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData, NoDestructor, AdvancedDisplay, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	ERotatorQuantization                          RotationQuantizationLevel;                         // 0x0033(0x0001)(Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData, NoDestructor, AdvancedDisplay, HasGetValueTypeHash, NativeAccessSpecifierPublic)
};

class AActor : public UObject
{
public:
	DEFINE_MEMBER(FRepMovement, AActor, ReplicatedMovement);
    DEFINE_PTR(USceneComponent, AActor, RootComponent);
    DEFINE_BOOL(AActor, bHidden);
public:
    FVector GetActorLocation();
    FVector GetActorForwardVector();
    FVector GetActorUpVector();
    FVector GetActorRightVector();
	bool K2_SetActorLocation(const struct FVector& NewLocation, bool bSweep, struct FHitResult* SweepHitResult, bool bTeleport);
    void K2_DestroyActor();
	FVector K2_GetActorLocation();
    FRotator K2_GetActorRotation();
    FTransform GetTransform();
	void OnRep_ReplicatedMovement();
    AActor* GetOwner();
    bool K2_SetActorRotation(const struct FRotator& NewRotation, bool bTeleportPhysics);
    void K2_TeleportTo(const FVector Location, const FRotator Rot);
    float GetDistanceTo(AActor* Actor);
public:
    DECLARE_STATIC_CLASS(AActor);
    DECLARE_DEFAULT_OBJECT(AActor);
};
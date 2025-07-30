#pragma once
#include "pch.h"

#include "Engine/Actor/Header/Actor.h"
#include "Engine/DataTable/Header/DataTable.h"
#include "FortniteGame/FortPlayerController/Header/FortPlayerController.h"
#include "FortniteGame/FortKismetLibrary/Header/FortKismetLibrary.h"
#include "FortniteGame/Common/Header/Tags.h"

class UFortWeaponItemDefinition;
class AFortPlayerControllerAthena;
struct FGameplayEffectContextHandle;

struct FBuildingClassData final
{
public:
    DEFINE_MEMBER(TSubclassOf<class ABuildingActor>, FBuildingClassData, BuildingClass); // 0x0000(0x0008)(ZeroConstructor, Transient, NoDestructor, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    int32                                         PreviousBuildingLevel;                             // 0x0008(0x0004)(ZeroConstructor, Transient, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    DEFINE_MEMBER(uint8, FBuildingClassData, UpgradeLevel);
    uint8                                         Pad_D[0x3];                                        // 0x000D(0x0003)(Fixing Struct Size After Last Property [ Dumper-7 ])
};

struct FCreateBuildingActorData { uint32_t BuildingClassHandle; FVector BuildLoc; FRotator BuildRot; bool bMirrored; };

class AFortWeapon : public AActor
{
public:
	DEFINE_PTR(UFortWeaponItemDefinition, AFortWeapon, WeaponData)
};

struct FHitResult final
{
public:
	uint8_t Padding[0x88]; // 0xe8 on LWC builds
};

class ABuildingActor : public AActor
{
public:
    DEFINE_MEMBER(bool, ABuildingActor, bPlayerPlaced);
};

class ABuildingSMActor : public ABuildingActor
{
public:
    DEFINE_MEMBER(FCurveTableRowHandle, ABuildingSMActor, BuildingResourceAmountOverride)
    DEFINE_MEMBER(EFortResourceType, ABuildingSMActor, ResourceType)
public:
    DefHookOg(void, OnDamageServer, ABuildingSMActor*, float, FGameplayTagContainer, FVector, FHitResult, AFortPlayerControllerAthena*, AActor*, FGameplayEffectContextHandle);
public:
    DECLARE_STATIC_CLASS(ABuildingSMActor)
    DECLARE_DEFAULT_OBJECT(ABuildingSMActor)
};


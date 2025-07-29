#pragma once
#include "pch.h"

#include "Engine/Actor/Header/Actor.h"
#include "Engine/DataTable/Header/DataTable.h"
#include "FortniteGame/FortPlayerController/Header/FortPlayerController.h"
#include "FortniteGame/FortKismetLibrary/Header/FortKismetLibrary.h"

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

struct FCreateBuildingActorData final
{
public:
    uint32                                        BuildingClassHandle;                               // 0x0000(0x0004)(ZeroConstructor, Transient, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    uint8                                         Pad_4[0x4];                                        // 0x0004(0x0004)(Fixing Size After Last Property [ Dumper-7 ])
    DEFINE_MEMBER(FVector_NetQuantize10, FCreateBuildingActorData, BuildLoc); // 0x0008(0x0018)(Transient, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    DEFINE_MEMBER(FRotator, FCreateBuildingActorData, BuildRot); // 0x0020(0x0018)(Transient, NoDestructor, NativeAccessSpecifierPublic)
    bool                                          bMirrored;                                         // 0x0038(0x0001)(ZeroConstructor, Transient, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    uint8                                         Pad_39[0x3];                                       // 0x0039(0x0003)(Fixing Size After Last Property [ Dumper-7 ])
    float                                         SyncKey;                                           // 0x003C(0x0004)(ZeroConstructor, Transient, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    DEFINE_MEMBER(FBuildingClassData, FCreateBuildingActorData, BuildingClassData); // 0x0040(0x0010)(Transient, NoDestructor, NativeAccessSpecifierPrivate)
};

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

struct FGameplayTagContainer final
{
public:
    TArray<struct FGameplayTag>                   GameplayTags;                                      // 0x0000(0x0010)(Edit, BlueprintVisible, ZeroConstructor, EditConst, SaveGame, Protected, NativeAccessSpecifierProtected)
    TArray<struct FGameplayTag>                   ParentTags;                                        // 0x0010(0x0010)(ZeroConstructor, Transient, Protected, NativeAccessSpecifierProtected)
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
};


#pragma once
#include "pch.h"

#include "Engine/Actor/Header/Actor.h"
#include "Engine/DataTable/Header/DataTable.h"
#include "FortniteGame/FortPlayerController/Header/FortPlayerController.h"

class UFortWeaponItemDefinition;
class AFortPlayerControllerAthena;
struct FGameplayEffectContextHandle;

class AFortWeapon : public AActor
{
public:
	DEFINE_PTR(UFortWeaponItemDefinition, AFortWeapon, WeaponData)
};

struct FGameplayTag
{
public:
	class FName                                   TagName;                                           // 0x0000(0x0004)(Edit, ZeroConstructor, EditConst, SaveGame, IsPlainOldData, NoDestructor, Protected, HasGetValueTypeHash, NativeAccessSpecifierProtected)
};

struct FHitResult final
{
public:
	
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
public:
    static void OnDamageServer(ABuildingSMActor*, float, FGameplayTagContainer, FVector, FHitResult, AFortPlayerControllerAthena*, AActor*, FGameplayEffectContextHandle);
};


inline void (*OnDamageServerOG)(ABuildingSMActor*, float, FGameplayTagContainer, FVector, FHitResult, AFortPlayerControllerAthena*, AActor*, FGameplayEffectContextHandle);
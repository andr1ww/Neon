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
    DEFINE_MEMBER(FGuid, AFortWeapon, ItemEntryGuid)
    DEFINE_PTR(UFortWeaponItemDefinition, AFortWeapon, WeaponData);
    DEFINE_MEMBER(int32, AFortWeapon, AmmoCount);
public:
    DECLARE_STATIC_CLASS(AFortWeapon)
    DECLARE_DEFAULT_OBJECT(AFortWeapon)
};

struct FHitResult final
{
public:
	uint8_t Padding[0x88]; // 0xe8 on LWC builds
};

class ABuildingActor : public AActor
{
public:
    float GetHealth() {
        static class UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("BuildingActor", "GetHealth");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return 0.f;

        struct BuildingActor_GetHealth final
        {
        public:
            float ReturnValue;
        };
        BuildingActor_GetHealth Params;

        this->ProcessEvent(Func, &Params);

        return Params.ReturnValue;
    }

    float GetMaxHealth() {
        static class UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("BuildingActor", "GetMaxHealth");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return 0.f;

        struct BuildingActor_GetMaxHealth final
        {
        public:
            float ReturnValue;
        };
        BuildingActor_GetMaxHealth Params;

        this->ProcessEvent(Func, &Params);

        return Params.ReturnValue;
    }

    void InitializeKismetSpawnedBuildingActor(class ABuildingActor* BuildingOwner, class AFortPlayerController* SpawningController, bool bUsePlayerBuildAnimations)
    {
        static class UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("BuildingActor", "InitializeKismetSpawnedBuildingActor");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return;

        struct BuildingActor_InitializeKismetSpawnedBuildingActor final
        {
        public:
            class ABuildingActor*                         BuildingOwner;                                     // 0x0000(0x0008)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            class AFortPlayerController*                  SpawningController;                                // 0x0008(0x0008)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            bool                                          bUsePlayerBuildAnimations;                         // 0x0010(0x0001)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            uint8                                         Pad_11[0x7];                                       // 0x0011(0x0007)(Fixing Struct Size After Last Property [ Dumper-7 ])
        } Params {};

        Params.BuildingOwner = BuildingOwner;
        Params.SpawningController = SpawningController;
        Params.bUsePlayerBuildAnimations = bUsePlayerBuildAnimations;

        this->ProcessEvent(Func, &Params);
    }
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


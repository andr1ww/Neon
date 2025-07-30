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

class UAthenaCharacterItemDefinition : public UObject 
{
public:
    DEFINE_PTR(UFortHeroType, UAthenaCharacterItemDefinition, HeroDefinition);

public:
    DECLARE_STATIC_CLASS(UAthenaCharacterItemDefinition)
    DECLARE_DEFAULT_OBJECT(UAthenaCharacterItemDefinition)
};


class UFortWeaponMeleeItemDefinition : public UFortItemDefinition
{
    
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

class AFortPlayerController : public APlayerController
{
public:
    DEFINE_PTR(AFortInventory, AFortPlayerController, WorldInventory);
    DEFINE_MEMBER(FFortAthenaLoadout, AFortPlayerController, CosmeticLoadoutPC);
    DEFINE_PTR(AFortPlayerPawn, AFortPlayerController, MyFortPawn);
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
    DEFINE_MEMBER(TArray<UFortItemDefinition*>, UFortAthenaAIBotInventoryItems, Items);
};

class UAthenaDanceItemDefinition : public UFortMontageItemDefinitionBase
{
public:
    DEFINE_BOOL(UAthenaDanceItemDefinition, bMovingEmote);
    DEFINE_BOOL(UAthenaDanceItemDefinition, bMoveForwardOnly);
    DEFINE_BOOL(UAthenaDanceItemDefinition, bMoveFollowingOnly);
    DEFINE_MEMBER(float, UAthenaDanceItemDefinition, WalkForwardSpeed);
    DEFINE_PTR(UClass, UAthenaDanceItemDefinition, CustomDanceAbility);
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

class UGAB_Spray_Generic_C : public UGAB_Emote_Generic_C
{
public:
    DECLARE_DEFAULT_OBJECT(UGAB_Spray_Generic_C);
    DECLARE_STATIC_CLASS(UGAB_Spray_Generic_C);
};

class UActorComponent final : public UObject
{
public:
    AActor* GetOwner()
    {
        static SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("ActorComponent", "GetOwner");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return nullptr;
        AActor* Owner;
        
        SDK::StaticClassImpl("ActorComponent")->GetClassDefaultObject()->ProcessEvent(Func, &Owner);
    }
};

enum class EFortTeam : uint8
{
    Spectator                                = 0,
    HumanCampaign                            = 1,
    Monster                                  = 2,
    HumanPvP_Team1                           = 3,
    HumanPvP_Team2                           = 4,
    MAX                                      = 255,
};

class UFortEditToolItemDefinition : public UObject
{
public:
    DECLARE_STATIC_CLASS(UFortEditToolItemDefinition)
    DECLARE_DEFAULT_OBJECT(UFortEditToolItemDefinition)
};

class AFortWeap_EditingTool : public UObject
{
    
};

struct FFortPlayerDeathReport final
{
public:
	float                                         ServerTimeForRespawn;                              // 0x0000(0x0004)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, Transient, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	float                                         ServerTimeForResurrect;                            // 0x0004(0x0004)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, Transient, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	float                                         LethalDamage;                                      // 0x0008(0x0004)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, Transient, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	uint8                                         Pad_C[0x4];                                        // 0x000C(0x0004)(Fixing Size After Last Property [ Dumper-7 ])
    DEFINE_PTR(AFortPlayerStateAthena, FFortPlayerDeathReport, KillerPlayerState); // 0x0010(0x0008)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, Transient, NoDestructor, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    DEFINE_MEMBER(TWeakObjectPtr<AFortPlayerPawn>, FFortPlayerDeathReport, KillerPawn); // 0x0018(0x0008)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, Transient, IsPlainOldData, NoDestructor, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	float                                         KillerHealthPercent;                               // 0x0020(0x0004)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, Transient, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	float                                         KillerShieldPercent;                               // 0x0024(0x0004)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, Transient, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	float                                         KillerOvershieldPercent;                           // 0x0028(0x0004)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, Transient, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	uint8                                         Pad_2C[0x4];                                       // 0x002C(0x0004)(Fixing Size After Last Property [ Dumper-7 ])
	class UFortWeaponItemDefinition*              KillerWeapon;                                      // 0x0030(0x0008)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, Transient, NoDestructor, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	TWeakObjectPtr<class AActor>                  DamageCauser;                                      // 0x0038(0x0008)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, Transient, IsPlainOldData, NoDestructor, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	uint8                                         bDroppedBackpack : 1;                              // 0x0040(0x0001)(BitIndex: 0x00, PropSize: 0x0001 (BlueprintVisible, BlueprintReadOnly, Transient, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic))
	uint8                                         bNotifyUI : 1;                                     // 0x0040(0x0001)(BitIndex: 0x01, PropSize: 0x0001 (BlueprintVisible, BlueprintReadOnly, Transient, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic))
	uint8                                         Pad_41[0x7];                                       // 0x0041(0x0007)(Fixing Size After Last Property [ Dumper-7 ])
    FGameplayTagContainer                  Tags;                                              // 0x0048(0x0020)(BlueprintVisible, BlueprintReadOnly, Transient, NativeAccessSpecifierPublic)
	struct FVector                                ViewLocationAtTimeOfDeath;                         // 0x0068(0x0018)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, Transient, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	struct FRotator                               ViewRotationAtTimeOfDeath;                         // 0x0080(0x0018)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, Transient, IsPlainOldData, NoDestructor, NativeAccessSpecifierPublic)
};

class AFortPlayerControllerAthena : public AFortPlayerController 
{
public:
    DEFINE_PTR(AFortPlayerStateAthena, AFortPlayerControllerAthena, PlayerState);
    DEFINE_BOOL(AFortPlayerControllerAthena, bHasServerFinishedLoading);
public:
    static void ServerAttemptAircraftJump(UActorComponent* Comp, FFrame& Stack);
    static void ServerAcknowledgePossession(AFortPlayerControllerAthena* PlayerController, FFrame& Stack);
    static void ServerExecuteInventoryItem(AFortPlayerControllerAthena* PlayerController, FFrame& Stack);
    static void ServerPlayEmoteItem(AFortPlayerControllerAthena* PlayerController, FFrame& Stack);
    static void ServerCreateBuildingActor(AFortPlayerControllerAthena* PlayerController, FFrame& Stack);
    static void ServerBeginEditingBuildingActor(AFortPlayerControllerAthena* PlayerController, FFrame& Stack);
    static void ServerEditBuildingActor(AFortPlayerControllerAthena* PlayerController, FFrame& Stack);
    static void ServerEndEditingBuildingActor(AFortPlayerControllerAthena* PlayerController, FFrame& Stack);
    static void ServerRepairBuildingActor(AFortPlayerControllerAthena* PlayerController, FFrame& Stack);
    DefHookOg(void, ClientOnPawnDied, AFortPlayerControllerAthena* PlayerController, FFortPlayerDeathReport& DeathReport);
};
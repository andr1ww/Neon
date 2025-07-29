#pragma once
#include "pch.h"
#include "Neon/Runtime/Runtime.h"
#include "Engine/PlayerController/Header/PlayerController.h"
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
};


class UFortWeaponMeleeItemDefinition : public UFortItemDefinition
{
    
};

class UAthenaPickaxeItemDefinition : public UObject 
{
public:
    DEFINE_PTR(UFortWeaponMeleeItemDefinition, UAthenaPickaxeItemDefinition, WeaponDefinition);
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
};

class AFortWeap_EditingTool : public UObject
{
    
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
};

class AAIController : public AController
{
    
};

class AFortAthenaAIBotController : public AAIController
{
    
};
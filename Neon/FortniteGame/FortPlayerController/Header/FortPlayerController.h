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

struct FAthenaRewardResult final
{
public:
    DEFINE_MEMBER(int32, FAthenaRewardResult, LevelsGained);
    DEFINE_MEMBER(int32, FAthenaRewardResult, BookLevelsGained);
    DEFINE_MEMBER(int32, FAthenaRewardResult, TotalSeasonXpGained);
    DEFINE_MEMBER(int32, FAthenaRewardResult, TotalBookXpGained);
    DEFINE_MEMBER(int32, FAthenaRewardResult, PrePenaltySeasonXpGained);
    uint8                                         Pad_14[0x4];
    DEFINE_MEMBER(float, FAthenaRewardResult, AntiAddictionMultiplier);
    uint8                                         Pad_3C[0x4];
};

struct FAthenaMatchStats final
{
public:
    DEFINE_MEMBER(FString, FAthenaMatchStats, StatBucket);
    DEFINE_MEMBER(FString, FAthenaMatchStats, MatchID);
    DEFINE_MEMBER(FString, FAthenaMatchStats, MatchEndTime);
    DEFINE_MEMBER(FString, FAthenaMatchStats, MatchPlatform);
    int32                                         Stats[0x14];
    DEFINE_BOOL(FAthenaMatchStats, bIsValid);
    uint8                                         Pad_D5[0x3];
    DEFINE_MEMBER(FString, FAthenaMatchStats, FactionTag);
};

struct FAthenaMatchTeamStats final
{
public:
    DEFINE_MEMBER(int32, FAthenaMatchTeamStats, Place);
    DEFINE_MEMBER(int32, FAthenaMatchTeamStats, TotalPlayers);
};

class UAthenaPlayerMatchReport : public UObject
{
public:
    DEFINE_MEMBER(FAthenaRewardResult, UAthenaPlayerMatchReport, EndOfMatchResults); // 0x0000(0x0038)(BlueprintVisible, BlueprintReadOnly, Transient, NativeAccessSpecifierPublic)
    DEFINE_MEMBER(FAthenaMatchTeamStats, UAthenaPlayerMatchReport, TeamStats); // 0x0000(0x0028)(BlueprintVisible, BlueprintReadOnly, Transient, NativeAccessSpecifierPublic)
    DEFINE_MEMBER(FAthenaMatchStats, UAthenaPlayerMatchReport, MatchStats); // 0x0028(0x00A0)(BlueprintVisible, BlueprintReadOnly, Transient, NativeAccessSpecifierPublic)
public:
    DECLARE_STATIC_CLASS(UAthenaPlayerMatchReport);
    DECLARE_DEFAULT_OBJECT(UAthenaPlayerMatchReport);
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
        struct { UActorComponent* Comp; AActor* ReturnValue; } GetOwnerParams{ this };
        
        this->ProcessEvent(Func, &GetOwnerParams);

        return GetOwnerParams.ReturnValue;
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
    DEFINE_MEMBER(float, FFortPlayerDeathReport, ServerTimeForRespawn);
    DEFINE_MEMBER(float, FFortPlayerDeathReport, ServerTimeForResurrect);
    DEFINE_MEMBER(float, FFortPlayerDeathReport, LethalDamage);
    uint8                                         Pad_C[0x4];
    DEFINE_PTR(AFortPlayerStateAthena, FFortPlayerDeathReport, KillerPlayerState);
    DEFINE_PTR(AFortPlayerPawn, FFortPlayerDeathReport, KillerPawn);
    DEFINE_MEMBER(float, FFortPlayerDeathReport, KillerHealthPercent);
    DEFINE_MEMBER(float, FFortPlayerDeathReport, KillerShieldPercent);
    DEFINE_MEMBER(float, FFortPlayerDeathReport, KillerOvershieldPercent);
    uint8                                         Pad_2C[0x4];
    DEFINE_PTR(UFortWeaponItemDefinition, FFortPlayerDeathReport, KillerWeapon);
    DEFINE_PTR(AActor, FFortPlayerDeathReport, DamageCauser);
    uint8                                         bDroppedBackpack : 1;
    uint8                                         bNotifyUI : 1;
    uint8                                         Pad_41[0x7];
    DEFINE_MEMBER(FGameplayTagContainer, FFortPlayerDeathReport, Tags);
    DEFINE_MEMBER(FVector, FFortPlayerDeathReport, ViewLocationAtTimeOfDeath);
    DEFINE_MEMBER(FRotator, FFortPlayerDeathReport, ViewRotationAtTimeOfDeath);
};

class UFortPlayerControllerAthenaXPComponent : public UObject
{
public:
    
};

class AFortProjectileBase : public UObject
{
public:
    DECLARE_STATIC_CLASS(AFortProjectileBase)
    DECLARE_DEFAULT_OBJECT(AFortProjectileBase)
};

class AFortPlayerControllerAthena : public AFortPlayerController 
{
public:
    DEFINE_PTR(UFortPlayerControllerAthenaXPComponent, AFortPlayerControllerAthena, XPComponent);
    DEFINE_PTR(UAthenaPlayerMatchReport, AFortPlayerControllerAthena, MatchReport);
    DEFINE_PTR(AFortPlayerStateAthena, AFortPlayerControllerAthena, PlayerState);
    DEFINE_BOOL(AFortPlayerControllerAthena, bHasServerFinishedLoading);
public:
    static void ServerAttemptAircraftJump(UActorComponent* Comp, FFrame& Stack);
    static void ServerLoadingScreenDropped(AFortPlayerControllerAthena* PlayerController, FFrame& Stack);
    static void ServerAcknowledgePossession(AFortPlayerControllerAthena* PlayerController, FFrame& Stack);
    static void ServerExecuteInventoryItem(AFortPlayerControllerAthena* PlayerController, FFrame& Stack);
    static void ServerPlayEmoteItem(AFortPlayerControllerAthena* PlayerController, FFrame& Stack);
    static void ServerCreateBuildingActor(AFortPlayerControllerAthena* PlayerController, FFrame& Stack);
    static void ServerBeginEditingBuildingActor(AFortPlayerControllerAthena* PlayerController, FFrame& Stack);
    static void ServerEditBuildingActor(AFortPlayerControllerAthena* PlayerController, FFrame& Stack);
    static void ServerEndEditingBuildingActor(AFortPlayerControllerAthena* PlayerController, FFrame& Stack);
    static void ServerRepairBuildingActor(AFortPlayerControllerAthena* PlayerController, FFrame& Stack);
    DefHookOg(void, ClientOnPawnDied, AFortPlayerControllerAthena* PlayerController, FFortPlayerDeathReport& DeathReport);
public:
    DECLARE_STATIC_CLASS(AFortPlayerControllerAthena)
    DECLARE_DEFAULT_OBJECT(AFortPlayerControllerAthena)
public:
    void ClientSendEndBattleRoyaleMatchForPlayer(bool bSuccess, const struct FAthenaRewardResult& Result)
    {
        static SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortPlayerControllerAthena", "ClientSendEndBattleRoyaleMatchForPlayer");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return;

        struct FortPlayerControllerAthena_ClientSendEndBattleRoyaleMatchForPlayer final
        {
        public:
            bool                                          bSuccess;                                          // 0x0000(0x0001)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            uint8                                         Pad_2B47[0x7];                                     // 0x0001(0x0007)(Fixing Size After Last Property [ Dumper-7 ])
            struct FAthenaRewardResult                    Result;                                            // 0x0008(0x0040)(ConstParm, Parm, ReferenceParm, NativeAccessSpecifierPublic)
        } FortPlayerControllerAthena_ClientSendEndBattleRoyaleMatchForPlayer_Params{ };

        FortPlayerControllerAthena_ClientSendEndBattleRoyaleMatchForPlayer_Params.bSuccess = bSuccess;
        FortPlayerControllerAthena_ClientSendEndBattleRoyaleMatchForPlayer_Params.Result = Result;
    
        this->ProcessEvent(Func, &FortPlayerControllerAthena_ClientSendEndBattleRoyaleMatchForPlayer_Params);
    }

    void ClientSendMatchStatsForPlayer(const struct FAthenaMatchStats& Result)
    {
        static SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortPlayerControllerAthena", "ClientSendEndBattleRoyaleMatchForPlayer");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return;

        struct FortPlayerControllerAthena_ClientSendMatchStatsForPlayer final
        {
        public:
            struct FAthenaMatchStats                  TeamStats;                                         // 0x0000(0x0008)(ConstParm, Parm, ReferenceParm, NoDestructor, NativeAccessSpecifierPublic)
        } FortPlayerControllerAthena_ClientSendMatchStatsForPlayer_Params{ Result };
    
        this->ProcessEvent(Func, &FortPlayerControllerAthena_ClientSendMatchStatsForPlayer_Params);
    }

    void ClientSendTeamStatsForPlayer(const struct FAthenaMatchTeamStats& Result)
    {
        static SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortPlayerControllerAthena", "ClientSendTeamStatsForPlayer");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return;

        struct FortPlayerControllerAthena_ClientSendTeamStatsForPlayer final
        {
        public:
            struct FAthenaMatchTeamStats                  TeamStats;                                         // 0x0000(0x0008)(ConstParm, Parm, ReferenceParm, NoDestructor, NativeAccessSpecifierPublic)
        } FortPlayerControllerAthena_ClientSendTeamStatsForPlayer_Params{ Result };
    
        this->ProcessEvent(Func, &FortPlayerControllerAthena_ClientSendTeamStatsForPlayer_Params);
    }
};
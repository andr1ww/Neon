#pragma once
#include "pch.h"

#include "Engine/Array/Header/Array.h"
#include "Engine/UniqueID/Header/UniqueId.h"
#include "FortniteGame/BuildingSMActor/Header/BuildingSMActor.h"
#include "FortniteGame/FortAthenaMapInfo/Header/FortAthenaMapInfo.h"
#include "FortniteGame/FortPlaylistAthena/Header/FortPlaylistAthena.h"

enum class EAthenaGamePhase : uint8
{
    None = 0,
    Setup = 1,
    Warmup = 2,
    Aircraft = 3,
    SafeZones = 4,
    EndGame = 5,
    Count = 6,
    EAthenaGamePhase_MAX = 7,
};

// NumValues: 0x0005
enum class EAirCraftBehavior : uint8
{
    Default = 0,
    OpposingAirCraftForEachTeam = 1,
    FlyTowardFirstCircleCenter = 2,
    NoAircraft = 3,
    EAirCraftBehavior_MAX = 4,
};

enum class EAthenaGamePhaseStep : uint8
{
    None                                     = 0,
    Setup                                    = 1,
    Warmup                                   = 2,
    GetReady                                 = 3,
    BusLocked                                = 4,
    BusFlying                                = 5,
    StormForming                             = 6,
    StormHolding                             = 7,
    StormShrinking                           = 8,
    Countdown                                = 9,
    FinalCountdown                           = 10,
    EndGame                                  = 11,
    Count                                    = 12,
    EAthenaGamePhaseStep_MAX                 = 13,
};

struct FGameMemberInfo final : public FFastArraySerializerItem
{
public:
    uint8                                         SquadId;                                           // 0x000C(0x0001)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    uint8                                         TeamIndex;                                         // 0x000D(0x0001)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    uint8                                         Pad_1B81[0x2];                                     // 0x000E(0x0002)(Fixing Size After Last Property [ Dumper-7 ])
     FUniqueNetIdRepl                       MemberUniqueId;                                    // 0x0010(0x0028)(HasGetValueTypeHash, NativeAccessSpecifierPublic)
};

class AFortGameState : public UObject
{
public:
    DEFINE_BOOL(AFortGameState, bDBNOEnabledForGameMode)
    DEFINE_MEMBER(int32, AFortGameState, WorldLevel);
    DEFINE_MEMBER(TArray<struct FAdditionalLevelStreamed>, AFortGameState, AdditionalPlaylistLevelsStreamed);
public:
    void OnRep_AdditionalPlaylistLevelsStreamed()
    {
        static SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortGameState", "OnRep_AdditionalPlaylistLevelsStreamed");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return;
		
        this->ProcessEvent(Func, nullptr);
    }

    void OnFinishedStreamingAdditionalPlaylistLevel()
    {
        static SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortGameState", "OnFinishedStreamingAdditionalPlaylistLevel");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return;
		
        this->ProcessEvent(Func, nullptr);
    }
};

struct FGameMemberInfoArray final : public FFastArraySerializer
{
public:
    DEFINE_MEMBER(TArray<FGameMemberInfo>, FGameMemberInfoArray, Members);
    class AFortGameStateAthena*                   OwningGameState;                                   // 0x0118(0x0008)(ZeroConstructor, Transient, IsPlainOldData, RepSkip, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPrivate)
};

using Map = TMap<TSubclassOf<ABuildingActor>, int32>;

class AFortAthenaAircraft : public AActor
{
public:
    DEFINE_MEMBER(FAircraftFlightInfo, AFortAthenaAircraft, FlightInfo)
    DEFINE_MEMBER(float, AFortAthenaAircraft, FlightStartTime)
    DEFINE_MEMBER(float, AFortAthenaAircraft, FlightEndTime)
public:
    DECLARE_STATIC_CLASS(AFortAthenaAircraft)
    DECLARE_DEFAULT_OBJECT(AFortAthenaAircraft)
};

enum class EVolumeState : uint8
{
    Uninitialized                            = 0,
    ReadOnly                                 = 1,
    Initializing                             = 2,
    Ready                                    = 3,
    EVolumeState_MAX                         = 4,
};

class AFortVolume : public AActor
{
public:
    DEFINE_MEMBER(EVolumeState, AFortVolume, VolumeState)
    DECLARE_STATIC_CLASS(AFortVolume)
    DECLARE_DEFAULT_OBJECT(AFortVolume)
};

class AFortAthenaCreativePortal : public ABuildingActor
{
public:
    AFortVolume* GetLinkedVolume()
    {
        static SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortAthenaCreativePortal", "GetLinkedVolume");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return nullptr;

        // 0x0008 (0x0008 - 0x0000)
        struct FortAthenaCreativePortal_GetLinkedVolume final
        {
        public:
            class AFortVolume*                            ReturnValue;                                       // 0x0000(0x0008)(Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
        } Params{};
		
        this->ProcessEvent(Func, &Params);

        return Params.ReturnValue;
    }

    void OnRep_OwningPlayer()
    {
        static SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortAthenaCreativePortal", "OnRep_OwningPlayer");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return;
		
        this->ProcessEvent(Func, nullptr);
    }

    void OnRep_PlayersReady()
    {
        static SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortAthenaCreativePortal", "OnRep_PlayersReady");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return;
		
        this->ProcessEvent(Func, nullptr);
    }

    void OnRep_PortalOpen()
    {
        static SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortAthenaCreativePortal", "OnRep_PortalOpen");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return;
		
        this->ProcessEvent(Func, nullptr);
    }

    DEFINE_MEMBER(TArray<struct FUniqueNetIdRepl>, AFortAthenaCreativePortal, PlayersReady)
    DEFINE_MEMBER(FUniqueNetIdRepl, AFortAthenaCreativePortal, OwningPlayer)
    DEFINE_BOOL(AFortAthenaCreativePortal, bPortalOpen)
    DEFINE_BOOL(AFortAthenaCreativePortal, bUserInitiatedLoad)
    DEFINE_BOOL(AFortAthenaCreativePortal, bInErrorState)

    DECLARE_STATIC_CLASS(AFortAthenaCreativePortal)
    DECLARE_DEFAULT_OBJECT(AFortAthenaCreativePortal)
};

class AFortCreativePortalManager final : public AActor
{
public:
    DEFINE_MEMBER(TArray<class AFortAthenaCreativePortal*>, AFortCreativePortalManager, AllPortals);
    DECLARE_STATIC_CLASS(AFortCreativePortalManager)
    DECLARE_DEFAULT_OBJECT(AFortCreativePortalManager)
};

class AFortGameStateZone : public AFortGameState
{
public:
    DEFINE_PTR(AFortCreativePortalManager, AFortGameStateZone, CreativePortalManager);
    DEFINE_BOOL(AFortGameStateZone, bDBNODeathEnabled)
};

struct TeamsArrayContainer // totally not stolen from rebooot
{
    TArray<TArray<TWeakObjectPtr<AFortPlayerStateAthena>>> TeamsArray; // 13D0
    TArray<int> TeamIdk1; // 13E0
    TArray<int> TeamIndexesArray; // 13F0

    uintptr_t idfk; //(or 2 ints) // 1400

    TArray<TArray<TWeakObjectPtr<AFortPlayerStateAthena>>> SquadsArray; // Index = SquadId // 1408
    TArray<int> SquadIdk1; // 1418
    TArray<int> SquadIdsArray; // 0x1428
};

class AFortSafeZoneIndicator : public UObject
{
public:
    DEFINE_MEMBER(FVector_NetQuantize100, AFortSafeZoneIndicator, NextRadius);
    DEFINE_MEMBER(FVector_NetQuantize100, AFortSafeZoneIndicator, NextCenter);
public:
    DECLARE_STATIC_CLASS(AFortSafeZoneIndicator)
    DECLARE_DEFAULT_OBJECT(AFortSafeZoneIndicator)
};

class AFortGameStateAthena : public AFortGameStateZone
{
public:
    DEFINE_PTR(AFortSafeZoneIndicator, AFortGameStateAthena, SafeZoneIndicator);
    void OnRep_WinningPlayerState()
    {
        static SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortGameStateAthena", "OnRep_WinningPlayerState");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return;
		
        this->ProcessEvent(Func, nullptr);
    }

    void OnRep_WinningTeam()
    {
        static SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortGameStateAthena", "OnRep_WinningTeam");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return;
		
        this->ProcessEvent(Func, nullptr);
    }

    void OnRep_PlayersLeft()
    {
        static SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortGameStateAthena", "OnRep_PlayersLeft");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return;
		
        this->ProcessEvent(Func, nullptr);
    }

    inline TSparseArray<TArray<TWeakObjectPtr<AFortPlayerStateAthena>>>& GetSquadArray()
    {
        return *reinterpret_cast<TSparseArray<TArray<TWeakObjectPtr<AFortPlayerStateAthena>>>*>(__int64(this) + 0x1500);
    }

    inline TSparseArray<TArray<TWeakObjectPtr<AFortPlayerStateAthena>>>& GetTeamArray()
    {
        return *reinterpret_cast<TSparseArray<TArray<TWeakObjectPtr<AFortPlayerStateAthena>>>*>(__int64(this) + 0x14C8);
    }

public:
    DEFINE_MEMBER(float, AFortGameStateAthena, DefaultRebootMachineHotfix)
    DEFINE_MEMBER(int, AFortGameStateAthena, PlayersLeft)
    DEFINE_MEMBER(TArray<AFortAthenaAircraft*>, AFortGameStateAthena, Aircrafts);
    DEFINE_MEMBER(Map, AFortGameStateAthena, AllPlayerBuildableClassesIndexLookup);
    DEFINE_MEMBER(TArray<TSubclassOf<ABuildingActor>>, UWorld, BuildingActorClasses);
    DEFINE_PTR(FPlaylistPropertyArray, AFortGameStateAthena, CurrentPlaylistInfo)
    DEFINE_BOOL(AFortGameStateAthena, bGameModeWillSkipAircraft)
    DEFINE_MEMBER(float, AFortGameStateAthena, SafeZonesStartTime)
    DEFINE_BOOL(AFortGameStateAthena, bAircraftIsLocked)
    DEFINE_PTR(UFortPlaylistAthena, AFortGameStateAthena, CurrentPlaylistData)
    void OnRep_CurrentPlaylistId();
    void OnRep_CurrentPlaylistData();
    void OnRep_CurrentPlaylistInfo();
    DEFINE_MEMBER(int32, AFortGameStateAthena, CurrentPlaylistId);
    DEFINE_MEMBER(int, AFortGameModeAthena, SafeZonePhase);
    DEFINE_PTR(AFortAthenaMapInfo, AFortGameStateAthena, MapInfo);
    DEFINE_MEMBER(FGameMemberInfoArray, AFortGameStateAthena, GameMemberInfoArray);
    DEFINE_MEMBER(EAthenaGamePhaseStep, AFortGameStateAthena, GamePhaseStep);
    DEFINE_MEMBER(EAthenaGamePhase, AFortGameStateAthena, GamePhase);
    DEFINE_MEMBER(EAirCraftBehavior, AFortGameStateAthena, AirCraftBehavior);
public:
    DECLARE_STATIC_CLASS(AFortGameStateAthena);
    DECLARE_DEFAULT_OBJECT(AFortGameStateAthena)
};



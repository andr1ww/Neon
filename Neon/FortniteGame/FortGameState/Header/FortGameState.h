#pragma once
#include "pch.h"

#include "Engine/Array/Header/Array.h"
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

struct FUniqueNetIdWrapper
{
public:
    uint8                                         Pad_18[0x1];                                       // 0x0000(0x0001)(Fixing Struct Size After Last Property [ Dumper-7 ])
};

struct FUniqueNetIdRepl final : public FUniqueNetIdWrapper
{
public:
    uint8                                         Pad_B8[0x17];                                      // 0x0001(0x0017)(Fixing Size After Last Property [ Dumper-7 ])
    TArray<uint8>                                 ReplicationBytes;                                  // 0x0018(0x0010)(ZeroConstructor, Transient, Protected, HasGetValueTypeHash, NativeAccessSpecifierProtected)
};

struct FGameMemberInfo final : public FFastArraySerializerItem
{
public:
    uint8                                         SquadId;                                           // 0x000C(0x0001)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    uint8                                         TeamIndex;                                         // 0x000D(0x0001)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    uint8                                         Pad_1B81[0x2];                                     // 0x000E(0x0002)(Fixing Size After Last Property [ Dumper-7 ])
    struct FUniqueNetIdRepl                       MemberUniqueId;                                    // 0x0010(0x0028)(HasGetValueTypeHash, NativeAccessSpecifierPublic)
};

class AFortGameState : public UObject
{
public:
    DEFINE_MEMBER(TArray<struct FAdditionalLevelStreamed>, AFortGameState, AdditionalPlaylistLevelsStreamed);
};

struct FGameMemberInfoArray final : public FFastArraySerializer
{
public:
    DEFINE_MEMBER(TArray<FGameMemberInfo>, FGameMemberInfoArray, Members);
    class AFortGameStateAthena*                   OwningGameState;                                   // 0x0118(0x0008)(ZeroConstructor, Transient, IsPlainOldData, RepSkip, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPrivate)
};

using Map = TMap<TSubclassOf<ABuildingActor>, int32>;

class AFortGameStateAthena : public AFortGameState
{
public:
    DEFINE_MEMBER(Map, AFortGameStateAthena, AllPlayerBuildableClassesIndexLookup);
    DEFINE_MEMBER(TArray<TSubclassOf<ABuildingActor>>, UWorld, BuildingActorClasses);
    DEFINE_PTR(FPlaylistPropertyArray, AFortGameStateAthena, CurrentPlaylistInfo)
    DEFINE_PTR(UFortPlaylistAthena, AFortGameStateAthena, CurrentPlaylistData)
    void OnRep_CurrentPlaylistId();
    void OnRep_CurrentPlaylistData();
    void OnRep_CurrentPlaylistInfo();
    DEFINE_MEMBER(int32, AFortGameStateAthena, CurrentPlaylistId);
    DEFINE_PTR(AFortAthenaMapInfo, AFortGameStateAthena, MapInfo);
    DEFINE_MEMBER(FGameMemberInfoArray, AFortGameStateAthena, GameMemberInfoArray);

    DEFINE_MEMBER(EAthenaGamePhase, AFortGameStateAthena, GamePhase);
public:
    DECLARE_STATIC_CLASS(AFortGameStateAthena);
};



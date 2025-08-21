#pragma once
#include "pch.h"

#include "Engine/Actor/Header/Actor.h"
#include "FortniteGame/BuildingSMActor/Header/BuildingSMActor.h"

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
    DEFINE_BOOL(AFortVolume, bShowPublishWatermark)
    DEFINE_BOOL(AFortVolume, bNeverAllowSaving)
    DEFINE_MEMBER(EVolumeState, AFortVolume, VolumeState)
    DECLARE_STATIC_CLASS(AFortVolume)
    DECLARE_DEFAULT_OBJECT(AFortVolume)

    void OnRep_VolumeState()
    {
        static SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortVolume", "OnRep_VolumeState");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return;
		
        this->ProcessEvent(Func, nullptr);
    }

};

enum class EJoinInProgress : uint32
{
    Spectate                                 = 0,
    JoinOnNextRound                          = 1,
    JoinImmediately                          = 2,
    JoinSpecificTeam                         = 3,
    EJoinInProgress_MAX                      = 4,
};

struct FCreativeIslandMatchmakingSettings final
{
public:
	int32                                         MinimumNumberOfPlayers;                            // 0x0000(0x0004)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	int32                                         MaximumNumberOfPlayers;                            // 0x0004(0x0004)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	class FString                                 MmsType;                                           // 0x0008(0x0010)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	int32                                         PlayerCount;                                       // 0x0018(0x0004)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	int32                                         NumberOfTeams;                                     // 0x001C(0x0004)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	int32                                         PlayersPerTeam;                                    // 0x0020(0x0004)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	bool                                          bAllowJoinInProgress;                              // 0x0024(0x0001)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	uint8                                         Pad_25[0x3];                                       // 0x0025(0x0003)(Fixing Size After Last Property [ Dumper-7 ])
	EJoinInProgress                               JoinInProgressType;                                // 0x0028(0x0004)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	uint8                                         JoinInProgressTeam;                                // 0x002C(0x0001)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	uint8                                         Pad_2D[0x3];                                       // 0x002D(0x0003)(Fixing Struct Size After Last Property [ Dumper-7 ])
};

struct FCreativeLoadedLinkData final
{
public:
	class FString                                 CreatorName;                                       // 0x0000(0x0010)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	class FString                                 SupportCode;                                       // 0x0010(0x0010)(ZeroConstructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	class FString                                 Mnemonic;                                          // 0x0020(0x0010)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	int32                                         Version;                                           // 0x0030(0x0004)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	uint8                                         Pad_34[0x4];                                       // 0x0034(0x0004)(Fixing Size After Last Property [ Dumper-7 ])
	TArray<struct FLocalizedStringPair>           LinkTitle;                                         // 0x0038(0x0010)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, NativeAccessSpecifierPublic)
	class FText                                   AltTitle;                                          // 0x0048(0x0018)(BlueprintVisible, BlueprintReadOnly, NativeAccessSpecifierPublic)
	TArray<struct FLocalizedStringPair>           LinkTagline;                                       // 0x0060(0x0010)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, NativeAccessSpecifierPublic)
	TArray<class FString>                         DescriptionTags;                                   // 0x0070(0x0010)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, NativeAccessSpecifierPublic)
	TArray<struct FLocalizedStringPair>           IslandIntroduction;                                // 0x0080(0x0010)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, NativeAccessSpecifierPublic)
	class FString                                 LinkYoutubeId;                                     // 0x0090(0x0010)(ZeroConstructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	class FString                                 ImageUrl;                                          // 0x00A0(0x0010)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	class FString                                 IslandType;                                        // 0x00B0(0x0010)(ZeroConstructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	class FString                                 QuestContextTag;                                   // 0x00C0(0x0010)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	class FString                                 AccountId;                                         // 0x00D0(0x0010)(ZeroConstructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	struct FCreativeIslandMatchmakingSettings     MatchmakingSettings;                               // 0x00E0(0x0030)(BlueprintVisible, BlueprintReadOnly, NativeAccessSpecifierPublic)
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

    void OnRep_IslandInfo()
    {
        static SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortAthenaCreativePortal", "OnRep_IslandInfo");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return;
		
        this->ProcessEvent(Func, nullptr);
    }

    static void TeleportPlayerToLinkedVolume(AFortAthenaCreativePortal* Portal, FFrame& Stack);
    static void TeleportPlayerForPlotLoadComplete(AFortAthenaCreativePortal* Portal, FFrame& Stack);

    DEFINE_MEMBER(TArray<struct FUniqueNetIdRepl>, AFortAthenaCreativePortal, PlayersReady)
    DEFINE_MEMBER(FUniqueNetIdRepl, AFortAthenaCreativePortal, OwningPlayer)
    DEFINE_BOOL(AFortAthenaCreativePortal, bPortalOpen)
    DEFINE_BOOL(AFortAthenaCreativePortal, bUserInitiatedLoad)
    DEFINE_BOOL(AFortAthenaCreativePortal, bInErrorState)
    DEFINE_MEMBER(FCreativeLoadedLinkData, AFortAthenaCreativePortal, IslandInfo)

    DECLARE_STATIC_CLASS(AFortAthenaCreativePortal)
    DECLARE_DEFAULT_OBJECT(AFortAthenaCreativePortal)
};

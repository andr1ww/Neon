#pragma once
#include "pch.h"

#include "Engine/DataTable/Header/DataTable.h"
#include "Engine/ObjectPtr/Header/ObjectPtr.h"
#include "FortniteGame/Common/Header/Tags.h"

class UAthenaAISettings;

enum class EFortGameType : uint32
{
    BR                                       = 0,
    Creative                                 = 1,
    CreativeLTM                              = 2,
    Playground                               = 3,
    STW                                      = 4,
    BRArena                                  = 5,
    BRLTM                                    = 6,
    Social                                   = 7,
    MAX                                      = 8,
};

class UFortPlaylist : public UObject
{
public:
    DEFINE_MEMBER(EFortGameType, UFortPlaylist, GameType); // 0x0028(0x0004)(Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    DEFINE_MEMBER( FGameplayTagContainer,UFortPlaylist,                  GameplayTagContainer)
    DEFINE_MEMBER(TSoftObjectPtr<UDataTable>, UFortPlaylist, LootPackages); // 0x0280(0x0028)(Edit, DisableEditOnInstance, Protected, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierProtected)
    DEFINE_MEMBER(TSoftObjectPtr<UDataTable>, UFortPlaylist, LootTierData); // 0x0280(0x0028)(Edit, DisableEditOnInstance, Protected, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierProtected)DEFINE_MEMBER(FName, UFortPlaylist, PlaylistName);
  
    DEFINE_MEMBER(FName, UFortPlaylist, PlaylistName);
    DEFINE_MEMBER(int32,UFortPlaylist, PlaylistId);
    DEFINE_MEMBER(int32, UFortPlaylist, MaxTeamSize);
    DEFINE_MEMBER(int32, UFortPlaylist, MaxTeamCount);
    DEFINE_MEMBER(int32, UFortPlaylist, MaxPlayers);
public:
    DECLARE_DEFAULT_OBJECT(UFortPlaylist)
    DECLARE_STATIC_CLASS(UFortPlaylist)
};

class UFortPlaylistAthena : public UFortPlaylist
{
public:
    DEFINE_MEMBER(FString, UFortPlaylistAthena, RatingType)
    DEFINE_BOOL(UFortPlaylistAthena, bIsTournament)
    DEFINE_BOOL(UFortPlaylistAthena, bLimitedTimeMode)
    DEFINE_PTR(UAthenaAISettings, UFortPlaylistAthena, AISettings);
public:
    DECLARE_STATIC_CLASS(UFortPlaylistAthena);
    DECLARE_DEFAULT_OBJECT(UFortPlaylistAthena)
};
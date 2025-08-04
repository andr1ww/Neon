#pragma once
#include "pch.h"

#include "Engine/DataTable/Header/DataTable.h"
#include "Engine/ObjectPtr/Header/ObjectPtr.h"
#include "FortniteGame/Common/Header/Tags.h"

class UAthenaAISettings;

class UFortPlaylist : public UObject
{
public:
    DEFINE_MEMBER( FGameplayTagContainer,UFortPlaylist,                  GameplayTagContainer)
    DEFINE_MEMBER(TSoftObjectPtr<UDataTable>, UFortPlaylist, LootPackages); // 0x0280(0x0028)(Edit, DisableEditOnInstance, Protected, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierProtected)
    DEFINE_MEMBER(TSoftObjectPtr<UDataTable>, UFortPlaylist, LootTierData); // 0x0280(0x0028)(Edit, DisableEditOnInstance, Protected, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierProtected)DEFINE_MEMBER(FName, UFortPlaylist, PlaylistName);
  
    DEFINE_MEMBER(FName, UFortPlaylist, PlaylistName);
    DEFINE_MEMBER(int32,UFortPlaylist, PlaylistId);

    DEFINE_MEMBER(int32, UFortPlaylist, MaxPlayers);
};

class UFortPlaylistAthena : public UFortPlaylist
{
public:
    DEFINE_PTR(UAthenaAISettings, UFortPlaylistAthena, AISettings);
public:
    DECLARE_STATIC_CLASS(UFortPlaylistAthena);
};
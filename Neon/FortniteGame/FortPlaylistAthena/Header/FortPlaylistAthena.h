#pragma once
#include "pch.h"

class UAthenaAISettings;

class UFortPlaylist : public UObject
{
public:
    DEFINE_MEMBER(FName, UFortPlaylist, PlaylistName);
    DEFINE_MEMBER(int32,UFortPlaylist, PlaylistId);
};

class UFortPlaylistAthena : public UFortPlaylist
{
public:
    DEFINE_PTR(UAthenaAISettings, UFortPlaylistAthena, AISettings);
public:
    DECLARE_STATIC_CLASS(UFortPlaylistAthena);
};
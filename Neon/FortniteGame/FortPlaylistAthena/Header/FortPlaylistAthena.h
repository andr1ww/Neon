#pragma once
#include "pch.h"

class UFortPlaylist : public UObject
{
public:
    DEFINE_MEMBER(FName, UFortPlaylist, PlaylistName);
    DEFINE_MEMBER(int32,UFortPlaylist, PlaylistId);
};

class UFortPlaylistAthena : public UFortPlaylist
{

public:
    DECLARE_STATIC_CLASS(UFortPlaylistAthena);
};
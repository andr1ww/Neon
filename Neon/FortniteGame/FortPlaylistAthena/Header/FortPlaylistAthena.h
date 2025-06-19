#pragma once
#include "pch.h"

class UFortPlaylistAthena : public UObject
{
public:
    DEFINE_MEMBER(FName, UFortPlaylistAthena, PlaylistName);
    DEFINE_MEMBER(int32,UFortPlaylistAthena, PlaylistId);

public:
    DECLARE_STATIC_CLASS(UFortPlaylistAthena);
};
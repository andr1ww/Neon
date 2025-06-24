#pragma once
#include "pch.h"

#include "FortniteGame/FortPlaylistAthena/Header/FortPlaylistAthena.h"


enum class EFastArraySerializerDeltaFlags : uint8
{
    None = 0,
    HasBeenSerialized = 1,
    HasDeltaBeenRequested = 2,
    IsUsingDeltaSerialization = 4,
    EFastArraySerializerDeltaFlags_MAX = 5,
};

struct FPlaylistPropertyArray : public FFastArraySerializer
{
public:
    DEFINE_MEMBER(UFortPlaylistAthena*, FPlaylistPropertyArray, BasePlaylist);
    DEFINE_MEMBER(UFortPlaylistAthena*, FPlaylistPropertyArray, OverridePlaylist);
    DEFINE_MEMBER(int32, FPlaylistPropertyArray, PlaylistReplicationKey);
public:
    DECLARE_STATIC_CLASS(FPlaylistPropertyArray);
};
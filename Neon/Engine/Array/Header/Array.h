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


struct FFastArraySerializerItem
{
public:
    int32 ReplicationID;
    int32 ReplicationKey;
    int32 MostRecentArrayReplicationKey;
};

struct FFastArraySerializer
{
public:
    uint8 ItemMap[80];
    int32 IDCounter;
    int32 ArrayReplicationKey;
    char GuidReferencesMap[0x50];
    char GuidReferencesMap_StructDelta[0x50];

    int32 CachedNumItems;
    int32 CachedNumItemsToConsiderForWriting;

    EFastArraySerializerDeltaFlags DeltaFlags;

    uint8 _Padding1[0x7];

    void MarkItemDirty(FFastArraySerializerItem& Item, bool markArrayDirty = true)
    {
        if (Item.ReplicationID == -1)
        {
            Item.ReplicationID = ++IDCounter;
            if (IDCounter == -1)
            {
                IDCounter++;
            }
        }

        Item.ReplicationKey++;
        if (markArrayDirty) MarkArrayDirty();
    }

    void MarkArrayDirty()
    {
        ArrayReplicationKey++;
        if (ArrayReplicationKey == -1)
        {
            ArrayReplicationKey++;
        }

        CachedNumItems = -1;
        CachedNumItemsToConsiderForWriting = -1;
    }
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
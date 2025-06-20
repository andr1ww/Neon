#pragma once
#include "pch.h"

#include "Engine/Level/Header/Level.h"
#include "Engine/ObjectPtr/Header/ObjectPtr.h"
#include "Engine/PlayerController/Header/PlayerController.h"
#include "FortniteGame/FortGameState/Header/FortGameState.h"
#include "Neon/Finder/Header/Finder.h"


struct FURL
{
public:
    FString Protocol;
    FString Host;
    int32 Port;
    int32 Valid;
    FString Map;
    FString RedirectUrl;
    TArray<FString> Op;
    FString Portal;
};

class UPlayer : public UObject
{
public:
    DEFINE_MEMBER(APlayerController*, UPlayer, PlayerController);
};

class UNetConnection : public UPlayer
{
public:
    DEFINE_MEMBER(AActor*, UNetConnection, OwningActor);
    DEFINE_MEMBER(AActor*, UNetConnection, ViewTarget);
    DEFINE_MEMBER(SDK::TArray<class UChildConnection*>, UNetConnection, Children);
};

static TSet<FName>* GetClientVisibleLevelNames(UNetConnection* NetConnection)
{
    return (TSet<FName>*)(__int64(NetConnection) + 0x336C8);
}

class UChildConnection : public UNetConnection
{
public:
    DEFINE_MEMBER(UNetConnection*, UChildConnection, Parent);
};

class UReplicationDriver : public UObject
{
public:

};

class UChannel : public UObject
{
public:

};

class UActorChannel : public UChannel
{
public:

};

enum class ENetDormancy : uint8
{
    DORM_Never                               = 0,
    DORM_Awake                               = 1,
    DORM_DormantAll                          = 2,
    DORM_DormantPartial                      = 3,
    DORM_Initial                             = 4,
    DORM_MAX                                 = 5,
};


enum class ENetRole : uint8
{
    ROLE_None                                = 0,
    ROLE_SimulatedProxy                      = 1,
    ROLE_AutonomousProxy                     = 2,
    ROLE_Authority                           = 3,
    ROLE_MAX                                 = 4,
};

enum class EChannelCreateFlags : uint32_t
{
    None = (1 << 0),
    OpenedLocally = (1 << 1)
};

class UWorld;

template< class ObjectType>
class TSharedPtr
{
public:
    ObjectType* Object;

    int32 SharedReferenceCount;
    int32 WeakReferenceCount;

    FORCEINLINE ObjectType* Get()
    {
        return Object;
    }
    FORCEINLINE ObjectType* Get() const
    {
        return Object;
    }
    FORCEINLINE ObjectType& operator*()
    {
        return *Object;
    }
    FORCEINLINE const ObjectType& operator*() const
    {
        return *Object;
    }
    FORCEINLINE ObjectType* operator->()
    {
        return Object;
    }
    FORCEINLINE ObjectType* operator->() const
    {
        return Object;
    }
};

struct FNetworkObjectInfo
{
    AActor* Actor;

    TWeakObjectPtr<AActor> WeakActor;

    double NextUpdateTime;

    double LastNetReplicateTime;

    float OptimalNetUpdateDelta;

    float LastNetUpdateTimeStamp;

    TSet<TWeakObjectPtr<UNetConnection>> DormantConnections;
    TSet<TWeakObjectPtr<UNetConnection>> RecentlyDormantConnections;

    uint32 bPendingNetUpdate : 1;

    uint8 bDirtyForReplay : 1;

    uint8 bSwapRolesOnReplicate : 1;

    uint32 ForceRelevantFrame = 0;
};

class FNetworkObjectList
{
public:
    using FNetworkObjectSet = TSet<TSharedPtr<FNetworkObjectInfo>>;

    FNetworkObjectSet AllNetworkObjects;
    FNetworkObjectSet ActiveNetworkObjects;
    FNetworkObjectSet ObjectsDormantOnAllConnections;

    TMap<TWeakObjectPtr<UNetConnection>, int32> NumDormantObjectsPerConnection;
};

class FNetworkGUID
{
public:

    uint32 Value;

public:

    FNetworkGUID()
        : Value(0)
    {
    }

    FNetworkGUID(uint32 V)
        : Value(V)
    {
    }

public:

    friend bool operator==(const FNetworkGUID& X, const FNetworkGUID& Y)
    {
        return (X.Value == Y.Value);
    }

    friend bool operator!=(const FNetworkGUID& X, const FNetworkGUID& Y)
    {
        return (X.Value != Y.Value);
    }

public:

    void BuildFromNetIndex(int32 StaticNetIndex)
    {
        Value = (StaticNetIndex << 1 | 1);
    }

    int32 ExtractNetIndex()
    {
        if (Value & 1)
        {
            return Value >> 1;
        }
        return 0;
    }

    friend uint32 GetTypeHash(const FNetworkGUID& Guid)
    {
        return Guid.Value;
    }

    bool IsDynamic() const
    {
        return Value > 0 && !(Value & 1);
    }

    bool IsStatic() const
    {
        return Value & 1;
    }

    bool IsValid() const
    {
        return Value > 0;
    }

    bool IsDefault() const
    {
        return (Value == 1);
    }

    /* CORE_API*/ static FNetworkGUID GetDefault()
    {
        return FNetworkGUID(1);
    }

    void Reset()
    {
        Value = 0;
    }

public:

    /* CORE_API */ static FNetworkGUID Make(int32 seed, bool bIsStatic)
    {
        return FNetworkGUID(seed << 1 | (bIsStatic ? 1 : 0));
    }
};


struct FActorDestructionInfo
{
    TWeakObjectPtr<ULevel>		Level;
    TWeakObjectPtr<UObject>		ObjOuter;
    FVector			DestroyedPosition;
    FNetworkGUID	NetGUID;
    FString			PathName;
    FName			StreamingLevelName;
};

struct FNetViewer final
{
public:
    class UNetConnection*                         Connection;                                        // 0x0000(0x0008)(ZeroConstructor, NoDestructor, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    class AActor*                                 InViewer;                                          // 0x0008(0x0008)(ZeroConstructor, NoDestructor, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    class AActor*                                 ViewTarget;                                        // 0x0010(0x0008)(ZeroConstructor, NoDestructor, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    struct FVector                                ViewLocation;                                      // 0x0018(0x0018)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    struct FVector                                ViewDir;                                           // 0x0030(0x0018)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
};


class UNetDriver : public UObject
{
public:
    DEFINE_MEMBER(FName, UNetDriver, NetDriverName);
    DEFINE_MEMBER(UReplicationDriver*, UNetDriver, ReplicationDriver);
    DEFINE_MEMBER(SDK::TArray<class UNetConnection*>, UNetDriver, ClientConnections);
public:
    bool InitListen(UWorld* Networknotify, FURL URL, bool bReuseAddressAndPort);
    void SetWorld(UWorld* World);
    
    void TickFlush(UNetDriver* NetDriver, float DeltaSeconds);

    int32& ReplicationFrame()
    {
        static int RepFrameOffset = 0x440; 
    
        int Major = SDK::Fortnite_Version.GetMajorVersion();
    
        if (Engine_Version == 419)
        {
            RepFrameOffset = 0x2C8;
        }

        else if (Major >= 20 && Major < 22)
        {
            RepFrameOffset = 0x3D8;
        }
        else if (SDK::Fortnite_Version >= 2.5 && SDK::Fortnite_Version <= 3.1) 
        {
            RepFrameOffset = 0x328;
        }
        else if (SDK::Fortnite_Version == 3.2 || SDK::Fortnite_Version == 3.3)
        {
            RepFrameOffset = 0x330;
        }
        else if (Major >= 22)
        {
            RepFrameOffset = 0x440; 
        }
    
        return *reinterpret_cast<int32*>(__int64(this) + RepFrameOffset);
    }
    
    // Replication
    int32 ServerReplicateActors(UNetDriver* NetDriver, float DeltaSeconds);
    int32 ServerReplicateActors_PrepConnections(UNetDriver*, float DeltaSeconds);
    void ServerReplicateActors_BuildConsiderList(UNetDriver* Driver, TArray<AActor*>& OutConsiderList, const float ServerTickTime);
    int32 ServerReplicateActors_ProcessActors(UNetDriver*, UNetConnection* Connection, TArray<AActor*>& Actors);
    bool IsActorRelevantToConnection(const AActor* Actor, const TArray<FNetViewer>& ConnectionViewers);
    UNetConnection* IsActorOwnedByAndRelevantToConnection(const AActor* Actor, const TArray<FNetViewer>& ConnectionViewers, bool& bOutHasNullViewTarget);
public:
    DECLARE_STATIC_CLASS(UNetDriver);
    DECLARE_DEFAULT_OBJECT(UNetDriver);
};

inline void (*TickFlushOriginal)(UNetDriver*, float DeltaSeconds);

static bool ReplicateToClient(UNetDriver* Driver, AActor* Actor, UNetConnection* Client) {
    if (!Client || !Actor) {
        return false;
    }
    if (Actor->IsA(APlayerController::StaticClass()) && Client->GetPlayerController() && Actor != Client->GetPlayerController()) {
        return false;
    }
 
    bool (*DemoReplicateActor)(UNetDriver *, AActor *, UNetConnection *, bool) = decltype(DemoReplicateActor)(Finder->DemoReplicateActor());
    DemoReplicateActor(Driver, Actor, Client, false);
	
    return true;
}

class ULocalPlayer : public UObject
{
public:
    DEFINE_MEMBER(APlayerController*, ULocalPlayer, PlayerController);
public:
    DECLARE_STATIC_CLASS(ULocalPlayer);
    DECLARE_DEFAULT_OBJECT(ULocalPlayer);
};

class UGameInstance : public UObject
{
public:
    DEFINE_MEMBER(TArray<ULocalPlayer*>, UGameInstance, LocalPlayers);
public:
    DECLARE_STATIC_CLASS(UGameInstance);
    DECLARE_DEFAULT_OBJECT(UGameInstance);
};

struct FLevelCollection
{
    BYTE CollectionType[1];
    bool bIsVisible;
    AFortGameStateAthena* GameState;
    UNetDriver* NetDriver;
    void* DemoNetDriver;
    void* PersistentLevel;
    uint8_t Levels[0x50];
};

class UWorld : public UObject
{
public:
    DEFINE_MEMBER(AFortGameStateAthena*, UWorld, GameState);
    DEFINE_MEMBER(UGameInstance*, UWorld, OwningGameInstance);
    DEFINE_MEMBER(UNetDriver*, UWorld, NetDriver);
    DEFINE_MEMBER(SDK::TArray<struct FLevelCollection>, UWorld, LevelCollections);
    DEFINE_MEMBER(ULevel*, UWorld, CurrentLevelPendingVisibility);
    DEFINE_MEMBER(ULevel*, UWorld, CurrentLevelPendingInvisibility);
public:
    DECLARE_STATIC_CLASS(UWorld);
    DECLARE_DEFAULT_OBJECT(UWorld);
    static UWorld* GetWorld();
};

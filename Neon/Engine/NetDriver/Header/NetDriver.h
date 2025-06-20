#pragma once
#include "pch.h"

#include "Engine/PlayerController/Header/PlayerController.h"
#include "FortniteGame/FortGameState/Header/FortGameState.h"

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

class UChildConnection : public UNetConnection
{
public:
    DEFINE_MEMBER(UNetConnection*, UChildConnection, Parent);
};

class UReplicationDriver : public UObject
{
public:

};

class UWorld;

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
public:
    DECLARE_STATIC_CLASS(UNetDriver);
    DECLARE_DEFAULT_OBJECT(UNetDriver);
};

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
public:
    DECLARE_STATIC_CLASS(UWorld);
    DECLARE_DEFAULT_OBJECT(UWorld);
    static UWorld* GetWorld();
};

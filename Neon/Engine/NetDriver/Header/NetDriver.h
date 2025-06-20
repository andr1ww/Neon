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
public:
    bool InitListen(UWorld* Networknotify, FURL URL, bool bReuseAddressAndPort);
    void SetWorld(UWorld* World);
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

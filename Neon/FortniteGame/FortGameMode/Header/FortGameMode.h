#pragma once
#include "pch.h"
#include "Engine/Actor/Header/Actor.h"
#include "Engine/PlayerController/Header/PlayerController.h"
#include "Engine/Transform/Header/Transform.h"
#include "FortniteGame/FortGameState/Header/FortGameState.h"
#include "FortniteGame/FortPlayerController/Header/FortPlayerController.h"
#include "FortniteGame/FortServerBotManager/Header/FortServerBotManager.h"
#include "Engine/ObjectPtr/Header/ObjectPtr.h"

class AFortGameStateAthena;
class UFortItemDefinition;
class AFortPlayerControllerAthena;
class AFortAIDirector;
class AFortAIGoalManager;
class AFortAthenaMutator_Bots;
class UFortServerBotManagerAthena;

struct FItemAndCount final 
{
public:
    DEFINE_MEMBER(int32, FItemAndCount, Count);
    DEFINE_PTR(UFortItemDefinition, FItemAndCount, Item);
};

class AGameModeBase : public UObject
{
public:
    DEFINE_PTR(AFortGameStateAthena, AGameModeBase, GameState);
};

class AFortGameMode : public AGameModeBase
{
public:
    DEFINE_MEMBER(int32, AFortGameMode, CurrentPlaylistId);
    DEFINE_MEMBER(FName, AFortGameMode, CurrentPlaylistName);
    DEFINE_BOOL(AFortGameMode, bWorldIsReady);
};

static inline AFortAthenaMutator_Bots* BotMutator = nullptr;

struct FAdditionalLevelStreamed final
{
public:
    class FName                                   LevelName;                                         // 0x0000(0x0008)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    bool                                          bIsServerOnly;                                     // 0x0008(0x0001)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    uint8                                         Pad_1B33[0x3];                                     // 0x0009(0x0003)(Fixing Struct Size After Last Property [ Dumper-7 ])
};

template<class TObjectID>
struct TPersistentObjectPtr
{
public:
    /** Once the object has been noticed to be loaded, this is set to the object weak pointer **/
    mutable FWeakObjectPtr	WeakPtr;
    /** Compared to CurrentAnnotationTag and if they are not equal, a guid search will be performed **/
    mutable int			TagAtLastTest;
    /** Guid for the object this pointer points to or will point to. **/
    TObjectID				ObjectID;
};

struct FSoftObjectPath
{
public:
    /** Asset path, patch to a top level object in a package. This is /package/path.assetname */
    SDK::FName AssetPathName;

    /** Optional FString for subobject within an asset. This is the sub path after the : */
    SDK::FString SubPathString;
};

struct FSoftObjectPtr : public TPersistentObjectPtr<FSoftObjectPath>
{
public:
};

template<class T = SDK::UObject>
struct TSoftObjectPtr
{
public:
    FSoftObjectPtr SoftObjectPtr;

    bool IsValid()
    {
        return SoftObjectPtr.ObjectID.AssetPathName.GetComparisonIndex();
    }

    T* Get(SDK::UClass* ClassToLoad = nullptr, bool bTryToLoad = false)
    {
        if (SoftObjectPtr.ObjectID.AssetPathName.GetComparisonIndex() <= 0)
            return nullptr;

        if (bTryToLoad)
        {
            return Runtime::StaticLoadObject<T>(SoftObjectPtr.ObjectID.AssetPathName.ToString(), ClassToLoad);
        }

        return Runtime::StaticFindObject<T>(SoftObjectPtr.ObjectID.AssetPathName.ToString());
    }
};

class ULevelStreamingDynamic final : public UObject
{
public:
    uint8                                         bInitiallyLoaded : 1;                              // 0x0148(0x0001)(BitIndex: 0x00, PropSize: 0x0001 (Edit, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic))
    uint8                                         bInitiallyVisible : 1;                             // 0x0148(0x0001)(BitIndex: 0x01, PropSize: 0x0001 (Edit, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic))
    uint8                                         Pad_6BF[0x7];                                      // 0x0149(0x0007)(Fixing Struct Size After Last Property [ Dumper-7 ])
};

class AFortGameModeAthena : public AFortGameMode
{
public:
    DEFINE_MEMBER(int32, AFortGameModeAthena, WarmupRequiredPlayerCount);
    DEFINE_MEMBER(int32, AFortGameModeAthena, NumPlayers);
    DEFINE_MEMBER(TArray<FItemAndCount>, AFortGameModeAthena, StartingItems);
    DEFINE_MEMBER(TArray<AFortPlayerControllerAthena*>, AFortGameModeAthena, AlivePlayers);
    DEFINE_PTR(UFortServerBotManagerAthena, AFortGameModeAthena, ServerBotManager);
    DEFINE_PTR(AFortAIDirector, AFortGameModeAthena, AIDirector);
    DEFINE_PTR(AFortAIGoalManager, AFortGameModeAthena, AIGoalManager);
public:
    DECLARE_DEFAULT_OBJECT(AFortGameModeAthena);
    DECLARE_STATIC_CLASS(AFortGameModeAthena);
public:
    static bool ReadyToStartMatch(AFortGameModeAthena* GameMode, FFrame& Stack, bool* Result);
    static APawn* SpawnDefaultPawnFor(AFortGameModeAthena* GameMode, AFortPlayerControllerAthena* NewPlayer, AActor* StartSpot);
};

inline bool (*ReadyToStartMatchOriginal)(AFortGameModeAthena* GameMode);

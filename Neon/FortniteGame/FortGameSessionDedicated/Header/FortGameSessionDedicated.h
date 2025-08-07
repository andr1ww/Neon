#pragma once
#include "pch.h"

#include "FortniteGame/FortGameMode/Header/FortGameMode.h"

struct FServicePermissionsMcp {
public:
    char Unk_0[0x10];
    class FString Id;
};

class AFortGameSessionDedicatedAthena : public UObject
{
public:
    DECLARE_STATIC_CLASS(AFortGameSessionDedicatedAthena)
    DECLARE_DEFAULT_OBJECT(AFortGameSessionDedicatedAthena)
};

namespace FortGameSessionDedicated
{
    UClass** GetGameSessionClass(AFortGameModeAthena* GameMode, UClass** ClassRet);
    inline const wchar_t* (*GetOG)();
    const wchar_t* Get();
    void UWorld_Listen();
    FServicePermissionsMcp* MatchmakingServicePerms(int64, int64);
    void Restart(void*);
}
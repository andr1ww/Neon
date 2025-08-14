#pragma once
#include "pch.h"
#include "Engine/Actor/Header/Actor.h"
#include "Engine/UniqueID/Header/UniqueId.h"


class APlayerState : public AActor
{
public:
    DEFINE_MEMBER(FUniqueNetIdRepl, APlayerState, UniqueId);

    inline FString GetPlayerName()
    {
        static SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("PlayerState", "GetPlayerName");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return 0;

        struct PlayerState_GetPlayerName final
        {
        public:
            class FString                                 ReturnValue;                                       // 0x0000(0x0010)(Parm, OutParm, ZeroConstructor, ReturnParm, HasGetValueTypeHash, NativeAccessSpecifierPublic)
        } Params{};
        
        this->ProcessEvent(Func, &Params);

        return Params.ReturnValue;
    }
public:
    DECLARE_STATIC_CLASS(APlayerState);
    DECLARE_DEFAULT_OBJECT(APlayerState);
};

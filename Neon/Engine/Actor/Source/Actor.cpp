#include "pch.h"
#include "Engine/Actor/Header/Actor.h"

FVector AActor::GetActorLocation()
{
    struct
    {
        FVector ReturnValue;
    } Params{};

    GetDefaultObj()->Call(L"K2_GetActorLocation", &Params);
    return Params.ReturnValue;
}

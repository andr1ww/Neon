#pragma once
#include "pch.h"
#include "Engine/Actor/Header/Actor.h"
#include "Engine/UniqueID/Header/UniqueId.h"


class APlayerState : public AActor
{
public:
    DEFINE_MEMBER(FUniqueNetIdRepl, APlayerState, UniqueId);
public:
    DECLARE_STATIC_CLASS(APlayerState);
    DECLARE_DEFAULT_OBJECT(APlayerState);
};

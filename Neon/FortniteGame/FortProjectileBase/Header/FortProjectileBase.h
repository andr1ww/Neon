#pragma once
#include "pch.h"

#include "Engine/Actor/Header/Actor.h"

class AFortProjectileBase : public AActor
{
public:
    DefHookOg(void, OnStopCallback, AFortProjectileBase*, struct FFrame& Stack)
public:
    DECLARE_STATIC_CLASS(AFortProjectileBase)
    DECLARE_DEFAULT_OBJECT(AFortProjectileBase)
};

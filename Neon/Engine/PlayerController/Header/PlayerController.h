#pragma once
#include "pch.h"
#include "Engine/Actor/Header/Actor.h"

class AController : public AActor
{
public:
    AActor* GetViewTarget();
};

class APlayerController : public AController
{
private:

public:
    DECLARE_STATIC_CLASS(APlayerController);
    DECLARE_DEFAULT_OBJECT(APlayerController);
};

#pragma once
#include "pch.h"
#include "Engine/Actor/Header/Actor.h"
#include "Engine/Pawn/Header/Pawn.h"

class AController : public AActor
{
public:
    DEFINE_MEMBER(APawn*, AController, Pawn);
public:
    AActor* GetViewTarget();
};

class APlayerController : public AController
{
public:
    DEFINE_MEMBER(APawn*, APlayerController, AcknowledgedPawn);
public:
    DECLARE_STATIC_CLASS(APlayerController);
    DECLARE_DEFAULT_OBJECT(APlayerController);
};

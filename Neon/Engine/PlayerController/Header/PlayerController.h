#pragma once
#include "pch.h"
#include "Engine/Actor/Header/Actor.h"
#include "Engine/Pawn/Header/Pawn.h"
#include "Engine/PlayerState/Header/PlayerState.h"

class AController : public AActor
{
public:
    DEFINE_MEMBER(APlayerState*, AController, PlayerState);
    DEFINE_MEMBER(APawn*, AController, Pawn);
public:
    AActor* GetViewTarget();
};

class APlayerController : public AController
{
public:
    
public:
    DEFINE_MEMBER(APawn*, APlayerController, AcknowledgedPawn);
public:
    DECLARE_STATIC_CLASS(APlayerController);
    DECLARE_DEFAULT_OBJECT(APlayerController);
};

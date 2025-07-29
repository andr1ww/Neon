#pragma once
#include "pch.h"

#include "Engine/Actor/Header/Actor.h"
#include "Engine/PlayerController/Header/PlayerController.h"

class AController;

class APawn : public AActor
{
public:
    DEFINE_PTR(AController*, APawn, Controller);
};

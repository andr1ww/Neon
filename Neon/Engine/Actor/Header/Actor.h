#pragma once
#include "pch.h"

#include "Engine/Transform/Header/Transform.h"

class AActor : public UObject
{
public:

public:
    FVector GetActorLocation();
public:
    DECLARE_STATIC_CLASS(AActor);
    DECLARE_DEFAULT_OBJECT(AActor);
};
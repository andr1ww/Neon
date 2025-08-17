#pragma once
#include "pch.h"

#include "Engine/Transform/Header/Transform.h"

class AActor : public UObject
{
public:

public:
    FVector GetActorLocation();
    FVector GetActorForwardVector();
    FVector GetActorUpVector();
    FVector GetActorRightVector();
    void K2_DestroyActor();
	FVector K2_GetActorLocation();
    FRotator K2_GetActorRotation();
    FTransform GetTransform();
    bool K2_SetActorRotation(const struct FRotator& NewRotation, bool bTeleportPhysics);
    void K2_TeleportTo(const FVector Location, const FRotator Rot);
    float GetDistanceTo(AActor* Actor);
public:
    DECLARE_STATIC_CLASS(AActor);
    DECLARE_DEFAULT_OBJECT(AActor);
};
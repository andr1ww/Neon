#pragma once
#include "pch.h"

#include "Engine/Transform/Header/Transform.h"

class USceneComponent : public UObject
{
public:
    DEFINE_PTR(USceneComponent, USceneComponent, RootComponent);
    DECLARE_DEFAULT_OBJECT(USceneComponent)
    DECLARE_STATIC_CLASS(USceneComponent)
};

class AActor : public UObject
{
public:
    DEFINE_PTR(USceneComponent, AActor, RootComponent);
    DEFINE_BOOL(AActor, bHidden);
public:
    FVector GetActorLocation();
    FVector GetActorForwardVector();
    FVector GetActorUpVector();
    FVector GetActorRightVector();
    void K2_DestroyActor();
	FVector K2_GetActorLocation();
    FRotator K2_GetActorRotation();
    FTransform GetTransform();
    AActor* GetOwner();
    bool K2_SetActorRotation(const struct FRotator& NewRotation, bool bTeleportPhysics);
    void K2_TeleportTo(const FVector Location, const FRotator Rot);
    float GetDistanceTo(AActor* Actor);
public:
    DECLARE_STATIC_CLASS(AActor);
    DECLARE_DEFAULT_OBJECT(AActor);
};
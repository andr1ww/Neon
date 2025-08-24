#pragma once
#include "pch.h"

#include "Engine/Actor/Header/Actor.h"
#include "FortniteGame/Common/Header/ItemDefinition.h"
#include "FortniteGame/FortAthenaAIBotController/Header/FortAthenaAIBotController.h"

class AFortProjectileBase : public AActor
{
public:
    DefHookOg(void, OnStopCallback, AFortProjectileBase*, struct FFrame& Stack)
public:
    DECLARE_STATIC_CLASS(AFortProjectileBase)
    DECLARE_DEFAULT_OBJECT(AFortProjectileBase)
};

class UFortPlaysetItemDefinition : public UFortWorldItemDefinition
{
public:
    DECLARE_STATIC_CLASS(UFortPlaysetItemDefinition)
    DECLARE_DEFAULT_OBJECT(UFortPlaysetItemDefinition)
};

class UStaticMeshComponent : public UPrimitiveComponent
{
public:
    DECLARE_STATIC_CLASS(UStaticMeshComponent)
    DECLARE_DEFAULT_OBJECT(UStaticMeshComponent)
};

class UFortPlaysetGrenadeItemDefinition : public UFortWorldItemDefinition
{
public:
    DECLARE_STATIC_CLASS(UFortPlaysetGrenadeItemDefinition)
    DECLARE_DEFAULT_OBJECT(UFortPlaysetGrenadeItemDefinition)
};

class AB_Prj_Athena_PlaysetGrenade_C : public AFortProjectileBase
{
public:
    DEFINE_PTR(UFortPlaysetItemDefinition, AB_Prj_Athena_PlaysetGrenade_C, CachedPlayset)
public:
    DECLARE_STATIC_CLASS(AB_Prj_Athena_PlaysetGrenade_C);
    DECLARE_DEFAULT_OBJECT(AB_Prj_Athena_PlaysetGrenade_C)
};
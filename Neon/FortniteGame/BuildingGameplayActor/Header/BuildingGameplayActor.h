#pragma once
#include "pch.h"

#include "Engine/Actor/Header/Actor.h"
#include "Engine/ObjectPtr/Header/ObjectPtr.h"

class ABuildingGameplayActorSpawnMachine : public AActor
{
public:
    static void RebootingDelegate(ABuildingGameplayActorSpawnMachine* SpawnMachine);
public:
    DEFINE_MEMBER(TArray<struct FUniqueNetIdRepl>, ABuildingGameplayActorSpawnMachine, PlayerIdsForResurrection)
    DEFINE_PTR(AActor, ABuildingGameplayActorSpawnMachine, ResurrectLocation)
    DEFINE_MEMBER(uint8, ABuildingGameplayActorSpawnMachine, SquadId)
    DEFINE_MEMBER(TWeakObjectPtr<class AFortPlayerControllerAthena>, ABuildingGameplayActorSpawnMachine, InstigatorPC)
    
    DECLARE_STATIC_CLASS(ABuildingGameplayActorSpawnMachine)
    DECLARE_DEFAULT_OBJECT(ABuildingGameplayActorSpawnMachine)
};

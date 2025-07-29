#pragma once
#include "pch.h"

#include "Engine/NetDriver/Header/NetDriver.h"

enum class ESpawnActorCollisionHandlingMethod : uint8
{
	Undefined,
	AlwaysSpawn,
	AdjustIfPossibleButAlwaysSpawn,
	AdjustIfPossibleButDontSpawnIfColliding,
	DontSpawnIfColliding
};

class UGameplayStatics : public UObject
{
public:
	static TArray<AActor*> GetAllActorsOfClass(const UObject* World, UClass* ActorClass);
	static float GetTimeSeconds(UWorld* World);
	static AActor* FinishSpawningActor(AActor* Actor, const FTransform& SpawnTransform);
	static AActor* BeginDeferredActorSpawnFromClass(const UObject* WorldContextObject, UClass* ActorClass, const FTransform& SpawnTransform, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::Undefined, AActor* Owner = nullptr);
	static UObject* SpawnObject(TSubclassOf<class UObject> ObjectClass, class UObject* Param_Outer);

	static AActor* SpawnActor(UClass* Class, FTransform& Transform, AActor* Owner = nullptr);
	static AActor* SpawnActor(UClass* Class, FVector& Loc, FRotator& Rot, AActor* Owner = nullptr);
    
	template <typename T>
	static T* SpawnActor(UClass* Class, FVector Loc, FRotator Rot = {}, AActor* Owner = nullptr)
	{
		return (T*)SpawnActor(Class, Loc, Rot, Owner);
	}

	template <typename T>
	static T* SpawnActor(UClass* Class, FTransform& Transform, AActor* Owner = nullptr)
	{
		return (T*)SpawnActor(Class, Transform, Owner);
	}

	template <typename T>
	static T* SpawnActor(FVector Loc, FRotator Rot = {}, AActor* Owner = nullptr)
	{
		return (T*)SpawnActor(T::StaticClass(), Loc, Rot, Owner);
	}

	template <typename T>
	static T* SpawnActor(FTransform& Transform, AActor* Owner = nullptr)
	{
		return (T*)SpawnActor(T::StaticClass(), Transform, Owner);
	}
public:
	DECLARE_STATIC_CLASS(UGameplayStatics);
	DECLARE_DEFAULT_OBJECT(UGameplayStatics);
};
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

struct FActorSpawnParameters
{
public:
	FName Name;

	AActor* Template;
	AActor* Owner;
	APawn* Instigator;
	ULevel* OverrideLevel;
	ESpawnActorCollisionHandlingMethod SpawnCollisionHandlingOverride;

private:
	uint8	bRemoteOwned : 1;
public:
	uint8	bNoFail : 1;
	uint8	bDeferConstruction : 1;
	uint8	bAllowDuringConstructionScript : 1;
	EObjectFlags ObjectFlags;
};

// from magma's gs i cba bro
inline void sinCos(float* ScalarSin, float* ScalarCos, float Value)
{
	float quotient = (0.31830988618f * 0.5f) * Value;
	if (Value >= 0.0f)
	{
		quotient = (float)((int)(quotient + 0.5f));
	}
	else
	{
		quotient = (float)((int)(quotient - 0.5f));
	}
	float y = Value - (2.0f * 3.1415926535897932f) * quotient;

	float sign;
	if (y > 1.57079632679f)
	{
		y = 3.1415926535897932f - y;
		sign = -1.0f;
	}
	else if (y < -1.57079632679f)
	{
		y = -3.1415926535897932f - y;
		sign = -1.0f;
	}
	else
	{
		sign = +1.0f;
	}

	float y2 = y * y;

	*ScalarSin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;

	float p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
	*ScalarCos = sign * p;
}
// from magma's gs i cba bro

inline FQuat FRotToQuat(FRotator Rot)
{
	const float DEG_TO_RAD = 3.1415926535897932f / (180.f);
	const float DIVIDE_BY_2 = DEG_TO_RAD / 2.f;
	float SP, SY, SR;
	float CP, CY, CR;

	sinCos(&SP, &CP, Rot.Pitch * DIVIDE_BY_2);
	sinCos(&SY, &CY, Rot.Yaw * DIVIDE_BY_2);
	sinCos(&SR, &CR, Rot.Roll * DIVIDE_BY_2);

	FQuat RotationQuat;
	RotationQuat.X = CR * SP * SY - SR * CP * CY;
	RotationQuat.Y = -CR * SP * CY - SR * CP * SY;
	RotationQuat.Z = CR * CP * SY - SR * SP * CY;
	RotationQuat.W = CR * CP * CY + SR * SP * SY;

	return RotationQuat;
}

class UGameplayStatics : public UObject
{
public:
	static TArray<AActor*> GetAllActorsOfClass(const UObject* World, UClass* ActorClass);
	static float GetTimeSeconds(UWorld* World);
	static AActor* FinishSpawningActor(AActor* Actor, const FTransform& SpawnTransform);
	static AActor* BeginDeferredActorSpawnFromClass(const UObject* WorldContextObject, UClass* ActorClass, const FTransform& SpawnTransform, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::Undefined, AActor* Owner = nullptr);
	static UObject* SpawnObject(TSubclassOf<class UObject> ObjectClass, class UObject* Param_Outer);

	static AActor* SpawnActorOG(UClass* Class, FVector Loc, FRotator Rot = FRotator(0,0,0), AActor* Owner = nullptr);
    
	template <typename T>
	static T* SpawnActorOG(UClass* Class, FVector Loc, FRotator Rot = FRotator(0,0,0), AActor* Owner = nullptr)
	{
		return static_cast<T*>(SpawnActorOG(Class, Loc, Rot, Owner));
	}
	
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
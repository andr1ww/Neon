#pragma once
#include "pch.h"

#include "Engine/NetDriver/Header/NetDriver.h"

class UGameplayStatics : public UObject
{
public:
	static TArray<AActor*> GetAllActorsOfClass(const UObject* World, UClass* ActorClass);
	static float GetTimeSeconds(UWorld* World);
public:
	DECLARE_STATIC_CLASS(UGameplayStatics);
	DECLARE_DEFAULT_OBJECT(UGameplayStatics);
};
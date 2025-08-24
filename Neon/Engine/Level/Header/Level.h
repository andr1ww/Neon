#pragma once
#include "pch.h"

#include "Engine/Actor/Header/Actor.h"
#include "Engine/NetDriver/Header/NetDriver.h"

class UWorld;

class ULevel : public UObject
{
public:
    DEFINE_MEMBER(TArray<class AActor*>, ULevel, Actors)
    DEFINE_PTR(UWorld, ULevel, OwningWorld);
public:
    static ULevel* GetLevel(AActor* Actor) {
        auto Outer = Actor->GetOuter();

        while (Outer)
        {
            if (Outer->GetClass() == ULevel::StaticClass())
                return (ULevel*)Outer;
            else
                Outer = Outer->GetOuter();
        }
        return nullptr;
    }
public:
    DECLARE_STATIC_CLASS(ULevel);
    DECLARE_DEFAULT_OBJECT(ULevel);
};
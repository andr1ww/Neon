#pragma once
#include "pch.h"

#include "Engine/Actor/Header/Actor.h"
#include "Engine/NetDriver/Header/NetDriver.h"

class UWorld;

class ULevel : public UObject
{
public:
    DEFINE_MEMBER(UWorld*, ULevel, OwningWorld);
public:
    static ULevel* GetLevel(AActor* Actor) {
        auto Outer = Actor->Outer;

        while (Outer)
        {
            if (Outer->Class == ULevel::StaticClass())
                return (ULevel*)Outer;
            else
                Outer = Outer->Outer;
        }
        return nullptr;
    }
public:
    DECLARE_STATIC_CLASS(ULevel);
    DECLARE_DEFAULT_OBJECT(ULevel);
};
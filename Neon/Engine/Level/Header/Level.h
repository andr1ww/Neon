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
    ULevel* GetLevel(AActor* Actor) {
        auto Outer = Actor->GetOuter();

        while (Outer)
        {
            if (Outer->GetClass() == ULevel::GetClass())
                return (ULevel*)Outer;
            else
                Outer = Outer->GetOuter();
        }
        return nullptr;
    }
};

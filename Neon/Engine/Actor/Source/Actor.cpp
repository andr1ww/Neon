#include "pch.h"
#include "Engine/Actor/Header/Actor.h"

FVector AActor::GetActorLocation()
{
    return this->CallFunc<FVector>("Actor","K2_GetActorLocation");
}

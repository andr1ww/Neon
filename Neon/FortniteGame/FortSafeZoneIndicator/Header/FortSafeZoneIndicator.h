#pragma once
#include "pch.h"

#include "Engine/Actor/Header/Actor.h"
#include "Neon/Runtime/Runtime.h"

enum class EFortSafeZoneState : uint8
{
    None                                     = 0,
    Starting                                 = 1,
    Holding                                  = 2,
    Shrinking                                = 3,
    EFortSafeZoneState_MAX                   = 4,
};
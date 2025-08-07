#pragma once
#include "pch.h"
#include "FortniteGame/FortGameMode/Header/FortGameMode.h"

enum class EAmmoType : uint8
{
    Assault = 0,
    Shotgun = 1,
    Submachine = 2,
    Rocket = 3,
    Sniper = 4,
    EFortAmmoType_MAX = 5,
};

namespace ItemAndCount {
    FItemAndCount GetShotguns();
    FItemAndCount GetAssaultRifles();
    FItemAndCount GetSnipers();
    FItemAndCount GetHeals();

    UFortAmmoItemDefinition* GetAmmo(EAmmoType);
};
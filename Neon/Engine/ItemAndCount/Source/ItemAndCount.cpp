#include "pch.h"
#include "FortniteGame/FortKismetLibrary/Header/FortKismetLibrary.h"
#include "FortniteGame/FortLoot/Header/FortLootPackage.h"
#include "../Header/ItemAndCount.h"

FItemAndCount ItemAndCount::GetShotguns()
{
    static std::vector<FItemAndCount> Shotguns{
        FItemAndCount(1, {}, Runtime::StaticFindObject<UFortWeaponRangedItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_UC_Ore_T03.WID_Shotgun_Standard_Athena_UC_Ore_T03")), // pump 
        FItemAndCount(1, {}, Runtime::StaticFindObject<UFortWeaponRangedItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_VR_Ore_T03.WID_Shotgun_Standard_Athena_VR_Ore_T03")), // pump 
        FItemAndCount(1, {}, Runtime::StaticFindObject<UFortWeaponRangedItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_SR_Ore_T03.WID_Shotgun_Standard_Athena_SR_Ore_T03")), // pump 
    };

    return Shotguns[rand() % (Shotguns.size() - 1)];
}

FItemAndCount ItemAndCount::GetAssaultRifles()
{
    static std::vector<FItemAndCount> AssaultRifles{
        FItemAndCount(1, {}, Runtime::StaticFindObject<UFortWeaponRangedItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_Heavy_Athena_R_Ore_T03.WID_Assault_Heavy_Athena_R_Ore_T03")), // heavy ar 
        FItemAndCount(1, {}, Runtime::StaticFindObject<UFortWeaponRangedItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_Auto_Athena_R_Ore_T03.WID_Assault_Auto_Athena_R_Ore_T03")), // scar blue
        FItemAndCount(1, {}, Runtime::StaticFindObject<UFortWeaponRangedItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_AutoHigh_Athena_VR_Ore_T03.WID_Assault_AutoHigh_Athena_VR_Ore_T03")), // scar epic
        FItemAndCount(1, {}, Runtime::StaticFindObject<UFortWeaponRangedItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_AutoHigh_Athena_SR_Ore_T03.WID_Assault_AutoHigh_Athena_SR_Ore_T03")), // scar gold 
        FItemAndCount(1, {}, Runtime::StaticFindObject<UFortWeaponRangedItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_Suppressed_Athena_VR_Ore_T03.WID_Assault_Suppressed_Athena_VR_Ore_T03")), // suppresed epic 
        FItemAndCount(1, {}, Runtime::StaticFindObject<UFortWeaponRangedItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_Suppressed_Athena_SR_Ore_T03.WID_Assault_Suppressed_Athena_SR_Ore_T03")), // suppresed gold
        FItemAndCount(1, {}, Runtime::StaticFindObject<UFortWeaponRangedItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_Heavy_Athena_R_Ore_T03.WID_Assault_Heavy_Athena_R_Ore_T03")), // gold p90
        FItemAndCount(1, {}, Runtime::StaticFindObject<UFortWeaponRangedItemDefinition>("/Game/Athena/Items/Weapons/Boss/WID_Boss_Adventure_AR.WID_Boss_Adventure_AR")), // skyes mythic ar
        FItemAndCount(1, {}, Runtime::StaticFindObject<UFortWeaponRangedItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_SemiAuto_Athena_SR_Ore_T03.WID_Assault_SemiAuto_Athena_SR_Ore_T03")), // burst gold
        FItemAndCount(1, {}, Runtime::StaticFindObject<UFortWeaponRangedItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_SemiAuto_Athena_VR_Ore_T03.WID_Assault_SemiAuto_Athena_VR_Ore_T03")), // burst purple
    };

    return AssaultRifles[rand() % (AssaultRifles.size())];
}

FItemAndCount ItemAndCount::GetSnipers()
{
    static std::vector<FItemAndCount> Snipers{
        FItemAndCount(1, {}, Runtime::StaticFindObject<UFortWeaponRangedItemDefinition>("/Game/Athena/Items/Weapons/WID_Sniper_Heavy_Athena_VR_Ore_T03.WID_Sniper_Heavy_Athena_VR_Ore_T03")), // heavy sniper
        FItemAndCount(1, {}, Runtime::StaticFindObject<UFortWeaponRangedItemDefinition>("/Game/Athena/Items/Weapons/WID_Sniper_Heavy_Athena_SR_Ore_T03.WID_Sniper_Heavy_Athena_SR_Ore_T03")), // heavy sniper
        FItemAndCount(1, {}, Runtime::StaticFindObject<UFortWeaponRangedItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_HandCannon_Athena_VR_Ore_T03.WID_Pistol_HandCannon_Athena_VR_Ore_T03")), // digle
        FItemAndCount(1, {}, Runtime::StaticFindObject<UFortWeaponRangedItemDefinition>("/Game/Athena/Items/Weapons/WID_Sniper_BoltAction_Scope_Athena_R_Ore_T03.WID_Sniper_BoltAction_Scope_Athena_R_Ore_T03")), // bolt sniper
    };

    return Snipers[rand() % (Snipers.size() - 1)];
}

FItemAndCount ItemAndCount::GetHeals()
{
    static std::vector<FItemAndCount> Heals{
        FItemAndCount(3, {}, Runtime::StaticFindObject<UFortWeaponRangedItemDefinition>("/Game/Athena/Items/Consumables/Shields/Athena_Shields.Athena_Shields")), // big pots
        FItemAndCount(6, {}, Runtime::StaticFindObject<UFortWeaponRangedItemDefinition>("/Game/Athena/Items/Consumables/ShieldSmall/Athena_ShieldSmall.Athena_ShieldSmall")), // minis
        FItemAndCount(6, {}, Runtime::StaticFindObject<UFortWeaponRangedItemDefinition>("/Game/Athena/Items/Consumables/ShockwaveGrenade/Athena_ShockGrenade.Athena_ShockGrenade")), // shockwave
        FItemAndCount(4, {}, Runtime::StaticFindObject<UFortWeaponRangedItemDefinition>("/Game/Athena/Items/Consumables/ChillBronco/Athena_ChillBronco.Athena_ChillBronco")), // chug splash
        FItemAndCount(3, {}, Runtime::StaticFindObject<UFortWeaponRangedItemDefinition>("/Game/Athena/Items/Consumables/Flopper/WID_Athena_Flopper.WID_Athena_Flopper")), // flopper
     };

    return Heals[rand() % (Heals.size() - 1)];
}

UFortAmmoItemDefinition* ItemAndCount::GetAmmo(EAmmoType AmmoType)
{
    static auto Assault = Runtime::StaticFindObject<UFortAmmoItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsLight.AthenaAmmoDataBulletsLight");
    static auto Shotgun = Runtime::StaticFindObject<UFortAmmoItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataShells.AthenaAmmoDataShells");
    static auto Submachine = Runtime::StaticFindObject<UFortAmmoItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsMedium.AthenaAmmoDataBulletsMedium");
    static auto Rocket = Runtime::StaticFindObject<UFortAmmoItemDefinition>("/Game/Athena/Items/Ammo/AmmoDataRockets.AmmoDataRockets");
    static auto Sniper = Runtime::StaticFindObject<UFortAmmoItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsHeavy.AthenaAmmoDataBulletsHeavy");

    switch (AmmoType) {
        case EAmmoType::Assault: return Assault;
        case EAmmoType::Shotgun: return Shotgun;
        case EAmmoType::Submachine: return Submachine;
        case EAmmoType::Rocket: return Rocket;
        case EAmmoType::Sniper: return Sniper;
        default: return nullptr;
    }
}
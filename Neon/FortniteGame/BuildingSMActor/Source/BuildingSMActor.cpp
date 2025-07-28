#include "pch.h"
#include "../Header/BuildingSMActor.h"

#include "Engine/Kismet/Header/Kismet.h"
#include "Engine/NetDriver/Header/NetDriver.h"
#include "FortniteGame/FortGameState/Header/FortGameState.h"

void ABuildingSMActor::OnDamageServer(ABuildingSMActor* BuildingActor,
                                      float Damage,
                                      FGameplayTagContainer DamageTags,
                                      FVector Momentum,
                                      FHitResult HitInfo,
                                      AFortPlayerControllerAthena* Controller,
                                      AActor* DamageCauser,
                                      FGameplayEffectContextHandle Context)
{
    AFortGameStateAthena* GameState = UWorld::GetWorld()->GetGameState();
    
    if (!BuildingActor || !Controller || !GameState)
        return OnDamageServerOG(BuildingActor, Damage, DamageTags, Momentum, HitInfo, Controller, DamageCauser, Context);

    if (BuildingActor->Get<bool>("BuildingActor", "bPlayerPlaced") || BuildingActor->CallFunc<float>("BuildingActor", "GetHealth") <= 1.0f)
        return OnDamageServerOG(BuildingActor, Damage, DamageTags, Momentum, HitInfo, Controller, DamageCauser, Context);

    if (!DamageCauser || 
    !DamageCauser->IsA<AFortWeapon>() || 
    !static_cast<AFortWeapon*>(DamageCauser)->GetWeaponData()->IsA<UFortWeaponMeleeItemDefinition>()) {
        return OnDamageServerOG(BuildingActor, Damage, DamageTags, Momentum, HitInfo, Controller, DamageCauser, Context);
    }

    static FName PickaxeTag = UKismetStringLibrary::GetDefaultObj()->CallFunc<FName>("KismetStringLibrary","Conv_StringToName", FString(L"Weapon.Melee.Impact.Pickaxe"));
    FGameplayTag* DamageTagEntry = nullptr;
    for (FGameplayTag& entry : DamageTags.GameplayTags) {
        if (entry.TagName.GetComparisonIndex() == PickaxeTag.GetComparisonIndex()) {
            DamageTagEntry = &entry;
            break;
        }
    }

    if (!DamageTagEntry) {
        return OnDamageServerOG(BuildingActor, Damage, DamageTags, Momentum, HitInfo, Controller, DamageCauser, Context);
    }
}

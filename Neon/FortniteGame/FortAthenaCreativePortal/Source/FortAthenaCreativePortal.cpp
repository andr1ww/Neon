#include "pch.h"
#include "../Header/FortAthenaCreativePortal.h"

void AFortAthenaCreativePortal::TeleportPlayerToLinkedVolume(AFortAthenaCreativePortal* Portal, FFrame& Stack)
{
    AFortPlayerPawn* PlayerPawn;
    bool bUseSpawnTags;
    Stack.StepCompiledIn(&PlayerPawn);
    Stack.StepCompiledIn(&bUseSpawnTags);
    Stack.IncrementCode();

    if (!PlayerPawn)
        return;

    auto Volume = Portal->GetLinkedVolume();
    auto Location = Volume->K2_GetActorLocation();
    Location.Z = 10000;

    PlayerPawn->K2_TeleportTo(Location, FRotator());
    PlayerPawn->BeginSkydiving(false);
}

void AFortAthenaCreativePortal::TeleportPlayerForPlotLoadComplete(AFortAthenaCreativePortal* Portal, FFrame& Stack)
{
    AFortPlayerPawn* PlayerPawn = nullptr;
    FRotator TeleportRotation;
    Stack.StepCompiledIn(&PlayerPawn);
    Stack.StepCompiledIn(&TeleportRotation);
    Stack.IncrementCode();

    AFortInventory::GiveItem((AFortPlayerControllerAthena*) PlayerPawn->GetController(), Runtime::StaticFindObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/Prototype/WID_CreativeTool.WID_CreativeTool"), 1, 1, 0);
}

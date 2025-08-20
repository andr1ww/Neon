#include "pch.h"
#include "../Header/BuildingGameplayActor.h"

#include "Engine/NetDriver/Header/NetDriver.h"

void ABuildingGameplayActorSpawnMachine::RebootingDelegate(ABuildingGameplayActorSpawnMachine* SpawnMachine)
{
    UE_LOG(LogNeon, Log, "Oi FUck you mate");
    
    if (!SpawnMachine->GetResurrectLocation())
    {
        return;
    }
    
    if (SpawnMachine->GetPlayerIdsForResurrection().Num() <= 0)
        return;
    
    auto GameState = UWorld::GetWorld()->GetGameState();

    AFortPlayerControllerAthena* PlayerController = nullptr;
    
    auto& SquadArray = GameState->GetSquadArray()[SpawnMachine->GetSquadId()];

    for (int i = 0; i < SquadArray.ElementData.Num(); ++i)
    {
        auto StrongPlayerState = SquadArray.ElementData[i].Get();

        if (!StrongPlayerState)
            continue;

        PlayerController = (AFortPlayerControllerAthena*)(StrongPlayerState->GetOwner());

        if (!PlayerController)
            continue;

        if (PlayerController->GetUniqueID() == SpawnMachine->GetInstigatorPC().ObjectIndex)
            continue;

        break;
    }

    if (!PlayerController)
        return;

    auto PlayerState = PlayerController->GetPlayerState();
    
    if (!PlayerState)
        return;

    auto ResurrectionComponent = PlayerController->GetResurrectionComponent();

    if (!ResurrectionComponent)
        return;
    
    ResurrectionComponent->GetResurrectionLocation().ObjectIndex = SpawnMachine->GetResurrectLocation()->GetUniqueID();
    ResurrectionComponent->GetResurrectionLocation().ObjectSerialNumber = GUObjectArray.GetSerialNumber(SpawnMachine->GetResurrectLocation()->GetUniqueID());

    auto StrongResurrectionLocation = ResurrectionComponent->GetResurrectionLocation().Get();
    
    if (!StrongResurrectionLocation)
        return;

    auto GameMode = UWorld::GetWorld()->GetAuthorityGameMode();
    GameMode->RestartPlayer(PlayerController);

    PlayerState->GetRespawnData().bRespawnDataAvailable = false;
    PlayerController->SetbPlayerIsWaiting(false);
    PlayerController->CallFunc<void>("FortPlayerControllerAthena", "RespawnPlayerAfterDeath", false);

    PlayerController->CallFunc<void>("FortPlayerControllerZone", "ClientClearDeathNotification");
    
    PlayerState->SetRebootCounter(PlayerState->GetRebootCounter() + 1);
    PlayerState->CallFunc<void>("FortPlayerStateAthena", "OnRep_RebootCounter");

    static void (*FinishResurrection)(ABuildingGameplayActorSpawnMachine* SpawnMachine, int SquadId) = decltype(FinishResurrection)(Finder->FinishResurrection());
    if (FinishResurrection)
    {
        static void (*AddToAlivePlayersOriginal)(AFortGameModeAthena * GameMode, AFortPlayerControllerAthena * Player) = decltype(AddToAlivePlayersOriginal)(Finder->AddToAlivePlayers());

        AddToAlivePlayersOriginal(UWorld::GetWorld()->GetAuthorityGameMode(), PlayerController);
        
        FinishResurrection(SpawnMachine, SpawnMachine->GetSquadId());

        PlayerState->SetDeathInfo({});
        PlayerState->OnRep_DeathInfo();
        PlayerController->CallFunc<void>("FortPlayerControllerZone", "ClientOnPawnSpawned");
    }
}

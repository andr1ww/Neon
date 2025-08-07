#include "pch.h"
#include "../Header/FortSafeZoneIndicator.h"

#include "Engine/NetDriver/Header/NetDriver.h"
#include "FortniteGame/FortGameState/Header/FortGameState.h"
#include "Neon/Config.h"

// this doesnt get called idk why?
void AFortSafeZoneIndicator::OnSafeZoneStateChange(AFortSafeZoneIndicator* Indicator, EFortSafeZoneState NewState, bool bInitial)
{
    OnSafeZoneStateChangeOG(Indicator, NewState, bInitial);
    
    if (Config::bLateGame)
    {
        static bool SetSafeZone = false;

        if (!SetSafeZone)
        {
            SetSafeZone = true;
            AFortGameStateAthena* GameState = UWorld::GetWorld()->GetGameState();
            
            GameState->SetSafeZonesStartTime(0.0001f);
            GameState->SetGamePhase(EAthenaGamePhase::SafeZones);
            GameState->CallFunc<void>("FortGameStateAthena", "OnRep_GamePhase", EAthenaGamePhase::Aircraft);
        }
    }
}

#pragma once
#include "pch.h"

class UFinder {
public:
    static inline uintptr_t FindBytes(Memcury::Scanner& Scanner,
        const std::vector<uint8_t>& Bytes,
        int Count = 255,
        int SkipBytes = 0,
        bool bGoUp = false,
        int Skip = 0,
        const bool bPrint = false) {
        if (!Scanner.Get()) return 0;

        auto Base = __int64(GetModuleHandleW(0));

        for (int i = SkipBytes; i < Count + SkipBytes; i++) {
            auto CurrentByte = *(Memcury::ASM::MNEMONIC*)(bGoUp ? Scanner.Get() - i : Scanner.Get() + i);

            if (CurrentByte == Bytes[0]) {
                bool Found = true;
                for (int j = 1; j < Bytes.size(); j++) {
                    if (*(Memcury::ASM::MNEMONIC*)(bGoUp ? Scanner.Get() - i + j : Scanner.Get() + i + j) != Bytes[j]) {
                        Found = false;
                        break;
                    }
                }
                if (Found) {
                    if (Skip > 0) {
                        Skip--;
                        continue;
                    }
                    return bGoUp ? Scanner.Get() - i : Scanner.Get() + i;
                }
            }
        }
        return -1;
    }

    uint64 FinishResurrection();

    uint64 RebootingDelegate();
    
    uint64 StartAircraftPhase();

    uint64 StartNewSafeZonePhase();

    uint64_t WorldNetMode();

    uint64 GIsClient();

    uint64 TickFlush();

    uint64 InitHost();

    uint64 PauseBeaconRequests();

    uint64 SetWorld();

    uint64 GetMaxTickRate();

    uint64 DispatchRequest();

    uint64 CreateNetDriver();

    uint64 ProcessEvent();

    uint64 ApplyCharacterCustomization();

    uint64 KickPlayer();

    uint64 InitListen();

    uint64 GameSessionPatch();

    uint64 GetWorldContextFromObject();

    uint64 CreateNetDriverLocal();

    uint64 CreateChannel();

    uint64 SendClientAdjustment();

    uint64 CallPreReplication();

    uint64 RemoveNetworkActor();

    uint64 DemoReplicateActor();

    uint64 WorldGetNetMode();

    uint64 RepDriverServerReplicateActors();

    uint64 StaticFindObject();

    uint64 StaticLoadObject();

    uint64 InternalTryActivateAbility();

    uint64 ConstructSpec();

    uint64 GiveAbility();

    uint64 GiveAbilityAndActivateOnce();

    uint64 GetInterfaceAddress();

    uint64 OnDamageServer();
    
    uint64 ClientOnPawnDied();

    uint64 SpawnBot();

    uint64 SpawnActor();

    uint64 SetupPickup();
    
    uint64 CantBuild();

    uint64 ReplaceBuildingActor();

    uint64 BotManagerSetup();

    uint64 SpawnBotRet();

    uint64 OnPossessedPawnDied();

    uint64 CompletePickupAnimation();

    uint64 RemoveFromAlivePlayers();

    uint64 RegisterComponentWithWorld();

    uint64 CreateAndConfigureNavigationSystem();

    uint64 SendComplexCustomStatEvent();

    uint64 SpawnLoot();

    uint64 ReloadWeapon();

    uint64 GetCommandLet();

    uint64 GetSquadIdForCurrentPlayer();
    
    uint64 OnSafeZoneStateChange();

    uint64 InstructionForCollision();

    uint64 GetGameSessionClass();

    uint64 MatchmakingSerivcePerms();

    uint64 EnterAircraft();

    uint64 OnRep_ZiplineState();

    uint64 PickTeam();

    uint64 ServerOnAttemptInteract();

    uint64 OnPlayImpactFX();

    uint64 AddToAlivePlayers();

    uint64 LoadPlayset(const std::vector<uint8_t>& Bytes = std::vector<uint8_t>({ 0x48, 0x89, 0x5C }), int recursive = 0);
};

inline UFinder* Finder = new UFinder();
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

    uint64 SpawnBot();

    uint64 SpawnActor();
    
    uint64 CantBuild();
};

inline UFinder* Finder = new UFinder();
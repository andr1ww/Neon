#pragma once
#include "pch.h"

namespace Addresses {
    extern uint64_t WorldNetMode;
    extern uint64_t GIsClient;
    extern uint64_t TickFlush;
    extern uint64_t InitHost;
    extern uint64_t PauseBeaconRequests;
    extern uint64_t SetWorld;
    extern uint64_t GetMaxTickRate;
    extern uint64_t DispatchRequest;
    extern uint64_t CreateNetDriver;
    extern uint64_t ProcessEvent;
    extern uint64_t ApplyCharacterCustomization;
    extern uint64_t KickPlayer;
    extern uint64_t InitListen;
    extern uint64_t GameSessionPatch;
    extern uint64_t GetWorldContextFromObject;
    extern uint64_t CreateNetDriverLocal;
    extern uint64_t CreateChannel;
    extern uint64_t SendClientAdjustment;
    extern uint64_t CallPreReplication;
    extern uint64_t RemoveNetworkActor;
    extern uint64_t DemoReplicateActor;
    extern uint64_t WorldGetNetMode;
    extern uint64_t RepDriverServerReplicateActors;
    extern uint64_t StaticFindObject;
    extern uint64_t InternalTryActivateAbility;
    extern uint64_t ConstructSpec;
    extern uint64_t GiveAbility;
    extern uint64_t GiveAbilityAndActivateOnce;
    extern uint64_t GetInterfaceAddress;
}

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

    void Init();

private:
    uint64_t FindWorldNetMode();
    uint64_t FindGIsClient();
    uint64_t FindTickFlush();
    uint64_t FindInitHost();
    uint64_t FindPauseBeaconRequests();
    uint64_t FindSetWorld();
    uint64_t FindGetMaxTickRate();
    uint64_t FindDispatchRequest();
    uint64_t FindCreateNetDriver();
    uint64_t FindProcessEvent();
    uint64_t FindApplyCharacterCustomization();
    uint64_t FindKickPlayer();
    uint64_t FindInitListen();
    uint64_t FindGameSessionPatch();
    uint64_t FindGetWorldContextFromObject();
    uint64_t FindCreateNetDriverLocal();
    uint64_t FindCreateChannel();
    uint64_t FindSendClientAdjustment();
    uint64_t FindCallPreReplication();
    uint64_t FindRemoveNetworkActor();
    uint64_t FindDemoReplicateActor();
    uint64_t FindWorldGetNetMode();
    uint64_t FindRepDriverServerReplicateActors();
    uint64_t FindStaticFindObject();
    uint64_t FindInternalTryActivateAbility();
    uint64_t FindConstructSpec();
    uint64_t FindGiveAbility();
    uint64_t FindGiveAbilityAndActivateOnce();
    uint64_t FindGetInterfaceAddress();
};

inline UFinder* Finder = new UFinder();

#define INIT Finder->Init()
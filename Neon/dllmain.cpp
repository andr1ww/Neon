#include "pch.h"

#include "FortniteGame/FortGameMode/Header/FortGameMode.h"

void Main() {
    AllocConsole();
    FILE* File = nullptr;
    freopen_s(&File, "CONOUT$", "w+", stdout);
    freopen_s(&File, "CONOUT$", "w+", stderr);
    SetConsoleTitleA("Neon | Setting up");

    SDK::Init();

    UE_LOG(LogNeon, Error, "FUCK FUCK TRANNY");

    AFortGameModeAthena* GameMode = nullptr;
    GameMode->GetbWorldIsReady();
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)Main, nullptr, 0, nullptr);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


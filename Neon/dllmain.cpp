#include "pch.h"

#include "Engine/Kismet/Header/Kismet.h"
#include "Engine/NetDriver/Header/NetDriver.h"
#include "FortniteGame/FortGameMode/Header/FortGameMode.h"
#include "Neon/Finder/Header/Finder.h"
#include "Neon/Runtime/Runtime.h"

void Main() {
    AllocConsole();
    FILE* File = nullptr;
    freopen_s(&File, "CONOUT$", "w+", stdout);
    freopen_s(&File, "CONOUT$", "w+", stderr);
    SetConsoleTitleA("Neon | Setting up");
    SDK::Init();
     
    MH_Initialize();
    Sleep(7500);

    *(bool*)(IMAGEBASE + 0xEBD8A4C) = false; //for zonngster: this is GISClient which defines if the game is client 
    *(bool*)(IMAGEBASE + 0xEBD8A4C + 1) = true; // this is GISServer which defines if the game is server
    
    Runtime::ExecVFT<AFortGameModeAthena>("/Script/Engine.GameMode.ReadyToStartMatch", AFortGameModeAthena::ReadyToStartMatch);
    Runtime::Hook(Finder->TickFlush(), UNetDriver::TickFlush, (void**)TickFlushOriginal);

    Runtime::Hook(Finder->KickPlayer(), RetTrue);
    Runtime::Hook(Finder->WorldNetMode(), RetTrue);
    Runtime::Hook(Finder->WorldGetNetMode(), RetTrue);
    Runtime::Patch(IMAGEBASE + 0xA065C42 + 1, 0x85);
    Runtime::Patch(IMAGEBASE + 0x2CB0094, 0xC3);
    Runtime::Patch(IMAGEBASE + 0x6060C30, 0xC3);
    Runtime::Patch(IMAGEBASE + 0x25F340C, 0xC3);
    Runtime::Patch(IMAGEBASE + 0x28EAA10, 0xC3);
    
    UWorld::GetWorld()->GetOwningGameInstance()->GetLocalPlayers().Remove(0);
    UKismetSystemLibrary::GetDefaultObj()->CallFunc<void>("KismetSystemLibrary", "ExecuteConsoleCommand", UWorld::GetWorld(), FString(L"open Asteria_Terrain"), nullptr);
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


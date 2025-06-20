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

    *(bool*)(Finder->GIsClient()) = false; 
    *(bool*)(Finder->GIsClient() + 1) = true;
    
    Runtime::ExecVFT<AFortGameModeAthena>("/Script/Engine.GameMode.ReadyToStartMatch", AFortGameModeAthena::ReadyToStartMatch);
    Runtime::Hook(Finder->TickFlush(), UNetDriver::TickFlush, (void**)TickFlushOriginal);

    Runtime::Hook(Finder->KickPlayer(), RetTrue);
    Runtime::Hook(Finder->WorldNetMode(), RetTrue);
    Runtime::Hook(Finder->WorldGetNetMode(), RetTrue);
    
    UWorld::GetWorld()->GetOwningGameInstance()->GetLocalPlayers().Remove(0);
    FString WorldName;
    if (Fortnite_Version <= 10.40)
    {
        WorldName = FString("open Athena_Terrain");
    }
    else if (Fortnite_Version <= 18.40 && Fortnite_Version >= 10.40)
    {
        WorldName = FString("open Apollo_Terain");
    }
    else if (Fortnite_Version <= 22.40 && Fortnite_Version >= 19.00)
    {
        WorldName = FString("open Artemis_Terrain");
    } else if (Fortnite_Version <= 23.00)
    {
        WorldName = FString("open Asteria_Terrain");
    }
    
    UKismetSystemLibrary::GetDefaultObj()->CallFunc<void>("KismetSystemLibrary", "ExecuteConsoleCommand", UWorld::GetWorld(), WorldName, nullptr);
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


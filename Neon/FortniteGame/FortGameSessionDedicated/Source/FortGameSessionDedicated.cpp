#include "pch.h"
#include "../Header/FortGameSessionDedicated.h"

#include "Engine/GameplayStatics/Header/GameplayStatics.h"
#include "Engine/UEngine/Header/UEngine.h"
#include "FortniteGame/FortLootPackage/Header/FortLootPackage.h"

UClass** FortGameSessionDedicated::GetGameSessionClass(AFortGameModeAthena* GameMode, UClass** ClassRet)
{
    *ClassRet = AFortGameSessionDedicatedAthena::StaticClass();
    return ClassRet;
}

const wchar_t* FortGameSessionDedicated::Get()
{
    static auto cmdLine = std::wstring(GetOG()) + L" -AllowAllPlaylistsInShipping -EnableMMS";
    return cmdLine.c_str(); // -EnableMMS may work for backfill?
}

void FortGameSessionDedicated::Restart(void*)
{
    TerminateProcess(GetCurrentProcess(), 1);
}

void FortGameSessionDedicated::UWorld_Listen()
{
    UNetDriver* NetDriver = UWorld::GetWorld()->GetNetDriver();
    FName GameNetDriver = UKismetStringLibrary::Conv_StringToName(L"GameNetDriver");
    
    if (Fortnite_Version >= 16.40)
    {
        static void* (*GetWorldContextFromObject)(UEngine*, UWorld*) = decltype(GetWorldContextFromObject)(Finder->GetWorldContextFromObject());
        void* WorldContext = GetWorldContextFromObject(UEngine::GetEngine(), UWorld::GetWorld());
        static UNetDriver* (*CreateNetDriver_Local)(UEngine*, void* WorldContext, FName) = decltype(CreateNetDriver_Local)(Finder->CreateNetDriverLocal());
        NetDriver = CreateNetDriver_Local(UEngine::GetEngine(), WorldContext, GameNetDriver);
    } else
    {
        NetDriver = ((UNetDriver * (*)(UObject*, UObject*, FName)) Finder->CreateNetDriver())(UEngine::GetEngine(), UWorld::GetWorld(), GameNetDriver);
    }
        
    UWorld::GetWorld()->SetNetDriver(NetDriver);
    
    NetDriver->SetNetDriverName(GameNetDriver);
    NetDriver->NetDriverSetWorld(UWorld::GetWorld());

    FURL URL{};
    URL.Port = 7777;

    NetDriver->InitListen(UWorld::GetWorld(), URL, false);
    NetDriver->NetDriverSetWorld(UWorld::GetWorld());
            
    for (int i = 0; i < UWorld::GetWorld()->GetLevelCollections().Num(); i++)
    {
        UWorld::GetWorld()->GetLevelCollections()[i].NetDriver = NetDriver;
    }

    SetConsoleTitleA("Neon | Listening on Port: 7777");

   // return true;
}

FServicePermissionsMcp* FortGameSessionDedicated::MatchmakingServicePerms(int64, int64)
{
    static FServicePermissionsMcp* ServicePerms = nullptr;
    if (!ServicePerms)
    {
        ServicePerms = new FServicePermissionsMcp{ .Id = L"ec684b8c687f479fadea3cb2ad83f5c6" };
    }
    
    return ServicePerms;
}
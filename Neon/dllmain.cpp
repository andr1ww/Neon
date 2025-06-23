#include "pch.h"

#include "Engine/Kismet/Header/Kismet.h"
#include "Engine/NetDriver/Header/NetDriver.h"
#include "FortniteGame/AbilitySystemComponent/Header/AbilitySystemComponent.h"
#include "FortniteGame/FortGameMode/Header/FortGameMode.h"
#include "FortniteGame/FortPlayerController/Header/FortPlayerController.h"
#include "Neon/Finder/Header/Finder.h"
#include "Neon/Runtime/Runtime.h"

static inline std::vector<uint64_t> NullFuncs = {};
static inline std::vector<uint64_t> RetTrueFuncs = {};

void InitNullsAndRetTrues() {
	if (Fortnite_Version == 0) NullFuncs.push_back(Memcury::Scanner::FindPattern("48 89 54 24 ? 48 89 4C 24 ? 55 53 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 8B 41 08 C1 E8 05").Get());
	else if (Fortnite_Version >= 3.3 && Fortnite_Version <= 4.5) NullFuncs.push_back(Memcury::Scanner::FindPattern("48 8B C4 57 48 81 EC ? ? ? ? 4C 8B 82 ? ? ? ? 48 8B F9 0F 29 70 E8 0F 29 78 D8").Get());
	else if (Fortnite_Version == 4.1) NullFuncs.push_back(Memcury::Scanner::FindPattern("4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 89 5B 10 48 8D 05 ? ? ? ? 48 8B 1D ? ? ? ? 49 89 73 18 33 F6 40").Get());
	else if (Fortnite_Version >= 5.00 && Fortnite_Version < 7.00) {
		NullFuncs.push_back(Memcury::Scanner::FindPattern("48 8B C4 48 89 58 08 48 89 70 10 57 48 81 EC ? ? ? ? 48 8B BA ? ? ? ? 48 8B DA 0F 29").Get());
		NullFuncs.push_back(Memcury::Scanner::FindStringRef(L"Widget Class %s - Running Initialize On Archetype, %s.").ScanFor(Fortnite_Version < 6.3 ? std::vector<uint8_t>{ 0x40, 0x55 } : std::vector<uint8_t>{ 0x48, 0x89, 0x5C }, false).Get());
	}
	else if (Fortnite_Version >= 7.00 && Fortnite_Version <= 12.00) NullFuncs.push_back(Memcury::Scanner::FindPattern("48 89 5C 24 ? 57 48 83 EC 30 48 8B 41 28 48 8B DA 48 8B F9 48 85 C0 74 34 48 8B 4B 08 48 8D").Get());
	else if (Fortnite_Version >= 12.21 && Fortnite_Version < 13.00) {
		NullFuncs.push_back(Memcury::Scanner::FindStringRef(L"Widget Class %s - Running Initialize On Archetype, %s.").ScanFor(std::vector<uint8_t>{ 0x48, 0x89, 0x5C }, false).Get()); // for 12.41
		NullFuncs.push_back(Memcury::Scanner::FindPattern(Fortnite_Version == 12.41 ? "40 57 41 56 48 81 EC ? ? ? ? 80 3D ? ? ? ? ? 0F B6 FA 44 8B F1 74 3A 80 3D ? ? ? ? ? 0F" : "40 57 41 56 48 81 EC ? ? ? ? 80 3D ? ? ? ? ? 0F B6 FA 44 8B F1 74 3A 80 3D ? ? ? ? ? 0F 82").Get());
		if (Fortnite_Version == 12.41) NullFuncs.push_back(Memcury::Scanner::FindPattern("48 8B C4 48 89 58 08 48 89 68 10 48 89 70 18 48 89 78 20 41 54 41 56 41 57 48 83 EC 70 48 8B 35").Get());
		else if (Fortnite_Version == 12.61) NullFuncs.push_back(Memcury::Scanner::FindPattern("48 89 5C 24 ? 55 57 41 54 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 20 4C 8B A5").Get());
	}
	else if (Fortnite_Version == 14.60) NullFuncs.push_back(Memcury::Scanner::FindPattern("40 55 57 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 80 3D ? ? ? ? ? 0F B6 FA 44 8B F9 74 3B 80 3D ? ? ? ? ? 0F").Get());
	else if (Fortnite_Version >= 17.00) {
		NullFuncs.push_back(Memcury::Scanner::FindPattern("48 89 5C 24 10 48 89 6C 24 20 56 57 41 54 41 56 41 57 48 81 EC ? ? ? ? 65 48 8B 04 25 ? ? ? ? 4C 8B F9").Get());
		if (Fortnite_Version.GetMajorVersion() == 17) NullFuncs.push_back(Memcury::Scanner::FindPattern("48 8B C4 48 89 70 08 48 89 78 10 55 41 54 41 55 41 56 41 57 48 8D 68 A1 48 81 EC ? ? ? ? 45 33 ED").Get());
		else if (Fortnite_Version >= 19.00) NullFuncs.push_back(Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 55 41 55 41 56 48 8B EC 48 83 EC 50 83 65 28 00 40 B6 05 40 38 35 ? ? ? ? 4C").Get());
		else if (Fortnite_Version >= 20.00) {
			NullFuncs.push_back(Memcury::Scanner::FindPattern("48 8B C4 48 89 58 08 4C 89 40 18 48 89 50 10 55 56 57 41 54 41 55 41 56 41 57 48 8D 68 98 48 81 EC ? ? ? ? 49 8B 48 20 45 33").Get());
			NullFuncs.push_back(Memcury::Scanner::FindPattern("48 89 5C 24 ? 57 48 83 EC 20 48 8B 41 20 48 8B FA 48 8B D9 BA ? ? ? ? 83 78 08 03 0F 8D").Get());
			NullFuncs.push_back(Memcury::Scanner::FindPattern("4C 89 44 24 ? 53 55 56 57 41 54 41 55 41 56 41 57 48 83 EC 68 48 8D 05 ? ? ? ? 0F").Get());
			NullFuncs.push_back(Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC 30 48 8B F9 48 8B CA E8").Get());
			NullFuncs.push_back(Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 ? 41 ? 48 83 EC 60 45 33 F6 4C 8D ? ? ? ? ? 48 8B DA").Get());
		}
	}

	if (Fortnite_Version >= 19.00) NullFuncs.push_back(Memcury::Scanner::FindPattern("48 89 5C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 83 EC 50 4C 8B FA 48 8B F1 E8").Get());
	else if (Fortnite_Version >= 16.40) NullFuncs.push_back(Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 55 41 54 41 55 41 56 41 57 48 8B EC 48 83 EC 70 4C 8B FA 4C").Get());
	else if (Fortnite_Version == 2.5) NullFuncs.push_back(Memcury::Scanner::FindPattern("40 55 56 41 56 48 8B EC 48 81 EC ? ? ? ? 48 8B 01 4C 8B F2").Get());
	else {
		auto sRef = Memcury::Scanner::FindStringRef(L"Changing GameSessionId from '%s' to '%s'");
		NullFuncs.push_back(sRef.ScanFor({ 0x40, 0x55 }, false, 0, 1, 2000).Get());
	}
	if (Fortnite_Version < 16.40) {
		auto Addr = Memcury::Scanner::FindStringRef(L"STAT_CollectGarbageInternal");
		NullFuncs.push_back(Addr.ScanFor({ 0x48, 0x89, 0x5C }, false, 0, 1, 2000).Get());
	}

	NullFuncs.push_back(Finder->KickPlayer());
	RetTrueFuncs.push_back(Finder->WorldNetMode());
	RetTrueFuncs.push_back(Finder->WorldGetNetMode());
	
	if (Fortnite_Version == 0) RetTrueFuncs.push_back(Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 6C 24 ? 57 41 56 41 57 48 81 EC ? ? ? ? 48 8B 01 49 8B E9 45 0F B6 F8").Get());
	else if (Fortnite_Version >= 16.40) {
		if (Fortnite_Version.GetMajorVersion() == 17) RetTrueFuncs.push_back(Memcury::Scanner::FindPattern("48 8B C4 48 89 58 08 48 89 70 10 48 89 78 18 4C 89 60 20 55 41 56 41 57 48 8B EC 48 83 EC 60 4D 8B F9 41 8A F0 4C 8B F2 48 8B F9 45 32 E4").Get());
		RetTrueFuncs.push_back(Memcury::Scanner::FindPattern("48 8B C4 48 89 58 08 48 89 70 10 48 89 78 18 4C 89 60 20 55 41 56 41 57 48 8B EC 48 83 EC 60 49 8B D9 45 8A").Get());
	}

	for (auto& Func : NullFuncs) {
		if (Func == 0x0) continue;
		Runtime::Patch(Func, 0xC3);
	}

	for (auto& Func : RetTrueFuncs) {
		if (Func == 0x0) continue;
		Runtime::Hook(Func, RetTrue);
	}
}

void Main() {
    AllocConsole();
    FILE* File = nullptr;
    freopen_s(&File, "CONOUT$", "w+", stdout);
    freopen_s(&File, "CONOUT$", "w+", stderr);
    SetConsoleTitleA("Neon | Setting up");
    SDK::Init();

    MH_Initialize();
    Sleep(5000);
	
    *(bool*)(Finder->GIsClient()) = false; 
    *(bool*)(Finder->GIsClient() + 1) = true;

	InitNullsAndRetTrues();
	
	Runtime::Hook<&AFortGameModeAthena::StaticClass>("ReadyToStartMatch", AFortGameModeAthena::ReadyToStartMatch, ReadyToStartMatchOriginal);
	Runtime::Hook<&AFortGameModeAthena::StaticClass>("SpawnDefaultPawnFor", AFortGameModeAthena::SpawnDefaultPawnFor);
	Runtime::Exec<&AFortPlayerControllerAthena::StaticClass>("ServerAcknowledgePossession", AFortPlayerControllerAthena::ServerAcknowledgePossession);

	int InternalServerTryActivateAbilityIndex = 0;

	if (Engine_Version > 4.20)
	{
		static auto OnRep_ReplicatedAnimMontageFn = (UFunction*)GUObjectArray.FindObject("OnRep_ReplicatedAnimMontage");
		InternalServerTryActivateAbilityIndex = (Runtime::GetFunctionIdx(OnRep_ReplicatedAnimMontageFn) - 8) / 8;
		UE_LOG(LogNeon, Log, "InternalServerTryActivateAbilityIndex: 0x%x", InternalServerTryActivateAbilityIndex);
	}
	
	Runtime::Every<UAbilitySystemComponent>(InternalServerTryActivateAbilityIndex, UAbilitySystemComponent::InternalServerTryActivateAbility);

    Runtime::Hook(Finder->TickFlush(), UNetDriver::TickFlush, (void**)&TickFlushOriginal);
	Runtime::Hook(Finder->DispatchRequest(), UNetDriver::DispatchRequest, (void**)&DispatchRequestOriginal);
	
    UWorld::GetWorld()->GetOwningGameInstance()->GetLocalPlayers().Remove(0);
    FString WorldName;
    if (Fortnite_Version <= 10.40)
    {
        WorldName = FString(L"open Athena_Terrain");
    }
    else if (Fortnite_Version <= 18.40 && Fortnite_Version >= 10.40)
    {
        WorldName = FString(L"open Apollo_Terain");
    }
    else if (Fortnite_Version <= 22.40 && Fortnite_Version >= 19.00)
    {
        WorldName = FString(L"open Artemis_Terrain");
    } else if (Fortnite_Version <= 23.00)
    {
        WorldName = FString(L"open Asteria_Terrain");
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


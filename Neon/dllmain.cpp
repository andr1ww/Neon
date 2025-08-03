#include "pch.h"

#include "Engine/HTTP/Header/HTTP.h"
#include "Engine/Kismet/Header/Kismet.h"
#include "Engine/NetDriver/Header/NetDriver.h"
#include "FortniteGame/AbilitySystemComponent/Header/AbilitySystemComponent.h"
#include "FortniteGame/BuildingSMActor/Header/BuildingSMActor.h"
#include "FortniteGame/FortGameMode/Header/FortGameMode.h"
#include "FortniteGame/FortLoot/Header/FortLootPackage.h"
#include "FortniteGame/FortPlayerController/Header/FortPlayerController.h"
#include "FortniteGame/FortQuestManager/Header/FortQuestManager.h"
#include "Neon/Finder/Header/Finder.h"
#include "Neon/Runtime/Runtime.h"

static inline std::vector<uint64_t> NullFuncs = {};
static inline std::vector<uint64_t> RetTrueFuncs = {};
static inline std::vector<uint64_t> FuncsTo85 = {};

static void* (*ProcessEventOG)(UObject*, UFunction*, void*);
void* ProcessEvent(UObject* Obj, UFunction* Function, void* Params)
{
	if (Function && Config::bLogProcessEvent) {
		UE_LOG(LogNeon, Log, "ProcessEvent: %s", Function->GetFName().ToString().ToString().c_str());
	}

	return ProcessEventOG(Obj, Function, Params);
}

void InitNullsAndRetTrues() {
	if (Fortnite_Version >= 23)
		NullFuncs.push_back(Memcury::Scanner::FindPattern("48 89 5C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 83 EC ? 4C 8B E2 4C 8B F1").Get());
	
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

	if (Fortnite_Version == 23.50)
	{
		NullFuncs.push_back(Memcury::Scanner::FindPattern("40 53 48 83 EC ? 48 8B 01 48 8B D9 FF 90 ? ? ? ? 48 85 C0 74 ? F6 80 ? ? ? ? ? 74").Get());
	}

	NullFuncs.push_back(Finder->KickPlayer());
	RetTrueFuncs.push_back(Finder->WorldNetMode());
	RetTrueFuncs.push_back(Finder->WorldGetNetMode());
	
	if (Fortnite_Version == 0) RetTrueFuncs.push_back(Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 6C 24 ? 57 41 56 41 57 48 81 EC ? ? ? ? 48 8B 01 49 8B E9 45 0F B6 F8").Get());
	else if (Fortnite_Version >= 16.40) {
		if (Fortnite_Version.GetMajorVersion() == 17) RetTrueFuncs.push_back(Memcury::Scanner::FindPattern("48 8B C4 48 89 58 08 48 89 70 10 48 89 78 18 4C 89 60 20 55 41 56 41 57 48 8B EC 48 83 EC 60 4D 8B F9 41 8A F0 4C 8B F2 48 8B F9 45 32 E4").Get());
		RetTrueFuncs.push_back(Memcury::Scanner::FindPattern("48 8B C4 48 89 58 08 48 89 70 10 48 89 78 18 4C 89 60 20 55 41 56 41 57 48 8B EC 48 83 EC 60 49 8B D9 45 8A").Get());
	}

	if (Fortnite_Version == 23.50)
	{
		RetTrueFuncs.push_back(Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 4C 8B D1").Get());
		NullFuncs.push_back(Memcury::Scanner::FindPattern("48 8B C4 48 89 58 ? 48 89 70 ? 48 89 78 ? 55 48 8D 68 ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 ? 83 64 24").Get());
	}

	if (Fortnite_Version >= 23.00)
	{
		uintptr_t rawAddress = Memcury::Scanner::FindPattern("0F 84 ? ? ? ? 48 8D 15 ? ? ? ? 48 8D 4C 24 ? E8 ? ? ? ? 80 3D").Get();

		uintptr_t Offset = rawAddress - IMAGEBASE; 
		Offset = Offset + 1;                  
		uintptr_t Addr = Offset + IMAGEBASE;  

		FuncsTo85.push_back(Addr);
	}
	
	for (auto& Func : NullFuncs) {
		if (Func == 0x0) continue;
		Runtime::Patch(Func, 0xC3);
	}

	for (auto& Func : RetTrueFuncs) {
		if (Func == 0x0) continue;
		Runtime::Hook(Func, RetTrue);
	}
	
	for (auto& Func : FuncsTo85)
	{
		if (Func == 0x0) continue;
		Runtime::Patch(Func, 0x85);
	}
	if (Fortnite_Version <= 13.00 && Fortnite_Version >= 12.50)
	{
		Runtime::Hook(IMAGEBASE + 0x2E688D0, RetTrue); // server context
		Runtime::Hook(IMAGEBASE + 0x1BDC660, FortLootPackage::SpawnLoot);
		Runtime::Hook(IMAGEBASE + 0x3F88350, RetTrue); // IsThereAnywhereToBuildNavigation
		Runtime::Hook(IMAGEBASE + 0x243E860, UFortQuestManager::SendComplexCustomStatEvent, (void**)&UFortQuestManager::SendComplexCustomStatEventOG);
	}
}

void Main()
{
	AllocConsole();
	FILE* File = nullptr;
	freopen_s(&File, "CONOUT$", "w+", stdout);
	freopen_s(&File, "CONOUT$", "w+", stderr);
	SetConsoleTitleA("Neon | Setting up");
	SDK::Init();
	Offsets::FMemory__Realloc = Memcury::Scanner::FindPattern("48 89 5C 24 08 48 89 74 24 10 57 48 83 EC ? 48 8B F1 41 8B D8 48 8B 0D ? ? ? ?").Get();
	
	MH_Initialize();
	Sleep(5000);
	
	if (Finder->GIsClient())
	{
		*(bool*)(Finder->GIsClient()) = false; 
		*(bool*)(Finder->GIsClient() + 1) = true;
	} else
	{
		*(bool*)(IMAGEBASE + 0xEBD8A4C) = false; 
		*(bool*)(IMAGEBASE + 0xEBD8A4C + 1) = true; // 23.50
	}
	
	InitNullsAndRetTrues();
	
	Runtime::Exec("/Script/Engine.GameMode.ReadyToStartMatch", AFortGameModeAthena::ReadyToStartMatch, (void**)&ReadyToStartMatchOriginal);
	Runtime::Hook<&AFortGameModeAthena::StaticClass>("SpawnDefaultPawnFor", AFortGameModeAthena::SpawnDefaultPawnFor);
	Runtime::Exec("/Script/Engine.PlayerController.ServerAcknowledgePossession", AFortPlayerControllerAthena::ServerAcknowledgePossession);
	Runtime::Exec("/Script/FortniteGame.FortPlayerController.ServerExecuteInventoryItem", AFortPlayerControllerAthena::ServerExecuteInventoryItem);
	Runtime::Exec("/Script/FortniteGame.FortPlayerController.ServerPlayEmoteItem", AFortPlayerControllerAthena::ServerPlayEmoteItem);
	Runtime::Exec("/Script/FortniteGame.FortControllerComponent_Aircraft.ServerAttemptAircraftJump", AFortPlayerControllerAthena::ServerAttemptAircraftJump);
	Runtime::Hook(Finder->OnDamageServer(), ABuildingSMActor::OnDamageServer, (void**)&ABuildingSMActor::OnDamageServerOG);
	Runtime::Exec("/Script/FortniteGame.FortPlayerController.ServerCreateBuildingActor", AFortPlayerControllerAthena::ServerCreateBuildingActor);
	Runtime::Exec("/Script/FortniteGame.FortPlayerController.ServerBeginEditingBuildingActor", AFortPlayerControllerAthena::ServerBeginEditingBuildingActor);
	Runtime::Exec("/Script/FortniteGame.FortPlayerController.ServerEditBuildingActor", AFortPlayerControllerAthena::ServerEditBuildingActor);
	Runtime::Exec("/Script/FortniteGame.FortPlayerController.ServerEndEditingBuildingActor", AFortPlayerControllerAthena::ServerEndEditingBuildingActor);
	Runtime::Exec("/Script/FortniteGame.FortPlayerController.ServerRepairBuildingActor", AFortPlayerControllerAthena::ServerRepairBuildingActor);
	Runtime::Exec("/Script/FortniteGame.FortPlayerPawn.ServerHandlePickupInfo", AFortPlayerPawn::ServerHandlePickupInfo);
	Runtime::Exec("/Script/FortniteGame.FortPlayerController.ServerLoadingScreenDropped", AFortPlayerControllerAthena::ServerLoadingScreenDropped);
	Runtime::Exec("/Script/FortniteGame.FortPlayerController.ServerAttemptInventoryDrop", AFortPlayerControllerAthena::ServerAttemptInventoryDrop);

	Runtime::Exec("/Script/FortniteGame.FortKismetLibrary.GiveItemToInventoryOwner", AFortPlayerPawn::GiveItemToInventoryOwner, (void**)&AFortPlayerPawn::GiveItemToInventoryOwnerOG);
	Runtime::Exec("/Script/FortniteGame.FortKismetLibrary.K2_RemoveItemFromPlayerByGuid", AFortPlayerControllerAthena::K2_RemoveItemFromPlayerByGuid, (void**)&AFortPlayerControllerAthena::K2_RemoveItemFromPlayerByGuidOG);

	Runtime::Exec("/Script/FortniteGame.FortAthenaAIBotController.OnAlertLevelChanged", UFortServerBotManagerAthena::OnAlertLevelChanged, (void**)&UFortServerBotManagerAthena::OnAlertLevelChangedOG);
	
	Runtime::Hook(Finder->ClientOnPawnDied(), AFortPlayerControllerAthena::ClientOnPawnDied, (void**)&AFortPlayerControllerAthena::ClientOnPawnDiedOG);
	Runtime::VFTHook(UAthenaNavSystem::GetDefaultObj()->GetVTable(), 0x53, UFortServerBotManagerAthena::InitializeForWorld, (void**)&UFortServerBotManagerAthena::InitializeForWorldOG);
	
	if (Finder->CompletePickupAnimation())
	{
		Runtime::Exec("/Script/FortniteGame.FortPlayerPawn.ServerHandlePickup", AFortPlayerPawn::ServerHandlePickup);
		Runtime::Hook(Finder->CompletePickupAnimation(), AFortPlayerPawn::CompletePickupAnimation, (void**)&AFortPlayerPawn::CompletePickupAnimationOG);
		UE_LOG(LogNeon, Log, "CompletePickupAnimation: 0x%x", Finder->CompletePickupAnimation() - IMAGEBASE);
	} else
	{
		Runtime::Exec("/Script/FortniteGame.FortPlayerPawn.ServerHandlePickup", AFortPlayerPawn::ServerHandlePickup);
	}
	
	if (Finder->OnPossessedPawnDied()) Runtime::Hook(Finder->OnPossessedPawnDied(), AFortAthenaAIBotController::OnPossessedPawnDied, (void**)&AFortAthenaAIBotController::OnPossessedPawnDiedOG); 
	int InternalServerTryActivateAbilityIndex = 0;

	if (Engine_Version > 4.20)
	{
		static auto OnRep_ReplicatedAnimMontageFn = Runtime::StaticFindObject<UFunction>("/Script/GameplayAbilities.AbilitySystemComponent.OnRep_ReplicatedAnimMontage");
		InternalServerTryActivateAbilityIndex = Runtime::GetVTableIndex(OnRep_ReplicatedAnimMontageFn) - 1;
		UE_LOG(LogNeon, Log, "InternalServerTryActivateAbilityIndex: 0x%x", InternalServerTryActivateAbilityIndex);
	}
	
	Runtime::VFTHook(SDK::StaticClassImpl("FortAbilitySystemComponentAthena")->GetClassDefaultObject()->GetVTable(), InternalServerTryActivateAbilityIndex, UAbilitySystemComponent::InternalServerTryActivateAbility);

	Runtime::Hook(Finder->TickFlush(), UNetDriver::TickFlush, (void**)&TickFlushOriginal);
	Runtime::Hook(Finder->GetMaxTickRate(), UNetDriver::GetMaxTickRate);
	if (Finder->DispatchRequest())
	{
		Runtime::Hook(Finder->DispatchRequest(), HTTP::DispatchRequest, (void**)&HTTP::DispatchRequestOriginal);
	}

	if (Finder->SpawnBot())
	{
		Runtime::Hook(Finder->SpawnBot(), UFortServerBotManagerAthena::SpawnBot, (void**)&UFortServerBotManagerAthena::SpawnBotOG);
	}

	if (Finder->CreateAndConfigureNavigationSystem()) {
		Runtime::Hook(Finder->CreateAndConfigureNavigationSystem(), UFortServerBotManagerAthena::CreateAndConfigureNavigationSystem, (void**)&UFortServerBotManagerAthena::CreateAndConfigureNavigationSystemOG);
		UE_LOG(LogNeon, Log, "CreateAndConfigureNavigationSystem: 0x%x", Finder->CreateAndConfigureNavigationSystem() - IMAGEBASE);
	}

	Runtime::Hook(IMAGEBASE + 0x1EE9720, AFortPlayerControllerAthena::K2_RemoveItemFromPlayer, (void**)&AFortPlayerControllerAthena::K2_RemoveItemFromPlayerOG);
	Runtime::Hook(IMAGEBASE + 0x2ebf890, ProcessEvent, (void**)&ProcessEventOG);
	
	UWorld::GetWorld()->GetOwningGameInstance()->GetLocalPlayers().Remove(0);
	FString WorldName;
	if (Fortnite_Version <= 10.40)
	{
		WorldName = L"open Athena_Terrain";
	}
	else if (Fortnite_Version <= 18.40 && Fortnite_Version >= 10.40)
	{
		WorldName = L"open Apollo_Terrain";
	}
	else if (Fortnite_Version <= 22.40 && Fortnite_Version >= 19.00)
	{
		WorldName = L"open Artemis_Terrain";
	} else if (Fortnite_Version >= 23.00)
	{
		WorldName = L"open Asteria_Terrain";
	}
	ExecuteConsoleCommand(UWorld::GetWorld(), L"log LogAthenaBots VeryVerbose", nullptr);
	ExecuteConsoleCommand(UWorld::GetWorld(), L"log LogNavigationDataBuild VeryVerbose", nullptr);

	ExecuteConsoleCommand(UWorld::GetWorld(), WorldName, nullptr);
	if (Fortnite_Version >= 19.10)
	{
		ExecuteConsoleCommand(UWorld::GetWorld(), L"log LogFortUIDirector", nullptr);
	}
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Main, 0, 0, 0);
		break;
	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}
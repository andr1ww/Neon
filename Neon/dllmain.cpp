#include "pch.h"

#include "Engine/GameplayStatics/Header/GameplayStatics.h"
#include "Engine/HTTP/Header/HTTP.h"
#include "Engine/Kismet/Header/Kismet.h"
#include "Engine/NetDriver/Header/NetDriver.h"
#include "FortniteGame/AbilitySystemComponent/Header/AbilitySystemComponent.h"
#include "FortniteGame/BuildingGameplayActor/Header/BuildingGameplayActor.h"
#include "FortniteGame/BuildingSMActor/Header/BuildingSMActor.h"
#include "FortniteGame/FortAthenaCreativePortal/Header/FortAthenaCreativePortal.h"
#include "FortniteGame/FortAthenaMutator/Header/FortAthenaMutator.h"
#include "FortniteGame/FortGameMode/Header/FortGameMode.h"
#include "FortniteGame/FortGameSessionDedicated/Header/FortGameSessionDedicated.h"
#include "FortniteGame/FortLootPackage/Header/FortLootPackage.h"
#include "FortniteGame/FortMinigameSettingsBuilding/Header/FortMinigameSettingsBuilding.h"
#include "FortniteGame/FortPlayerController/Header/FortPlayerController.h"
#include "FortniteGame/FortProjectileBase/Header/FortProjectileBase.h"
#include "FortniteGame/FortQuestManager/Header/FortQuestManager.h"
#include "FortniteGame/FortSafeZoneIndicator/Header/FortSafeZoneIndicator.h"
#include "FortniteGame/GAB_InterrogatePlayer/Header/GAB_InterrogatePlayer.h"
#include "Neon/Config.h"
#include "Neon/Finder/Header/Finder.h"
#include "Neon/Nexa/Echo/Echo.h"
#include "Neon/Runtime/Runtime.h"
#include "Neon/TickService/FortAthenaAI/Header/FortAthenaAI.h"

// I didnt do shit (@obsessedtech)

static inline std::vector<uint64_t> NullFuncs = {};
static inline std::vector<uint64_t> RetTrueFuncs = {};
static inline std::vector<uint64_t> FuncsTo85 = {};

static void* (*ProcessEventOG)(UObject*, UFunction*, void*);
void* ProcessEvent(UObject* Obj, UFunction* Function, void* Params)
{
	if (Function && Config::bLogProcessEvent) {
		static bool firstCall = true;
		static std::ofstream logFile("ProcessEvent.log", firstCall ? std::ios::trunc : std::ios::app);
		if (firstCall) {
			firstCall = false;
		}
		std::string FunctionName = Function->GetFName().ToString().ToString();
			if (!strstr(FunctionName.c_str(), ("EvaluateGraphExposedInputs")) &&
			!strstr(FunctionName.c_str(), ("Tick")) &&
			!strstr(FunctionName.c_str(), ("OnSubmixEnvelope")) &&
			!strstr(FunctionName.c_str(), ("OnSubmixSpectralAnalysis")) &&
			!strstr(FunctionName.c_str(), ("OnMouse")) &&
			!strstr(FunctionName.c_str(), ("Pulse")) &&
			!strstr(FunctionName.c_str(), ("BlueprintUpdateAnimation")) &&
			!strstr(FunctionName.c_str(), ("BlueprintPostEvaluateAnimation")) &&
			!strstr(FunctionName.c_str(), ("BlueprintModifyCamera")) &&
			!strstr(FunctionName.c_str(), ("BlueprintModifyPostProcess")) &&
			!strstr(FunctionName.c_str(), ("Loop Animation Curve")) &&
			!strstr(FunctionName.c_str(), ("UpdateTime")) &&
			!strstr(FunctionName.c_str(), ("GetMutatorByClass")) &&
			!strstr(FunctionName.c_str(), ("UpdatePreviousPositionAndVelocity")) &&
			!strstr(FunctionName.c_str(), ("IsCachedIsProjectileWeapon")) &&
			!strstr(FunctionName.c_str(), ("LockOn")) &&
			!strstr(FunctionName.c_str(), ("GetAbilityTargetingLevel")) &&
			!strstr(FunctionName.c_str(), ("ReadyToEndMatch")) &&
			!strstr(FunctionName.c_str(), ("ReceiveDrawHUD")) &&
			!strstr(FunctionName.c_str(), ("OnUpdateDirectionalLightForTimeOfDay")) &&
			!strstr(FunctionName.c_str(), ("GetSubtitleVisibility")) &&
			!strstr(FunctionName.c_str(), ("GetValue")) &&
			!strstr(FunctionName.c_str(), ("InputAxisKeyEvent")) &&
			!strstr(FunctionName.c_str(), ("ServerTouchActiveTime")) &&
			!strstr(FunctionName.c_str(), ("SM_IceCube_Blueprint_C")) &&
			!strstr(FunctionName.c_str(), ("OnHovered")) &&
			!strstr(FunctionName.c_str(), ("OnCurrentTextStyleChanged")) &&
			!strstr(FunctionName.c_str(), ("OnButtonHovered")) &&
			!strstr(FunctionName.c_str(), ("ExecuteUbergraph_ThreatPostProcessManagerAndParticleBlueprint")) &&
			!strstr(FunctionName.c_str(), "PinkOatmeal") &&
			!strstr(FunctionName.c_str(), "CheckForDancingAtFish") &&
			!strstr(FunctionName.c_str(), ("UpdateCamera")) &&
			!strstr(FunctionName.c_str(), ("GetMutatorContext")) &&
			!strstr(FunctionName.c_str(), ("CanJumpInternal")) &&
			!strstr(FunctionName.c_str(), ("OnDayPhaseChanged")) &&
			!strstr(FunctionName.c_str(), ("Chime")) &&
			!strstr(FunctionName.c_str(), ("ServerMove")) &&
			!strstr(FunctionName.c_str(), ("OnVisibilitySetEvent")) &&
			!strstr(FunctionName.c_str(), "ReceiveHit") &&
			!strstr(FunctionName.c_str(), "K2_GetComponentToWorld") &&
			!strstr(FunctionName.c_str(), "ClientAckGoodMove") &&
			!strstr(FunctionName.c_str(), "Prop_WildWest_WoodenWindmill_01") &&
			!strstr(FunctionName.c_str(), "ContrailCheck") &&
			!strstr(FunctionName.c_str(), "B_StockBattleBus_C") &&
			!strstr(FunctionName.c_str(), "Subtitles.Subtitles_C.") &&
			!strstr(FunctionName.c_str(), "/PinkOatmeal/PinkOatmeal_") &&
			!strstr(FunctionName.c_str(), "BP_SpectatorPawn_C") &&
			!strstr(FunctionName.c_str(), "FastSharedReplication") &&
			!strstr(FunctionName.c_str(), "OnCollisionHitEffects") &&
			!strstr(FunctionName.c_str(), "BndEvt__SkeletalMesh") &&
			!strstr(FunctionName.c_str(), ".FortAnimInstance.AnimNotify_") &&
			!strstr(FunctionName.c_str(), "OnBounceAnimationUpdate") &&
			!strstr(FunctionName.c_str(), "ShouldShowSoundIndicator") &&
			!strstr(FunctionName.c_str(), "Primitive_Structure_AmbAudioComponent_C") &&
			!strstr(FunctionName.c_str(), "PlayStoppedIdleRotationAudio") &&
			!strstr(FunctionName.c_str(), "UpdateOverheatCosmetics") &&
			!strstr(FunctionName.c_str(), "StormFadeTimeline__UpdateFunc") &&
			!strstr(FunctionName.c_str(), "BindVolumeEvents") &&
			!strstr(FunctionName.c_str(), "UpdateStateEvent") &&
			!strstr(FunctionName.c_str(), "VISUALS__UpdateFunc") &&
			!strstr(FunctionName.c_str(), "Flash__UpdateFunc") &&
			!strstr(FunctionName.c_str(), "SetCollisionEnabled") &&
			!strstr(FunctionName.c_str(), "SetIntensity") &&
			!strstr(FunctionName.c_str(), "Storm__UpdateFunc") &&
			!strstr(FunctionName.c_str(), "CloudsTimeline__UpdateFunc") &&
			!strstr(FunctionName.c_str(), "SetRenderCustomDepth") &&
			!strstr(FunctionName.c_str(), "K2_UpdateCustomMovement") &&
			!strstr(FunctionName.c_str(), "AthenaHitPointBar_C.Update") &&
			!strstr(FunctionName.c_str(), "ExecuteUbergraph_Farm_WeatherVane_01") &&
			!strstr(FunctionName.c_str(), "HandleOnHUDElementVisibilityChanged") &&
			!strstr(FunctionName.c_str(), "ExecuteUbergraph_Fog_Machine") &&
			!strstr(FunctionName.c_str(), "ReceiveBeginPlay") &&
			!strstr(FunctionName.c_str(), "OnMatchStarted") &&
			!strstr(FunctionName.c_str(), "CustomStateChanged") &&
			!strstr(FunctionName.c_str(), "OnBuildingActorInitialized") &&
			!strstr(FunctionName.c_str(), "OnWorldReady") &&
						!strstr(FunctionName.c_str(), "OnMovementModeChange") &&
						!strstr(FunctionName.c_str(), "OnGameplayTaskResourcesClaimed") &&
						!strstr(FunctionName.c_str(), "K2_OnMovementModeChanged") &&
			!strstr(FunctionName.c_str(), "ShouldDie") &&
			!strstr(FunctionName.c_str(), "OnAttachToBuilding") &&
			!strstr(FunctionName.c_str(), "Clown Spinner") &&
			!strstr(FunctionName.c_str(), "K2_GetActorLocation") &&
			!strstr(FunctionName.c_str(), "GetViewTarget") &&
			!strstr(FunctionName.c_str(), "GetAllActorsOfClass") &&
			!strstr(FunctionName.c_str(), "OnUpdateMusic") &&
			!strstr(FunctionName.c_str(), "Check Closest Point") &&
			!strstr(FunctionName.c_str(), "OnSubtitleChanged__DelegateSignature") &&
			!strstr(FunctionName.c_str(), "OnServerBounceCallback") &&
			!strstr(FunctionName.c_str(), "BlueprintGetInteractionTime") &&
			!strstr(FunctionName.c_str(), "OnServerStopCallback") &&
			!strstr(FunctionName.c_str(), "Light Flash Timeline__UpdateFunc") &&
			!strstr(FunctionName.c_str(), "MainFlightPath__UpdateFunc") &&
			!strstr(FunctionName.c_str(), "PlayStartedIdleRotationAudio") &&
			!strstr(FunctionName.c_str(), "BGA_Athena_FlopperSpawn_") &&
			!strstr(FunctionName.c_str(), "CheckShouldDisplayUI") &&
			!strstr(FunctionName.c_str(), "Timeline_0__UpdateFunc") &&
			!strstr(FunctionName.c_str(), "ClientMoveResponsePacked") &&
			!strstr(FunctionName.c_str(), "ExecuteUbergraph_B_Athena_FlopperSpawnWorld_Placement") &&
			!strstr(FunctionName.c_str(), "Countdown__UpdateFunc") &&
			!strstr(FunctionName.c_str(), "OnParachuteTrailUpdated") &&
			!strstr(FunctionName.c_str(), "Moto FadeOut__UpdateFunc") &&
			!strstr(FunctionName.c_str(), "ExecuteUbergraph_Apollo_GasPump_Valet") &&
			!strstr(FunctionName.c_str(), "GetOverrideMeshMaterial") &&
			!strstr(FunctionName.c_str(), "VendWobble__UpdateFunc") &&
			!strstr(FunctionName.c_str(), "WaitForPawn") &&
			!strstr(FunctionName.c_str(), "FragmentMovement__UpdateFunc") &&
			!strstr(FunctionName.c_str(), "TrySetup") &&
			!strstr(FunctionName.c_str(), "Fade Doused Smoke__UpdateFunc") &&
			!strstr(FunctionName.c_str(), "SetPlayerToSkydive") &&
			!strstr(FunctionName.c_str(), "BounceCar__UpdateFunc") &&
			!strstr(FunctionName.c_str(), "BP_CalendarDynamicPOISelect") &&
			!strstr(FunctionName.c_str(), "OnComponentHit_Event_0") &&
			!strstr(FunctionName.c_str(), "HandleSimulatingComponentHit") &&
			!strstr(FunctionName.c_str(), "CBGA_GreenGlop_WithGrav_C") &&
			!strstr(FunctionName.c_str(), "WarmupCountdownEndTimeUpdated") &&
			!strstr(FunctionName.c_str(), "BP_CanInteract") &&
			!strstr(FunctionName.c_str(), "AthenaHitPointBar_C") &&
			!strstr(FunctionName.c_str(), "ServerFireAIDirectorEvent") &&
			!strstr(FunctionName.c_str(), "BlueprintThreadSafeUpdateAnimation") &&
			!strstr(FunctionName.c_str(), "On Amb Zap Spawn") &&
			!strstr(FunctionName.c_str(), "ServerSetPlayerCanDBNORevive") &&
			!strstr(FunctionName.c_str(), "BGA_Petrol_Pickup_C") &&
			!strstr(FunctionName.c_str(), "GetMutatorsForContextActor") &&
			!strstr(FunctionName.c_str(), "GetControlRotation") &&
			!strstr(FunctionName.c_str(), "K2_GetComponentLocation") &&
			!strstr(FunctionName.c_str(), "MoveFromOffset__UpdateFunc") &&
			!strstr(FunctionName.c_str(), "BlueprintGetAllHighlightableComponents") &&
			!strstr(FunctionName.c_str(), "ServerTriggerCombatEvent") &&
			!strstr(FunctionName.c_str(), "SpinCubeTimeline__UpdateFunc") &&
			!strstr(FunctionName.c_str(), "GetTextValue") &&
			!strstr(FunctionName.c_str(), "OnActorBump") &&
			!strstr(FunctionName.c_str(), "GetTimeSeconds") &&
			!strstr(FunctionName.c_str(), "ServerUpdateLevelVisibility") &&
			!strstr(FunctionName.c_str(), "UpdatePlayerPositionsWithinTheMID") &&
			!strstr(FunctionName.c_str(), "ExecuteUbergraph_BGA_Petrol_Pickup"))
			{
				logFile << "ProcessEvent: " << FunctionName << std::endl;
			}
	}

	return ProcessEventOG(Obj, Function, Params);
}

inline bool IsInFrontend()
{
	if (auto World = UWorld::GetWorld())
	{
		if (World->GetFName().ToString().ToString().contains("Frontend"))
		{
			return true;
		}
	}
}

DefHookOg(void, AFortAthenaMutatorOnSafeZoneUpdated, AFortAthenaMutator_Bots* Bots);
void AFortAthenaMutatorOnSafeZoneUpdated(AFortAthenaMutator_Bots* a1)
{
	AFortAthenaMutatorOnSafeZoneUpdatedOG(a1);
	((void(*)(AFortAthenaMutator_Bots*))(IMAGEBASE + 0x1A9FF90))(a1);
}

DefHookOg(void, InitializeMMRInfos, AFortAthenaMutator_Bots* a1);
void InitializeMMRInfos(AFortAthenaMutator_Bots* a1)
{
	UBotELOSpawningInfo* PhoebeMMRInfo = (UBotELOSpawningInfo*)UBotELOSpawningInfo::GetDefaultObj();
	PhoebeMMRInfo->SetCachedGameMode(UWorld::GetWorld()->GetAuthorityGameMode());
	PhoebeMMRInfo->SetNumItemsToSpawn(FScalableFloat(10.0f));
	PhoebeMMRInfo->SetBotSpawningDataInfo(Runtime::StaticLoadObject<UFortAthenaMutator_PlayerBotSpawningPolicyData>("/Game/Athena/AI/Phoebe/BP_PhoebeSpawningItemData.BP_PhoebeSpawningItemData_C"));
	a1->GetCachedMMRSpawningInfo().ELOSpawningInfos.Add(PhoebeMMRInfo);

	*(int32*)(IMAGEBASE + 0x78B2F68) = 6700;
	*(int32*)(IMAGEBASE + 0x78B2F64) = 70;
	*(int32*)(IMAGEBASE + 0x78B2F6C) = 70;

    return InitializeMMRInfosOG(a1);
}

class UModel final : public UObject
{
public:
	uint8                                         Pad_28[0x2A8];                                     // 0x0028(0x02A8)(Fixing Struct Size After Last Property [ Dumper-7 ])
};


// Enum Engine.ECollisionTraceFlag
// NumValues: 0x0005
enum class ECollisionTraceFlag : uint8
{
	CTF_UseDefault                           = 0,
	CTF_UseSimpleAndComplex                  = 1,
	CTF_UseSimpleAsComplex                   = 2,
	CTF_UseComplexAsSimple                   = 3,
	CTF_MAX                                  = 4,
};

class UBodySetup : public UObject
{
public:
	DEFINE_BOOL(UBodySetup, bDoubleSidedGeometry)
	DEFINE_BOOL(UBodySetup, bGenerateMirroredCollision)
	DEFINE_MEMBER(ECollisionTraceFlag, UBodySetup, CollisionTraceFlag)
	DECLARE_STATIC_CLASS(UBodySetup)
	DECLARE_DEFAULT_OBJECT(UBodySetup)
};

class UBrushComponent : public AActor
{
public:
	DEFINE_PTR(UBodySetup, UBrushComponent, BrushBodySetup)
	DEFINE_PTR(UModel, UBrushComponent, Brush)
public:
	DECLARE_STATIC_CLASS(UBrushComponent)
	DECLARE_DEFAULT_OBJECT(UBrushComponent)
};

class ABrush : public AActor
{
public:
	DEFINE_PTR(UModel, UBrushComponent, Brush)
	DEFINE_PTR(UBrushComponent, ABrush, BrushComponent)
public:
	DECLARE_STATIC_CLASS(ABrush)
	DECLARE_DEFAULT_OBJECT(ABrush)
};

class AVolume : public ABrush
{
public:
	DECLARE_STATIC_CLASS(AVolume)
	DECLARE_DEFAULT_OBJECT(AVolume)
};

class AFortPoiVolume : public AVolume
{
public:
	DECLARE_STATIC_CLASS(AFortPoiVolume)
	DECLARE_DEFAULT_OBJECT(AFortPoiVolume)
};

enum class EAttachmentRule : uint8
{
	KeepRelative                             = 0,
	KeepWorld                                = 1,
	SnapToTarget                             = 2,
	EAttachmentRule_MAX                      = 3,
};

DefHookOg(void, PostInitializeComponentsVolume, AFortPoiVolume* This);
void PostInitializeComponentsVolume(AFortPoiVolume* This)
{
	This->SetBrushComponent((UBrushComponent*)UGameplayStatics::SpawnObject(
		UBrushComponent::StaticClass(), This));
	
	static void (*SetCollisonProfileName)(UBrushComponent*, __int64,
										  __int64) =
		decltype(SetCollisonProfileName)(
			This->GetBrushComponent()->GetVTable()[(0x620 / 8)]);
	((void (*)(UObject * Component, UObject * World))(Finder->RegisterComponentWithWorld()))(This->GetBrushComponent(), UWorld::GetWorld());

	static void (*SetGenerateOverlapEvents)(UBrushComponent*, bool) =
			decltype(SetGenerateOverlapEvents)(IMAGEBASE + 0x40ECA30);
	
	SetCollisonProfileName(
		This->GetBrushComponent(),
		*reinterpret_cast<__int64*>(IMAGEBASE + 0x8317CB0), 1);
	SetGenerateOverlapEvents(This->GetBrushComponent(), 0);
	
	UBodySetup* BodySetup = (UBodySetup*)UGameplayStatics::SpawnObject(
	UBodySetup::StaticClass(), This->GetBrushComponent());
	BodySetup->SetCollisionTraceFlag(ECollisionTraceFlag::CTF_UseComplexAsSimple);
	BodySetup->SetbGenerateMirroredCollision(false);
	BodySetup->SetbDoubleSidedGeometry(true);

	This->GetBrushComponent()->SetBrushBodySetup(BodySetup);
	
	return PostInitializeComponentsVolumeOG(This);
}

void InitNullsAndRetTrues() {
	auto ServerCheatAllIndex = Runtime::GetFunctionIdx(Runtime::StaticFindObject<UFunction>("/Script/FortniteGame.FortPlayerController.ServerCheatAll"));

	if (ServerCheatAllIndex)
		NullFuncs.push_back(__int64(AFortPlayerControllerAthena::GetDefaultObj()->GetVTable()[ServerCheatAllIndex]));
	
	if (Fortnite_Version >= 23)
	{
		NullFuncs.push_back(Memcury::Scanner::FindPattern("48 89 5C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 83 EC ? 4C 8B E2 4C 8B F1").Get());
	}
	
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

	auto GamePhaseStepStringAddr = Memcury::Scanner::FindStringRef(L"Gamephase Step: %s", false).Get();

	uint64 BeginningOfGamePhaseStepFn = 0;
	uint8_t* ByteToPatch = 0;

	if (!GamePhaseStepStringAddr)
	{
		BeginningOfGamePhaseStepFn = Memcury::Scanner::FindPattern("48 89 5C 24 ? 57 48 83 EC 20 E8 ? ? ? ? 48 8B D8 48 85 C0 0F 84 ? ? ? ? E8").Get(); // not actually the func but its fine
	}

	if (!BeginningOfGamePhaseStepFn && !ByteToPatch)
	{
		for (int i = 0; i < 3000; i++)
		{
			if (*(uint8_t*)(uint8_t*)(GamePhaseStepStringAddr - i) == 0x40 && *(uint8_t*)(uint8_t*)(GamePhaseStepStringAddr - i + 1) == 0x55)
			{
				BeginningOfGamePhaseStepFn = GamePhaseStepStringAddr - i;
				break;
			}

			if (*(uint8_t*)(uint8_t*)(GamePhaseStepStringAddr - i) == 0x48 && *(uint8_t*)(uint8_t*)(GamePhaseStepStringAddr - i + 1) == 0x89 && *(uint8_t*)(uint8_t*)(GamePhaseStepStringAddr - i + 2) == 0x5C)
			{
				BeginningOfGamePhaseStepFn = GamePhaseStepStringAddr - i;
				break;
			}

			if (*(uint8_t*)(uint8_t*)(GamePhaseStepStringAddr - i) == 0x48 && *(uint8_t*)(uint8_t*)(GamePhaseStepStringAddr - i + 1) == 0x8B && *(uint8_t*)(uint8_t*)(GamePhaseStepStringAddr - i + 2) == 0xC4)
			{
				BeginningOfGamePhaseStepFn = GamePhaseStepStringAddr - i;
				break;
			}
		}
	}

	if (!ByteToPatch)
	{
		for (int i = 0; i < 500; i++)
		{
			if (*(uint8_t*)(uint8_t*)(BeginningOfGamePhaseStepFn + i) == 0x0F && *(uint8_t*)(uint8_t*)(BeginningOfGamePhaseStepFn + i + 1) == 0x84)
			{
				ByteToPatch = (uint8_t*)(uint8_t*)(BeginningOfGamePhaseStepFn + i + 1);
				break;
			}
		}
	}
	
	DWORD dwProtection;
	VirtualProtect((PVOID)ByteToPatch, 1, PAGE_EXECUTE_READWRITE, &dwProtection);

	*ByteToPatch = 0x85; // jz -> jnz

	DWORD dwTemp;
	VirtualProtect((PVOID)ByteToPatch, 1, dwProtection, &dwTemp);
	
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

	if (Config::bGameSessions)
	{
		Runtime::Hook(Finder->GetCommandLet(), FortGameSessionDedicated::Get, (void**)&FortGameSessionDedicated::GetOG);
		Runtime::Hook(Finder->MatchmakingSerivcePerms(), FortGameSessionDedicated::MatchmakingServicePerms);
		Runtime::VFTHook(AFortGameModeAthena::GetDefaultObj()->GetVTable(), 0xD3, FortGameSessionDedicated::GetGameSessionClass);
		FuncsTo85.push_back(Memcury::Scanner::FindPattern("0F 84 ? ? ? ? 48 8B CF E8 ? ? ? ? 48 8B 4E").Get());
		RetTrueFuncs.push_back(Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B D9 4D 8B F1").Get());
	}

	if (Engine_Version == 421 || Engine_Version == 422)
		RetTrueFuncs.push_back(Memcury::Scanner::FindPattern("4C 89 4C 24 20 55 56 57 41 56 48 8D 6C 24 D1").Get());

	auto Addrr = Memcury::Scanner::FindStringRef(L"CanActivateAbility %s failed, blueprint refused", true, 0, Engine_Version >= 500).Get();

	if (Addrr)
	{
		for (int i = 0; i < 2000; i++)
		{
			if (*(uint8_t*)(uint8_t*)(Addrr - i) == 0x48 && *(uint8_t*)(uint8_t*)(Addrr - i + 1) == 0x89 && *(uint8_t*)(uint8_t*)(Addrr - i + 2) == 0x5C)
			{
				RetTrueFuncs.push_back(Addrr - i);
				break;
			}

			if (*(uint8_t*)(uint8_t*)(Addrr - i) == 0x48 && *(uint8_t*)(uint8_t*)(Addrr - i + 1) == 0x8B && *(uint8_t*)(uint8_t*)(Addrr - i + 2) == 0xC4)
			{
				RetTrueFuncs.push_back(Addrr - i);
				break;
			}
		}
	}

	auto BeginPlayPedestalScanner = Memcury::Scanner::FindPattern("40 53 41 56 48 83 EC 48 48 89 6C 24 ? 48 8B D9 48 89 74 24 ? 48 89 7C 24");

	if (auto BeginPlayPedestal = BeginPlayPedestalScanner.Get())
	{
		NullFuncs.push_back(BeginPlayPedestal);
	}

	NullFuncs.push_back(IMAGEBASE + 0x2983710);
	
	for (auto& Func : NullFuncs) {
		if (Func == 0x0) continue;
		UE_LOG(LogNeon, Log, "NullFuncs: 0x%x", Func - IMAGEBASE);
		Runtime::Hook(Func, Return);
	}

	RetTrueFuncs.push_back(Memcury::Scanner::FindPattern("48 8B C4 48 89 58 08 48 89 70 10 48 89 78 18 4C 89 60 20 55 41 56 41 57 48 8B EC 48 83 EC 60 49 8B D9 45 8A").Get()); // No reserve

	for (auto& Func : RetTrueFuncs) {
		if (Func == 0x0) continue;
		UE_LOG(LogNeon, Log, "RetTrueFunc: 0x%x", Func - IMAGEBASE);
		Runtime::Hook(Func, RetTrue);
	}
	
	for (auto& Func : FuncsTo85)
	{
		if (Func == 0x0) continue;
		Runtime::Patch(Func, 0x85);
	}
	
	if (Fortnite_Version < 13.00 && Fortnite_Version >= 12.50)
	{
		Runtime::Patch(IMAGEBASE + 0x1A45182, 0x90);
		Runtime::Patch(IMAGEBASE + 0x1A45183, 0x90);
		Runtime::Patch(IMAGEBASE + 0x1A45184, 0x90);
		Runtime::Patch(IMAGEBASE + 0x1A45185, 0x90);
		Runtime::Patch(IMAGEBASE + 0x1A45186, 0x90);
		Runtime::Patch(IMAGEBASE + 0x1A45187, 0x90);
		
		Runtime::Hook(IMAGEBASE + 0x1A45060, PostInitializeComponentsVolume, (void**)&PostInitializeComponentsVolumeOG);
		Runtime::Hook(IMAGEBASE + 0x1A3A640, RetTrue);
		Runtime::Patch(IMAGEBASE + 0x1A9FFB6, 0xEB);
		Runtime::Hook(IMAGEBASE + 0x1A8ED30, AFortAthenaMutatorOnSafeZoneUpdated, (void**)&AFortAthenaMutatorOnSafeZoneUpdatedOG);
		Runtime::Hook(IMAGEBASE + 0x1A9FF90, InitializeMMRInfos, (void**)&InitializeMMRInfosOG);
		Runtime::Hook(IMAGEBASE + 0x6BB920, RetTrue);
		Runtime::Hook(IMAGEBASE + 0x1EE9720, AFortPlayerControllerAthena::K2_RemoveItemFromPlayer, (void**)&AFortPlayerControllerAthena::K2_RemoveItemFromPlayerOG);
		Runtime::Hook(IMAGEBASE + 0x2E688D0, RetTrue); // server context
		Runtime::Hook(IMAGEBASE + 0x3F88350, RetTrue); // IsThereAnywhereToBuildNavigation
	}

	if (Config::bLogProcessEvent)
	{
		Runtime::Hook(SDK::Offsets::UObject__ProcessEvent, ProcessEvent, (void**)&ProcessEventOG);
	}

	if (Fortnite_Version == 10.40)
	{
		Runtime::Hook(IMAGEBASE + 0x176D8D0, UFortQuestManager::SendStatEventWithTags, (void**)&UFortQuestManager::SendStatEventWithTagsOG);
	}

	if (Fortnite_Version != 22.4)
	{
		auto matchmaking = Memcury::Scanner::FindPattern("83 BD ? ? ? ? 01 7F 18 49 8D 4D D8 48 8B D6 E8 ? ? ? ? 48", false).Get(); // 1.11

		if (!matchmaking)
			matchmaking = Memcury::Scanner::FindPattern("83 7D 88 01 7F 0D 48 8B CE E8", false).Get();
		if (!matchmaking)
			matchmaking = Memcury::Scanner::FindPattern("83 BD ? ? ? ? ? 7F 18 49 8D 4D D8 48 8B D7 E8").Get(); // 4.20
		if (!matchmaking)
			matchmaking = Memcury::Scanner::FindPattern("83 7C 24 ?? 01 7F 0D 48 8B CF E8").Get();
	    
		bool bMatchmakingSupported = matchmaking;
		int idx = 0;

		if (bMatchmakingSupported) 
		{
			for (int i = 0; i < 9; i++)
			{
				auto byte = (uint8_t*)(matchmaking + i);

				if (IsBadReadPtr(byte, sizeof(uint8_t)))
					continue;
				
				if (*byte == 0x7F) 
				{
					bMatchmakingSupported = true;
					idx = i;
					break;
				}

				bMatchmakingSupported = false;
			}
		}

		if (bMatchmakingSupported)
		{
			auto before = (uint8_t*)(matchmaking + idx);
			
			DWORD dwProtection;
			VirtualProtect((PVOID)before, 1, PAGE_EXECUTE_READWRITE, &dwProtection);

			*before = 0x74; 

			DWORD dwTemp;
			VirtualProtect((PVOID)before, 1, dwProtection, &dwTemp);
		}
	}

	auto GameSessionDedicatedAthenaPatch = Memcury::Scanner::FindPattern("3B 41 38 7F ? 48 8B D0 48 8B 41 30 4C 39 04 D0 75 ? 48 8D 96", false).Get(); // todo check this sig more

	if (GameSessionDedicatedAthenaPatch)
	{
	//	Runtime::PatchBytes(GameSessionDedicatedAthenaPatch, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
	}
	else
	{
		auto S19Patch = Memcury::Scanner::FindPattern("74 1A 48 8D 97 ? ? ? ? 49 8B CF E8 ? ? ? ? 88 87 ? ? ? ? E9", false).Get();
					
		if (S19Patch)
		{
			Runtime::Patch(S19Patch, 0x75);
		}
		else
		{
			auto S18Patch = Memcury::Scanner::FindPattern("75 02 33 F6 41 BE ? ? ? ? 48 85 F6 74 17 48 8D 93").Get();

			if (S18Patch)
			{
				Runtime::Patch(S18Patch, 0x74);
			}
		}
	}
}

DefHookOg(FGameplayTag*, Ok, void*, void*, FName f);
FGameplayTag* Ok(void* a1, void* a2, FName f)
{
	static FGameplayTag* Tag = nullptr;
	static int32 Size = 0;
	if (!Tag)
	{
		Size = StaticClassImpl("GameplayTag")->GetSize();
		Tag = (FGameplayTag*)VirtualAlloc(0, Size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (Tag) new(Tag) FGameplayTag();
	}
	if (!Tag) return OkOG(a1, a2, f);
	
	if (f.ToString().ToString() == "PlayerTech:ptid_spyteam_01")
	{
		Tag->TagName = UKismetStringLibrary::Conv_StringToName(FString(L"Athena.Faction.Alter"));
		return Tag;
	} else if (f.ToString().ToString() == "PlayerTech:ptid_spyteam_02")
	{
		Tag->TagName = UKismetStringLibrary::Conv_StringToName(FString(L"Athena.Faction.Ego"));
		return Tag;
	} 

	return OkOG(a1, a2, f);
}

void Main()
{
	std::srand(static_cast<unsigned int>(std::time(nullptr)));
	
	AllocConsole();
	FILE* File = nullptr;
	freopen_s(&File, "CONOUT$", "w+", stdout);
	freopen_s(&File, "CONOUT$", "w+", stderr);
	SetConsoleTitleA("Neon | Setting up");
	SDK::Init();
	Offsets::FMemory__Realloc = Memcury::Scanner::FindPattern("48 89 5C 24 08 48 89 74 24 10 57 48 83 EC ? 48 8B F1 41 8B D8 48 8B 0D ? ? ? ?").Get();
	
	MH_Initialize();

	if (Config::bEchoSessions)
	{
		Config::Port = UKismetMathLibrary::RandomIntegerInRange(7777, 8888);

		std::thread t([]() {
			 Nexa::Echo::CreateEchoSession(); 
		 });
		t.detach();
		CreateThread(0, 0, Nexa::Echo::Heartbeat, 0, 0, 0);
	}

	Finder->Init();
	
	Sleep(5000);
	while (!IsInFrontend())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
	
	if (Finder->GIsClient())
	{
		*(bool*)(Finder->GIsClient()) = false; 
		*(bool*)(Finder->GIsClient() + 1) = true;
	}
	
	InitNullsAndRetTrues();
	
	if (Fortnite_Version < 13.00 && Fortnite_Version >= 12.20)
	{
		Runtime::Hook(Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 4C 89 44 24 ? 57 48 83 EC ? 48 8B FA 48 8B F1 33 D2").Get(), Ok, (void**)&OkOG);
	}
	
	auto ListenInstruction = Memcury::Scanner::FindPattern("E8 ? ? ? ? 84 C0 75 ? 80 3D ? ? ? ? ? 72 ? 45 33 C0 48 8D 55").Get();
	if (!ListenInstruction)
	{
		ListenInstruction = Memcury::Scanner::FindPattern("E8 ? ? ? ? 84 C0 75 ? 80 3D ? ? ? ? ? 72 ? 45 33 C0 48 8D 95").Get();
	}
	
	Runtime::ModifyInstruction(ListenInstruction, Finder->InstructionForCollision());
	Runtime::Hook(Finder->InstructionForCollision(), FortGameSessionDedicated::UWorld_Listen);
	
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

	if (Config::bCreative && Fortnite_Version >= 11.00)
	{
		WorldName = L"open Creative_NoApollo_Terrain";
	}

	ExecuteConsoleCommand(UWorld::GetWorld(), L"log LogFortQuest VeryVerbose", nullptr);
	ExecuteConsoleCommand(UWorld::GetWorld(), L"log LogFortAI VeryVerbose", nullptr);
	ExecuteConsoleCommand(UWorld::GetWorld(), L"log LogAISpawnerData VeryVerbose", nullptr);
	ExecuteConsoleCommand(UWorld::GetWorld(), L"log LogMutatorAI VeryVerbose", nullptr);
	ExecuteConsoleCommand(UWorld::GetWorld(), L"log LogAthenaBots VeryVerbose", nullptr);
//	ExecuteConsoleCommand(UWorld::GetWorld(), L"log LogNavigation VeryVerbose", nullptr);
	ExecuteConsoleCommand(UWorld::GetWorld(), L"log LogPackageLocalizationCache", nullptr);

	if (Fortnite_Version >= 19.10)
	{
		ExecuteConsoleCommand(UWorld::GetWorld(), L"log LogFortUIDirector", nullptr);
	}
	
	if (Finder->CreateAndConfigureNavigationSystem() && Fortnite_Version <= 13.00 && Fortnite_Version >= 11.00) {
		Runtime::Hook(Finder->CreateAndConfigureNavigationSystem(), UFortServerBotManagerAthena::CreateAndConfigureNavigationSystem, (void**)&UFortServerBotManagerAthena::CreateAndConfigureNavigationSystemOG);
		UE_LOG(LogNeon, Log, "CreateAndConfigureNavigationSystem: 0x%x", Finder->CreateAndConfigureNavigationSystem() - IMAGEBASE);
	}
	
	Runtime::Exec("/Script/Engine.GameMode.ReadyToStartMatch", AFortGameModeAthena::ReadyToStartMatch, (void**)&AFortGameModeAthena::ReadyToStartMatchOG);
	Runtime::Hook<&AFortGameModeAthena::StaticClass>("SpawnDefaultPawnFor", AFortGameModeAthena::SpawnDefaultPawnFor);
	int InternalServerTryActivateAbilityIndex = 0;

	if (Engine_Version > 4.20)
	{
		static auto OnRep_ReplicatedAnimMontageFn = Runtime::StaticFindObject<UFunction>("/Script/GameplayAbilities.AbilitySystemComponent.OnRep_ReplicatedAnimMontage");
		if (OnRep_ReplicatedAnimMontageFn)
		{
			InternalServerTryActivateAbilityIndex = Runtime::GetVTableIndex(OnRep_ReplicatedAnimMontageFn) - 1;
			UE_LOG(LogNeon, Log, "InternalServerTryActivateAbilityIndex: 0x%x", InternalServerTryActivateAbilityIndex);
		}
	}

	Runtime::VFTHook(SDK::StaticClassImpl("FortAbilitySystemComponentAthena")->GetClassDefaultObject()->GetVTable(), InternalServerTryActivateAbilityIndex, UAbilitySystemComponent::InternalServerTryActivateAbility);
	Runtime::Hook(Finder->GetPlayerViewPoint(), AFortPlayerControllerAthena::GetPlayerViewPoint, (void**)&AFortPlayerControllerAthena::GetPlayerViewPointOG);
	Runtime::Hook(Finder->TickFlush(), UNetDriver::TickFlush, (void**)&TickFlushOriginal);
	Runtime::Hook(Finder->GetMaxTickRate(), UNetDriver::GetMaxTickRate);
	if (Finder->DispatchRequest()) Runtime::Hook(Finder->DispatchRequest(), HTTP::DispatchRequest, (void**)&HTTP::DispatchRequestOriginal);

	Runtime::Exec("/Game/Athena/Items/Consumables/Parents/GA_Athena_MedConsumable_Parent.GA_Athena_MedConsumable_Parent_C.Triggered_4C02BFB04B18D9E79F84848FFE6D2C32", UGA_Athena_MedConsumable_Parent_C::Athena_MedConsumable_Triggered, (void**)&UGA_Athena_MedConsumable_Parent_C::Athena_MedConsumable_TriggeredOG);
	Runtime::Exec("/Game/Abilities/Player/Interrogation/GAB_InterrogatePlayer_Reveal.GAB_InterrogatePlayer_Reveal_C.EndInterrogation", UGAB_InterrogatePlayer_Reveal_C::EndInterrogation);
	if (Fortnite_Version >= 12.00) Runtime::Exec("/Script/FortniteGame.FortPlayerPawn.OnCapsuleBeginOverlap", AFortPlayerPawn::OnCapsuleBeginOverlap);
	Runtime::Exec("/Script/Engine.PlayerController.ServerAcknowledgePossession", AFortPlayerControllerAthena::ServerAcknowledgePossession);
	Runtime::Exec("/Script/FortniteGame.FortPlayerController.ServerExecuteInventoryItem", AFortPlayerControllerAthena::ServerExecuteInventoryItem);
	Runtime::Exec("/Script/FortniteGame.FortPlayerController.ServerCheat", AFortPlayerControllerAthena::ServerCheat);
	Runtime::Exec("/Script/FortniteGame.FortPlayerController.ServerPlayEmoteItem", AFortPlayerControllerAthena::ServerPlayEmoteItem);
	Runtime::Exec("/Script/FortniteGame.FortControllerComponent_Aircraft.ServerAttemptAircraftJump", AFortPlayerControllerAthena::ServerAttemptAircraftJump);
	Runtime::Hook(Finder->OnDamageServer(), ABuildingSMActor::OnDamageServer, (void**)&ABuildingSMActor::OnDamageServerOG);
	Runtime::Exec("/Script/FortniteGame.FortPlayerController.ServerReadyToStartMatch", AFortPlayerControllerAthena::ServerReadyToStartMatch, (void**)&AFortPlayerControllerAthena::ServerReadyToStartMatchOG);
	Runtime::Exec("/Script/FortniteGame.FortAthenaCreativePortal.TeleportPlayerToLinkedVolume", AFortAthenaCreativePortal::TeleportPlayerToLinkedVolume);
	Runtime::Exec("/Script/FortniteGame.FortPlayerController.ServerCreateBuildingActor", AFortPlayerControllerAthena::ServerCreateBuildingActor, (void**)&AFortPlayerControllerAthena::ServerCreateBuildingActorOG);
	Runtime::Exec("/Script/FortniteGame.FortPlayerController.ServerBeginEditingBuildingActor", AFortPlayerControllerAthena::ServerBeginEditingBuildingActor);
	Runtime::Exec("/Script/FortniteGame.FortPlayerController.ServerEditBuildingActor", AFortPlayerControllerAthena::ServerEditBuildingActor);
	Runtime::Exec("/Script/FortniteGame.FortPlayerController.ServerEndEditingBuildingActor", AFortPlayerControllerAthena::ServerEndEditingBuildingActor);
	Runtime::Exec("/Script/FortniteGame.FortPlayerController.ServerRepairBuildingActor", AFortPlayerControllerAthena::ServerRepairBuildingActor);
	Runtime::Exec("/Script/FortniteGame.FortPlayerPawn.ServerHandlePickupInfo", AFortPlayerPawn::ServerHandlePickupInfo);
	Runtime::Exec("/Script/FortniteGame.FortPlayerPawn.ServerSendZiplineState", AFortPlayerPawn::ServerSendZiplineState);
	Runtime::Exec("/Script/FortniteGame.FortPlayerController.ServerLoadingScreenDropped", AFortPlayerControllerAthena::ServerLoadingScreenDropped, (void**)&AFortPlayerControllerAthena::ServerLoadingScreenDroppedOG);
	Runtime::Exec("/Script/FortniteGame.FortPlayerController.ServerAttemptInventoryDrop", AFortPlayerControllerAthena::ServerAttemptInventoryDrop);
	Runtime::Exec("/Script/FortniteGame.FortKismetLibrary.GiveItemToInventoryOwner", AFortPlayerPawn::GiveItemToInventoryOwner, (void**)&AFortPlayerPawn::GiveItemToInventoryOwnerOG);
	Runtime::Exec("/Script/FortniteGame.FortKismetLibrary.K2_RemoveItemFromPlayerByGuid", AFortPlayerControllerAthena::K2_RemoveItemFromPlayerByGuid, (void**)&AFortPlayerControllerAthena::K2_RemoveItemFromPlayerByGuidOG);
	Runtime::Hook(Finder->SendComplexCustomStatEvent(), UFortQuestManager::SendComplexCustomStatEvent, (void**)&UFortQuestManager::SendComplexCustomStatEventOG);
	Runtime::Hook(Finder->ClientOnPawnDied(), AFortPlayerControllerAthena::ClientOnPawnDied, (void**)&AFortPlayerControllerAthena::ClientOnPawnDiedOG);
	Runtime::Exec("/Script/FortniteGame.FortPhysicsPawn.ServerMove", AFortPhysicsPawn::ServerMove);
	Runtime::Hook(Finder->PostUpdate(), FortLootPackage::PostUpdate, (void**)&FortLootPackage::PostUpdateOG);
	Runtime::Hook(Finder->AFortMinigameSettingsBuilding_BeginPlay(), AFortMinigameSettingsBuilding::BeginPlay);
	if (Fortnite_Version < 13.00 && Fortnite_Version >= 12.20)
	{
		Runtime::VFTHook(UAthenaNavSystem::GetDefaultObj()->GetVTable(), 0x53, UFortServerBotManagerAthena::InitializeForWorld, (void**)&UFortServerBotManagerAthena::InitializeForWorldOG);
	}
	if (Fortnite_Version < 13.00 && Fortnite_Version >= 12.20)
	{
		Runtime::VFTHook(StaticClassImpl("FortPlayerPawnAthena")->GetClassDefaultObject()->GetVTable(), 0x119, AFortPlayerPawn::NetMulticast_Athena_BatchedDamageCues, (void**)&AFortPlayerPawn::NetMulticast_Athena_BatchedDamageCuesOG);
	}
	Runtime::Exec("/Script/FortniteGame.FortPlayerControllerAthena.ServerGiveCreativeItem", AFortPlayerControllerAthena::ServerGiveCreativeItem);
	Runtime::Exec("/Script/FortniteGame.FortProjectileBase.OnStopCallback", AFortProjectileBase::OnStopCallback, (void**)&AFortProjectileBase::OnStopCallbackOG);
	
	Runtime::Hook(Finder->ReloadWeapon(), AFortPlayerPawn::ReloadWeapon, (void**)&AFortPlayerPawn::ReloadWeaponOG); // this is right um we can make it uni after we get it to fucking call 
	Runtime::Hook(Finder->StartAircraftPhase(), AFortGameModeAthena::StartAircraftPhase, (void**)&AFortGameModeAthena::StartAircraftPhaseOG);
	Runtime::Hook(Finder->StartNewSafeZonePhase(), AFortGameModeAthena::StartNewSafeZonePhase, (void**)&AFortGameModeAthena::StartNewSafeZonePhaseOG);
	Runtime::Hook<&AFortGameModeAthena::StaticClass>("HandleStartingNewPlayer", AFortGameModeAthena::HandleStartingNewPlayer, AFortGameModeAthena::HandleStartingNewPlayerOG);
	Runtime::Hook<&AFortPlayerPawn::StaticClass>("ServerReviveFromDBNO", AFortPlayerPawn::ServerReviveFromDBNO);
//	Runtime::Hook<&AFortPlayerControllerAthena::StaticClass>("ServerDBNOReviveInterrupted", AFortPlayerControllerAthena::ServerDBNOReviveInterrupted, AFortPlayerControllerAthena::ServerDBNOReviveInterruptedOG);
	if (Config::bEchoSessions) Runtime::Hook(Finder->PickTeam(), AFortGameModeAthena::PickTeam, (void**)&AFortGameModeAthena::PickTeamOG);
	Runtime::Hook(Finder->GetSquadIdForCurrentPlayer(), FortGameSessionDedicated::GetSquadIdForCurrentPlayer);
	Runtime::Exec("/Script/FortniteGame.FortAthenaMutator_GiveItemsAtGamePhaseStep.OnGamePhaseStepChanged", AFortAthenaMutator_GiveItemsAtGamePhaseStep::OnGamePhaseStepChanged);
	Runtime::Exec("/Script/FortniteGame.FortAthenaMutator_Barrier.OnGamePhaseStepChanged", AFortAthenaMutator_Barrier::OnGamePhaseStepChanged);
	uint64 Addr = __int64(Runtime::StaticFindObject<UFunction>("/Script/Engine.PlayerController.SetVirtualJoystickVisibility")->GetNativeFunc());
	Runtime::ModifyInstructionLEA(Finder->RebootingDelegate(), Addr, 3); 
	Runtime::Hook(Addr, ABuildingGameplayActorSpawnMachine::RebootingDelegate);
	Runtime::Exec("/Script/FortniteGame.FortControllerComponent_Interaction.ServerAttemptInteract", FortLootPackage::ServerAttemptInteract, (void**)&FortLootPackage::ServerAttemptInteractOG);
	
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
	if (Finder->DispatchRequest()) Runtime::Hook(Finder->DispatchRequest(), HTTP::DispatchRequest, (void**)&HTTP::DispatchRequestOriginal);
//	Runtime::Exec("/Script/FortniteGame.BuildingFoundation.SetDynamicFoundationTransform", ABuildingFoundation::SetDynamicFoundationTransformF, (void**)&ABuildingFoundation::SetDynamicFoundationTransformFOG);
//	Runtime::Exec("/Script/FortniteGame.BuildingFoundation.SetDynamicFoundationEnabled", ABuildingFoundation::SetDynamicFoundationTransformF, (void**)&ABuildingFoundation::SetDynamicFoundationEnabledFOG);

	if (Fortnite_Version >= 11.00)
	{
		if (Finder->SpawnBot()) Runtime::Hook(Finder->SpawnBot(), UFortServerBotManagerAthena::SpawnBot, (void**)&UFortServerBotManagerAthena::SpawnBotOG);
	}
	
	Runtime::Hook(Finder->SendStatEventWithTags(), UFortQuestManager::SendStatEventWithTags, (void**)&UFortQuestManager::SendStatEventWithTagsOG);
	
	ExecuteConsoleCommand(UWorld::GetWorld(), WorldName, nullptr);
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
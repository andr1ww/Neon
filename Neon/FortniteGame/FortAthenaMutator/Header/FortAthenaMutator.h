#pragma once
#include "pch.h"

#include "FortniteGame/FortServerBotManager/Header/FortServerBotManager.h"

class IFortSafeZoneInterface final : public IInterface
{
};

struct FItemsToGive final
{
public:
    class UFortWorldItemDefinition*               ItemToDrop;                                        // 0x0000(0x0008)(Edit, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    struct FScalableFloat                         NumberToGive;                                      // 0x0008(0x0020)(Edit, NativeAccessSpecifierPublic)
};

class AFortAthenaMutator_GiveItemsAtGamePhaseStep : public AFortAthenaMutator
{
public:
    DEFINE_MEMBER(EAthenaGamePhaseStep, AFortAthenaMutator_GiveItemsAtGamePhaseStep, PhaseToGiveItems);
    DEFINE_MEMBER(TArray<struct FItemsToGive> , AFortAthenaMutator_GiveItemsAtGamePhaseStep, ItemsToGive);
    static void OnGamePhaseStepChanged(AFortAthenaMutator_GiveItemsAtGamePhaseStep* Mutator, FFrame& Stack);
public:
    DECLARE_STATIC_CLASS(AFortAthenaMutator_GiveItemsAtGamePhaseStep)
    DECLARE_DEFAULT_OBJECT(AFortAthenaMutator_GiveItemsAtGamePhaseStep)
};

struct FPerkMutatorData final
{
public:
	struct FGameplayTag                           PerkTag;                                           // 0x0000(0x0008)(Edit, NoDestructor, Protected, HasGetValueTypeHash, NativeAccessSpecifierProtected)
	struct FGameplayTag                           ShowPerkSelectTag;                                 // 0x0008(0x0008)(Edit, NoDestructor, Protected, HasGetValueTypeHash, NativeAccessSpecifierProtected)
	struct FGameplayTag                           PassivePerkTag;                                    // 0x0010(0x0008)(Edit, NoDestructor, Protected, HasGetValueTypeHash, NativeAccessSpecifierProtected)
	struct FGameplayTag                           ItemPerkTag;                                       // 0x0018(0x0008)(Edit, NoDestructor, Protected, HasGetValueTypeHash, NativeAccessSpecifierProtected)
	struct FGameplayTag                           FirstRollTag;                                      // 0x0020(0x0008)(Edit, NoDestructor, Protected, HasGetValueTypeHash, NativeAccessSpecifierProtected)
	struct FGameplayTag                           BlockRespawnTag;                                   // 0x0028(0x0008)(Edit, NoDestructor, Protected, HasGetValueTypeHash, NativeAccessSpecifierProtected)
	TSubclassOf<class UGameplayEffect>            PerkUnlockedGameplayEffectClass;                   // 0x0030(0x0008)(Edit, ZeroConstructor, IsPlainOldData, NoDestructor, Protected, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierProtected)
	TSubclassOf<class UGameplayEffect>            ShowPerkSelectGameplayEffectClass;                 // 0x0038(0x0008)(Edit, ZeroConstructor, IsPlainOldData, NoDestructor, Protected, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierProtected)
	TSubclassOf<class UGameplayEffect>            BlockRespawnGameplayEffectClass;                   // 0x0040(0x0008)(Edit, ZeroConstructor, IsPlainOldData, NoDestructor, Protected, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierProtected)
	TSubclassOf<class UUserWidget>                PerkScreenIntroWidgetClass;                        // 0x0048(0x0008)(Edit, ZeroConstructor, IsPlainOldData, NoDestructor, Protected, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierProtected)
	struct FScalableFloat                         StartingRerollCount;                               // 0x0050(0x0020)(Edit, Protected, NativeAccessSpecifierProtected)
	struct FScalableFloat                         RerollsToGivePerPerkUnlock;                        // 0x0070(0x0020)(Edit, Protected, NativeAccessSpecifierProtected)
	struct FScalableFloat                         MaxRerollsPerPlayer;                               // 0x0090(0x0020)(Edit, Protected, NativeAccessSpecifierProtected)
	TMap<class FName, struct FGameplayTag>        FactionItemMapping;                                // 0x00B0(0x0050)(Edit, ZeroConstructor, Protected, NativeAccessSpecifierProtected)
	bool                                          bShouldShowBackgroundImage;                        // 0x0100(0x0001)(Edit, ZeroConstructor, IsPlainOldData, NoDestructor, Protected, HasGetValueTypeHash, NativeAccessSpecifierProtected)
	uint8                                         Pad_101[0x7];                                      // 0x0101(0x0007)(Fixing Struct Size After Last Property [ Dumper-7 ])
};

class AFortAthenaMutator_PerkSystemMutator : public AFortAthenaMutator
{
public:
	DEFINE_MEMBER(FPerkMutatorData, AFortAthenaMutator_PerkSystemMutator, MutatorData);
public:
    DECLARE_STATIC_CLASS(AFortAthenaMutator_PerkSystemMutator)
    DECLARE_DEFAULT_OBJECT(AFortAthenaMutator_PerkSystemMutator)
};

struct FFutureTechData final
{
public:
	int32                                         UnlockLevel;                                       // 0x0000(0x0004)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	int32                                         XpToGetThisLevelFromRoundStartLevel;               // 0x0004(0x0004)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	int32                                         SingleLevelRequiredXp;                             // 0x0008(0x0004)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	uint8                                         Pad_C[0x4];                                        // 0x000C(0x0004)(Fixing Size After Last Property [ Dumper-7 ])
	class UFortSpyTechItemDefinition*             PerkItemDef;                                       // 0x0010(0x0008)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
};

struct FRoundTechDataCache final
{
public:
	TArray<struct FFutureTechData>                FuturePerks;                                       // 0x0000(0x0010)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, NativeAccessSpecifierPublic)
	int32                                         LevelAtRoundStart;                                 // 0x0010(0x0004)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	int32                                         FactionXPAtRoundStart;                             // 0x0014(0x0004)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	bool                                          bDataReady;                                        // 0x0018(0x0001)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	uint8                                         Pad_19[0x3];                                       // 0x0019(0x0003)(Fixing Size After Last Property [ Dumper-7 ])
	int32                                         MaxCalandarLevel;                                  // 0x001C(0x0004)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	int32                                         MaxLevel;                                          // 0x0020(0x0004)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	uint8                                         Pad_24[0x4];                                       // 0x0024(0x0004)(Fixing Struct Size After Last Property [ Dumper-7 ])
};

class UFortSpyTechItemDefinition : UFortItemDefinition
{
public:
	DECLARE_STATIC_CLASS(UFortSpyTechItemDefinition)
	DECLARE_DEFAULT_OBJECT(UFortSpyTechItemDefinition)
};

// 0x0018 (0x0018 - 0x0000)
struct FPerkItemSet final
{
public:
	TArray<class UFortSpyTechItemDefinition*>     Items;                                             // 0x0000(0x0010)(ZeroConstructor, NativeAccessSpecifierPublic)
	float                                         Time;                                              // 0x0010(0x0004)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	uint8                                         Pad_14[0x4];                                       // 0x0014(0x0004)(Fixing Struct Size After Last Property [ Dumper-7 ])
};

struct FPlayerPerkLevel final
{
public:
	TArray<class UFortSpyTechItemDefinition*>     PlayerPerks;                                       // 0x0000(0x0010)(Edit, ZeroConstructor, NativeAccessSpecifierPublic)
};

class UFortPlayerPerksItemDefinition : UFortItemDefinition
{
public:
	DEFINE_MEMBER(TArray<struct FPlayerPerkLevel>, UFortPlayerPerksItemDefinition, PlayerPerkLevels)
public:
	DECLARE_STATIC_CLASS(UFortPlayerPerksItemDefinition)
	DECLARE_DEFAULT_OBJECT(UFortPlayerPerksItemDefinition)
};

class UFortControllerComponent_PerkSystem : public UObject
{
public:
	DEFINE_MEMBER(FGameplayTag, UFortControllerComponent_PerkSystem, CachedFactionTag)
	DEFINE_MEMBER(TArray<struct FPerkItemSet>, UFortControllerComponent_PerkSystem, PerkSelection);
	DEFINE_MEMBER(TArray<class UFortSpyTechItemDefinition*>, UFortControllerComponent_PerkSystem, DefaultPerks);
	DEFINE_MEMBER(TArray<class UFortSpyTechItemDefinition*>, UFortControllerComponent_PerkSystem, SpyTechArray);
	DEFINE_MEMBER(FRoundTechDataCache, UFortControllerComponent_PerkSystem, RoundStartCache);
	DEFINE_MEMBER(int32, UFortControllerComponent_PerkSystem, RerollCount);
	DEFINE_MEMBER(FPerkMutatorData, AFortAthenaMutator_PerkSystemMutator, MutatorData);
public:
	DECLARE_STATIC_CLASS(UFortControllerComponent_PerkSystem)
	DECLARE_DEFAULT_OBJECT(UFortControllerComponent_PerkSystem)
};
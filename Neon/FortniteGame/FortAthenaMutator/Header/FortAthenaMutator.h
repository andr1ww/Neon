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

class AFortAthenaMutator_GiveItemsAtGamePhaseStep : public AFortAthenaMutator_Bots
{
public:
    DEFINE_MEMBER(EAthenaGamePhaseStep, AFortAthenaMutator_GiveItemsAtGamePhaseStep, PhaseToGiveItems);
    DEFINE_MEMBER(TArray<struct FItemsToGive> , AFortAthenaMutator_GiveItemsAtGamePhaseStep, ItemsToGive);
    static void OnGamePhaseStepChanged(AFortAthenaMutator_GiveItemsAtGamePhaseStep* Mutator, FFrame& Stack);
public:
    DECLARE_STATIC_CLASS(AFortAthenaMutator_GiveItemsAtGamePhaseStep)
    DECLARE_DEFAULT_OBJECT(AFortAthenaMutator_GiveItemsAtGamePhaseStep)
};
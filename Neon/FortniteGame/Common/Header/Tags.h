#pragma once
#include "pch.h"

#include "ItemDefinition.h"

struct FGameplayTagContainer final
{
public:
    DEFINE_MEMBER(TArray<FGameplayTag>, FGameplayTagContainer, GameplayTags);
    DEFINE_MEMBER(TArray<FGameplayTag>, FGameplayTagContainer, ParentTags);
};

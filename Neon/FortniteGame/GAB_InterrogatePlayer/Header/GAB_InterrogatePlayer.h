#pragma once
#include "pch.h"

#include "FortniteGame/AbilitySystemComponent/Header/AbilitySystemComponent.h"

class UGAB_InterrogatePlayer_Reveal_C : public UGameplayAbility
{
public:
    DefHookOg(void, EndInterrogation, UGAB_InterrogatePlayer_Reveal_C* _this, FFrame& Stack);
public:
    DEFINE_MEMBER(TArray<AActor*>, UGAB_InterrogatePlayer_Reveal_C, ActorsToMark);
    
    DECLARE_STATIC_CLASS(UGAB_InterrogatePlayer_Reveal_C)
    DECLARE_DEFAULT_OBJECT(UGAB_InterrogatePlayer_Reveal_C)
};
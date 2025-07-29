#pragma once
#include "pch.h"

class ACharacter : public APawn
{
};

class AFGF_Character : public ACharacter
{
    
};

class AFortPawn : public AFGF_Character
{
public:
    DEFINE_BOOL(AFortPawn, bMovingEmote);
    DEFINE_BOOL(AFortPawn, bMovingEmoteForwardOnly);
    DEFINE_BOOL(AFortPawn, bMovingEmoteFollowingOnly);
    DEFINE_MEMBER(float, AFortPawn, EmoteWalkSpeed);
};

class AFortPlayerPawn : public AFortPawn
{

};

class ADefaultPawn : public APawn
{
    
};
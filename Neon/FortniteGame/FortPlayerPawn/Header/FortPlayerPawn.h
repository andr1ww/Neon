#pragma once
#include "pch.h"
#include "FortniteGame/FortPlayerController/Header/FortPlayerController.h"

struct FFortAthenaLoadout;

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
public:
    DEFINE_MEMBER(FFortAthenaLoadout*, AFortPlayerPawn, CosmeticLoadout);
};

class ADefaultPawn : public APawn
{
    
};
#pragma once
#include "pch.h"

#include "Engine/PlayerController/Header/PlayerController.h"

class AFortPlayerControllerAthena : public APlayerController
{
public:
    static void ServerAcknowledgePossession(AFortPlayerControllerAthena* PlayerController, APawn* Pawn);
};

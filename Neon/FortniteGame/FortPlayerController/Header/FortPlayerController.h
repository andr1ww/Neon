#pragma once
#include "pch.h"

#include "Neon/Runtime/Runtime.h"
#include "Engine/PlayerController/Header/PlayerController.h"

class AFortPlayerControllerAthena : public APlayerController
{
public:
    static void ServerAcknowledgePossession(AFortPlayerControllerAthena* PlayerController, FFrame& Stack);
};

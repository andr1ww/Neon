#include "pch.h"
#include "../Header/PlayerController.h"

AActor* AController::GetViewTarget()
{
    this->CallFunc<AActor*>("AController","GetViewTarget");
}

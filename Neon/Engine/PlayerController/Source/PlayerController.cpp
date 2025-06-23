#include "pch.h"
#include "../Header/PlayerController.h"

AActor* AController::GetViewTarget()
{
    struct ParamsStruct { AActor* ReturnValue; } Params{};
    this->Call(L"GetViewTarget", &Params);
    return Params.ReturnValue;
}

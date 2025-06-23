#include "pch.h"
#include "../Header/GameplayStatics.h"

float UGameplayStatics::GetTimeSeconds(UWorld* World)
{
    if (!World) return 0.0f;
    float Result = 0.0f;
    struct { float ReturnValue; } Params;
    Params.ReturnValue = 0.0f;
    World->Call(L"GetTimeSeconds", &Params);
    Result = Params.ReturnValue;
    return Result;
}

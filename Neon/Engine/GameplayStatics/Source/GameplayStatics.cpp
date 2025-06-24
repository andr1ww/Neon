#include "pch.h"
#include "../Header/GameplayStatics.h"

float UGameplayStatics::GetTimeSeconds(UWorld* World)
{
    return GetDefaultObj()->CallFunc<float>("GameplayStatics","GetTimeSeconds", World);
}

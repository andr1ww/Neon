#include "pch.h"
#include "../Header/GameplayStatics.h"

float UGameplayStatics::GetTimeSeconds(UWorld* World)
{
    return UGameplayStatics::GetDefaultObj()->CallFunc<float>("UGameplayStatics","GetTimeSeconds", World);
}

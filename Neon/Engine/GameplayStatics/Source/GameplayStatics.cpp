#include "pch.h"
#include "../Header/GameplayStatics.h"

float UGameplayStatics::GetTimeSeconds(UWorld* World)
{
    static SDK::UFunction* Func = nullptr;
    SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("GameplayStatics", "GetTimeSeconds");

    if (Func == nullptr)
        Func = Info.Func;
    if (!Func)
        return 0.f;

    struct { UObject* WorldContextObject; float TimeSeconds; } GetTimeSecondsParams{World};
    
    SDK::StaticClassImpl("KismetSystemLibrary")->GetClassDefaultObject()->ProcessEvent(Func, &GetTimeSecondsParams);
    
    return GetTimeSecondsParams.TimeSeconds;
}

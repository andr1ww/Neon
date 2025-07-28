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

TArray<AActor*> UGameplayStatics::GetAllActorsOfClass(const UObject* World, UClass* ActorClass)
{
    static SDK::UFunction* Func = nullptr;
    SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("GameplayStatics", "GetAllActorsOfClass");

    if (Func == nullptr)
        Func = Info.Func;
    if (!Func)
        return TArray<AActor*>();

    struct { const UObject* WorldContextObject; UClass* ActorClass; TArray<AActor*> OutActors; }
    UGameplayStatics_GetAllActorsOfClass_Params{ World, ActorClass };
    
    SDK::StaticClassImpl("KismetSystemLibrary")->GetClassDefaultObject()->ProcessEvent(Func, &UGameplayStatics_GetAllActorsOfClass_Params);
    
    return UGameplayStatics_GetAllActorsOfClass_Params.OutActors;
}


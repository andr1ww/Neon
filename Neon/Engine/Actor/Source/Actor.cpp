#include "pch.h"
#include "Engine/Actor/Header/Actor.h"

FVector AActor::GetActorLocation()
{
    return this->CallFunc<FVector>("Actor","K2_GetActorLocation");
}

FVector AActor::GetActorForwardVector()
{
    return this->CallFunc<FVector>("Actor","GetActorForwardVector");
}

FVector AActor::GetActorUpVector()
{
    return this->CallFunc<FVector>("Actor","GetActorUpVector");
}

FVector AActor::GetActorRightVector()
{
    return this->CallFunc<FVector>("Actor","GetActorRightVector");
}

void AActor::K2_DestroyActor()
{
    static SDK::UFunction* Func = nullptr;
    SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("Actor", "K2_DestroyActor");

    if (Func == nullptr)
        Func = Info.Func;
    if (!Func)
        return;

    this->ProcessEvent(Func, nullptr);
}

FVector AActor::K2_GetActorLocation()
{
    static SDK::UFunction* Func = nullptr;
    SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("Actor", "K2_DestroyActor");

    if (Func == nullptr)
        Func = Info.Func;
    if (!Func)
        return FVector();

    struct Actor_K2_GetActorLocation final
    {
    public:
        FVector ReturnValue;
    };
	Actor_K2_GetActorLocation Params;

    this->ProcessEvent(Func, &Params);

	return Params.ReturnValue;
}



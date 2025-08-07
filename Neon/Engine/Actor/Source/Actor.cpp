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
    SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("Actor", "K2_GetActorLocation");

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

FRotator AActor::K2_GetActorRotation()
{
    static SDK::UFunction* Func = nullptr;
    SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("Actor", "K2_GetActorRotation");

    if (Func == nullptr)
        Func = Info.Func;
    if (!Func)
        return FRotator();

    struct Actor_K2_GetActorLocation final
    {
    public:
        FRotator ReturnValue;
    };
    Actor_K2_GetActorLocation Params;

    this->ProcessEvent(Func, &Params);

    return Params.ReturnValue;
}

FTransform AActor::GetTransform()
{
    static SDK::UFunction* Func = nullptr;
    SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("Actor", "GetTransform");

    if (Func == nullptr)
        Func = Info.Func;
    if (!Func)
        return FTransform();

    struct Actor_GetTransform final
    {
    public:
        FTransform ReturnValue;
    };
    Actor_GetTransform Params;

    this->ProcessEvent(Func, &Params);

    return Params.ReturnValue;
}

bool AActor::K2_SetActorRotation(const struct FRotator& NewRotation, bool bTeleportPhysics)
{
    static SDK::UFunction* Func = nullptr;
    SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("Actor", "K2_SetActorRotation");

    if (Func == nullptr)
        Func = Info.Func;
    if (!Func)
        return false;

    struct Actor_K2_SetActorRotation final
    {
    public:
        struct FRotator                               NewRotation; 
        bool                                          bTeleportPhysics;                                  
        bool                                          ReturnValue;                                       
    };
    Actor_K2_SetActorRotation Params;
    Params.NewRotation = std::move(NewRotation);
    Params.bTeleportPhysics = bTeleportPhysics;

    this->ProcessEvent(Func, &Params);

    return Params.ReturnValue;
}


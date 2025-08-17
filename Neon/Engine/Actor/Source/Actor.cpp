#include "pch.h"
#include "Engine/Actor/Header/Actor.h"

FVector AActor::GetActorLocation()
{
    return this->CallFunc<FVector>("Actor","K2_GetActorLocation");
}

FVector AActor::GetActorForwardVector()
{
    static SDK::UFunction* Func = nullptr;
    SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("Actor", "GetActorForwardVector");

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

FVector AActor::GetActorUpVector()
{
    static SDK::UFunction* Func = nullptr;
    SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("Actor", "GetActorUpVector");

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

float AActor::GetDistanceTo(AActor* Actor)
{
    static SDK::UFunction* Func = nullptr;
    SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("Actor", "GetDistanceTo");

    if (Func == nullptr)
        Func = Info.Func;
    if (!Func)
        return false;

    struct Actor_GetDistanceTo final
    {
    public:
        const class AActor*                           OtherActor;                                        // 0x0000(0x0008)(ConstParm, Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
        float                                         ReturnValue;                                       // 0x0008(0x0004)(Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
        uint8                                         Pad_C[0x4];                                        // 0x000C(0x0004)(Fixing Struct Size After Last Property [ Dumper-7 ])
    };
    Actor_GetDistanceTo Params;
    Params.OtherActor = std::move(Actor);

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

void AActor::K2_TeleportTo(const FVector Location, const FRotator Rot)
{
    static SDK::UFunction* Func = nullptr;
    SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("Actor", "K2_TeleportTo");

    if (Func == nullptr)
        Func = Info.Func;
    if (!Func)
        return;

    struct Actor_K2_TeleportTo final
    {
    public:
        struct FVector                                DestLocation;                                      // 0x0000(0x000C)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
        struct FRotator                               DestRotation;                                      // 0x000C(0x000C)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, NativeAccessSpecifierPublic)
        bool                                          ReturnValue;                                       // 0x0018(0x0001)(Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
        uint8                                         Pad_19[0x3];                                       // 0x0019(0x0003)(Fixing Struct Size After Last Property [ Dumper-7 ])
    };
    
    Actor_K2_TeleportTo Params;
    Params.DestLocation = Location;
    Params.DestRotation = Rot;

    this->ProcessEvent(Func, &Params);
}



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

    static UObject* Object = nullptr;
    if (!Object) Object = SDK::StaticClassImpl("GameplayStatics")->GetClassDefaultObject();
    
    Object->ProcessEvent(Func, &GetTimeSecondsParams);
    
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
    
    SDK::StaticClassImpl("GameplayStatics")->GetClassDefaultObject()->ProcessEvent(Func, &UGameplayStatics_GetAllActorsOfClass_Params);
    
    return UGameplayStatics_GetAllActorsOfClass_Params.OutActors;
}

AActor* UGameplayStatics::BeginDeferredActorSpawnFromClass(const UObject* WorldContextObject, UClass* ActorClass, const FTransform& SpawnTransform, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride, AActor* Owner)
{
    static SDK::UFunction* Func = nullptr;
    SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("GameplayStatics", "BeginDeferredActorSpawnFromClass");

    if (Func == nullptr)
        Func = Info.Func;
    if (!Func)
        return nullptr;

    struct
    {
        const UObject* WorldContextObject;                                       // (ConstParm, Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
        UClass* ActorClass;                                               // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic)
        FTransform                                  SpawnTransform;                                           // (ConstParm, Parm, OutParm, ReferenceParm, IsPlainOldData, NoDestructor, NativeAccessSpecifierPublic)
        ESpawnActorCollisionHandlingMethod                 CollisionHandlingOverride;                                // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
        AActor* Owner;                                                    // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
        AActor* ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    } UGameplayStatics_BeginDeferredActorSpawnFromClass_Params{ WorldContextObject, ActorClass, SpawnTransform, CollisionHandlingOverride, Owner };
    
    SDK::StaticClassImpl("GameplayStatics")->GetClassDefaultObject()->ProcessEvent(Func, &UGameplayStatics_BeginDeferredActorSpawnFromClass_Params);
    
    return UGameplayStatics_BeginDeferredActorSpawnFromClass_Params.ReturnValue;
}

AActor* UGameplayStatics::FinishSpawningActor(AActor* Actor, const FTransform& SpawnTransform)
{
    static SDK::UFunction* Func = nullptr;
    SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("GameplayStatics", "FinishSpawningActor");

    if (Func == nullptr)
        Func = Info.Func;
    if (!Func)
        return nullptr;

    struct
    {
        AActor* Actor;                                                    // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
        FTransform                                  SpawnTransform;                                           // (ConstParm, Parm, OutParm, ReferenceParm, IsPlainOldData, NoDestructor, NativeAccessSpecifierPublic)
        AActor* ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    } UGameplayStatics_FinishSpawningActor_Params{ Actor, SpawnTransform };
    
    SDK::StaticClassImpl("GameplayStatics")->GetClassDefaultObject()->ProcessEvent(Func, &UGameplayStatics_FinishSpawningActor_Params);
    
    return UGameplayStatics_FinishSpawningActor_Params.ReturnValue;
}

AActor* UGameplayStatics::SpawnActor(UClass* Class, FTransform& Transform, AActor* Owner)
{
    if (!Class || !UWorld::GetWorld())
        return nullptr;

    return FinishSpawningActor(BeginDeferredActorSpawnFromClass(UWorld::GetWorld(), Class, Transform, ESpawnActorCollisionHandlingMethod::AlwaysSpawn, Owner), Transform);
}

// from magma's gs i cba bro
inline void sinCos(float* ScalarSin, float* ScalarCos, float Value)
{
    float quotient = (0.31830988618f * 0.5f) * Value;
    if (Value >= 0.0f)
    {
        quotient = (float)((int)(quotient + 0.5f));
    }
    else
    {
        quotient = (float)((int)(quotient - 0.5f));
    }
    float y = Value - (2.0f * 3.1415926535897932f) * quotient;

    float sign;
    if (y > 1.57079632679f)
    {
        y = 3.1415926535897932f - y;
        sign = -1.0f;
    }
    else if (y < -1.57079632679f)
    {
        y = -3.1415926535897932f - y;
        sign = -1.0f;
    }
    else
    {
        sign = +1.0f;
    }

    float y2 = y * y;

    *ScalarSin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;

    float p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
    *ScalarCos = sign * p;
}
// from magma's gs i cba bro

inline FQuat FRotToQuat(FRotator Rot)
{
    const float DEG_TO_RAD = 3.1415926535897932f / (180.f);
    const float DIVIDE_BY_2 = DEG_TO_RAD / 2.f;
    float SP, SY, SR;
    float CP, CY, CR;

    sinCos(&SP, &CP, Rot.Pitch * DIVIDE_BY_2);
    sinCos(&SY, &CY, Rot.Yaw * DIVIDE_BY_2);
    sinCos(&SR, &CR, Rot.Roll * DIVIDE_BY_2);

    FQuat RotationQuat;
    RotationQuat.X = CR * SP * SY - SR * CP * CY;
    RotationQuat.Y = -CR * SP * CY - SR * CP * SY;
    RotationQuat.Z = CR * CP * SY - SR * SP * CY;
    RotationQuat.W = CR * CP * CY + SR * SP * SY;

    return RotationQuat;
}

AActor* UGameplayStatics::SpawnActor(UClass* Class, FVector& Loc, FRotator& Rot, AActor* Owner)
{
    if (!Class || !UWorld::GetWorld())
        return nullptr;

    FTransform Transform(Loc, FRotToQuat(Rot));
    return FinishSpawningActor(BeginDeferredActorSpawnFromClass(UWorld::GetWorld(), Class, Transform, ESpawnActorCollisionHandlingMethod::AlwaysSpawn, Owner), Transform);
}

UObject* UGameplayStatics::SpawnObject(TSubclassOf<class UObject> ObjectClass, class UObject* Param_Outer)
{
    static SDK::UFunction* Func = nullptr;
    SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("GameplayStatics", "SpawnObject");

    if (Func == nullptr)
        Func = Info.Func;
    if (!Func)
        return nullptr;
    
    struct
    {
        UClass* ObjectClass;                                              // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic)
        UObject* Outer;                                                    // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
        UObject* ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    } UGameplayStatics_SpawnObject_Params{ObjectClass, Param_Outer};
    
    SDK::StaticClassImpl("GameplayStatics")->GetClassDefaultObject()->ProcessEvent(Func, &UGameplayStatics_SpawnObject_Params);
    
    return UGameplayStatics_SpawnObject_Params.ReturnValue;
}

inline FQuat FRotToQuat2(FRotator Rot)
{
    const float DEG_TO_RAD = 3.1415926535897932f / 180.0f;
    const float DIVIDE_BY_2 = DEG_TO_RAD / 2.0f;
    
    float HalfPitch = Rot.Pitch * DIVIDE_BY_2;
    float HalfYaw = Rot.Yaw * DIVIDE_BY_2;
    float HalfRoll = Rot.Roll * DIVIDE_BY_2;
    
    float SP, SY, SR;
    float CP, CY, CR;

    sinCos(&SP, &CP, HalfPitch);
    sinCos(&SY, &CY, HalfYaw);
    sinCos(&SR, &CR, HalfRoll);

    FQuat RotationQuat;
    RotationQuat.X = CR * SP * SY - SR * CP * CY;
    RotationQuat.Y = -CR * SP * CY - SR * CP * SY;
    RotationQuat.Z = CR * CP * SY - SR * SP * CY;
    RotationQuat.W = CR * CP * CY + SR * SP * SY;

    return RotationQuat;
}

AActor* UGameplayStatics::SpawnActorOG(UClass* Class, FVector Loc, FRotator Rot, AActor* Owner)
{
    UWorld* World = UWorld::GetWorld();
    if (!World || !Class) 
        return nullptr;
    
    FActorSpawnParameters addr{};

    addr.Owner = Owner;
    addr.bDeferConstruction = false;
    addr.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    
    return ((AActor * (*)(UWorld*, UClass*, FVector const*, FRotator const*, FActorSpawnParameters*))(Finder->SpawnActor()))(UWorld::GetWorld(), Class, &Loc, &Rot, &addr);
}

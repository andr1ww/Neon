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
    
    static UObject* Object = nullptr;
    if (!Object) Object = SDK::StaticClassImpl("GameplayStatics")->GetClassDefaultObject();
    
    Object->ProcessEvent(Func, &UGameplayStatics_GetAllActorsOfClass_Params);
    
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

    static UObject* Object = nullptr;
    if (!Object) Object = SDK::StaticClassImpl("GameplayStatics")->GetClassDefaultObject();
    
    Object->ProcessEvent(Func, &UGameplayStatics_BeginDeferredActorSpawnFromClass_Params);
    
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
    
    static UObject* Object = nullptr;
    if (!Object) Object = SDK::StaticClassImpl("GameplayStatics")->GetClassDefaultObject();
    
    Object->ProcessEvent(Func, &UGameplayStatics_FinishSpawningActor_Params);
    
    return UGameplayStatics_FinishSpawningActor_Params.ReturnValue;
}

AActor* UGameplayStatics::SpawnActor(UClass* Class, FTransform& Transform, AActor* Owner)
{
    if (!Class || !UWorld::GetWorld())
        return nullptr;

    return FinishSpawningActor(BeginDeferredActorSpawnFromClass(UWorld::GetWorld(), Class, Transform, ESpawnActorCollisionHandlingMethod::AlwaysSpawn, Owner), Transform);
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
    
    static UObject* Object = nullptr;
    if (!Object) Object = SDK::StaticClassImpl("GameplayStatics")->GetClassDefaultObject();
    
    Object->ProcessEvent(Func, &UGameplayStatics_SpawnObject_Params);
    
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

#pragma once
#include "pch.h"

#include "Engine/Actor/Header/Actor.h"

class AController;
class APlayerState;

class UMovementComponent : public UObject
{
public:
    DEFINE_MEMBER(FVector, UMovementComponent, Velocity);
public:
    DECLARE_STATIC_CLASS(UMovementComponent)
    DECLARE_DEFAULT_OBJECT(UMovementComponent)
};

class UNavMovementComponent : public UMovementComponent
{
public:
    bool IsFlying()
    {
        static class UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("NavMovementComponent", "IsFlying");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return false;

        struct NavMovementComponent_IsFlying final
        {
        public:
            bool                                          ReturnValue;
        };
        NavMovementComponent_IsFlying Params;

        this->ProcessEvent(Func, &Params);

        return Params.ReturnValue;
    }

    bool IsFalling()
    {
        static class UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("NavMovementComponent", "IsFalling");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return false;

        struct NavMovementComponent_IsFalling final
        {
        public:
            bool                                          ReturnValue;
        };
        NavMovementComponent_IsFalling Params;

        if (!this) return false;
        this->ProcessEvent(Func, &Params);

        return Params.ReturnValue;
    }
public:
    DECLARE_DEFAULT_OBJECT(UNavMovementComponent);
    DECLARE_STATIC_CLASS(UNavMovementComponent);
};

class UPawnMovementComponent : public UNavMovementComponent
{
public:
    DECLARE_DEFAULT_OBJECT(UPawnMovementComponent);
    DECLARE_STATIC_CLASS(UPawnMovementComponent);
};

class APawn : public AActor
{
public:
    DEFINE_PTR(APlayerState, APawn, PlayerState)
    DEFINE_PTR(AController, APawn, Controller);
public:
    UPawnMovementComponent* GetMovementComponent() {
        static class UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("Pawn", "GetMovementComponent");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return nullptr;

        struct Pawn_GetMovementComponent final
        {
        public:
            class UPawnMovementComponent*                 ReturnValue;
        };
        Pawn_GetMovementComponent Params;

        this->ProcessEvent(Func, &Params);

        return Params.ReturnValue;
    }

    void AddMovementInput(const struct FVector& WorldDirection, float ScaleValue, bool bForce)
    {
        static class UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("Pawn", "AddMovementInput");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return;

        struct Pawn_AddMovementInput final
        {
        public:
            struct FVector                                WorldDirection;                                    // 0x0000(0x000C)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            float                                         ScaleValue;                                        // 0x000C(0x0004)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            bool                                          bForce;                                            // 0x0010(0x0001)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            uint8                                         Pad_11[0x3];                                       // 0x0011(0x0003)(Fixing Struct Size After Last Property [ Dumper-7 ])
        } Parms;
        
        Parms.WorldDirection = std::move(WorldDirection);
        Parms.ScaleValue = ScaleValue;
        Parms.bForce = bForce;

        this->ProcessEvent(Func, &Parms);
    }
};

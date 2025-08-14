#pragma once
#include "pch.h"

#include "Engine/Actor/Header/Actor.h"

class AController;
class APlayerState;

class UNavMovementComponent : public UObject
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
};

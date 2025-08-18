#pragma once
#include "pch.h"
#include "Engine/Actor/Header/Actor.h"
#include "Engine/Pawn/Header/Pawn.h"
#include "Engine/PlayerState/Header/PlayerState.h"

class AController : public AActor
{
public:
    DEFINE_PTR(APlayerState, AController, PlayerState);
    DEFINE_PTR(APawn, AController, Pawn);
public:
    AActor* GetViewTarget();
public:
    void SetControlRotation(const struct FRotator& NewRotation)
    {
        static class SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("Controller", "SetControlRotation");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return;

        struct Controller_SetControlRotation final
        {
        public:
            struct FRotator                               NewRotation;
        };
        Controller_SetControlRotation Params{};
        Params.NewRotation = std::move(NewRotation);

        this->ProcessEvent(Func, &Params);
    }

    void StopMovement()
    {
        static class SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("Controller", "StopMovement");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return;

        this->ProcessEvent(Func, nullptr);
    }
};

class APlayerController : public AController
{
public:
    
public:
    DEFINE_MEMBER(APawn*, APlayerController, AcknowledgedPawn);
public:
    DECLARE_STATIC_CLASS(APlayerController);
    DECLARE_DEFAULT_OBJECT(APlayerController);
};

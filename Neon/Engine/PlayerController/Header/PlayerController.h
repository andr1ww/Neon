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

    bool LineOfSightTo(const class AActor* Other, const struct FVector& ViewPoint, bool bAlternateChecks) 
    {
        static class SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("Controller", "LineOfSightTo");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return false;

        struct Controller_LineOfSightTo final
        {
        public:
            const class AActor*                           Other;                                             // 0x0000(0x0008)(ConstParm, Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            struct FVector                                ViewPoint;                                         // 0x0008(0x000C)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            bool                                          bAlternateChecks;                                  // 0x0014(0x0001)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            bool                                          ReturnValue;                                       // 0x0015(0x0001)(Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            uint8                                         Pad_16[0x2];                                       // 0x0016(0x0002)(Fixing Struct Size After Last Property [ Dumper-7 ])
        } Params; 

        Params.Other = Other;
        Params.ViewPoint = std::move(ViewPoint);
        Params.bAlternateChecks = bAlternateChecks;
        
        this->ProcessEvent(Func, &Params);

        return Params.ReturnValue;
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

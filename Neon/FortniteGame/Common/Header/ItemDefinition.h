#pragma once
#include "pch.h"
#include "Neon/Runtime/Runtime.h"

class UFortItemDefinition : public UObject
{
public:
    class UFortWorldItem* CreateTemporaryItemInstanceBP(int32 Count, int32 Level)
    {
        static UFunction* Func = Runtime::StaticFindObject<UFunction>("/Script/FortniteGame.FortItemDefinition.CreateTemporaryItemInstanceBP");

        struct FortItemDefinition_CreateTemporaryItemInstanceBP final
        {
        public:
            int32                                         Count;                                             // 0x0000(0x0004)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            int32                                         Level;                                             // 0x0004(0x0004)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            UFortWorldItem* ReturnValue;                                       // 0x0008(0x0008)(Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
        };

        FortItemDefinition_CreateTemporaryItemInstanceBP Parms{};
        Parms.Count = Count;
        Parms.Level = Level;

        auto Flgs = Func->FunctionFlags();
        Func->FunctionFlags() |= 0x400;

        this->ProcessEvent(Func, &Parms);

        Func->FunctionFlags() = Flgs;

        return Parms.ReturnValue;
    }
};

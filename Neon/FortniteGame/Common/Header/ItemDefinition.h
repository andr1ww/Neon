#pragma once
#include "pch.h"
#include "Neon/Runtime/Runtime.h"

class UFortItemDefinition : public UObject
{
public:
    class UFortWorldItem* CreateTemporaryItemInstanceBP(int32 Count, int32 Level)
    {
        struct FortItemDefinition_CreateTemporaryItemInstanceBP final
        {
        public:
            int32                                                 Count;                                             // 0x0000(0x0004)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            int32                                                 Level;                                             // 0x0004(0x0004)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            UFortWorldItem*                               ReturnValue;                                       // 0x0008(0x0008)(Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
        };

        FortItemDefinition_CreateTemporaryItemInstanceBP Params{};

        Params.Count = Count;
        Params.Level = Level;
    
        static UFunction* Func = nullptr;

        if (!Func)
        {
            Func = Runtime::StaticFindObject<UFunction>("/Script/FortniteGame.FortItemDefinition.CreateTemporaryItemInstanceBP");
        }
        
        /*auto Flgs = Func->FunctionFlags();
        Func->FunctionFlags() |= 0x400;*/

        ProcessEvent(Func, &Params);

        //Func->FunctionFlags() = Flgs;

        return Params.ReturnValue;
    }
};

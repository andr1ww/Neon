#pragma once
#include "pch.h"

class UFortItemDefinition : public UObject
{
public:
    UObject* CreateTemporaryItemInstanceBP(int32 Count, int32 Level)
    {
        struct FortItemDefinition_CreateTemporaryItemInstanceBP final
        {
        public:
            int32                                                 Count;                                             // 0x0000(0x0004)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            int32                                                 Level;                                             // 0x0004(0x0004)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            UObject*                               ReturnValue;                                       // 0x0008(0x0008)(Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
        };

        FortItemDefinition_CreateTemporaryItemInstanceBP Params{};

        Params.Count = Count;
        Params.Level = Level;
    
        static SDK::UFunction* Func = nullptr;

        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortItemDefinition", "CreateTemporaryItemInstanceBP");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
        {
            UE_LOG(LogNeon, Fatal, "Failed to find function CreateTemporaryItemInstanceBP in FortItemDefinition");
            return nullptr;
        }
        
        auto Flgs = Func->FunctionFlags();
        Func->FunctionFlags() |= 0x400;
    
        this->ProcessEvent(Func, &Params);

        Func->FunctionFlags() = Flgs;

        return Params.ReturnValue;
    }
};

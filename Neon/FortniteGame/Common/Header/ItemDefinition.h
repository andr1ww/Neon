#pragma once
#include "pch.h"
#include "Neon/Runtime/Runtime.h"

class UFortItemDefinition : public UObject
{
public:
    class UFortWorldItem* CreateTemporaryItemInstanceBP(int32 Count, int32 Level)
    {
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortItemDefinition", "CreateTemporaryItemInstanceBP");

        if (Info.Func)
        {
            size_t ParmsSize = 0x10;//Info.Func->GetParmsSize();
            void* Parms = ::malloc(ParmsSize);

            if (Parms)
            {
                memset(Parms, 0, ParmsSize);

                *(int32*)(__int64(Parms) + 0x0) = Count;
                *(int32*)(__int64(Parms) + 0x4) = Level;

                ProcessEvent(Info.Func, Parms);

                UFortWorldItem* ReturnValue = *(UFortWorldItem**)(__int64(Parms) + 0x8);
                ::free(Parms);

                return ReturnValue;
            }
        }

        return nullptr;

        //struct FortItemDefinition_CreateTemporaryItemInstanceBP final
        //{
        //public:
        //    int32                                                 Count;                                             // 0x0000(0x0004)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
        //    int32                                                 Level;                                             // 0x0004(0x0004)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
        //    UFortWorldItem*                               ReturnValue;                                       // 0x0008(0x0008)(Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
        //};

        //FortItemDefinition_CreateTemporaryItemInstanceBP Params{};

        //Params.Count = Count;
        //Params.Level = Level;
    
        //static UFunction* Func = nullptr;

        //if (!Func)
        //{
        //    Func = Runtime::StaticFindObject<UFunction>("/Script/FortniteGame.FortItemDefinition.CreateTemporaryItemInstanceBP");
        //}

        //return nullptr;

        //struct FortItemDefinition_CreateTemporaryItemInstanceBP final
        //{
        //public:
        //    int32                                                 Count;                                             // 0x0000(0x0004)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
        //    int32                                                 Level;                                             // 0x0004(0x0004)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
        //    UFortWorldItem*                               ReturnValue;                                       // 0x0008(0x0008)(Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
        //};

        //FortItemDefinition_CreateTemporaryItemInstanceBP Params{};

        //Params.Count = Count;
        //Params.Level = Level;
    
        //static SDK::UFunction* Func = nullptr;

        //SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortItemDefinition", "CreateTemporaryItemInstanceBP");

        //if (Func == nullptr)
        //    Func = Info.Func;
        //if (!Func)
        //{
        //    UE_LOG(LogNeon, Fatal, "Failed to find function CreateTemporaryItemInstanceBP in FortItemDefinition");
        //    return nullptr;
        //}
        //
        ///*auto Flgs = Func->FunctionFlags();
        //Func->FunctionFlags() |= 0x400;*/

        //ProcessEvent(Func, &Params);

        ////Func->FunctionFlags() = Flgs;

        //return Params.ReturnValue;
    }
};

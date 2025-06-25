#pragma once
#include "pch.h"
#include "Neon/Runtime/Runtime.h"

class UFortItemDefinition : public UObject
{
public:
    class UFortWorldItem* CreateTemporaryItemInstanceBP(int32 Count, int32 Level)
    {
        static UFunction* Func = Runtime::StaticFindObject<UFunction>("/Script/FortniteGame.FortItemDefinition.CreateTemporaryItemInstanceBP");

        if (Func)
        {
            size_t ParmsSize = 0x10;
            void* Parms = ::malloc(ParmsSize);

            if (Parms)
            {
                memset(Parms, 0, ParmsSize);

                *(int32*)(__int64(Parms) + 0x0) = Count;
                *(int32*)(__int64(Parms) + 0x4) = Level;

                ProcessEvent(Func, &Parms);

                UFortWorldItem* ReturnValue = *(UFortWorldItem**)(__int64(&Parms) + 0x8);
                ::free(Parms);

                return ReturnValue;
            }
        }

        return nullptr;
    }
};

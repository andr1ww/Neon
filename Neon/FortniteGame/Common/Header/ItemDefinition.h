#pragma once
#include "pch.h"

class UFortItemDefinition : public UObject
{
public:
    UObject* CreateTemporaryItemInstanceBP(int32 Count, int32 Level)
    {
        this->CallFunc<UObject*>("FortItemDefinition", "CreateTemporaryItemInstanceBP", Count, Level);
    }
};

#pragma once
#include "pch.h"

namespace HTTP
{
    inline void (*DispatchRequestOriginal)(__int64 a1, __int64* a2, int a3);
    void DispatchRequest(__int64 a1, __int64* a2, int a3);
}
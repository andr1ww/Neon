#pragma once
#include "pch.h"

struct FGuid final
{
public:
    int32                                         A;                                                 // 0x0000(0x0004)(Edit, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    int32                                         B;                                                 // 0x0004(0x0004)(Edit, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    int32                                         C;                                                 // 0x0008(0x0004)(Edit, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    int32                                         D;                                                 // 0x000C(0x0004)(Edit, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)

    bool operator==(FGuid& _Rhs)
    {
#define c(v) v == _Rhs.v
        return c(A) && c(B) && c(C) && c(D);
    }
};
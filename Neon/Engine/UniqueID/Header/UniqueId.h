#pragma once
#include "pch.h"

struct FUniqueNetIdWrapper
{
public:
    uint8                                         Pad_18[0x1];                                       // 0x0000(0x0001)(Fixing Struct Size After Last Property [ Dumper-7 ])
};

struct FUniqueNetIdRepl final : public FUniqueNetIdWrapper
{
public:
    uint8                                         Pad_B8[0x17];                                      // 0x0001(0x0017)(Fixing Size After Last Property [ Dumper-7 ])
    TArray<uint8>                                 ReplicationBytes;                                  // 0x0018(0x0010)(ZeroConstructor, Transient, Protected, HasGetValueTypeHash, NativeAccessSpecifierProtected)
};
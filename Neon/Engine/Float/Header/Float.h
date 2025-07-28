#pragma once
#include "pch.h"

struct FScalableFloat
{
public:
	float                                         Value;                                             // 0x0000(0x0004)(Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	uint8                                         Pad_4[0x4];                                        // 0x0004(0x0004)(Fixing Size After Last Property [ Dumper-7 ])
	struct FCurveTableRowHandle                   Curve;                                             // 0x0008(0x0010)(Edit, BlueprintVisible, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	struct FDataRegistryType                      RegistryType;                                      // 0x0018(0x0004)(Edit, BlueprintVisible, ZeroConstructor, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	uint8                                         Pad_1C[0xC];                                       // 0x001C(0x000C)(Fixing Struct Size After Last Property [ Dumper-7 ])
};
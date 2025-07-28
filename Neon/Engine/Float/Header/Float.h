#pragma once
#include "pch.h"

#include "Engine/DataTable/Header/DataTable.h"

struct FScalableFloat
{
public:
	float                                        Value;                                             // 0x0(0x4)(Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	uint8                                        Pad_3BF0[0x4];                                     // Fixing Size After Last Property  [ Dumper-7 ]
	 FCurveTableRowHandle                  Curve;                                             // 0x8(0x10)(Edit, BlueprintVisible, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
};

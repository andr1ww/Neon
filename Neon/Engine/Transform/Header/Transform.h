#pragma once
#include "pch.h"

#include "Engine/Quat/Header/Quat.h"
#include "Engine/Vector/Header/Vector.h"

struct FTransform {
    FQuat Rotation;
    FVector Translation;
    uint8_t Pad_1C[0x4];
    FVector Scale3D;
    uint8_t Pad_2C[0x4];

    FTransform()
        : Rotation(0, 0, 0, 1), Translation(0, 0, 0), Scale3D(1, 1, 1) {
    }

    FTransform(const FQuat& InRotation, const FVector& InTranslation, const FVector& InScale3D)
        : Rotation(InRotation), Translation(InTranslation), Scale3D(InScale3D) {
    }

};

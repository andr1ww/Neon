#pragma once
#include "pch.h"

struct FRotator
{
    float Pitch;
    float Yaw;
    float Roll;

    FRotator(float InPitch = 0, float InYaw = 0, float InRoll = 0)
        : Pitch(InPitch), Yaw(InYaw), Roll(InRoll) {
    }

    void Normalize()
    {
        Pitch = NormalizeAxis(Pitch);
        Yaw = NormalizeAxis(Yaw);
        Roll = NormalizeAxis(Roll);
    }

    bool Equals(const FRotator& Other, float Tolerance = 1e-4f) const
    {
        return std::abs(NormalizeAxis(Pitch - Other.Pitch)) < Tolerance &&
            std::abs(NormalizeAxis(Yaw - Other.Yaw)) < Tolerance &&
            std::abs(NormalizeAxis(Roll - Other.Roll)) < Tolerance;
    }
private:
    static float NormalizeAxis(float Angle)
    {
        Angle = std::fmod(Angle + 180.f, 360.f);
        if (Angle < 0.f)
            Angle += 360.f;
        return Angle - 180.f;
    }
};

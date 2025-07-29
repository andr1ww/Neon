#pragma once
#include "pch.h"

#include "Engine/Rotator/Header/Rotator.h"

struct FQuat {
    float X, Y, Z, W;

    FQuat() : X(0), Y(0), Z(0), W(1) {}
    FQuat(float x, float y, float z, float w) : X(x), Y(y), Z(z), W(w) {}

    FQuat Conjugate() const
    {
        return FQuat(-X, -Y, -Z, W);
    }

    FQuat Inverse() const
    {
        float Norm = X * X + Y * Y + Z * Z + W * W;
        if (Norm > 0.f)
        {
            float InvNorm = 1.f / Norm;
            return FQuat(-X * InvNorm, -Y * InvNorm, -Z * InvNorm, W * InvNorm);
        }
        return FQuat(0.f, 0.f, 0.f, 1.f);
    }

    FQuat Normalize() const
    {
        float Norm = std::sqrt(X * X + Y * Y + Z * Z + W * W);
        if (Norm > 0.f)
        {
            float InvNorm = 1.f / Norm;
            return FQuat(X * InvNorm, Y * InvNorm, Z * InvNorm, W * InvNorm);
        }
        return FQuat(0.f, 0.f, 0.f, 1.f);
    }

    FRotator ToRotator() const;

    FQuat operator*(const FQuat& Q) const
    {
        return FQuat(
            W * Q.X + X * Q.W + Y * Q.Z - Z * Q.Y,
            W * Q.Y - X * Q.Z + Y * Q.W + Z * Q.X,
            W * Q.Z + X * Q.Y - Y * Q.X + Z * Q.W,
            W * Q.W - X * Q.X - Y * Q.Y - Z * Q.Z
        );
    }

    static FQuat Slerp(const FQuat& A, const FQuat& B, float Alpha)
    {
        float Dot = A.X * B.X + A.Y * B.Y + A.Z * B.Z + A.W * B.W;
        FQuat BCopy = B;

        if (Dot < 0.f)
        {
            Dot = -Dot;
            BCopy = FQuat(-B.X, -B.Y, -B.Z, -B.W);
        }

        if (Dot > 0.9995f)
        {
            return FQuat(
                A.X + Alpha * (BCopy.X - A.X),
                A.Y + Alpha * (BCopy.Y - A.Y),
                A.Z + Alpha * (BCopy.Z - A.Z),
                A.W + Alpha * (BCopy.W - A.W)
            ).Normalize();
        }

        float Theta = std::acos(Dot);
        float SinTheta = std::sin(Theta);
        float ScaleA = std::sin((1.f - Alpha) * Theta) / SinTheta;
        float ScaleB = std::sin(Alpha * Theta) / SinTheta;

        return FQuat(
            ScaleA * A.X + ScaleB * BCopy.X,
            ScaleA * A.Y + ScaleB * BCopy.Y,
            ScaleA * A.Z + ScaleB * BCopy.Z,
            ScaleA * A.W + ScaleB * BCopy.W
        );
    }
};

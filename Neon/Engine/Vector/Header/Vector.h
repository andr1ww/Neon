#pragma once
#include "pch.h"

struct FVector2D {
    float X, Y;

    FVector2D() : X(0), Y(0) {}
    FVector2D(float x, float y) : X(x), Y(y) {}

    FVector2D operator+(const FVector2D& Other) const {
        return { X + Other.X, Y + Other.Y };
    }

    FVector2D operator-(const FVector2D& Other) const {
        return { X - Other.X, Y - Other.Y };
    }

    FVector2D operator*(const FVector2D& Other) const {
        return { X * Other.X, Y * Other.Y };
    }

    FVector2D operator*(float Scalar) const {
        return { X * Scalar, Y * Scalar };
    }

    FVector2D operator/(const FVector2D& Other) const {
        if (Other.X == 0.0f || Other.Y == 0.0f)
            return *this;
        return { X / Other.X, Y / Other.Y };
    }

    FVector2D operator/(float Scalar) const {
        if (Scalar == 0.0f)
            return *this;
        return { X / Scalar, Y / Scalar };
    }

    FVector2D& operator+=(const FVector2D& Other) {
        X += Other.X;
        Y += Other.Y;
        return *this;
    }

    FVector2D& operator-=(const FVector2D& Other) {
        X -= Other.X;
        Y -= Other.Y;
        return *this;
    }

    FVector2D& operator*=(float Scalar) {
        X *= Scalar;
        Y *= Scalar;
        return *this;
    }

    FVector2D& operator/=(float Scalar) {
        if (Scalar != 0.0f) {
            X /= Scalar;
            Y /= Scalar;
        }
        return *this;
    }

    bool operator==(const FVector2D& Other) const {
        return X == Other.X && Y == Other.Y;
    }

    bool operator!=(const FVector2D& Other) const {
        return !(*this == Other);
    }

    float operator|(const FVector2D& Other) const {
        return X * Other.X + Y * Other.Y;
    }

    FVector2D operator-() const {
        return { -X, -Y };
    }

    float Size() const {
        return sqrtf(X * X + Y * Y);
    }

    float SizeSquared() const {
        return X * X + Y * Y;
    }

    FVector2D GetNormalized() const {
        float Length = Size();
        if (Length > 0.0f) {
            return *this / Length;
        }
        return { 0, 0 };
    }

    void Normalize() {
        float Length = Size();
        if (Length > 0.0f) {
            *this /= Length;
        }
    }

    bool IsZero() const {
        return X == 0.0f && Y == 0.0f;
    }
};


struct FVector {
    float X, Y, Z;

    FVector() : X(0), Y(0), Z(0) {}
    FVector(float x, float y, float z) : X(x), Y(y), Z(z) {}
    
    FVector operator+(const FVector& Other) const {
        return FVector(X + Other.X, Y + Other.Y, Z + Other.Z);
    }

    FVector& operator/=(float Scalar)
    {
        *this = *this / Scalar;
        return *this;
    }

    FVector operator*(const FVector& Other) const
    {
        return { X * Other.X, Y * Other.Y, Z * Other.Z };
    }
    FVector operator*(float Scalar) const
    {
        return { X * Scalar, Y * Scalar, Z * Scalar };
    }
    
    FVector operator-(const FVector& Other) const
    {
        return { X - Other.X, Y - Other.Y, Z - Other.Z };
    }
    FVector operator/(const FVector& Other) const
    {
        if (Other.X == 0.0f || Other.Y == 0.0f ||Other.Z == 0.0f)
            return *this;

        return { X / Other.X, Y / Other.Y, Z / Other.Z };
    }
    FVector operator/(float Scalar) const
    {
        if (Scalar == 0.0f)
            return *this;

        return { X / Scalar, Y / Scalar, Z / Scalar };
    }

    bool operator!=(const FVector& Other) const
    {
        return X != Other.X || Y != Other.Y || Z != Other.Z;
    }
    
    bool operator==(const FVector& Other) const
    {
        return X == Other.X && Y == Other.Y && Z == Other.Z;
    }
    
    FVector& operator+=(const FVector& Other) {
        X += Other.X;
        Y += Other.Y;
        Z += Other.Z;
        return *this;
    }
    
    FVector& operator-=(const FVector& Other) {
        X -= Other.X;
        Y -= Other.Y;
        Z -= Other.Z;
        return *this;
    }
    
    FVector& operator*=(float Scalar)
    {
        X *= Scalar;
        Y *= Scalar;
        Z *= Scalar;
        return *this;
    }
    
    float operator|(const FVector& Other) const {
        return X * Other.X + Y * Other.Y + Z * Other.Z;
    }
    
    FVector operator^(const FVector& Other) const {
        return FVector(
            Y * Other.Z - Z * Other.Y,
            Z * Other.X - X * Other.Z,
            X * Other.Y - Y * Other.X
        );
    }

    FVector operator-() const {
        return FVector(-X, -Y, -Z);
    }
    
    float Size() const {
        return sqrtf(X * X + Y * Y + Z * Z);
    }
    
    float SizeSquared() const {
        return X * X + Y * Y + Z * Z;
    }
    
    FVector GetNormalized() const {
        float Length = Size();
        if (Length > 0.0f) {
            return *this / Length;
        }
        return FVector(0, 0, 0);
    }
    
    void Normalize() {
        float Length = Size();
        if (Length > 0.0f) {
            *this /= Length;
        }
    }
    
    bool IsZero() const {
        return X == 0.0f && Y == 0.0f && Z == 0.0f;
    }
};

inline FVector operator*(float Scalar, const FVector& Vector) {
    return Vector * Scalar;
}

struct FVector_NetQuantize10 final : public FVector
{

};

struct FVector_NetQuantizeNormal final : public FVector
{

};

struct FVector_NetQuantize100 final : public FVector
{
};
#pragma once
#include "pch.h"

struct FVector {
    float X, Y, Z;

    FVector() : X(0), Y(0), Z(0) {}
    FVector(float x, float y, float z) : X(x), Y(y), Z(z) {}
    
    FVector operator+(const FVector& Other) const {
        return FVector(X + Other.X, Y + Other.Y, Z + Other.Z);
    }
    
    FVector& operator+=(const FVector& Other) {
        X += Other.X;
        Y += Other.Y;
        Z += Other.Z;
        return *this;
    }
    
    FVector operator-(const FVector& Other) const {
        return FVector(X - Other.X, Y - Other.Y, Z - Other.Z);
    }
    
    FVector& operator-=(const FVector& Other) {
        X -= Other.X;
        Y -= Other.Y;
        Z -= Other.Z;
        return *this;
    }
    
    FVector operator*(float Scalar) const {
        return FVector(X * Scalar, Y * Scalar, Z * Scalar);
    }
    
    FVector& operator*=(float Scalar) {
        X *= Scalar;
        Y *= Scalar;
        Z *= Scalar;
        return *this;
    }
    
    FVector operator/(float Scalar) const {
        if (Scalar != 0.0f) {
            return FVector(X / Scalar, Y / Scalar, Z / Scalar);
        }
        return FVector(0, 0, 0); 
    }
    
    FVector& operator/=(float Scalar) {
        if (Scalar != 0.0f) {
            X /= Scalar;
            Y /= Scalar;
            Z /= Scalar;
        }
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
    
    bool operator==(const FVector& Other) const {
        return X == Other.X && Y == Other.Y && Z == Other.Z;
    }
    
    bool operator!=(const FVector& Other) const {
        return !(*this == Other);
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
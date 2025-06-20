#pragma once
#include "pch.h"

class UKismetStringLibrary : public UObject
{
private:
    struct KismetStringLibrary_ConvStringToName
    {
    public:
        FString InString;
        FName ReturnValue;
    };

public:
public:
    DECLARE_STATIC_CLASS(UKismetStringLibrary);
    DECLARE_DEFAULT_OBJECT(UKismetStringLibrary);
};


class UKismetSystemLibrary : public UObject
{
public:
public:
    DECLARE_STATIC_CLASS(UKismetSystemLibrary);
    DECLARE_DEFAULT_OBJECT(UKismetSystemLibrary);
};

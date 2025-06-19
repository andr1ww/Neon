#pragma once
#include "pch.h"

class AActor : public UObject
{
private:

public:

public:
    FTransform GetTransform();
public:
    DECLARE_STATIC_CLASS(AActor);
    DECLARE_DEFAULT_OBJECT(AActor);
};
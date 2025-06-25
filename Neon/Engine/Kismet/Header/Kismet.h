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

class FScriptInterface
{
public:
    UObject*                                      ObjectPointer;                                     // 0x0000(0x0008)(NOT AUTO-GENERATED PROPERTY)
    void*                                         InterfacePointer;                                  // 0x0008(0x0008)(NOT AUTO-GENERATED PROPERTY)

public:
    class UObject* GetObjectRef() const
    {
        return ObjectPointer;
    }
	
    void* GetInterfaceRef() const
    {
        return InterfacePointer;
    }
	
};

template<class InterfaceType>
class TScriptInterface : public FScriptInterface
{
public:
};

class UFortKismetLibrary : public UObject
{
public:

public:
    DECLARE_STATIC_CLASS(UFortKismetLibrary);
    DECLARE_DEFAULT_OBJECT(UFortKismetLibrary);
};

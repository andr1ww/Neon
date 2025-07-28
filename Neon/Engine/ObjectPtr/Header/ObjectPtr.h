#pragma once
#include "pch.h"

class FWeakObjectPtr
{
public:
    int32                                         ObjectIndex;                                       // 0x0000(0x0004)(NOT AUTO-GENERATED PROPERTY)
    int32                                         ObjectSerialNumber;                                // 0x0004(0x0004)(NOT AUTO-GENERATED PROPERTY)

    FWeakObjectPtr(int32 Index = 0, int32 SerialNumber = 0)
    : ObjectIndex(Index), ObjectSerialNumber(SerialNumber)
    {}

    FWeakObjectPtr(UObjectBase* Object) {
        ObjectIndex = Object->GetUniqueID();
        ObjectSerialNumber = GUObjectArray.IndexToObject(ObjectIndex)->SerialNumber;
    }
	
public:
    class UObject* Get() const;
    class UObject* operator->() const;
    bool operator==(const FWeakObjectPtr& Other) const;
    bool operator!=(const FWeakObjectPtr& Other) const;
    bool operator==(const class UObject* Other) const;
    bool operator!=(const class UObject* Other) const;
};

template<typename UEType>
class TWeakObjectPtr : public FWeakObjectPtr
{
public:
    TWeakObjectPtr(int32 Index = 0, int32 SerialNumber = 0)
    : FWeakObjectPtr(Index, SerialNumber)
    {}

    TWeakObjectPtr(UEType* Obj)
        : FWeakObjectPtr(Obj) 
    {}
	
    UEType* Get() const
    {
        return static_cast<UEType*>(FWeakObjectPtr::Get());
    }

    UEType* operator->() const
    {
        return static_cast<UEType*>(FWeakObjectPtr::Get());
    }
};
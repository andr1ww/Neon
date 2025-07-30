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
    static FString Conv_NameToString(FName InName)
    {
        static SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("KismetStringLibrary", "Conv_NameToString");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return FString();

        struct KismetStringLibrary_Conv_NameToString final
        {
        public:
            class FName                                   InName;                                            // 0x0000(0x0008)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            class FString                                 ReturnValue;                                       // 0x0008(0x0010)(Parm, OutParm, ZeroConstructor, ReturnParm, HasGetValueTypeHash, NativeAccessSpecifierPublic)
        } KismetStringLibrary_Conv_NameToString_Params{ InName };
    
        SDK::StaticClassImpl("KismetStringLibrary")->GetClassDefaultObject()->ProcessEvent(Func, &KismetStringLibrary_Conv_NameToString_Params);
    
        return KismetStringLibrary_Conv_NameToString_Params.ReturnValue;
    }
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
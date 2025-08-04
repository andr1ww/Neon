#pragma once
#include "pch.h"
#include "Engine/Text/Header/Text.h"

class UKismetMathLibrary final : public UObject {
private:
    struct KismetMathLibrary_RandomBoolWithWeight {
    public:
        float Weight;
        bool ReturnValue;
    };
public:
    static bool RandomBoolWithWeight(float Weight) {
        static class UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("KismetMathLibrary", "RandomBoolWithWeight");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return false;

        struct KismetMathLibrary_RandomBoolWithWeight final
        {
        public:
            float Weight;
            bool ReturnValue;
        } KismetMathLibrary_RandomBoolWithWeight{ Weight };

        static UObject* Object = nullptr;
        if (!Object) Object = SDK::StaticClassImpl("KismetMathLibrary")->GetClassDefaultObject();

        Object->ProcessEvent(Func, &KismetMathLibrary_RandomBoolWithWeight);

        return KismetMathLibrary_RandomBoolWithWeight.ReturnValue;
    }

public:
    DECLARE_STATIC_CLASS(UKismetMathLibrary);
    DECLARE_DEFAULT_OBJECT(UKismetMathLibrary);
};

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

        static UObject* Object = nullptr;
        if (!Object) Object = SDK::StaticClassImpl("KismetStringLibrary")->GetClassDefaultObject();
        
        Object->ProcessEvent(Func, &KismetStringLibrary_Conv_NameToString_Params);
    
        return KismetStringLibrary_Conv_NameToString_Params.ReturnValue;
    }

    static FText Conv_StringToText(FString InName)
    {
        static SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("KismetStringLibrary", "Conv_StringToText");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return FText();

        struct KismetStringLibrary_Conv_StringToText final
        {
        public:
            class FString                                   inString;                                            // 0x0000(0x0008)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            class FText                                 ReturnValue;                                       // 0x0008(0x0010)(Parm, OutParm, ZeroConstructor, ReturnParm, HasGetValueTypeHash, NativeAccessSpecifierPublic)
        } KismetStringLibrary_Conv_NameToString_Params{ InName };

        static UObject* Object = nullptr;
        if (!Object) Object = SDK::StaticClassImpl("KismetStringLibrary")->GetClassDefaultObject();
        
        Object->ProcessEvent(Func, &KismetStringLibrary_Conv_NameToString_Params);
    
        return KismetStringLibrary_Conv_NameToString_Params.ReturnValue;
    }

    static FName Conv_StringToName(const FString& InString) {
        static SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("KismetStringLibrary", "Conv_StringToName");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return FName();

        KismetStringLibrary_ConvStringToName Params;
		Params.InString = InString;

        static UObject* DefaultObject = nullptr;
        if (!DefaultObject) DefaultObject = SDK::StaticClassImpl("KismetStringLibrary")->GetClassDefaultObject();
        DefaultObject->ProcessEvent(Func, &Params);

        return Params.ReturnValue;
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
    DECLARE_STATIC_CLASS(TScriptInterface)
    DECLARE_DEFAULT_OBJECT(TScriptInterface)
};
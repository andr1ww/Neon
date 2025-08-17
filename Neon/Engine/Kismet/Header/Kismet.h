#pragma once
#include "pch.h"

#include "Engine/Rotator/Header/Rotator.h"
#include "Engine/Text/Header/Text.h"
#include "Engine/Vector/Header/Vector.h"

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

    static FRotator FindLookAtRotation(const struct FVector& Start, const struct FVector& Target) {
        static class UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("KismetMathLibrary", "FindLookAtRotation");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return 0;

        struct KismetMathLibrary_RandomIntegerInRange final
        {
        public:
            struct FVector  Start;
             struct FVector Target;
            FRotator                                         ReturnValue;
        };
        KismetMathLibrary_RandomIntegerInRange Params;
        Params.Start = std::move(Start);
        Params.Target = std::move(Target);

        static UObject* Object = nullptr;
        if (!Object) Object = SDK::StaticClassImpl("KismetMathLibrary")->GetClassDefaultObject();

        Object->ProcessEvent(Func, &Params);

        return Params.ReturnValue;
    }


    static int32 RandomIntegerInRange(int32 min_0, int32 max_0) {
        static class UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("KismetMathLibrary", "RandomIntegerInRange");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return 0;

        struct KismetMathLibrary_RandomIntegerInRange final
        {
        public:
            int32                                         min_0;
            int32                                         max_0;
            int32                                         ReturnValue;
        };
        KismetMathLibrary_RandomIntegerInRange Params;
        Params.min_0 = min_0;
        Params.max_0 = max_0;

        static UObject* Object = nullptr;
        if (!Object) Object = SDK::StaticClassImpl("KismetMathLibrary")->GetClassDefaultObject();

        Object->ProcessEvent(Func, &Params);

        return Params.ReturnValue;
    }

    static FVector GetDirectionUnitVector(const struct FVector& From, const struct FVector& To) {
        static class UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("KismetMathLibrary", "GetDirectionUnitVector");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return FVector();

        struct KismetMathLibrary_GetDirectionUnitVector final
        {
        public:
            struct FVector                                From;                                              // 0x0000(0x000C)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            struct FVector                                To;                                                // 0x000C(0x000C)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            struct FVector                                ReturnValue;                                       // 0x0018(0x000C)(Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
        };
        KismetMathLibrary_GetDirectionUnitVector Params;
        Params.From = std::move(From);
        Params.To = std::move(To);

        static UObject* Object = nullptr;
        if (!Object) Object = SDK::StaticClassImpl("KismetMathLibrary")->GetClassDefaultObject();

        Object->ProcessEvent(Func, &Params);

        return Params.ReturnValue;
    }

    static FVector NegateVector(const struct FVector& A) {
        static class UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("KismetMathLibrary", "NegateVector");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return FVector();

        struct KismetMathLibrary_NegateVector final
        {
        public:
            struct FVector                                A;                                                 // 0x0000(0x000C)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            struct FVector                                ReturnValue;                                       // 0x000C(0x000C)(Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
        };
        KismetMathLibrary_NegateVector Params;
        Params.A = std::move(A);

        static UObject* Object = nullptr;
        if (!Object) Object = SDK::StaticClassImpl("KismetMathLibrary")->GetClassDefaultObject();

        Object->ProcessEvent(Func, &Params);

        return Params.ReturnValue;
    }
    
    static float RandomFloatInRange(int32 min_0, int32 max_0) {
        static class UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("KismetMathLibrary", "RandomFloatInRange");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return 0.f;

        struct KismetMathLibrary_RandomFloatInRange final
        {
        public:
            float                                         min_0; 
            float                                         max_0;                                             
            float                                         ReturnValue;                                       
        };
        KismetMathLibrary_RandomFloatInRange Params;
        Params.min_0 = min_0;
        Params.max_0 = max_0;

        static UObject* Object = nullptr;
        if (!Object) Object = SDK::StaticClassImpl("KismetMathLibrary")->GetClassDefaultObject();

        Object->ProcessEvent(Func, &Params);

        return Params.ReturnValue;
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
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("KismetTextLibrary", "Conv_StringToText");

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
        if (!Object) Object = SDK::StaticClassImpl("KismetTextLibrary")->GetClassDefaultObject();
        
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
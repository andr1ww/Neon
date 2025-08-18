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

    static FVector Normal(const struct FVector& A, float Tolerance) {
        static class UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("KismetMathLibrary", "Normal");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return FVector();

        struct KismetMathLibrary_GetDirectionUnitVector final
        {
        public:
            struct FVector                                A;                                              // 0x0000(0x000C)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            float                                Tolerance;                                                // 0x000C(0x000C)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            struct FVector                                ReturnValue;                                       // 0x0018(0x000C)(Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
        };
        KismetMathLibrary_GetDirectionUnitVector Params;
        Params.A = std::move(A);
        Params.Tolerance = std::move(Tolerance);

        static UObject* Object = nullptr;
        if (!Object) Object = SDK::StaticClassImpl("KismetMathLibrary")->GetClassDefaultObject();

        Object->ProcessEvent(Func, &Params);

        return Params.ReturnValue;
    }

    static float Vector_Distance(const struct FVector& From, const struct FVector& To) {
        static class UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("KismetMathLibrary", "Vector_Distance");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return 0;

        struct KismetMathLibrary_GetDirectionUnitVector final
        {
        public:
            struct FVector                                From;                                              // 0x0000(0x000C)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            struct FVector                                To;                                                // 0x000C(0x000C)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            float                                ReturnValue;                                       // 0x0018(0x000C)(Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
        };
        KismetMathLibrary_GetDirectionUnitVector Params;
        Params.From = std::move(From);
        Params.To = std::move(To);

        static UObject* Object = nullptr;
        if (!Object) Object = SDK::StaticClassImpl("KismetMathLibrary")->GetClassDefaultObject();

        Object->ProcessEvent(Func, &Params);

        return Params.ReturnValue;
    }

    static float RandomFloat() {
        static class UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("KismetMathLibrary", "RandomFloat");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return 0;

        struct KismetMathLibrary_GetDirectionUnitVector final
        {
        public:
            float                                ReturnValue;                                       // 0x0018(0x000C)(Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
        };
        KismetMathLibrary_GetDirectionUnitVector Params;

        static UObject* Object = nullptr;
        if (!Object) Object = SDK::StaticClassImpl("KismetMathLibrary")->GetClassDefaultObject();

        Object->ProcessEvent(Func, &Params);

        return Params.ReturnValue;
    }
    
    static float Vector_DistanceSquared(const struct FVector& From, const struct FVector& To) {
        static class UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("KismetMathLibrary", "Vector_DistanceSquared");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return 0;

        struct KismetMathLibrary_GetDirectionUnitVector final
        {
        public:
            struct FVector                                From;                                              // 0x0000(0x000C)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            struct FVector                                To;                                                // 0x000C(0x000C)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            float                                ReturnValue;                                       // 0x0018(0x000C)(Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
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

enum class EObjectTypeQuery : uint8
{
    ObjectTypeQuery1                         = 0,
    ObjectTypeQuery2                         = 1,
    ObjectTypeQuery3                         = 2,
    ObjectTypeQuery4                         = 3,
    ObjectTypeQuery5                         = 4,
    ObjectTypeQuery6                         = 5,
    ObjectTypeQuery7                         = 6,
    ObjectTypeQuery8                         = 7,
    ObjectTypeQuery9                         = 8,
    ObjectTypeQuery10                        = 9,
    ObjectTypeQuery11                        = 10,
    ObjectTypeQuery12                        = 11,
    ObjectTypeQuery13                        = 12,
    ObjectTypeQuery14                        = 13,
    ObjectTypeQuery15                        = 14,
    ObjectTypeQuery16                        = 15,
    ObjectTypeQuery17                        = 16,
    ObjectTypeQuery18                        = 17,
    ObjectTypeQuery19                        = 18,
    ObjectTypeQuery20                        = 19,
    ObjectTypeQuery21                        = 20,
    ObjectTypeQuery22                        = 21,
    ObjectTypeQuery23                        = 22,
    ObjectTypeQuery24                        = 23,
    ObjectTypeQuery25                        = 24,
    ObjectTypeQuery26                        = 25,
    ObjectTypeQuery27                        = 26,
    ObjectTypeQuery28                        = 27,
    ObjectTypeQuery29                        = 28,
    ObjectTypeQuery30                        = 29,
    ObjectTypeQuery31                        = 30,
    ObjectTypeQuery32                        = 31,
    ObjectTypeQuery_MAX                      = 32,
    EObjectTypeQuery_MAX                     = 33,
};

class UKismetSystemLibrary : public UObject
{
public:
    static bool SphereOverlapActors(class UObject* WorldContextObject, const struct FVector& SpherePos, float SphereRadius, const TArray<EObjectTypeQuery>& ObjectTypes, class UClass* ActorClassFilter, const TArray<class AActor*>& ActorsToIgnore, TArray<class AActor*>* OutActors)
    {
        static SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("KismetSystemLibrary", "SphereOverlapActors");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return false;

        struct KismetSystemLibrary_SphereOverlapActors final
        {
        public:
            class UObject*                                WorldContextObject;                                // 0x0000(0x0008)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            struct FVector                                SpherePos;                                         // 0x0008(0x000C)(ConstParm, Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            float                                         SphereRadius;                                      // 0x0014(0x0004)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            TArray<EObjectTypeQuery>                      ObjectTypes;                                       // 0x0018(0x0010)(ConstParm, Parm, OutParm, ZeroConstructor, ReferenceParm, NativeAccessSpecifierPublic)
            class UClass*                                 ActorClassFilter;                                  // 0x0028(0x0008)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            TArray<class AActor*>                         ActorsToIgnore;                                    // 0x0030(0x0010)(ConstParm, Parm, OutParm, ZeroConstructor, ReferenceParm, NativeAccessSpecifierPublic)
            TArray<class AActor*>                         OutActors;                                         // 0x0040(0x0010)(Parm, OutParm, ZeroConstructor, NativeAccessSpecifierPublic)
            bool                                          ReturnValue;                                       // 0x0050(0x0001)(Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            uint8                                         Pad_51[0x7];                                       // 0x0051(0x0007)(Fixing Struct Size After Last Property [ Dumper-7 ])
        } Params;
        
        Params.WorldContextObject = WorldContextObject;
        Params.SpherePos = std::move(SpherePos);
        Params.SphereRadius = SphereRadius;
        Params.ObjectTypes = std::move(ObjectTypes);
        Params.ActorClassFilter = ActorClassFilter;
        Params.ActorsToIgnore = std::move(ActorsToIgnore);

        static UObject* DefaultObject = nullptr;
        if (!DefaultObject) DefaultObject = SDK::StaticClassImpl("KismetSystemLibrary")->GetClassDefaultObject();
        DefaultObject->ProcessEvent(Func, &Params);

        return Params.ReturnValue;
    }
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
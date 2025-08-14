#pragma once
#include "pch.h"

#include "Engine/Array/Header/Array.h"
#include "Neon/Runtime/Runtime.h"

struct Pad10 { char Pad[0x10]; };
struct Pad18 { char Pad[0x18]; };
struct PadA8 { char Pad[0xA8]; };
struct PadB0 { char Pad[0xB0]; };

// 0x0018 (0x0018 - 0x0000)
struct alignas(0x08) FGameplayEffectContextHandle final
{
    public:
    uint8                                         Pad_0[0x18];                                       // 0x0000(0x0018)(Fixing Struct Size After Last Property [ Dumper-7 ])
};

struct alignas(0x08) FGameplayAbilityTargetDataHandle final
{
    public:
    uint8                                         Pad_0[0x28];                                       // 0x0000(0x0028)(Fixing Struct Size After Last Property [ Dumper-7 ])
};

struct FGameplayEventData final
{
public:
	struct FGameplayTag                           EventTag;                                          // 0x0000(0x0008)(Edit, BlueprintVisible, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	class AActor*                                 Instigator;                                        // 0x0008(0x0008)(Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	class AActor*                                 Target;                                            // 0x0010(0x0008)(Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	class UObject*                                OptionalObject;                                    // 0x0018(0x0008)(Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	class UObject*                                OptionalObject2;                                   // 0x0020(0x0008)(Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	 FGameplayEffectContextHandle           ContextHandle;                                     // 0x0028(0x0018)(Edit, BlueprintVisible, NativeAccessSpecifierPublic)
	struct FGameplayTagContainer                  InstigatorTags;                                    // 0x0040(0x0020)(Edit, BlueprintVisible, NativeAccessSpecifierPublic)
	struct FGameplayTagContainer                  TargetTags;                                        // 0x0060(0x0020)(Edit, BlueprintVisible, NativeAccessSpecifierPublic)
	float                                         EventMagnitude;                                    // 0x0080(0x0004)(Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	uint8                                         Pad_84[0x4];                                       // 0x0084(0x0004)(Fixing Size After Last Property [ Dumper-7 ])
	struct FGameplayAbilityTargetDataHandle       TargetData;                                        // 0x0088(0x0028)(Edit, BlueprintVisible, NativeAccessSpecifierPublic)
};

class UGameplayAbility : public UObject
{
public:
    DECLARE_STATIC_CLASS(UGameplayAbility)
    DECLARE_DEFAULT_OBJECT(UGameplayAbility)
};

struct FPredictionKey final
{
public:
    uint8_t Pad[0x10]; 
};

struct FGameplayAbilitySpecHandle
{
   int Handle;
};

class IInterface : public UObject
{
public:
    DECLARE_STATIC_CLASS(IInterface)
    DECLARE_DEFAULT_OBJECT(IInterface)
};

class IAbilitySystemInterface : public IInterface
{
public:
    DECLARE_STATIC_CLASS(IAbilitySystemInterface)
    DECLARE_DEFAULT_OBJECT(IAbilitySystemInterface)
};

struct FGameplayAbilitySpec final : public FFastArraySerializerItem
{
    DEFINE_MEMBER(FGameplayAbilitySpecHandle, FGameplayAbilitySpec, Handle);
    DEFINE_BOOL(FGameplayAbilitySpec, InputPressed);
    DEFINE_PTR(UGameplayAbility, FGameplayAbilitySpec, Ability);
    DEFINE_PTR(UObject, FGameplayAbilitySpec, SourceObject);
    DEFINE_MEMBER(int32, FGameplayAbilitySpec, Level);
    DEFINE_MEMBER(int32, FGameplayAbilitySpec, InputID);
};

struct FGameplayAbilitySpecContainer : public FFastArraySerializer
{
    DEFINE_MEMBER(TArray<FGameplayAbilitySpec>, FGameplayAbilitySpecContainer, Items);
};

struct FGameplayEffectApplicationInfo final
{
public:
    DEFINE_PTR(UClass, FGameplayEffectApplicationInfo, GameplayEffect);
    float                                         Level;                                             // 0x0020(0x0004)(Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    uint8                                         Pad_24[0x4];                                       // 0x0024(0x0004)(Fixing Struct Size After Last Property [ Dumper-7 ])
};

class UFortAbilitySet : public UObject
{
public:
    DEFINE_MEMBER(TArray<TSubclassOf<UClass>>, UFortAbilitySet, GameplayAbilities);
    DEFINE_MEMBER(TArray<FGameplayEffectApplicationInfo>, UFortAbilitySet, PassiveGameplayEffects);
};

class UAbilitySystemComponent : public UObject
{
public:
    void BP_ApplyGameplayEffectToSelf(TSubclassOf<class UGameplayEffect> GameplayEffectClass, float Level, const struct FGameplayEffectContextHandle& EffectContext)
    {
        static SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("AbilitySystemComponent", "BP_ApplyGameplayEffectToSelf");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return;

        struct AbilitySystemComponent_BP_ApplyGameplayEffectToSelf final
        {
        public:
            TSubclassOf<class UGameplayEffect>            GameplayEffectClass;                               // 0x0000(0x0008)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            float                                         Level;                                             // 0x0008(0x0004)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            uint8                                         Pad_3CA2[0x4];                                     // 0x000C(0x0004)(Fixing Size After Last Property [ Dumper-7 ])
            struct FGameplayEffectContextHandle           EffectContext;                                     // 0x0010(0x0018)(Parm, NativeAccessSpecifierPublic)
            void*            ReturnValue;                                       // 0x0028(0x0008)(Parm, OutParm, ReturnParm, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
        };
        
        AbilitySystemComponent_BP_ApplyGameplayEffectToSelf Params{};

        Params.GameplayEffectClass = GameplayEffectClass;
        Params.Level = Level;
        Params.EffectContext = EffectContext;
    
        this->ProcessEvent(Func, &Params);
    }
public:
    DEFINE_MEMBER(FGameplayAbilitySpecContainer, UAbilitySystemComponent, ActivatableAbilities);
public:
    static void InternalServerTryActivateAbility(UAbilitySystemComponent*, FGameplayAbilitySpecHandle, bool, void*, FGameplayEventData*);
    static void GiveAbility(UAbilitySystemComponent* AbilitySystemComponent, UClass* Ability);
    static void GiveAbilitySet(UAbilitySystemComponent* AbilitySystemComponent, UFortAbilitySet* Set);
public:
    DECLARE_STATIC_CLASS(UAbilitySystemComponent);
    DECLARE_DEFAULT_OBJECT(UAbilitySystemComponent);
};

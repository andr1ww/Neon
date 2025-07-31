#pragma once
#include "pch.h"

#include "Engine/Array/Header/Array.h"
#include "Neon/Runtime/Runtime.h"

struct Pad10 { char Pad[0x10]; };
struct Pad18 { char Pad[0x18]; };
struct PadA8 { char Pad[0xA8]; };
struct PadB0 { char Pad[0xB0]; };

using FGameplayEventData = PadB0; 

class UGameplayAbility : public UObject
{
    
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

struct FGameplayEffectContextHandle
{
    unsigned char                                      UnknownData00[0x18];                                      
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
    static void InternalServerTryActivateAbility(UAbilitySystemComponent*, FGameplayAbilitySpecHandle, bool, FPredictionKey&, FGameplayEventData*);
    static void GiveAbility(UAbilitySystemComponent* AbilitySystemComponent, UClass* Ability);
    static void GiveAbilitySet(UAbilitySystemComponent* AbilitySystemComponent, UFortAbilitySet* Set);
public:
    DECLARE_STATIC_CLASS(UAbilitySystemComponent);
    DECLARE_DEFAULT_OBJECT(UAbilitySystemComponent);
};

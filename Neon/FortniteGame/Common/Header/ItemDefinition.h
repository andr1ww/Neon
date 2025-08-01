#pragma once
#include "pch.h"
#include "Neon/Runtime/Runtime.h"

enum class EFortResourceType : uint8
{
    Wood                                     = 0,
    Stone                                    = 1,
    Metal                                    = 2,
    Permanite                                = 3,
    GoldCurrency                             = 4,
    Ingredient                               = 5,
    None                                     = 6,
    EFortResourceType_MAX                    = 7,
};

struct FGameplayTag
{
public:
    class FName                                   TagName;                                           // 0x0000(0x0004)(Edit, ZeroConstructor, EditConst, SaveGame, IsPlainOldData, NoDestructor, Protected, HasGetValueTypeHash, NativeAccessSpecifierProtected)
};

class UFortItemDefinition : public UObject
{
public:
    class UFortWorldItem* CreateTemporaryItemInstanceBP(int32 Count, int32 Level)
    {
        static UFunction* Func = Runtime::StaticFindObject<UFunction>("/Script/FortniteGame.FortItemDefinition.CreateTemporaryItemInstanceBP");

        struct FortItemDefinition_CreateTemporaryItemInstanceBP final
        {
        public:
            int32                                         Count;                                             // 0x0000(0x0004)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            int32                                         Level;                                             // 0x0004(0x0004)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            UFortWorldItem* ReturnValue;                                       // 0x0008(0x0008)(Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
        };

        FortItemDefinition_CreateTemporaryItemInstanceBP Parms{};
        Parms.Count = Count;
        Parms.Level = Level;

        auto Flgs = Func->FunctionFlags();
        Func->FunctionFlags() |= 0x400;

        this->ProcessEvent(Func, &Parms);

        Func->FunctionFlags() = Flgs;

        return Parms.ReturnValue;
    }
};

struct FDataTableCategoryHandle final
{
public:
    class UDataTable*                             DataTable;                                         // 0x0000(0x0008)(Edit, BlueprintVisible, ZeroConstructor, NoDestructor, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    class FName                                   ColumnName;                                        // 0x0008(0x0004)(Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    class FName                                   RowContents;                                       // 0x000C(0x0004)(Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
};

class UFortWorldItemDefinition : public UFortItemDefinition
{
public:
    DEFINE_MEMBER(FDataTableCategoryHandle, UFortWorldItemDefinition, LootLevelData);
    DEFINE_MEMBER(int32, UFortWorldItemDefinition, MaxLevel);
    DEFINE_MEMBER(int32, UFortWorldItemDefinition, MinLevel);
    DEFINE_BOOL(UFortWorldItemDefinition, bCanBeDropped)
    DEFINE_BOOL(UFortWorldItemDefinition, bForceIntoOverflow)
    DEFINE_MEMBER(FScalableFloat, UFortWorldItemDefinition, MaxStackSize)
public:
    DECLARE_STATIC_CLASS(UFortWorldItemDefinition)
    DECLARE_DEFAULT_OBJECT(UFortWorldItemDefinition)
};
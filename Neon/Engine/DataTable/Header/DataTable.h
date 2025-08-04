#pragma once
#include "pch.h"

#include "Neon/Runtime/Runtime.h"


class UCurveTable : public UObject
{
public:
};

struct FDataTableRowHandle final
{
public:
    class UDataTable* DataTable; // 0x0000(0x0008)(Edit, BlueprintVisible, ZeroConstructor, NoDestructor, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    DEFINE_MEMBER(FName, FDataTableRowHandle, RowName); // 0x0008(0x0004)(Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    uint8                                         Pad_C[0x4];                                        // 0x000C(0x0004)(Fixing Struct Size After Last Property [ Dumper-7 ])
};

struct FCurveTableRowHandle
{
    UCurveTable* CurveTable;
    FName RowName; 
};

enum class EEvaluateCurveTableResult : uint8_t
{
    RowFound = 0,
    RowNotFound = 1,
    EEvaluateCurveTableResult_MAX = 2
};

class UDataAsset : public UObject {
public:
    DECLARE_STATIC_CLASS(UDataAsset);
    DECLARE_DEFAULT_OBJECT(UDataAsset);
};

class UDataTableFunctionLibrary : public UObject
{
public:
    static float EvaluateCurveTableRow(UCurveTable* CurveTable, FName RowName, float InXY,
        const FString& ContextString = FString(), EEvaluateCurveTableResult* OutResult = nullptr)
    {
        static SDK::UFunction* Func = nullptr;
        SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("DataTableFunctionLibrary", "EvaluateCurveTableRow");

        if (Func == nullptr)
            Func = Info.Func;
        if (!Func)
            return 0.0f;

        float Ok{};
        EEvaluateCurveTableResult Erm{};

        struct { UCurveTable* CurveTable; FName RowName; float InXY; EEvaluateCurveTableResult OutResult; float OutXY; FString ContextString; }
        Params{CurveTable, RowName, InXY, Erm, Ok, ContextString}; // ts param stolen from rbot

        static UObject* Object = nullptr;
        if (!Object) Object = SDK::StaticClassImpl("DataTableFunctionLibrary")->GetClassDefaultObject();

        Object->ProcessEvent(Func, &Params);
        
        if (OutResult)
            *OutResult = Params.OutResult;

        return Params.OutXY;
    }
};

class UDataTable : public UObject {
public:
    template <typename RowDataType = uint8_t>
    TMap<FName, RowDataType*>& GetRowMap()
    {
        static TMap<FName, RowDataType*> EmptyMap; 
        
    
        void* TargetAddress = reinterpret_cast<void*>(__int64(this) + 0x30);
        if (IsBadReadPtr(TargetAddress, sizeof(TMap<FName, RowDataType*>))) {
            UE_LOG(LogNeon, Error, TEXT("Cannot read DataTable RowMap at address: 0x%p"), TargetAddress);
            return EmptyMap;
        }
    
        return *(TMap<FName, RowDataType*>*)(__int64(this) + 0x30);
    }
    
    DECLARE_STATIC_CLASS(UDataTable)
    DECLARE_DEFAULT_OBJECT(UDataTable)
};

class UCompositeDataTable : public UDataTable
{
public:
    DEFINE_MEMBER(TArray<UDataTable*>, UCompositeDataTable, ParentTables);
    DEFINE_MEMBER(TArray<UDataTable*>, UCompositeDataTable, OldParentTables);
public:
    DECLARE_STATIC_CLASS(UCompositeDataTable);
    DECLARE_DEFAULT_OBJECT(UCompositeDataTable)
};
#pragma once
#include "pch.h"


class UCurveTable : public UObject
{
public:
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

        SDK::StaticClassImpl("DataTableFunctionLibrary")->GetClassDefaultObject()->ProcessEvent(Func, &Params);
        
        if (OutResult)
            *OutResult = Params.OutResult;

        return Params.OutXY;
    }
};

class UDataTable : public UObject {
public:
    typedef TMap<FName, uint8*> RowMapType;
    DEFINE_MEMBER(RowMapType, UDataTable, RowMap);
    
    DECLARE_STATIC_CLASS(UDataTable)
    DECLARE_DEFAULT_OBJECT(UDataTable)
};

class UCompositeDataTable : public UDataTable
{
public:
    DECLARE_STATIC_CLASS(UCompositeDataTable);
    DECLARE_DEFAULT_OBJECT(UCompositeDataTable)
};
#pragma once
#include "pch.h"

#include "Engine/Actor/Header/Actor.h"

struct FAircraftFlightInfo final
{
public:
    struct FVector_NetQuantize100                 FlightStartLocation;                               // 0x0000(0x000C)(Edit, BlueprintVisible, BlueprintReadOnly, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    struct FRotator                               FlightStartRotation;                               // 0x000C(0x000C)(Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, NativeAccessSpecifierPublic)
    float                                         FlightSpeed;                                       // 0x0018(0x0004)(Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    float                                         TimeTillFlightEnd;                                 // 0x001C(0x0004)(Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    float                                         TimeTillDropStart;                                 // 0x0020(0x0004)(Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
    float                                         TimeTillDropEnd;                                   // 0x0024(0x0004)(Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
};
class AFortAthenaMapInfo : public AActor
{
public:
    DEFINE_MEMBER(FScalableFloat, AFortAthenaMapInfo, AircraftDesiredDoorOpenTime);
    DEFINE_MEMBER(TArray<struct FAircraftFlightInfo>, AFortAthenaMapInfo, FlightInfos);
public:
    DECLARE_STATIC_CLASS(AFortAthenaMapInfo);
    DECLARE_DEFAULT_OBJECT(AFortAthenaMapInfo)
};

#ifndef PCH_H
#define PCH_H

#define WIN32_LEAN_AND_MEAN            
#include <windows.h>
#include <iostream>
#include <vector>
#include <memory>
#include <fstream>
#include <cstring>
#include <string>
#include <thread>
#include <random>
#include <MinHook.h>
using namespace std;
typedef uint64_t uint64;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t uint8;
typedef int64_t int64;
typedef int32_t int32;
typedef int16_t int16;
typedef int8_t int8;
#include "SDK/SDK.h"
DEFINE_LOG_CATEGORY(LogNeon);

#define IMAGEBASE uint64_t(GetModuleHandle(0))

static int RetTrue()
{
    return 1;
}

static int RetFalse()
{
    return 0;
}

static void Return()
{
    return;
}

static void ExecuteConsoleCommand(SDK::UObject* WorldContextObject, const SDK::FString& Command, SDK::UObject* Controller)
{
    static SDK::UFunction* Func = nullptr;
    SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("KismetSystemLibrary", "ExecuteConsoleCommand");

    if (Func == nullptr)
        Func = Info.Func;
    if (!Func)
        return;

    struct ExecuteConsoleCommand
    {
    public:
        SDK::UObject* WorldContextObject;
        SDK::FString Command;
        SDK::UObject* PlayerController;
    };

    ExecuteConsoleCommand Params{};

    Params.WorldContextObject = WorldContextObject;
    Params.Command = std::move(Command);
    Params.PlayerController = Controller;

    SDK::StaticClassImpl("KismetSystemLibrary")->GetClassDefaultObject()->ProcessEvent(Func, &Params);
}


#endif //PCH_H

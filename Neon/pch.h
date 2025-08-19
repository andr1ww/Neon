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
#include "SDK/include/SDK.h"
#pragma comment(lib, "SDK/FortSDK.lib")
using namespace SDK;
using namespace std;
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

    static UObject* Object = nullptr;
    if (!Object) Object = SDK::StaticClassImpl("KismetSystemLibrary")->GetClassDefaultObject();

    Object->ProcessEvent(Func, &Params);
}

#define DefHookOg(_Rt, _Name, ...) static inline _Rt (*_Name##OG)(##__VA_ARGS__); static _Rt _Name(##__VA_ARGS__);

#endif //PCH_H

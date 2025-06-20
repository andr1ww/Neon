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
#include "SDK/include/SDK.h"
#pragma comment(lib, "SDK/FortSDK.lib")
using namespace SDK;
DEFINE_LOG_CATEGORY(LogNeon);

#define IMAGEBASE uint64_t(GetModuleHandle(0))

#endif //PCH_H

#include "pch.h"
#include "../Header/Finder.h"

#include "Engine/NetDriver/Header/NetDriver.h"

uint64 UFinder::StartAircraftPhase()
{
    if (Engine_Version < 427) 
    {
        auto strRef = Memcury::Scanner::FindStringRef(L"STARTAIRCRAFT").Get();

        if (!strRef)
            return 0;

        int NumCalls = 0;

        for (int i = 0; i < 150; i++)
        {
            if (*(uint8_t*)(strRef + i) == 0xE8)
            {
                NumCalls++;

                if (NumCalls == 2) 
                {
                    return Memcury::Scanner(strRef + i).RelativeOffset(1).Get();
                }
            }
        }
    }
    else
    {
        auto StatAddress = Memcury::Scanner::FindStringRef(L"STAT_StartAircraftPhase").Get();

        for (int i = 0; i < 1000; i++)
        {
            if (*(uint8_t*)(uint8_t*)(StatAddress - i) == 0x48 && *(uint8_t*)(uint8_t*)(StatAddress - i + 1) == 0x8B && *(uint8_t*)(uint8_t*)(StatAddress - i + 2) == 0xC4)
            {
                return StatAddress - i;
            }
        }
    }

    return 0;
}

uint64_t UFinder::WorldNetMode()
{
    if (Fortnite_Version == 23.50)
    {
        return Memcury::Scanner::FindPattern("48 83 EC ? 48 83 79 ? ? 74 ? B8").Get();
    }
    auto Addr = Memcury::Scanner::FindStringRef(L"PREPHYSBONES");

    auto BeginningFunction = FindBytes(Addr, { 0x40, 0x55 }, 1000, 0, true);

    uint64 CallToFunc = 0;

    for (int i = 0; i < 400; i++)
    {
        if ((*(uint8_t*)(uint8_t*)(BeginningFunction + i) == 0xE8) && (*(uint8_t*)(uint8_t*)(BeginningFunction + i - 1) != 0x8B)) // scuffed but idk how to guarantee its not a register
        {
            CallToFunc = BeginningFunction + i;
            break;
        }
    }

    if (!CallToFunc)
    {
        return 0;
    }
        
    return Memcury::Scanner(CallToFunc).RelativeOffset(1).Get();
}

uint64 UFinder::GIsClient()
{
    Memcury::Scanner Scanner = Memcury::Scanner::FindPattern("38 05 ? ? ? ? 0F 95 C0 FF C0");
    if (Scanner.Get() == 0)
    {
        Scanner = Memcury::Scanner::FindPattern("88 1D ? ? ? ? 45 8A EC", true);
    }
    return Scanner.RelativeOffset(2).Get();
}

uint64 UFinder::StaticFindObject()
{
    static uint64 CachedResult = 0;
    if (CachedResult != 0)
        return CachedResult;
    
    if (Engine_Version == 500)
    {
        auto addr = Memcury::Scanner::FindPattern("40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 45 33 F6 4C 8B E1 45 0F B6 E9 49 8B F8 41 8B C6", false).Get();

        if (!addr)
            addr = Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 4C 89 64 24 ? 55 41 55 41 57 48 8B EC 48 83 EC 60 45 8A E1 4C 8B E9 48 83 FA").Get(); // 20.00

        return CachedResult = addr;
    }

    if (Engine_Version >= 427) 
    {
        if (Fortnite_Version.GetMajorVersion() < 18)
        {
            if (Fortnite_Version == 16.50)
                 return CachedResult = Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 55 41 54 41 55 41 56 41 57 48 8B EC 48 83 EC 60 45 33 ED 45 8A F9 44 38 2D ? ? ? ? 49 8B F8 48 8B F2 4C 8B E1").Get();
		
             return CachedResult = Memcury::Scanner::FindPattern("40 55 53 57 41 54 41 55 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85").Get();
        }
        else
             return CachedResult = Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 55 41 54 41 55 41 56 41 57 48 8B EC 48 83 EC 60 45 33 ED 45 8A F9 44 38 2D ? ? ? ? 49 8B F8 48 8B").Get();
    }

    if (Engine_Version == 4.16)
         return CachedResult = Memcury::Scanner::FindPattern("4C 8B DC 57 48 81 EC ? ? ? ? 80 3D ? ? ? ? ? 49 89 6B F0 49 89 73 E8").Get();

    if (Engine_Version == 4.19)
    {
        auto iasdfk = Memcury::Scanner::FindPattern("4C 8B DC 49 89 5B 08 49 89 6B 18 49 89 73 20 57 41 56 41 57 48 83 EC 60 80 3D", false).Get();

        if (!iasdfk)
             CachedResult = Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 55 57 41 54 41 56 41 57 48 8B EC 48 83 EC 60 80 3D ? ? ? ? ? 45 0F B6 F1 49 8B F8").Get();

         return CachedResult = iasdfk;
    }
    
    auto Addr = Memcury::Scanner::FindStringRef(L"Illegal call to StaticFindObject() while serializing object data!", true, 1, Engine_Version >= 427);
    auto Final = FindBytes(Addr, { 0x48, 0x89, 0x5C }, 255, 0, true, 0, false); 

    if (Final == 0)
    {
        Final = Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 55 57 41 56 48 8B EC 48 83 EC ? 33 DB 4C 8B F1").Get();
    }
    
    return CachedResult = Final;
}


uint64 UFinder::TickFlush()
{
    if (Engine_Version == 4.16)
        return Memcury::Scanner::FindPattern("4C 8B DC 55 53 56 57 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 41 0F 29 7B").Get(); // 2.4.2

    if (Engine_Version == 4.19)
        return Memcury::Scanner::FindPattern("4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 45 0F 29 43 ? 45 0F 29 4B ? 48 8B 05 ? ? ? ? 48").Get(); // 2.4.2

    if (Engine_Version == 4.27)
    {
        auto addr = Memcury::Scanner::FindPattern("48 8B C4 48 89 58 18 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 B8 0F 29 78 A8 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 8A", false).Get();

        if (!addr) // s18
            addr = Memcury::Scanner::FindPattern("48 8B C4 48 89 58 18 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 B8 0F 29 78 A8 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 44 0F", false).Get();
	
        if (!addr)
            addr = Memcury::Scanner::FindPattern("48 8B C4 48 89 58 18 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 B8 0F 29 78 A8 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B F9 48 89 4D 38 48 8D 4D 40").Get(); // 16.50

        return addr;
    }

    auto Addr = Memcury::Scanner::FindStringRef(L"STAT_NetTickFlush", false);

    if (!Addr.Get())
    {
        if (Engine_Version == 4.20) // 2.5
        {
            return Memcury::Scanner::FindPattern("4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 45 0F 29 43 ? 45 0F 29 4B ? 48 8B 05 ? ? ? ? 48 33").Get();
        }
    }

    return FindBytes(Addr, (Fortnite_Version < 18 ? std::vector<uint8_t>{ 0x4C, 0x8B } : std::vector<uint8_t>{ 0x48, 0x8B, 0xC4 }), 1000, 0, true);
}

uint64 UFinder::InitHost()
{
    if (SDK::Fortnite_Version >= 16.40) {
        auto addr = Memcury::Scanner::FindPattern(
            "48 89 5C 24 ? 48 89 74 24 ? 55 57 41 56 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B F1 4C 8D 05",
            false
        ).Get();

        if (!addr) {
            addr = Memcury::Scanner::FindPattern(
                "48 8B C4 48 89 58 10 48 89 70 18 48 89 78 20 55 41 56 41 57 48 8D 68 A1 48 81 EC ? ? ? ? 48 8B F1 4C 8D 35 ? ? ? ? 4D"
            ).Get();
        }

        if (SDK::Fortnite_Version == 18.10) {
            addr = Memcury::Scanner::FindPattern(
                "48 89 5C 24 ? 48 89 74 24 ? 55 57 41 56 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B F1 48 8D 1D ? ? ? ? 4C 8B C3 48 8D 4D D7 45"
            ).Get();
        }
        return addr;
    }

    auto Addr = Memcury::Scanner::FindStringRef(L"BeaconPort=");
    return Addr.ScanFor({ 0x48, 0x8B, 0xC4 }, false, 0, 0, 1000).Get();
}

uint64 UFinder::PauseBeaconRequests()
{
    if (SDK::Fortnite_Version >= 19.00) {
        return Memcury::Scanner::FindPattern(
            "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC 20 33 ED 48 8B F1 84 D2 74 27 80 3D"
        ).Get();
    }
    else if (SDK::Fortnite_Version >= 16.40) {
        return Memcury::Scanner::FindPattern(
            "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 30 33 F6 48 8B F9 84 D2 74"
        ).Get();
    }
    else if (SDK::Fortnite_Version >= 13) {
        auto Addr = Memcury::Scanner::FindPattern(
            "40 57 48 83 EC 30 48 8B F9 84 D2 74 62 80 3D"
        ).Get();
        if (!Addr) {
            Addr = Memcury::Scanner::FindPattern(
                "40 53 48 83 EC 30 48 8B ? 84 D2 74 ? 80 3D"
            ).Get();
        }
        return Addr;
    }
    else if (SDK::Fortnite_Version >= 2.5 && SDK::Fortnite_Version <= 4.5) {
        return Memcury::Scanner::FindPattern(
            "40 53 48 83 EC 30 48 8B D9 84 D2 74 68 80 3D ? ? ? ? ? 72 2C 48 8B 05 ? ? ? ? 4C 8D 44"
        ).Get();
    }
    else if (SDK::Fortnite_Version == 6.30 || SDK::Fortnite_Version == 6.31) {
        return Memcury::Scanner::FindPattern(
            "40 53 48 83 EC 30 48 8B D9 84 D2 74 68 80 3D"
        ).Get();
    }
    else if (SDK::Fortnite_Version == 0) {
        auto Addr = Memcury::Scanner::FindPattern(
            "40 53 48 83 EC 30 48 8B D9 84 D2 74 6F 80 3D ? ? ? ? ? 72 33 48 8B 05"
        ).Get();
        if (!Addr) {
            Addr = Memcury::Scanner::FindPattern(
                "40 53 48 83 EC 30 48 8B D9 84 D2 74 6F 80 3D",
                false
            ).Get();
        }
        if (!Addr) {
            Addr = Memcury::Scanner::FindPattern(
                "40 53 48 83 EC 30 48 8B D9 84 D2 74 68 80 3D ? ? ? ? ? 72"
            ).Get();
        }
        return Addr;
    }

    auto Addr = Memcury::Scanner::FindStringRef(L"All Beacon Requests Resumed.");
    return Addr.ScanFor({ 0x40, 0x53 }, false, 0, 1, 1000).Get();
}

uint64 UFinder::SetWorld()
{
    if (Fortnite_Version <= 13.00)
    {
        return Memcury::Scanner::FindStringRef(L"AOnlineBeaconHost::InitHost failed")
            .ScanFor({ 0x48, 0x8B, 0xD0, 0xE8 }, false)
            .RelativeOffset(4)
            .Get();
    }
    
    if (Fortnite_Version >= 23.00)
    {
        return Memcury::Scanner::FindPattern("E8 ? ? ? ? 48 8D 8B ? ? ? ? 48 8B D7 E8 ? ? ? ? 48 85 FF 0F 84 ? ? ? ? 83 64 24").RelativeOffset(1).Get();
    }

    int SetWorldI = 0;

    int VTIndex = 0;
    
    int Season = (int) std::floor(std::stod(Fortnite_Version.ToString()));
    if (Season == 13) VTIndex = 0x70;
    else if (Season == 14 || Fortnite_Version <= 15.2) VTIndex = 0x71;
    else if (Fortnite_Version >= 15.3 && Season < 18) VTIndex = 0x72;
    else if (Season == 18) VTIndex = 0x73;
    else if (Season >= 19 && Season < 21) VTIndex = 0x7A;
    if (Season == 20) VTIndex = 0x7B;
    if (Season >= 21) VTIndex = 0x7C;

    if (SetWorldI != 0)
    {
        return uint64(UNetDriver::GetDefaultObj()->GetVTable()[VTIndex]);
    } 
    
    if (Fortnite_Version.GetMajorVersion() == 14)
    {
        return uint64(UNetDriver::GetDefaultObj()->GetVTable()[0x5E]);
    }
    if (Fortnite_Version.GetMajorVersion() == 18)
    { 
        return uint64(UNetDriver::GetDefaultObj()->GetVTable()[0x73]);
    }
    
    if (Engine_Version >= 4.26)
        return Memcury::Scanner::FindPattern("40 55 56 41 56 48 8B EC 48 83 EC ? 48 89 5C 24", true).Get();
    
    return Memcury::Scanner::FindStringRef(L"AOnlineBeaconHost::InitHost failed", true).ScanFor({ 0xE8 }, false, 1).RelativeOffset(1).Get();
}

uint64 UFinder::GetMaxTickRate()
{
    if (SDK::Fortnite_Version >= 19.00) {
        return Memcury::Scanner::FindPattern(
            "40 53 48 83 EC 50 0F 29 74 24 ? 48 8B D9 0F 29 7C 24 ? 0F 28 F9 44 0F 29"
        ).Get();
    }

    if (SDK::Fortnite_Version >= 16.40) {
        return Memcury::Scanner::FindPattern(
            "40 53 48 83 EC 60 0F 29 74 24 ? 48 8B D9 0F 29 7C 24 ? 0F 28"
        ).Get();
    }

    auto sRef = Memcury::Scanner::FindStringRef(L"Hitching by request!").Get();

    if (!sRef) return 0;

    for (int i = 0; i < 400; i++) {
        if (*(uint8_t*)(uint8_t*)(sRef - i) == 0x40 && *(uint8_t*)(uint8_t*)(sRef - i + 1) == 0x53) {
            return sRef - i;
        }

        if (*(uint8_t*)(uint8_t*)(sRef - i) == 0x48 && *(uint8_t*)(uint8_t*)(sRef - i + 1) == 0x89 &&
            *(uint8_t*)(uint8_t*)(sRef - i + 2) == 0x5C) {
            return sRef - i;
        }
    }
    return 0;
}

uint64 UFinder::DispatchRequest()
{
    static uint64 CachedResult = 0;
    if (CachedResult != 0)
        return CachedResult;

    if (Fortnite_Version >= 23.00)
    {
        return CachedResult = Memcury::Scanner::FindPattern("48 8B C4 48 89 58 ? 48 89 70 ? 48 89 78 ? 55 41 56 41 57 48 8D 68 ? 48 81 EC ? ? ? ? 80 3D ? ? ? ? ? 48 8D 3D").Get();
    }
    
    auto Scanner = Memcury::Scanner::FindStringRef(L"MCP-Profile: Dispatching request to %s");
    if (Scanner.Get())
    {
         CachedResult = Scanner.ScanFor({ 0x48,0x89,0x5C }, false).Get();
    } 
    
    return CachedResult;
}  

uint64 UFinder::CreateNetDriver()
{ 
    auto CreateNetDriver = Memcury::Scanner::FindPattern("49 8B D8 48 8B F9 E8 ?? ?? ?? ?? 48 8B D0 4C 8B C3 48 8B CF 48 8B 5C 24 ?? 48 83 C4 ?? 5F E9 ?? ?? ?? ??").Get();
    if (!CreateNetDriver) {
        CreateNetDriver = Memcury::Scanner::FindPattern("48 8B D9 E8 ?? ?? ?? ?? 4C 8B 44 24 ?? 48 8B D0 48 8B CB E8 ?? ?? ?? ?? 48 83 C4 ?? 5B C3").Get();
        if (!CreateNetDriver) CreateNetDriver = Memcury::Scanner::FindPattern("33 D2 E8 ?? ?? ?? ?? 48 8B D0 4C 8B C3 48 8B CF E8 ?? ?? ?? ?? 48 8B 5C 24 ?? 48 83 C4 ?? 5F C3").Get();
    }
    for (int i = 0; i < 50; i++) {
        auto Ptr = (uint8_t*)(CreateNetDriver - i);

        if (*Ptr == 0x48 && *(Ptr + 1) == 0x89 && *(Ptr + 2) == 0x5c) {
            CreateNetDriver = uint64_t(Ptr);
            return CreateNetDriver;
        }
        else if (*Ptr == 0x4C && *(Ptr + 1) == 0x89 && *(Ptr + 2) == 0x44) {
            CreateNetDriver = uint64_t(Ptr);
            return CreateNetDriver;
        }
    }
    
    return 0;
}

uint64 UFinder::ProcessEvent()
{
    if (SDK::Fortnite_Version < 14) {
        auto Addr = Memcury::Scanner::FindStringRef(L"AccessNoneNoContext");
        return FindBytes(Addr, { 0x40, 0x55 }, 2000);
    }

    auto Addr = Memcury::Scanner::FindStringRef(
        L"UMeshNetworkComponent::ProcessEvent: Invalid mesh network node type: %s",
        true,
        0,
        SDK::Fortnite_Version >= 19.00
    );
    return Memcury::Scanner(
        FindBytes(Addr, { 0xE8 }, 2000, 0, false, SDK::Fortnite_Version < 19.00 ? 1 : 3)
    ).RelativeOffset(1).Get();
}

uint64 UFinder::ApplyCharacterCustomization()
{
    static uint64 CachedResult = 0;
    if (CachedResult != 0)
        return CachedResult;
    
    Memcury::Scanner Scanner = Memcury::Scanner::FindPattern("48 8B C4 48 89 50 ? 55 57 48 8D 68 ? 48 81 EC ? ? ? ? 80 B9");
    CachedResult = Scanner.Get();

    if (CachedResult == 0)
    {
        Scanner = Memcury::Scanner::FindPattern("E8 ? ? ? ? 44 0F B6 E0 EB ? 33 F6");

        if (!Scanner.Get())
        {
            Scanner = Memcury::Scanner::FindPattern("E8 ? ? ? ? E9 ? ? ? ? FF 90 ? ? ? ? 49 8B CE");
        }

        CachedResult = Scanner.RelativeOffset(1).Get();
    }

    return CachedResult;
}

uint64 UFinder::KickPlayer()
{
    if (Fortnite_Version >= 23.00)
        return Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 49 8B F0 48 8B DA 48 85 D2 74 ? 48 83 BA").Get();
    if (Engine_Version == 4.16) // <1.8
        return Memcury::Scanner::FindPattern("40 53 56 48 81 EC ? ? ? ? 48 8B DA 48 8B F1 E8 ? ? ? ? 48 8B 06 48 8B CE").Get(); // postlogin
    if (Fortnite_Version.GetMajorVersion() == 18)
        return Memcury::Scanner::FindPattern("48 8B C4 48 89 58 08 48 89 70 10 48 89 78 18 4C 89 60 20 55 41 56 41 57 48 8B EC 48 83 EC 60 48 83 65 ? ? 4C 8B F2 83 65 E8 00 4C 8B E1 83 65 EC").Get();
    if (Fortnite_Version.GetMajorVersion() == 19)
        return Memcury::Scanner::FindPattern("48 89 5C 24 ? 55 56 57 48 8B EC 48 83 EC 60 48 8B FA 48 8B F1 E8").Get();
    if (Engine_Version >= 4.23 || Engine_Version <= 4.25) // && instead of || ??
        return Memcury::Scanner::FindPattern("48 89 5C 24 08 48 89 74 24 10 57 48 83 EC ? 49 8B F0 48 8B DA 48 85 D2").Get();

    uint64 Ret = 0;

    auto Addr = Memcury::Scanner::FindStringRef(L"Validation Failure: %s. kicking %s", false, 0, Fortnite_Version >= 19);

    if (Addr.Get())
    {
        Ret = Addr.Get() ? FindBytes(Addr, { 0x40, 0x55 }, 1000, 0, true) : Ret;

        if (!Ret)
            Ret = Addr.Get() ? FindBytes(Addr, { 0x40, 0x53 }, 2000, 0, true) : Ret;
    }

    if (Ret)
        return Ret;

    auto Addr2 = Memcury::Scanner::FindStringRef(L"Failed to kick player"); // L"KickPlayer %s Reason %s"
    auto Addrr = Addr2.Get();

    for (int i = 0; i < 3000; i++)
    {
        if (*(uint8_t*)(uint8_t*)(Addrr - i) == 0x48 && *(uint8_t*)(uint8_t*)(Addrr - i + 1) == 0x89 && *(uint8_t*)(uint8_t*)(Addrr - i + 2) == 0x5C)
        {
            return Addrr - i;
        }

        if (Fortnite_Version >= 17)
        {
            if (*(uint8_t*)(uint8_t*)(Addrr - i) == 0x48 && *(uint8_t*)(uint8_t*)(Addrr - i + 1) == 0x8B && *(uint8_t*)(uint8_t*)(Addrr - i + 2) == 0xC4)
            {
                return Addrr - i;
            }
        }
    }

    return Memcury::Scanner::FindPattern("40 53 41 56 48 81 EC ? ? ? ? 48 8B 01 48 8B DA 4C 8B F1 FF 90").Get();
}

uint64 UFinder::InitListen()
{
    if (SDK::Fortnite_Version >= 19.00) {
        return Memcury::Scanner::FindPattern(
            "4C 8B DC 49 89 5B 10 49 89 73 18 57 48 83 EC 50 48 8B BC 24 ?? ?? ?? ?? 49 8B F0 48 8B"
        ).Get();
    }

    if (SDK::Fortnite_Version >= 16.40) {
        return Memcury::Scanner::FindPattern(
            "4C 8B DC 49 89 5B 08 49 89 73 10 57 48 83 EC 50 48 8B BC 24 ? ? ? ? 49 8B F0 48 8B 01 48 8B"
        ).Get();
    }

    auto Addr = Memcury::Scanner::FindStringRef(L"%s IpNetDriver listening on port %i");
    return FindBytes(
        Addr,
        SDK::Fortnite_Version < 18.40 ? std::vector<uint8_t>{ 0x48, 0x89, 0x5C } : std::vector<uint8_t>{ 0x4C, 0x8B, 0xDC },
        2000,
        0,
        true,
        1
    );
}

uint64 UFinder::GameSessionPatch()
{
    auto sRef = Memcury::Scanner::FindStringRef(L"Gamephase Step: %s", false).Get();
    uint64 Beginning = 0;

    if (!sRef) {
        Beginning = Memcury::Scanner::FindPattern(
            "48 89 5C 24 ? 57 48 83 EC 20 E8 ? ? ? ? 48 8B D8 48 85 C0 0F 84 ? ? ? ? E8"
        ).Get();

        if (!Beginning) return 0;
    }
    else {
        for (int i = 0; i < 3000; i++) {
            if (*(uint8_t*)(sRef - i) == 0x40 && *(uint8_t*)(sRef - i + 1) == 0x55) {
                Beginning = sRef - i;
                break;
            }

            if (*(uint8_t*)(sRef - i) == 0x48 && *(uint8_t*)(sRef - i + 1) == 0x89 &&
                *(uint8_t*)(sRef - i + 2) == 0x5C) {
                Beginning = sRef - i;
                break;
            }

            if (*(uint8_t*)(sRef - i) == 0x48 && *(uint8_t*)(sRef - i + 1) == 0x8B &&
                *(uint8_t*)(sRef - i + 2) == 0xC4) {
                Beginning = sRef - i;
                break;
            }
        }
    }

    if (!Beginning) return 0;

    for (int i = 0; i < 500; i++) {
        if (*(uint8_t*)(Beginning + i) == 0x0F && *(uint8_t*)(Beginning + i + 1) == 0x84) {
            return Beginning + i + 1;
        }
    }

    return 0;
}

uint64 UFinder::GetWorldContextFromObject()
{
    Memcury::Scanner Scanner = Memcury::Scanner::FindPattern("E8 ? ? ? ? 48 85 C0 74 ? 4C 63 B0");
    if (Scanner.Get())
        return Scanner.RelativeOffset(1).Get();
}

uint64 UFinder::CreateNetDriverLocal()
{
    Memcury::Scanner Scanner = Memcury::Scanner::FindPattern("E8 ? ? ? ? 48 89 87 ? ? ? ? 48 85 C0 74 ? 83 64 24 ? ? 48 8D 05");

    if (Scanner.Get())
        return Scanner.RelativeOffset(1).Get();
}

uint64 UFinder::CreateChannel()
{
    static uint64 CachedResult = 0;
    if (CachedResult != 0)
        return CachedResult;

    if (Fortnite_Version <= 3.3)
        CachedResult = Memcury::Scanner::FindPattern("40 56 57 41 54 41 55 41 57 48 83 EC 60 48 8B 01 41 8B F9 45 0F B6 E0").Get();
    else if (Fortnite_Version.GetMajorVersion() >= 20) // 21.00
        CachedResult = Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 44 89 4C 24 ? 55 57 41 54 41 56 41 57 48 8B EC 48 83 EC 50 45 33 E4 48 8D 05 ? ? ? ? 44 38 25").Get();

    return CachedResult;
}

uint64 UFinder::SendClientAdjustment()
{
    static uint64 CachedResult = 0;
    if (CachedResult != 0)
        return CachedResult;

    if (Fortnite_Version.GetMajorVersion() >= 20)
    {
        CachedResult = Memcury::Scanner::FindPattern(
            "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 83 3D ? ? ? ? ? 48 8B D9 75"
        ).Get();
    }

    return CachedResult;
}

uint64 UFinder::CallPreReplication()
{
    static uint64 CachedResult = 0;
    if (CachedResult != 0)
        return CachedResult;

    if (Fortnite_Version.GetMajorVersion() >= 23)
        return CachedResult = Memcury::Scanner::FindPattern("48 85 D2 0F 84 ? ? ? ? 48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 55 41 56 41 57 48 8B EC 48 83 EC ? F6 41").Get();
    
    if (Engine_Version == 4.16)
        CachedResult = Memcury::Scanner::FindPattern("48 85 D2 0F 84 ? ? ? ? 48 8B C4 55 57 41 57 48 8D 68 A1 48 81 EC").Get();
    else if (Engine_Version == 4.19)
        CachedResult = Memcury::Scanner::FindPattern("48 85 D2 0F 84 ? ? ? ? 48 8B C4 55 57 41 54 48 8D 68 A1 48 81 EC ? ? ? ? 48 89 58 08 4C").Get();
    else if (Fortnite_Version >= 2.5 && Fortnite_Version <= 3.3)
        CachedResult = Memcury::Scanner::FindPattern("48 85 D2 0F 84 ? ? ? ? 56 41 56 48 83 EC 38 4C 8B F2").Get();
    else if (Fortnite_Version.GetMajorVersion() == 20)
        CachedResult = Memcury::Scanner::FindPattern("48 85 D2 0F 84 ? ? ? ? 48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 83 EC 40 F6 41 58 30 48 8B EA 48 8B D9 40 B6 01").Get();
    else if (Fortnite_Version.GetMajorVersion() >= 21)
        CachedResult = Memcury::Scanner::FindPattern("48 85 D2 0F 84 ? ? ? ? 48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 83 EC 40 F6 41 58 30 4C 8B F2").Get();

    return CachedResult;
}

uint64 UFinder::RemoveNetworkActor()
{
    static uint64 CachedResult = 0;
    if (CachedResult != 0)
        return CachedResult;

    if (Fortnite_Version.GetMajorVersion() >= 20)
        CachedResult = Memcury::Scanner::FindPattern(
            "48 89 5C 24 ? 57 48 83 EC ? 48 8B D9 48 8B FA 48 8B 89 ? ? ? ? E8 ? ? ? ? 44 8B 83"
        ).Get();

    return CachedResult;
}

uint64 UFinder::DemoReplicateActor()
{
    static uint64 CachedResult = 0;
    if (CachedResult != 0)
        return CachedResult;

    if (Fortnite_Version.GetMajorVersion() >= 20)
        CachedResult = Memcury::Scanner::FindPattern(
            "48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 55 41 54 41 55 41 56 41 57 48 8B EC 48 83 EC ? 4C 8B E9 45 8A E1"
        ).Get();

    return CachedResult;
}

uint64 UFinder::WorldGetNetMode()
{
    if (Fortnite_Version.GetMajorVersion() >= 20)
        return Memcury::Scanner::FindPattern("48 83 EC ? 48 8B 01 FF 90 ? ? ? ? 84 C0 75").Get();

    return Memcury::Scanner::FindPattern("48 83 EC ? 48 83 79 ? ? 74 ? B8").Get();
}

uint64 UFinder::RepDriverServerReplicateActors()
{
    static uint64 CachedResult = 0;
    if (CachedResult != 0)
        return CachedResult;
    
    if (Engine_Version == 4.20)
        CachedResult = 0x53;
    else if (Engine_Version == 4.21)
        CachedResult = Fortnite_Version.GetMajorVersion() == 5 ? 0x54 : 0x56;
    else if (Engine_Version >= 4.22 && Engine_Version <= 4.24)
        CachedResult = Fortnite_Version >= 7.40 && Fortnite_Version < 8.40 ? 0x57 :
        Engine_Version == 4.24 ? (Fortnite_Version >= 11.00 && Fortnite_Version <= 11.10 ? 0x57 :
            (Fortnite_Version == 11.30 || Fortnite_Version == 11.31 ? 0x59 : 0x5A)) : 0x56;

    // ^ I know this makes no sense, 7.40-8.40 is 0x57, other 7-10 is 0x56, 11.00-11.10 = 0x57, 11.30-11.31 = 0x59, other S11 is 0x5A

    else if (Fortnite_Version.GetMajorVersion() == 12 || Fortnite_Version.GetMajorVersion() == 13)
        CachedResult = 0x5D;
    else if (Fortnite_Version.GetMajorVersion() == 14 || Fortnite_Version <= 15.2) // never tested 15.2
        CachedResult = 0x5E;
    else if (Fortnite_Version >= 15.3 && Engine_Version < 5.00) // 15.3-18 = 0x5F
        CachedResult = 0x5F;
    else if (Fortnite_Version.GetMajorVersion() >= 19 && Fortnite_Version.GetMajorVersion() <= 20)
        CachedResult = 0x66;
    else if (Fortnite_Version.GetMajorVersion() >= 21)
        CachedResult = 0x67; // checked onb 22.30

    return CachedResult;
}

uint64 UFinder::InternalTryActivateAbility()
{
    static uint64 CachedResult = 0;
    if (CachedResult != 0)
        return CachedResult;
    
    auto Addrr = Memcury::Scanner::FindStringRef(L"InternalTryActivateAbility called with invalid Handle! ASC: %s. AvatarActor: %s", true, 0, Fortnite_Version >= 16 && Fortnite_Version < 24).Get(); // checked 16.40

    for (int i = 0; i < 1000; i++)
    {
        if (*(uint8_t*)(uint8_t*)(Addrr - i) == 0x48 && *(uint8_t*)(uint8_t*)(Addrr - i + 1) == 0x8B && *(uint8_t*)(uint8_t*)(Addrr - i + 2) == 0xC4)
        {
            return CachedResult = Addrr - i;
        }

        if (*(uint8_t*)(uint8_t*)(Addrr - i) == 0x4C && *(uint8_t*)(uint8_t*)(Addrr - i + 1) == 0x89 && *(uint8_t*)(uint8_t*)(Addrr - i + 2) == 0x4C) // checked on 24.40
        {
            return CachedResult = Addrr - i;
        }
    }
	
    return 0;
}

uint64 UFinder::ConstructSpec()
{
    static uint64 CachedResult = 0;
    if (CachedResult != 0)
        return CachedResult;
    
    if (Engine_Version == 4.25)
    {
        auto ba = Memcury::Scanner::FindPattern("48 8B 44 24 ? 80 61 29 F8 80 61 31 FE 48 89 41 20 33 C0 89 41", false).Get();
        if (!ba)
            ba = Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 41 56 48 83 EC 20 45 33 F6 48 C7 01 ? ? ? ? 48 C7 41").Get();
        CachedResult = ba;
    }
    else if (Engine_Version == 4.26)
    {
        CachedResult = Memcury::Scanner::FindPattern("80 61 31 FE 0F 57 C0 80 61 29 F0 48 8B 44 24 ? 48").Get();
    }
    else if (Engine_Version == 4.27)
    {
        CachedResult = Memcury::Scanner::FindPattern("80 61 31 FE 41 83 C9 FF 80 61 29 F0 48 8B 44 24 ? 48 89 41").Get();
    }
    else if (Engine_Version == 5.00)
    {
        CachedResult = Memcury::Scanner::FindPattern("4C 8B C9 48 8B 44 24 ? 83 C9 FF 41 80 61 ? ? 41 80 61 ? ? 49 89 41 20 33 C0 41 88 41 30 49 89 41").Get();
    }
    else
    {
        CachedResult = Memcury::Scanner::FindPattern("80 61 29 F8 48 8B 44 24 ?").Get();
    }

    return CachedResult;
}

uint64 UFinder::GiveAbilityAndActivateOnce()
{
    static uint64 CachedResult = 0;
    if (CachedResult != 0)
        return CachedResult;

    if (Fortnite_Version >= 12.41)
    {
        return CachedResult = Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 55 48 8B EC 48 83 EC ? 49 8B 40").Get();
    }
    
    if (Engine_Version == 4.26)
    {
        auto sig1 = Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 40 49 8B 40 10 49 8B D8 48 8B FA 48 8B F1", false).Get();

        if (!sig1)
            sig1 = Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC 40 49 8B 40 10 49").Get(); // 15.50

        return CachedResult = sig1;
    }
    
    auto Addrr = Memcury::Scanner::FindStringRef(L"GiveAbilityAndActivateOnce called on ability %s on the client, not allowed!", true, 0, Engine_Version >= 5.00).Get();

    for (int i = 0; i < 1000; i++)
    {
        if (*(uint8_t*)(uint8_t*)(Addrr - i) == 0x40 && *(uint8_t*)(uint8_t*)(Addrr - i + 1) == 0x55)
        {
            return CachedResult = Addrr - i;
        }

        if (*(uint8_t*)(uint8_t*)(Addrr - i) == 0x48 && *(uint8_t*)(uint8_t*)(Addrr - i + 1) == 0x89 && *(uint8_t*)(uint8_t*)(Addrr - i + 2) == 0x5C)
        {
            return CachedResult = Addrr - i;
        }
    }

    return CachedResult = 0;
}

uint64 UFinder::GetInterfaceAddress()
{
    static uint64 CachedResult = 0;
    if (CachedResult != 0)
        return CachedResult;
    
    if (Engine_Version <= 4.21)
        return CachedResult = Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 33 FF 48 8B DA 48 8B F1 48").Get(); 

    return CachedResult = Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 33 DB 48 8B FA 48 8B F1 48 85 D2 0F 84 ? ? ? ? 8B 82 ? ? ? ? C1 E8").Get();
}

uint64 UFinder::GiveAbility()
{
    static uint64 CachedResult = 0;
    if (CachedResult != 0)
        return CachedResult;

    Memcury::Scanner Scanner = Memcury::Scanner::FindPattern("E8 ? ? ? ? 8B D3 49 8B CE E8 ? ? ? ? 48 8B 4D");
    if (!Scanner.Get())
    {
        Scanner = Memcury::Scanner::FindPattern("E8 ? ? ? ? 8B 5D ? 48 8B CE", true);
    }
    CachedResult = Scanner.RelativeOffset(1).Get();

    return CachedResult;
}

uint64 UFinder::OnDamageServer()
{
    auto Addr = Memcury::FindFunction(L"OnDamageServer", 
        Fortnite_Version <= 1.9 ? std::vector<uint8_t>{ 0x4C, 0x89, 0x4C } : 
        Fortnite_Version <= 2.4 || Fortnite_Version >= 19.00 ? std::vector<uint8_t>{ 0x48, 0x8B, 0xC4 } : std::vector<uint8_t>{ 0x40, 0x55 }
    );
    
    return Addr;
}

uint64 UFinder::ClientOnPawnDied()
{
    auto Addr = Memcury::FindFunction(L"ClientOnPawnDied", std::vector<uint8_t>{ 0x48, 0x89, 0x5C });
    return Addr;
}


uint64 UFinder::StaticLoadObject()
{
    static uint64 CachedResult = 0;
    if (CachedResult != 0)
        return CachedResult;

    auto Addr = Memcury::Scanner::FindStringRef(L"STAT_LoadObject", false).Get();

    if (!Addr)
    {
        auto StrRef = Memcury::Scanner::FindStringRef(L"Calling StaticLoadObject during PostLoad may result in hitches during streaming.");
        return CachedResult = FindBytes(StrRef, { 0x40, 0x55 }, 1000, 0, true);
    }

    for (int i = 0; i < 400; i++)
    {
        if (*(uint8_t*)(uint8_t*)(Addr - i) == 0x4C && *(uint8_t*)(uint8_t*)(Addr - i + 1) == 0x89 && *(uint8_t*)(uint8_t*)(Addr - i + 2) == 0x4C)
        {
            return CachedResult = Addr - i;
        }

        if (*(uint8_t*)(uint8_t*)(Addr - i) == 0x48 && *(uint8_t*)(uint8_t*)(Addr - i + 1) == 0x8B && *(uint8_t*)(uint8_t*)(Addr - i + 2) == 0xC4)
        {
            return CachedResult = Addr - i;
        }
    }

    return CachedResult = 0;
}

uint64 UFinder::SpawnBot()
{
    auto String = Memcury::Scanner::FindStringRef(L"UFortServerBotManagerAthena::SpawnBot invalid spawn location", true, 0, false).Get();

    for (int i = 0; i < 1000; i++)
    {
        if (*(uint8_t*)(String - i) == 0x48 && *(uint8_t*)(String - i + 1) == 0x8b && *(uint8_t*)(String - i + 2) == 0xc4)
        {
            return String - i;
        }
    }

    return 0;
}

uint64 UFinder::SpawnActor()
{
    static uint64 CachedResult = 0;
    if (CachedResult != 0)
        return CachedResult;

    if (Fortnite_Version >= 6.21 && Fortnite_Version <= 15.30)
    {
        return CachedResult = Memcury::Scanner::FindPattern("40 53 56 57 48 83 EC ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 44 24 ? 0F 28 1D ? ? ? ? 0F 57 D2 48 8B B4 24").Get();
    } 
    
    return CachedResult = 0;
}

uint64 UFinder::CantBuild()
{
    static uint64 CachedResult = 0;
    if (CachedResult != 0)
        return CachedResult;

    CachedResult = Memcury::Scanner::FindPattern("48 89 5C 24 10 48 89 6C 24 18 48 89 74 24 20 41 56 48 83 EC ? 49 8B E9 4D 8B F0", false).Get();
    if (!CachedResult)
        CachedResult = Memcury::Scanner::FindPattern("48 89 54 24 ? 55 56 41 56 48 83 EC 50", false).Get(); // 4.20
    if (!CachedResult)
        CachedResult = Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 83 EC 60 4D 8B F1 4D 8B F8", false).Get(); // 4.26.1
    if (!CachedResult)
        CachedResult = Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 83 EC 60 49 8B E9 4D 8B F8 48 8B DA 48 8B F9 BE ? ? ? ? 48", false).Get(); // 5.00
    if (!CachedResult)
        CachedResult = Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 83 EC 70 49 8B E9 4D 8B F8 48 8B DA 48 8B F9").Get(); // 20.00

    return CachedResult;
}

uint64 UFinder::ReplaceBuildingActor()
{
    static uint64 CachedResult = 0;
    if (CachedResult != 0)
        return CachedResult;

    auto StringRef = Memcury::Scanner::FindStringRef(L"STAT_Fort_BuildingSMActorReplaceBuildingActor", false);

    if (!StringRef.Get()) 
    {
        return CachedResult = Memcury::Scanner::FindPattern("4C 89 44 24 ? 55 56 57 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 45").Get(); 
    }

    auto BytesToFind = Engine_Version == 4.20 || (Engine_Version == 4.21 && Fortnite_Version < 6.30) || Engine_Version >= 4.27 ? std::vector<uint8_t>{ 0x48, 0x8B, 0xC4 }
    : std::vector<uint8_t>{ 0x4C, 0x8B };

    return CachedResult = FindBytes(StringRef, BytesToFind, 1000, 0, true);
}


uint64 UFinder::BotManagerSetup()
{
    static uint64 CachedResult = 0;
    if (CachedResult != 0)
        return CachedResult;

    if (Fortnite_Version >= 12.50 && Fortnite_Version <= 13.40)
    {
        return CachedResult = Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 7C 24 ? 55 41 54 41 55 41 56 41 57 48 8B EC 48 83 EC ? 45 33 E4").Get();
    }
    
    if (Fortnite_Version <= 12.41 && Fortnite_Version >= 12.00)
    {
        return CachedResult = Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 55 57 41 54 41 55 41 57 48 8B EC 48 83 EC ? 45 33 E4").Get();
    }

    auto StringRef = Memcury::Scanner::FindStringRef("Can't configure player bots if no GameMode set");

    return CachedResult = StringRef.Get() ? FindBytes(StringRef, { 0x40, 0x55 }, 1000, 0, true) : 0;
}

uint64 UFinder::SpawnBotRet()
{
    static uint64 CachedResult = 0;
    if (CachedResult != 0)
        return CachedResult;

    if (Fortnite_Version == 12.61)
    {
        return CachedResult = IMAGEBASE + 0x1aaa8df;
    }
    
    auto strRef = Memcury::Scanner::FindStringRef(L"Unable to create UFortAthenaAIBotCustomizationData object. BotClass = %s", true, 0, Fortnite_Version >= 19.00).Get();

    for (int i = 0; i < 1000; i++)
    {
        if ((uint8_t)(strRef + i) == 0x48 && (uint8_t)(strRef + i + 1) == 0x8b && (uint8_t)(strRef + i + 2) == 0xd8)
        {
            return CachedResult = strRef + i;
        }
    }

    return 0;
}

uint64 UFinder::OnPossessedPawnDied()
{
    static uint64 CachedResult = 0;
    if (CachedResult != 0)
        return CachedResult;

    auto Addr = Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 6C 24 ? 56 48 83 EC ? 49 8B F1 0F 29 74 24").Get(); 

    if (Addr)
    {
        return CachedResult = Addr;
    }
    
    return CachedResult = 0;
}

uint64 UFinder::SetupPickup() {
    static uint64 CachedResult = 0;
    if (CachedResult != 0)
        return CachedResult;
    
    if (Fortnite_Version >= 1.11 && Fortnite_Version < 2.5)
        return CachedResult = Memcury::Scanner::FindPattern("48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC 20 80 B9 ? ? ? ? ? 41 0F B6 E9").Get();
    
    if (Fortnite_Version >= 2.5 && Fortnite_Version <= 3.3)
        return CachedResult = Memcury::Scanner::FindPattern("48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC 20 80 B9 ? ? ? ? ? 41 0F B6 E9 49 8B F8 48 8B F1 0F 85 ? ? ? ? 48 83 7A").Get();
    
    if (Fortnite_Version >= 3.4 && Fortnite_Version < 5.0)
        return CachedResult = Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 41 56 48 83 EC 20 80 B9 ? ? ? ? ? 45 0F B6 F1 49 8B E8").Get();
    
    if (Fortnite_Version >= 5.0 && Fortnite_Version < 7.0) {
        auto addr = Memcury::Scanner::FindPattern("48 89 5C 24 ? 55 57 41 57 48 83 EC 30 80 B9 ? ? ? ? ? 41 0F B6", false).Get();
        
        if (!addr)
            addr = Memcury::Scanner::FindPattern("48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC 20 80 B9 ? ? ? ? ? 41 0F B6 E9").Get();
        
        return CachedResult = addr;
    }
    
    if (Fortnite_Version >= 7.0 && Fortnite_Version < 8.0)
        return CachedResult = Memcury::Scanner::FindPattern("48 89 5C 24 ? 57 41 56 41 57 48 83 EC 30 80 B9 ? ? ? ? ? 45 0F B6 F1").Get();
    
    if (Fortnite_Version >= 8.0)
        return CachedResult = Memcury::Scanner::FindPattern("48 89 5C 24 ? 57 41 56 41 57 48 83 EC 30 80 B9 ? ? ? ? ? 45 0F B6 F1 4D").Get();
    
    return CachedResult = 0;
}

uint64 UFinder::CompletePickupAnimation() {
    static uint64 CachedResult = 0;
    if (CachedResult != 0)
        return CachedResult;
    
    if (Fortnite_Version >= 1.7 && Fortnite_Version < 2.5)
        return CachedResult = Memcury::Scanner::FindPattern("4C 8B DC 53 55 56 48 83 EC 60 48 8B F1 48 8B 89 ? ? ? ? 48 85 C9").Get();
    
    if (Fortnite_Version >= 2.5 && Fortnite_Version < 5.0) {
        auto addy = Memcury::Scanner::FindPattern("4C 8B DC 53 55 56 48 83 EC 60 48 8B F1 48 8B 89", false).Get();
        if (!addy)
            addy = Memcury::Scanner::FindPattern("48 89 5C 24 ? 57 48 83 EC 20 48 8B D9 48 8B 89 ? ? ? ? 48 85 C9 74 20 48 8D 44 24").Get();
        return CachedResult = addy;
    }
    
    if (Fortnite_Version >= 5.0 && Fortnite_Version < 7.0) {
        auto adda = Memcury::Scanner::FindPattern("40 53 56 48 83 EC 38 4C 89 6C 24 ? 48 8B F1 4C 8B A9", false).Get();
        if (!adda)
            adda = Memcury::Scanner::FindPattern("40 53 56 57 48 83 EC 30 4C 89 6C 24 ? 48 8B F1 4C 8B A9 ? ? ? ? 4D 85 ED 0F 84").Get();
        return CachedResult = adda;
    }
    
    if (Fortnite_Version >= 7.0 && Fortnite_Version < 8.0)
        return CachedResult = Memcury::Scanner::FindPattern("40 53 56 57 48 83 EC 30 4C 89 6C 24 ? 48 8B F1 4C 8B A9 ? ? ? ? 4D 85 ED 0F 84").Get();
    
    if (Fortnite_Version >= 8.0 && Fortnite_Version < 17.0)
    {
        CachedResult = Memcury::Scanner::FindPattern("40 53 56 48 83 EC 38 4C 89 6C 24 ? 48 8B F1 4C 8B A9 ? ? ? ? 4D 85 ED").Get();
        if (CachedResult == 0)
        {
            return CachedResult == Memcury::Scanner::FindPattern("40 53 56 48 83 EC ? 4C 89 6C 24 ? 48 8B F1 4C 8B A9").Get();    
        }
        else {
            return CachedResult;
        }
    }
    
    if (Fortnite_Version >= 17.0 && Fortnite_Version < 19.0) {
        auto sig = Memcury::Scanner::FindPattern("48 8B C4 48 89 58 08 48 89 68 10 48 89 70 18 48 89 78 20 41 54 41 56 41 57 48 83 EC 20 48 8B B1 ? ? ? ? 48 8B D9 48 85 F6", false).Get();
        
        if (!sig)
            sig = Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 55 57 41 54 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B B9 ? ? ? ? 48 8B D9 48 85 FF 74 16 48 89", false).Get();
        if (!sig)
            sig = Memcury::Scanner::FindPattern("48 8B C4 48 89 58 10 48 89 68 18 57 48 83 EC 20 48 8B D9 48 8B 89 ? ? ? ? 48 85").Get();
        return CachedResult = sig;
    }
    
    if (Fortnite_Version >= 19.0) {
        auto addr = Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 55 57 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B B9", false).Get();
        if (!addr)
            addr = Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 55 57 41 54 48 8D AC 24 ? ? ? ? 48 81 EC A0 01 00 00", false).Get();
        if (!addr)
            addr = Memcury::Scanner::FindPattern("48 8B C4 48 89 58 10 48 89 70 18 48 89 78 20 55 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B B9 ? ? ? ? 45 33 E4 48 8B D9 48 85 FF 74 0F").Get();
        
        return CachedResult = addr;
    }
    
    return CachedResult = 0;
}

uint64 UFinder::RemoveFromAlivePlayers()
{
    static uint64 CachedResult = 0;
    if (CachedResult != 0)
        return CachedResult;
    
    auto Addrr = Memcury::Scanner::FindStringRef(L"FortGameModeAthena: Player [%s] removed from alive players list (Team [%d]).  Player count is now [%d].  Team count is now [%d].", false).Get();

    if (!Addrr)
        CachedResult = Addrr = Memcury::Scanner::FindStringRef(L"FortGameModeAthena: Player [%s] removed from alive players list (Team [%d]).  Player count is now [%d]. PlayerBots count is now [%d]. Team count is now [%d].", false).Get();

    if (!Addrr)
        CachedResult = Addrr = Memcury::Scanner::FindStringRef(L"FortGameModeAthena::RemoveFromAlivePlayers: Player [%s] PC [%s] removed from alive players list (Team [%d]).  Player count is now [%d]. PlayerBots count is now [%d]. Team count is now [%d].", true, 0, Fortnite_Version >= 16 && Fortnite_Version < 24).Get(); // checked on 16.40

    for (int i = 0; i < 2000; i++)
    {
        if (*(uint8_t*)(uint8_t*)(Addrr - i) == 0x4C && *(uint8_t*)(uint8_t*)(Addrr - i + 1) == 0x89 && *(uint8_t*)(uint8_t*)(Addrr - i + 2) == 0x4C) // most common
        {
            return CachedResult = Addrr - i;
        }

        if (*(uint8_t*)(uint8_t*)(Addrr - i) == 0x48 && *(uint8_t*)(uint8_t*)(Addrr - i + 1) == 0x89 && *(uint8_t*)(uint8_t*)(Addrr - i + 2) == 0x54) // idk what verisont bh
        {
            for (int z = 3; z < 50; z++)
            {
                if (*(uint8_t*)(uint8_t*)(Addrr - i - z) == 0x4C && *(uint8_t*)(uint8_t*)(Addrr - i - z + 1) == 0x89 && *(uint8_t*)(uint8_t*)(Addrr - i - z + 2) == 0x4C)
                {
                    return CachedResult = Addrr - i - z;
                }
            }

            return CachedResult = Addrr - i;
        }

        if (*(uint8_t*)(uint8_t*)(Addrr - i) == 0x48 && *(uint8_t*)(uint8_t*)(Addrr - i + 1) == 0x8B && *(uint8_t*)(uint8_t*)(Addrr - i + 2) == 0xC4) // i forgot what version
        {
            return CachedResult = Addrr - i;
        }
    }

    return CachedResult = 0;
}

uint64 UFinder::RegisterComponentWithWorld()
{
    static uint64 CachedResult = 0;
    if (CachedResult != 0)
        return CachedResult;

    auto Addr = Memcury::Scanner::FindPattern("48 83 EC ? 44 0F B6 05 ? ? ? ? 48 8D 05 ? ? ? ? 48 89 5C 24 ? 48 8B D9 48 89 6C 24", false).Get();
    if (Addr != 0) {
        return CachedResult = Addr;
    }

    return CachedResult = 0;
}

uint64 UFinder::CreateAndConfigureNavigationSystem()
{
    static uint64 CachedResult = 0;
    if (CachedResult != 0)
        return CachedResult;

    if (Fortnite_Version >= 12.f) {
        if (Fortnite_Version <= 13.00 && Fortnite_Version >= 12.60) {
            return IMAGEBASE + 0x48C8580;
        }
        auto Addr = Memcury::Scanner::FindPattern("48 89 5C 24 ? 56 48 83 EC ? F6 41 ? ? 48 8B DA", false).Get();
        if (Addr != 0) {
            return CachedResult = Addr;
        }
    }

    return CachedResult = 0;
}


uint64 UFinder::SendComplexCustomStatEvent()
{
    static uint64 CachedResult = 0;
    if (CachedResult != 0)
        return CachedResult;

    if (Fortnite_Version >= 7.40 && Fortnite_Version <= 14.40)
    {
        return CachedResult = Memcury::Scanner::FindPattern("4C 8B DC 49 89 5B ? 57 48 83 EC ? 33 C0 49 8D 53").Get();
    }

    return CachedResult = 0;
}

uint64 UFinder::SpawnLoot()
{
    static uint64 CachedResult = 0;
    if (CachedResult != 0)
        return CachedResult;

    auto Addr = Memcury::Scanner::FindPattern("40 53 48 83 EC ? 80 3D ? ? ? ? ? 48 8B D9 0F 82 ? ? ? ? 48 89 74 24 ? 48 8D 54 24 ? 45 33 C0");
    if (Addr.Get() != 0)
    {
        return CachedResult = Addr.Get();
    }

    auto StringAddr = Memcury::Scanner::FindStringRef("ABuildingContainer::SpawnLoot() called on %s (%s)...");
    
    auto Func = FindBytes(StringAddr, { 0x40, 0x55 }, 1000, 0, true);
    
    if (Func != 0)
    {
        CachedResult = Func;
    }

    return CachedResult;
}

uint64 UFinder::ReloadWeapon()
{
    static uint64 CachedResult = 0;
    if (CachedResult != 0)
        return CachedResult;

    auto Addr = Memcury::Scanner::FindPattern("89 54 24 ? 55 41 56 48 8D 6C 24 ? 48 81 EC ? ? ? ? 80 B9", false).Get();
    if (Addr != 0) {
        return CachedResult = Addr;
    }

    return CachedResult = 0;
}

uint64 UFinder::GetSquadIdForCurrentPlayer()
{
    auto Addrr = Memcury::Scanner::FindStringRef(L"GetSquadIdForCurrentPlayer failed to find a squad id for player %s", true, 0, Fortnite_Version >= 19).Get();

    if (!Addrr)
        return 0;

    for (int i = 0; i < 2000; i++)
    {
        if (*(uint8_t*)(uint8_t*)(Addrr - i) == 0x48 && *(uint8_t*)(uint8_t*)(Addrr - i + 1) == 0x89 && *(uint8_t*)(uint8_t*)(Addrr - i + 2) == 0x5C)
        {
            return Addrr - i;
        }
    }

    return 0;
}

uint64 UFinder::OnSafeZoneStateChange()
{
    auto OnSafeZoneStateChangeAddr = Memcury::FindFunction(L"OnSafeZoneStateChange", Fortnite_Version <= 2.4 ? std::vector<uint8_t>{ 0x48, 0x89, 0x54 } : std::vector<uint8_t>{ 0x48, 0x89, 0x5C });

    if (OnSafeZoneStateChangeAddr)
    {
        return OnSafeZoneStateChangeAddr;
    }

    return 0;
}

uint64 UFinder::StartNewSafeZonePhase()
{
    auto Addr = Memcury::Scanner::FindStringRef(L"FortGameModeAthena: No MegaStorm on SafeZone[%d].  GridCellThickness is less than 1.0.", true, 0, Engine_Version >= 427).Get();

    if (!Addr)
        return 0;

    for (int i = 0; i < 100000; i++)
    {
        if ((*(uint8_t*)(uint8_t*)(Addr - i) == 0x40 && *(uint8_t*)(uint8_t*)(Addr - i + 1) == 0x53) 
            || (*(uint8_t*)(uint8_t*)(Addr - i) == 0x40 && *(uint8_t*)(uint8_t*)(Addr - i + 1) == 0x55))
        {
            return Addr - i;
        }

        if (Fortnite_Version < 8)
        {
            if (*(uint8_t*)(uint8_t*)(Addr - i) == 0x48 && *(uint8_t*)(uint8_t*)(Addr - i + 1) == 0x89 && *(uint8_t*)(uint8_t*)(Addr - i + 2) == 0x5C)
            {
                return Addr - i;
            }
        }

        if (*(uint8_t*)(uint8_t*)(Addr - i) == 0x48 && *(uint8_t*)(uint8_t*)(Addr - i + 1) == 0x8B && *(uint8_t*)(uint8_t*)(Addr - i + 2) == 0xC4)
        {
            return Addr - i;
        }
    }

    return 0;
}

uint64 UFinder::InstructionForCollision()
{
    auto InstructionForCollision = Memcury::Scanner::FindPattern("40 53 48 83 EC ? 33 C0 48 89 7C 24 ? 48 89 44 24 ? 48 8D 54 24 ? 48 89 44 24 ? 48 8B D9").Get();

    if (InstructionForCollision)
    {
        return InstructionForCollision;
    }
}

uint64 UFinder::GetGameSessionClass() // IM LAZY TO TRY AND MAKE A SIG (ITS NOT EASY FOR THIS FUNC)))GOIJ02OIGOIWEGOIWG
{
    if (Fortnite_Version <= 13.00 && Fortnite_Version >= 12.60)
    {
        return IMAGEBASE + 0x1E4F780;
    }
}

uint64 UFinder::GetCommandLet()
{
    static uint64 CachedResult = 0;
    if (CachedResult != 0)
        return CachedResult;

    auto StringRef = Memcury::Scanner::FindStringRef(L"Attempting to get the command line but it hasn't been initialized yet.", false);
    
    if (StringRef.Get())
    {
        CachedResult = StringRef.ScanFor({ 0x40, 0x53 }, false, 0, 0, 1000).Get();
        
        if (!CachedResult)
            CachedResult = StringRef.ScanFor({ 0x48, 0x89, 0x5C }, false, 0, 0, 1000).Get();
            
        if (!CachedResult)
            CachedResult = StringRef.ScanFor({ 0x48, 0x8B, 0xC4 }, false, 0, 0, 1000).Get();
    }

    return CachedResult;
}

uint64 UFinder::MatchmakingSerivcePerms()
{
    if (Fortnite_Version <= 15.50 && Fortnite_Version >= 9.40)
    {
        return Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 41 56 48 83 EC ? 33 DB 4C 8B F2 48 8B F1 39 99 ? ? ? ? 7E ? 8B FB 66 0F 1F 84 00 ? ? ? ? 41 83 7E ? ? 48 8B 86 ? ? ? ? 74 ? 49 8B 16 EB ? 48 8D 15 ? ? ? ? 83 7C 07").Get();
    } 
}

uint64 UFinder::EnterAircraft()
{
    auto Addr = Memcury::Scanner::FindStringRef(L"EnterAircraft: [%s] is attempting to enter aircraft after having already exited.", true, 0, Engine_Version >= 5.00 && Fortnite_Version < 24.00).Get();

    for (int i = 0; i < 1000; i++)
    {
        if ((*(uint8_t*)(uint8_t*)(Addr - i) == 0x40 && *(uint8_t*)(uint8_t*)(Addr - i + 1) == 0x53)
            || (*(uint8_t*)(uint8_t*)(Addr - i) == 0x40 && *(uint8_t*)(uint8_t*)(Addr - i + 1) == 0x55))
        {
            return Addr - i;
        }

        if (Fortnite_Version >= 15)
        {
            if (*(uint8_t*)(uint8_t*)(Addr - i) == 0x48 && *(uint8_t*)(uint8_t*)(Addr - i + 1) == 0x89 && *(uint8_t*)(uint8_t*)(Addr - i + 2) == 0x5C && *(uint8_t*)(uint8_t*)(Addr - i + 3) == 0x24)
            {
                return Addr - i;
            }
        }

        if (*(uint8_t*)(uint8_t*)(Addr - i) == 0x48 && *(uint8_t*)(uint8_t*)(Addr - i + 1) == 0x89 && *(uint8_t*)(uint8_t*)(Addr - i + 2) == 0x74) // 4.1
        {
            return Addr - i;
        }
    }

    return 0;
}

uint64 UFinder::OnRep_ZiplineState()
{
    static uint64 CachedResult = 0;
    if (CachedResult != 0)
        return CachedResult;
    
    auto Addrr = Memcury::Scanner::FindStringRef(L"ZIPLINES!! Role(%s)  AFortPlayerPawn::OnRep_ZiplineState ZiplineState.bIsZiplining=%d", false).Get();

    if (!Addrr)
        Addrr = Memcury::Scanner::FindStringRef(L"ZIPLINES!! GetLocalRole()(%s)  AFortPlayerPawn::OnRep_ZiplineState ZiplineState.bIsZiplining=%d", false).Get();

    if (!Addrr)
        Addrr = Memcury::Scanner::FindStringRef("AFortPlayerPawn::HandleZiplineStateChanged").Get(); 

    if (!Addrr)
        return 0;

    for (int i = 0; i < 400; i++)
    {
        if (*(uint8_t*)(uint8_t*)(Addrr - i) == 0x40 && *(uint8_t*)(uint8_t*)(Addrr - i + 1) == 0x53)
        {
            return CachedResult = Addrr - i;
        }

        if (*(uint8_t*)(uint8_t*)(Addrr - i) == 0x48 && *(uint8_t*)(uint8_t*)(Addrr - i + 1) == 0x89 && *(uint8_t*)(uint8_t*)(Addrr - i + 2) == 0x5C)
        {
            return CachedResult = Addrr - i;
        }

        if (*(uint8_t*)(uint8_t*)(Addrr - i) == 0x48 && *(uint8_t*)(uint8_t*)(Addrr - i + 1) == 0x8B && *(uint8_t*)(uint8_t*)(Addrr - i + 2) == 0xC4)
        {
            return CachedResult = Addrr - i;
        }
    }
	
    return 0;
}

uint64 UFinder::PickTeam()
{
    if (Engine_Version == 4.26)
    {
        auto testAddr = Memcury::Scanner::FindPattern("88 54 24 10 53 56 41 54 41 55 41 56 48 83 EC 60 4C 8B A1", false).Get();

        if (!testAddr)
            testAddr = Memcury::Scanner::FindPattern("88 54 24 10 53 55 56 41 55 41 ? 48 83 EC 70 48", false).Get();

        if (testAddr)
            return testAddr;
    }

    else if (Engine_Version == 5.00)
        return Memcury::Scanner::FindPattern("48 89 5C 24 ? 88 54 24 10 55 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 83 EC 70 45 33 ED 4D").Get(); 

    else if (Engine_Version >= 4.27) 
        return Memcury::Scanner::FindPattern("48 89 5C 24 ? 88 54 24 10 55 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 83 EC 70 4C 8B A1").Get();

    if (Fortnite_Version == 7.20 || Fortnite_Version == 7.30)
        return Memcury::Scanner::FindPattern("89 54 24 10 53 56 41 54 41 55 41 56 48 81 EC").Get();

    auto Addr = Memcury::Scanner::FindStringRef(L"PickTeam for [%s] used beacon value [%d]", false, 0, Engine_Version >= 427);

    if (!Addr.Get())
        Addr = Memcury::Scanner::FindStringRef(L"PickTeam for [%s] used beacon value [%s]");

    return FindBytes(Addr, Fortnite_Version <= 4.1 ? std::vector<uint8_t>{ 0x48, 0x89, 0x6C } : std::vector<uint8_t>{ 0x40, 0x55 }, 1000, 0, true);
}

uint64 UFinder::ServerOnAttemptInteract()
{
    if (Fortnite_Version <= 14.40 && Fortnite_Version >= 12.00)
    {
        return Memcury::Scanner::FindPattern("40 55 53 56 57 41 56 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8D B9 ? ? ? ? 48 8B F1").Get();
    }
}

uint64 UFinder::OnPlayImpactFX()
{
    auto OnPlayImpactFXStringRef = Memcury::Scanner::FindStringRef(L"OnPlayImpactFX", true, 0);
    __int64 OnPlayImpactFXAddr = 0;

    if (OnPlayImpactFXStringRef.Get())
    {
        auto OnPlayImpactFXFunctionPtr = OnPlayImpactFXStringRef.ScanFor({ 0x48, 0x8D, 0x0D }).RelativeOffset(3).GetAs<void*>();
        auto OnPlayImpactFXPtrRef = Memcury::Scanner::FindPointerRef(OnPlayImpactFXFunctionPtr).Get();

        for (int i = 0; i < 2000; i++)
        {
            if (*(uint8_t*)(uint8_t*)(OnPlayImpactFXPtrRef - i) == 0x48 && *(uint8_t*)(uint8_t*)(OnPlayImpactFXPtrRef - i + 1) == 0x89 && *(uint8_t*)(uint8_t*)(OnPlayImpactFXPtrRef - i + 2) == 0x5C)
            {
                OnPlayImpactFXAddr = OnPlayImpactFXPtrRef - i;
                break;
            }

            if (*(uint8_t*)(uint8_t*)(OnPlayImpactFXPtrRef - i) == 0x4C && *(uint8_t*)(uint8_t*)(OnPlayImpactFXPtrRef - i + 1) == 0x8B && *(uint8_t*)(uint8_t*)(OnPlayImpactFXPtrRef - i + 2) == 0xDC)
            {
                OnPlayImpactFXAddr = OnPlayImpactFXPtrRef - i;
                break;
            }
        }
    }

    return OnPlayImpactFXAddr;
}

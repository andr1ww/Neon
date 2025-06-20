#include "pch.h"
#include "../Header/Finder.h"

uint64_t UFinder::WorldNetMode()
{
    if (SDK::Fortnite_Version == FFortniteVersion(18,0,0)) {
        return Memcury::Scanner::FindPattern(
            "48 83 EC 28 48 83 79 ? ? 75 20 48 8B 91 ? ? ? ? 48 85 D2 74 1E 48 8B 02 48 8B CA FF 90"
        ).Get();
    }

    auto stringRef = Memcury::Scanner::FindStringRef(L"PREPHYSBONES");
    auto fBegin = stringRef.ScanFor({ 0x40, 0x55 }, false).Get();

    for (int i = 0; i < 0x400; i++) {
        if (*(uint8_t*)(fBegin + i) == 0xe8 && *(uint8_t*)(fBegin + i - 1) != 0x8b) {
            return Memcury::Scanner(fBegin + i).RelativeOffset(1).Get();
        }
    }
    return 0;
}

uint64 UFinder::GIsClient()
{
    auto Addr = Memcury::Scanner::FindStringRef(L"AllowCommandletRendering");

    std::vector<std::vector<uint8_t>> BytesToCheck = {
        {0x88, 0x05},
        {0xC6, 0x05},
        {0x88, 0x1D},
        {0x44, 0x88}
    };

    int Picked = 0;
    int Skip = 2;

    for (int i = 0; i < 50; i++) {
        auto Curr = (uint8_t*)(Addr.Get() - i);
        for (const auto& Bytes : BytesToCheck) {
            if (*Curr == Bytes[0]) {
                bool Found = true;
                for (size_t j = 1; j < Bytes.size(); j++) {
                    if (*(Curr + j) != Bytes[j]) {
                        Found = false;
                        break;
                    }
                }
                if (Found) {
                    auto Relative = Bytes[0] == 0x44 ? 3 : 2;

                    if (Bytes[0] == 0x44 && *(Curr + 2) == 0x74) continue;

                    if (!Picked) Picked = Bytes[0];
                    else if (Picked != Bytes[0]) continue;

                    if (Skip > 0) {
                        Skip--;
                        continue;
                    }

                    auto Scanner = Memcury::Scanner(Curr);
                    return Bytes[0] == 0xC6 ?
                        Scanner.RelativeOffset(Relative, 1).Get() :
                        Scanner.RelativeOffset(Relative).Get();
                }
            }
        }
    }
    return 0;
}

uint64 UFinder::TickFlush()
{
    if (SDK::Fortnite_Version >= 16.40) {
        auto addr = Memcury::Scanner::FindPattern(
            "48 8B C4 48 89 58 18 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 B8 0F 29 78 A8 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 8A",
            false
        ).Get();

        if (!addr) {
            addr = Memcury::Scanner::FindPattern(
                "48 8B C4 48 89 58 18 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 B8 0F 29 78 A8 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 44 0F",
                false
            ).Get();
        }

        if (!addr) {
            addr = Memcury::Scanner::FindPattern(
                "48 8B C4 48 89 58 18 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 B8 0F 29 78 A8 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B F9 48 89 4D 38 48 8D 4D 40"
            ).Get();
        }
        return addr;
    }

    auto Addr = Memcury::Scanner::FindStringRef(L"STAT_NetTickFlush", false);

    if (!Addr.Get()) {
        if (SDK::Fortnite_Version >= 2.5 && SDK::Fortnite_Version <= 4.5) {
            return Memcury::Scanner::FindPattern(
                "4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 45 0F 29 43 ? 45 0F 29 4B ? 48 8B 05 ? ? ? ? 48 33"
            ).Get();
        }
    }

    return Addr.ScanFor(
        SDK::Fortnite_Version < 18 ? std::vector<uint8_t> { 0x4C, 0x8B } : std::vector<uint8_t>{ 0x48, 0x8B, 0xC4 },
        false, 0, 0, 1000
    ).Get();
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
    if (SDK::Fortnite_Version < 13.00) {
        auto address = Memcury::Scanner::FindStringRef(
            L"AOnlineBeaconHost::InitHost failed"
        ).ScanFor({ 0x48, 0x8B, 0xD0, 0xE8 }, false).RelativeOffset(4).Get();
        return address;
    }

    int VTIndex = 0;
    int Season = SDK::Fortnite_Version.GetMajorVersion();
    
    if (Season == 13) VTIndex = 0x70;
    else if (Season == 14 || SDK::Fortnite_Version <= 15.2) VTIndex = 0x71;
    else if (SDK::Fortnite_Version >= 15.3 && Season < 18) VTIndex = 0x72;
    else if (Season == 18) VTIndex = 0x73;
    else if (Season >= 19 && Season < 21) VTIndex = 0x7A;
    else if (Season == 20) VTIndex = 0x7B;
    else if (Season >= 21) VTIndex = 0x7C;

    return VTIndex;
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
    auto sRef = Memcury::Scanner::FindStringRef(
        L"MCP-Profile: Dispatching request to %s",
        false,
        0,
        SDK::Fortnite_Version >= FFortniteVersion(19,0,0)
    ).Get();

    if (!sRef) return 0;

    for (int i = 0; i < 1000; i++) {
        if (*(uint8_t*)(uint8_t*)(sRef - i) == 0x48 && *(uint8_t*)(uint8_t*)(sRef - i + 1) == 0x89 &&
            *(uint8_t*)(uint8_t*)(sRef - i + 2) == 0x5C) {
            return sRef - i;
        }

        if (*(uint8_t*)(uint8_t*)(sRef - i) == 0x48 && *(uint8_t*)(uint8_t*)(sRef - i + 1) == 0x8B &&
            *(uint8_t*)(uint8_t*)(sRef - i + 2) == 0xC4) {
            return sRef - i;
        }
    }
    return 0;
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

	auto Addrr = Memcury::Scanner::FindStringRef(L"AFortPlayerState::ApplyCharacterCustomization - Failed initialization, using default parts. Player Controller: %s PlayerState: %s, HeroId: %s", false, 0, SDK::Fortnite_Version >= 20, true).Get();

	if (!Addrr) return 0;

	for (int i = 0; i < 7000; i++)
	{
		if (*(uint8_t*)(uint8_t*)(Addrr - i) == 0x40 && *(uint8_t*)(uint8_t*)(Addrr - i + 1) == 0x53)
		{
			return Addrr - i;
		}

		if (SDK::Fortnite_Version >= 15)
		{
			if (*(uint8_t*)(uint8_t*)(Addrr - i) == 0x48 && *(uint8_t*)(uint8_t*)(Addrr - i + 1) == 0x89 && *(uint8_t*)(uint8_t*)(Addrr - i + 2) == 0x5C)
			{
				return Addrr - i;
			}
		}

		if (*(uint8_t*)(uint8_t*)(Addrr - i) == 0x48 && *(uint8_t*)(uint8_t*)(Addrr - i + 1) == 0x8B && *(uint8_t*)(uint8_t*)(Addrr - i + 2) == 0xC4)
		{
			return Addrr - i;
		}
	}

	uint64 Addr = Memcury::Scanner::FindPattern("48 8B C4 48 89 50 10 55 57 48 8D 68 A1 48 81 EC ? ? ? ? 80 B9").Get();

	return Addr;
}

uint64 UFinder::KickPlayer()
{
    if (SDK::Fortnite_Version < 1.8) {
        return Memcury::Scanner::FindPattern(
            "40 53 56 48 81 EC ? ? ? ? 48 8B DA 48 8B F1 E8 ? ? ? ? 48 8B 06 48 8B CE"
        ).Get();
    }

    if (SDK::Fortnite_Version == FFortniteVersion(18,0,0)) {
        return Memcury::Scanner::FindPattern(
            "48 8B C4 48 89 58 08 48 89 70 10 48 89 78 18 4C 89 60 20 55 41 56 41 57 48 8B EC 48 83 EC 60 48 83 65 ? ? 4C 8B F2 83 65 E8 00 4C 8B E1 83 65 EC"
        ).Get();
    }

    if (SDK::Fortnite_Version == FFortniteVersion(19,0,0)) {
        return Memcury::Scanner::FindPattern(
            "48 89 5C 24 ? 55 56 57 48 8B EC 48 83 EC 60 48 8B FA 48 8B F1 E8"
        ).Get();
    }

    if (SDK::Fortnite_Version >= 8.00 || SDK::Fortnite_Version <= 12.20) {
        return Memcury::Scanner::FindPattern(
            "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC ? 49 8B F0 48 8B DA 48 85 D2"
        ).Get();
    }

    uint64 Ret = 0;
    auto Addr = Memcury::Scanner::FindStringRef(
        L"Validation Failure: %s. kicking %s",
        false,
        0,
        SDK::Fortnite_Version >= FFortniteVersion(19,0,0)
    );

    if (Addr.Get()) {
        Ret = Addr.Get() ? FindBytes(Addr, { 0x40, 0x55 }, 1000, 0, true) : Ret;
        if (!Ret) {
            Ret = Addr.Get() ? FindBytes(Addr, { 0x40, 0x53 }, 2000, 0, true) : Ret;
        }
    }

    if (Ret) return Ret;

    auto Addr2 = Memcury::Scanner::FindStringRef(L"Failed to kick player");
    auto Addrr = Addr2.Get();

    for (int i = 0; i < 3000; i++) {
        if (*(uint8_t*)(uint8_t*)(Addrr - i) == 0x48 && *(uint8_t*)(uint8_t*)(Addrr - i + 1) == 0x89 &&
            *(uint8_t*)(uint8_t*)(Addrr - i + 2) == 0x5C) {
            return Addrr - i;
        }

        if (SDK::Fortnite_Version >= FFortniteVersion(17,0,0)) {
            if (*(uint8_t*)(uint8_t*)(Addrr - i) == 0x48 && *(uint8_t*)(uint8_t*)(Addrr - i + 1) == 0x8B &&
                *(uint8_t*)(uint8_t*)(Addrr - i + 2) == 0xC4) {
                return Addrr - i;
            }
        }
    }

    return Memcury::Scanner::FindPattern(
        "40 53 41 56 48 81 EC ? ? ? ? 48 8B 01 48 8B DA 4C 8B F1 FF 90"
    ).Get();
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
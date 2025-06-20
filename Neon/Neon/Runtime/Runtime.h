#pragma once
#include "pch.h"

namespace Funcs {
	static inline auto StaticFindObject = (SDK::UObject* (*)(SDK::UClass*, SDK::UObject*, const wchar_t*, bool)) (uint64_t(GetModuleHandle(0)) + 0x4795C44);
};

namespace Runtime
{
	static uint32_t GetFunctionIdx(UFunction* Function)
	{
		uint32_t Idx = 0;
		if (!Function)
			return Idx;
		uint64_t ExecFunctionAddress = reinterpret_cast<uint64_t>(Function->ExecFunction);

		Memcury::Scanner Scanner(ExecFunctionAddress);

		std::string FuncName = Function->GetName() + "_Validate";
	
		std::vector<BYTE> BytesToScanFor = { 0xFF, 0x90 };
		Scanner.ScanFor(BytesToScanFor, true, Memcury::Scanner::FindStringRef(
			std::wstring(FuncName.begin(), FuncName.end())
		).Get() != 0);
		
		if (Scanner.Get() == 0)
			return Idx;

		Scanner.AbsoluteOffset(2);

		Idx = *Scanner.GetAs<uint32_t*>();

		Idx = (Idx / 8);

		return Idx;
	}
	
	static void VFTHook(void** vft, int idx, void* newFunc, void** original = nullptr)
	{
		if (original)
		{
			*original = vft[idx];
		}

		DWORD dwProt;
		VirtualProtect(&vft[idx], sizeof(void*), PAGE_EXECUTE_READWRITE, &dwProt);
		vft[idx] = newFunc;
		DWORD dwTemp;
		VirtualProtect(&vft[idx], sizeof(void*), dwProt, &dwTemp);
	}

	static void Hook(uint64 Address, void* Detour, void** OG = nullptr)
	{
		MH_CreateHook(LPVOID(Address), Detour, OG);
		MH_EnableHook(LPVOID(Address));
	}
	
	static void Patch(uintptr_t ptr, uint8_t byte)
	{
		DWORD og;
		VirtualProtect(LPVOID(ptr), sizeof(byte), PAGE_EXECUTE_READWRITE, &og);
		*(uint8_t*)ptr = byte;
		VirtualProtect(LPVOID(ptr), sizeof(byte), og, &og);
	}

	static bool PatchDword(void* address, uint32_t newValue) {
		DWORD oldProtect;
		if (!VirtualProtect(address, sizeof(uint32_t), PAGE_EXECUTE_READWRITE, &oldProtect))
			return false;

		*(uint32_t*)address = newValue;

		VirtualProtect(address, sizeof(uint32_t), oldProtect, &oldProtect);
		return true;
	}
	    
	template <typename T>
	inline T* StaticFindObject(std::string ObjectPath, UClass* Class = UObject::GetClass()) {
		return (T*)Funcs::StaticFindObject(Class, nullptr, std::wstring(ObjectPath.begin(), ObjectPath.end()).c_str(), false);
	}

	__forceinline static void HookVFT(void** _Vt, uint32_t _Ind, void* _Detour)
	{
		DWORD _Vo;
		VirtualProtect(_Vt + _Ind, 8, PAGE_EXECUTE_READWRITE, &_Vo);
		_Vt[_Ind] = _Detour;
		VirtualProtect(_Vt + _Ind, 8, _Vo, &_Vo);
	}
	
	inline void* nullptrForHook = nullptr;
	
	template<typename Ct, typename Ot = void*>
	__forceinline static void ExecVFT(const char* Name, void* Detour, Ot& Orig = nullptrForHook) {
		auto Fn = StaticFindObject<UFunction>(Name);
		if (!Fn) return;

		uint32_t Ind = GetFunctionIdx(Fn);
		auto Vt = Ct::GetDefaultObj()->VTable;
    
		if (!std::is_same_v<Ot, void*>)
			Orig = (Ot)Vt[Ind];
    
		DWORD Vo;
		VirtualProtect(Vt + Ind, sizeof(void*), PAGE_EXECUTE_READWRITE, &Vo);
		Vt[Ind] = Detour;
		VirtualProtect(Vt + Ind, sizeof(void*), Vo, &Vo);
	}
}
#pragma once
#include "pch.h"
#include "../Finder/Header/Finder.h"

namespace Funcs {
	static inline auto StaticFindObject = (SDK::UObject* (*)(SDK::UClass*, SDK::UObject*, const wchar_t*, bool)) (uint64_t(Finder->StaticFindObject()));
};


template<int32 _Sl>
struct DefaultObjChars
{
	char _Ch[_Sl + 9];

	consteval DefaultObjChars(const char(&_St)[_Sl])
	{
		copy_n("Default__", 9, _Ch);
		copy_n(_St, _Sl, _Ch + 9);
	}

	operator const char* () const
	{
		return static_cast<const char*>(_Ch);
	}
};
template<int32 _Sl>
struct ConstexprString
{
	char _Ch[_Sl];

	consteval ConstexprString(const char(&_St)[_Sl])
	{
		copy_n(_St, _Sl, _Ch);
	}

	operator const char* () const
	{
		return static_cast<const char*>(_Ch);
	}
};

namespace Runtime
{
	static uint32_t GetFunctionIdx(UFunction* Function)
	{
		uint32_t Idx = 0;
		if (!Function)
			return Idx;
		uint64_t ExecFunctionAddress = reinterpret_cast<uint64_t>(Function->GetNativeFunc());

		Memcury::Scanner Scanner(ExecFunctionAddress);

		std::string FuncName = Function->GetFName().ToString().ToString() + "_Validate";
	
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

	inline void* nullptrForHook = nullptr;

	int32 GetVTableIndex(UFunction* Func) {
		if (!Func) return -1;
		auto ValidateName = Func->GetFName().ToString().ToString() + "_Validate";
		auto ValidateRef = Memcury::Scanner::FindStringRef(std::wstring(ValidateName.begin(), ValidateName.end()).c_str(), false);

		auto Addr = ValidateRef.Get();

		if (!Addr) {
			Addr = __int64(Func->GetNativeFunc());
		}

		if (Addr) {
			for (int i = 0; i < 2000; i++) {
				if (*((uint8*)Addr + i) == 0xFF && (*((uint8*)Addr + i + 1) == 0x90 || *((uint8*)Addr + i + 1) == 0x93 || *((uint8*)Addr + i + 1) == 0xA0)) {
					auto VTIndex = *(uint32_t*)(Addr + i + 2);

					return VTIndex / 8;
				}
			}
		}
		return -1;
	}
	
	template<auto ClassFunc, typename T = void*>
	__forceinline void Exec(const char* FuncName, void* detour, T& og = nullptrForHook) {
		UClass* ClassName = ClassFunc();
		if (!ClassName) return;
    
		UObject* DefaultObj = ClassName->GetClassDefaultObject();
		auto VTable = DefaultObj->GetVTable();
    
		UFunction* Function = DefaultObj->GetFunction(FuncName);
		if (!Function) return;
    
		int idx = GetVTableIndex(Function);
    
		if (!std::is_same_v<T, void*>)
			og = (T)VTable[idx];

		DWORD vpog;
		VirtualProtect(VTable + idx, 8, PAGE_EXECUTE_READWRITE, &vpog);
		VTable[idx] = detour;
		VirtualProtect(VTable + idx, 8, vpog, &vpog);
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
	inline T* StaticFindObject(std::string ObjectPath, UClass* Class = UObject::StaticClass()) {
		return (T*)Funcs::StaticFindObject(Class, nullptr, std::wstring(ObjectPath.begin(), ObjectPath.end()).c_str(), false);
	}

	__forceinline static void HookVFT(void** _Vt, uint32_t _Ind, void* _Detour)
	{
		DWORD _Vo;
		VirtualProtect(_Vt + _Ind, 8, PAGE_EXECUTE_READWRITE, &_Vo);
		_Vt[_Ind] = _Detour;
		VirtualProtect(_Vt + _Ind, 8, _Vo, &_Vo);
	}
	
	template<typename Ct, typename Ot = void*>
	__forceinline static void ExecVFT(const char* Name, void* Detour, Ot& Orig = nullptrForHook) {
		auto Fn = StaticFindObject<UFunction>(Name);
		if (!Fn) return;

		uint32_t Ind = GetFunctionIdx(Fn);
		auto Vt = Ct::GetDefaultObj()->GetVTable();
    
		if (!std::is_same_v<Ot, void*>)
			Orig = (Ot)Vt[Ind];
    
		DWORD Vo;
		VirtualProtect(Vt + Ind, sizeof(void*), PAGE_EXECUTE_READWRITE, &Vo);
		Vt[Ind] = Detour;
		VirtualProtect(Vt + Ind, sizeof(void*), Vo, &Vo);
	}
}
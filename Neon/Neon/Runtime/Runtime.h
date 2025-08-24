#pragma once
#include "pch.h"
#include "../Finder/Header/Finder.h"

namespace Funcs {
	static inline auto StaticFindObject = (SDK::UObject* (*)(SDK::UClass*, SDK::UObject*, const wchar_t*, bool)) (uint64_t(Finder->StaticFindObject()));
	static inline auto StaticLoadObject = (SDK::UObject* (*)(SDK::UClass*, SDK::UObject*, const TCHAR*, const TCHAR*, uint32_t, SDK::UObject*, bool)) (uint64_t(Finder->StaticLoadObject()));
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

class FOutputDevice
{
public:
	bool bSuppressEventTag;
	bool bAutoEmitLineTerminator;
	uint8_t _Padding1[0x6];
};

class Finder {
public:
	static uintptr_t GetCompiledInPattern() {
		static uintptr_t pattern = uintptr_t();
		if (!pattern) pattern = Memcury::Scanner::FindPattern("48 8B 41 20 4C 8B D2 48 8B D1 44 0F B6 08 48 FF").Get();
		return pattern;
	}
   
	static uintptr_t GetExplicitPropPattern() {
		static uintptr_t pattern = uintptr_t();
		if (!pattern) pattern = Memcury::Scanner::FindPattern("41 8B 40 ? 4D 8B C8").Get();
		return pattern;
	}
};

struct FFrame : public FOutputDevice
{
public:
    void** VFT;
    UFunction* Node;
    UObject* Object;
    uint8* Code;
    uint8* Locals;
    void* MostRecentProperty;
    uint8_t* MostRecentPropertyAddress;

private:
    static bool IsUsingFField() {
        return Fortnite_Version >= 11.00f;
    }

    static void* GetNext(void* Property) {
        if (!Property) return nullptr;
        
        if (IsUsingFField()) {
            return *(void**)((uint8_t*)Property + 0x28);
        } else {
            return *(void**)((uint8_t*)Property + 0x28);
        }
    }

public:
    void*& GetPropertyChainForCompiledIn()
    {
        static auto PropertyChainForCompiledInOffset = 0x80;
        return *(void**)(__int64(this) + PropertyChainForCompiledInOffset);
    }

    uint8_t*& GetMostRecentPropertyAddress()
    {
        return MostRecentPropertyAddress;
    }

    __forceinline void StepExplicitProperty(void* const Result, void* Property)
    {
        static void (*StepExplicitPropertyOriginal)(__int64 frame, void* const Result, void* Property) = decltype(StepExplicitPropertyOriginal)(Finder::GetExplicitPropPattern());
        StepExplicitPropertyOriginal(__int64(this), Result, Property);
    }

    __forceinline void Step(UObject* Context, RESULT_DECL)
    {
        static void (*StepOriginal)(__int64 frame, UObject* Context, RESULT_DECL) = decltype(StepOriginal)(Finder::GetCompiledInPattern());
        StepOriginal(__int64(this), Context, RESULT_PARAM);
    }

    __forceinline void StepCompiledIn(void* const Result, bool bPrint = false)
    {
        if (Code)
        {
            Step(Object, Result);
        }
        else
        {
            void* Property = GetPropertyChainForCompiledIn();
            if (Property) {
                GetPropertyChainForCompiledIn() = GetNext(Property);
                StepExplicitProperty(Result, Property);
            }
        }
    }

    template<typename TNativeType>
    __forceinline TNativeType& StepCompiledInRef(void* const TemporaryBuffer)
    {
        GetMostRecentPropertyAddress() = nullptr;

        if (Code)
        {
            Step(Object, TemporaryBuffer);
        }
        else
        {
            void* Property = GetPropertyChainForCompiledIn();
            if (Property) {
                GetPropertyChainForCompiledIn() = GetNext(Property);
                StepExplicitProperty(TemporaryBuffer, Property);
            }
        }

        return (GetMostRecentPropertyAddress() != nullptr) ? *(TNativeType*)(GetMostRecentPropertyAddress()) : *(TNativeType*)TemporaryBuffer;
    }
    
    template <typename T>
    T& StepCompiledInRef() {
        T TempVal{};
        GetMostRecentPropertyAddress() = nullptr;

        if (Code)
        {
            static void (*CompiledInFunc)(FFrame*, UObject*, void* const) = (decltype(CompiledInFunc))Finder::GetCompiledInPattern();
            CompiledInFunc(this, Object, &TempVal);
        }
        else
        {
            void* Property = GetPropertyChainForCompiledIn();
            if (Property) {
                GetPropertyChainForCompiledIn() = GetNext(Property);
                ((void (*)(FFrame*, void* const, void*))(Finder::GetExplicitPropPattern()))(this, &TempVal, Property);
            }
        }

        return GetMostRecentPropertyAddress() ? *(T*)GetMostRecentPropertyAddress() : TempVal;
    }

    void IncrementCode() {
        Code = (uint8_t*)(__int64(Code) + (bool)Code);
    }
};

template<typename ClassType>
class TSubclassOf
{
	class UClass* ClassPtr;

public:
	TSubclassOf() = default;

	inline TSubclassOf(UClass* Class)
		: ClassPtr(Class)
	{
	}

	inline UClass* Get()
	{
		return ClassPtr;
	}

	inline operator UClass* () const
	{
		return ClassPtr;
	}

	template<typename Target, typename = std::enable_if<std::is_base_of_v<Target, ClassType>, bool>::type>
	inline operator TSubclassOf<Target>() const
	{
		return ClassPtr;
	}

	inline UClass* operator->()
	{
		return ClassPtr;
	}

	inline TSubclassOf& operator=(UClass* Class)
	{
		ClassPtr = Class;

		return *this;
	}

	inline bool operator==(const TSubclassOf& Other) const
	{
		return ClassPtr == Other.ClassPtr;
	}

	inline bool operator!=(const TSubclassOf& Other) const
	{
		return ClassPtr != Other.ClassPtr;
	}

	inline bool operator==(UClass* Other) const
	{
		return ClassPtr == Other;
	}

	inline bool operator!=(UClass* Other) const
	{
		return ClassPtr != Other;
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

static inline void ModifyInstructionLEA(uintptr_t instrAddr, uintptr_t targetAddr, int offset)
{
    int64_t delta = static_cast<int64_t>(targetAddr) - 
                    static_cast<int64_t>(instrAddr + offset + 4);

    auto patchLocation = reinterpret_cast<int32_t*>(instrAddr + offset);

    DWORD oldProtect;
    VirtualProtect(patchLocation, sizeof(int32_t), PAGE_EXECUTE_READWRITE, &oldProtect);

    *patchLocation = static_cast<int32_t>(delta);

    DWORD temp;
    VirtualProtect(patchLocation, sizeof(int32_t), oldProtect, &temp);
}
	
	__forceinline static void ModifyInstruction(uintptr_t pinstrAddr, uintptr_t pDetourAddr)
	{
		uint8_t *DetourAddr = (uint8_t *)pDetourAddr;
		uint8_t *instrAddr = (uint8_t *)pinstrAddr;
		int64_t delta = (int64_t)(DetourAddr - (instrAddr + 5));
		auto addr = reinterpret_cast<int32_t *>(instrAddr + 1);
		DWORD dwProtection;
		VirtualProtect(addr, 4, PAGE_EXECUTE_READWRITE, &dwProtection);

		*addr = static_cast<int32_t>(delta);

		DWORD dwTemp;
		VirtualProtect(addr, 4, dwProtection, &dwTemp);
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

	inline int GetOffset(UObject* Obj, const std::string& PropName) {
		int Offset = -1;
		for (UStruct* Struct = Obj->GetClass(); Struct;
			Struct = Struct->GetSuperStruct()) {
			auto FuncInfo = SDK::PropLibrary->GetPropertyByName(
				Struct->GetFName().ToString().ToString(), PropName);

			Offset = FuncInfo.Offset;

			if (Offset != -1)
				break;
			}
		return Offset;
	}

	inline int GetOffsetClass(const std::string& Class, UObject* Obj, const std::string& PropName)
	{
		int Offset = -1;
		auto FuncInfo = SDK::PropLibrary->GetPropertyByName(
			Class, PropName);

		Offset = FuncInfo.Offset;

		if (Offset != -1)
			return 0;
			
		return Offset;
	}

	inline int GetOffsetStruct(const std::string& StructName, const std::string& PropName) {
		int Offset = -1;
		for (UStruct* Struct = (UStruct*)GUObjectArray.FindObject(StructName); Struct;
			Struct = Struct->GetSuperStruct()) {
			auto FuncInfo = SDK::PropLibrary->GetPropertyByName(
				Struct->GetFName().ToString().ToString(), PropName);

			Offset = FuncInfo.Offset;

			if (Offset != -1)
				break;
		}
		return Offset;
	}

	template <typename T>
inline T* StaticFindObject(std::string ObjectPath, UClass* Class = UObject::StaticClass()) {
		return (T*)Funcs::StaticFindObject(Class, nullptr, std::wstring(ObjectPath.begin(), ObjectPath.end()).c_str(), false);
	}
	
	
	inline int GetOffsetStructStatic(const std::string& StructName, const std::string& PropName) {
		int Offset = -1;
		for (UStruct* Struct = Runtime::StaticFindObject<UStruct>(StructName); Struct;
			Struct = Struct->GetSuperStruct()) {
			auto FuncInfo = SDK::PropLibrary->GetPropertyByName(
				Struct->GetFName().ToString().ToString(), PropName);

			Offset = FuncInfo.Offset;

			if (Offset != -1)
				break;
			}
		return Offset;
	}
	
	inline void* nullptrForHook = nullptr;

	inline int32 GetVTableIndex(UFunction* Func) {
		if (!Func) return -1;
		auto ValidateName = Func->GetFName().ToString().ToString() + "_Validate";
		auto ValidateRef = Memcury::Scanner::FindStringRef(std::wstring(ValidateName.begin(), ValidateName.end()).c_str(), false);

		auto Addr = ValidateRef.Get();
 
		if (!Addr) {
			Addr = __int64(Func->GetNativeFunc());
		}

		if (Addr) {
			for (int i = 0; i < 4000; i++) {
				if (*((uint8*)Addr + i) == 0xFF && (*((uint8*)Addr + i + 1) == 0x90 || *((uint8*)Addr + i + 1) == 0x93 || *((uint8*)Addr + i + 1) == 0xA0)) {
					auto VTIndex = *(uint32_t*)(Addr + i + 2);
					UE_LOG(LogNeon, Log, "VTable Index for %s: %x", Func->GetFName().ToString().ToString().c_str(), VTIndex / 8);
					return VTIndex / 8;
				}
			}
		}
		return -1;
	}
	
	template<typename T>
T* StaticLoadObject(std::string name) {
		T* Object = StaticFindObject<T>(name);
		if (!Object) {
			auto Name = std::wstring(name.begin(), name.end()).c_str();
			UObject* BaseObject = Funcs::StaticLoadObject(T::StaticClass(), nullptr, Name, nullptr, 0, nullptr, false);
			Object = static_cast<T*>(BaseObject);
		}
		return Object;
	}

	template<typename T>
T* StaticLoadObject(std::string name, UClass* ClassToLoad) {
		T* Object = StaticFindObject<T>(name);
		if (!Object) {
			auto Name = std::wstring(name.begin(), name.end()).c_str();
			UObject* BaseObject = Funcs::StaticLoadObject(ClassToLoad, nullptr, Name, nullptr, 0, nullptr, false);
			Object = static_cast<T*>(BaseObject);
		}
		return Object;
	}

	template<typename T>
T* StaticLoadObjectOnly(std::string name) {
		T* Object = nullptr;
		if (!Object) {
			auto Name = std::wstring(name.begin(), name.end()).c_str();
			UObject* BaseObject = Funcs::StaticLoadObject(T::StaticClass(), nullptr, Name, nullptr, 0, nullptr, false);
			Object = static_cast<T*>(BaseObject);
		}
		return Object;
	}

	__forceinline static void Exec(const char* _Name, void* _Detour, void** original = nullptr) {
		UFunction* Function = StaticFindObject<UFunction>(_Name);
		if (!Function) return;
		
		if (original)
		{
			*original = Function->GetNativeFunc();
		}
		
		Function->SetNativeFunc(reinterpret_cast<FNativeFuncPtr>(_Detour));

		UE_LOG(LogNeon, Log, "Hooked function: %s", _Name);
	}

	template <typename _Ct>
	__forceinline static void Every(uint32_t Ind, void* Detour) {
		for (int i = 0; i < GUObjectArray.GetAllocatedSize(); i++) {
			auto Obj = GUObjectArray.IndexToObject(i);
			if (Obj && Obj->Object->GetClass()->IsA(_Ct::StaticClass())) {
				VFTHook(Obj->Object->GetVTable(), Ind, Detour);
			}
		}
	}
	
	template<auto ClassFunc, typename T = void*>
	__forceinline void Hook(const char* FuncName, void* detour, T& og = nullptrForHook) {
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

	template<auto ClassFunc, typename T = void*>
__forceinline void HookStatic(const char* FuncName, void* detour, T& og = nullptrForHook) {
		UClass* ClassName = ClassFunc();
		if (!ClassName) return;
    
		UObject* DefaultObj = ClassName->GetClassDefaultObject();
		auto VTable = DefaultObj->GetVTable();
    
		UFunction* Function = StaticFindObject<UFunction>(FuncName);
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
		if (Address == 0x0) return;
		MH_CreateHook(LPVOID(Address), Detour, OG);
		MH_EnableHook(LPVOID(Address));
	}
	
	struct Info {
		uintptr_t address;
		uint8_t originalByte;
	};

	static std::vector<Info> patches;

	static void Patch(uintptr_t ptr, uint8_t byte)
	{
		DWORD og;
		VirtualProtect(LPVOID(ptr), sizeof(byte), PAGE_EXECUTE_READWRITE, &og);
    
		uint8_t originalByte = *(uint8_t*)ptr;
		patches.push_back({ptr, originalByte});
    
		*(uint8_t*)ptr = byte;
		VirtualProtect(LPVOID(ptr), sizeof(byte), og, &og);
	}

	inline void PatchBytes(uint64 addr, const std::vector<uint8_t>& Bytes)
	{
		if (!addr)
			return;

		for (int i = 0; i < Bytes.size(); i++)
		{
			Patch(addr + i, Bytes.at(i));
		}
	}

	static void Unpatch(uintptr_t ptr)
	{
		for (int i = patches.size() - 1; i >= 0; i--) {
			if (patches[i].address == ptr) {
				DWORD og;
				VirtualProtect(LPVOID(ptr), 1, PAGE_EXECUTE_READWRITE, &og);
				*(uint8_t*)ptr = patches[i].originalByte;
				VirtualProtect(LPVOID(ptr), 1, og, &og);
            
				patches.erase(patches.begin() + i);
				break;
			}
		}
	}

	static bool PatchDword(void* address, uint32_t newValue) {
		DWORD oldProtect;
		if (!VirtualProtect(address, sizeof(uint32_t), PAGE_EXECUTE_READWRITE, &oldProtect))
			return false;

		*(uint32_t*)address = newValue;

		VirtualProtect(address, sizeof(uint32_t), oldProtect, &oldProtect);
		return true;
	}

	__forceinline static void HookVFT(void** _Vt, uint32_t _Ind, void* _Detour)
	{
		DWORD _Vo;
		VirtualProtect(_Vt + _Ind, 8, PAGE_EXECUTE_READWRITE, &_Vo);
		_Vt[_Ind] = _Detour;
		VirtualProtect(_Vt + _Ind, 8, _Vo, &_Vo);
	}
	
	inline void* ResizeVirtualMemory(void* oldMemory, SIZE_T oldSize, SIZE_T newSize)
	{
		void* newMemory = VirtualAlloc(nullptr, newSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (!newMemory)
			return nullptr;

		memcpy(newMemory, oldMemory, oldSize);
		VirtualFree(oldMemory, 0, MEM_RELEASE);
		return newMemory;
	}
	
	template<typename Ct, typename Ot = void*>
	__forceinline static void ExecVFT(const char* Name, void* Detour, void** original = nullptr) {
		auto Fn = StaticFindObject<UFunction>(Name);
		if (!Fn) return;

		uint32_t Ind = GetFunctionIdx(Fn);
		auto Vt = Ct::GetDefaultObj()->GetVTable();
    
		if (!std::is_same_v<Ot, void*>)
			*original = (Ot)Vt[Ind];
    
		DWORD Vo;
		VirtualProtect(Vt + Ind, sizeof(void*), PAGE_EXECUTE_READWRITE, &Vo);
		Vt[Ind] = Detour;
		VirtualProtect(Vt + Ind, sizeof(void*), Vo, &Vo);
	}

	template<typename T>
std::vector<T*> GetObjectsOfClass(UClass* Class = T::StaticClass())
	{
		std::vector<T*> ArrayOfObjects;
		for (int i = 0; i < GUObjectArray.GetObjectArrayNum(); i++)
		{
			SDK::FUObjectItem *IndexedObject =
							 SDK::GUObjectArray.IndexToObject( i );
			if ( !IndexedObject )
				continue;
			UObject *Object = static_cast<UObject*>(IndexedObject->Object);
			if ( !Object )
				continue;

			if (Object)
			{
				if (Object->IsA(Class))
				{
					if (Object->GetFName().ToString().ToString().contains("Default"))
						continue;

					if (Object->GetFName().ToString().ToString().contains("Test"))
						continue;
					
					ArrayOfObjects.push_back(Cast<T>((UObject*)Object));
				}
			}
		}

		return ArrayOfObjects;
	}
}

    
#define DefUHookOg(_Name) static inline void (*_Name##OG)(UObject*, FFrame&); static void _Name(UObject*, FFrame&)

#define callExecOG(_Tr, _Pt, _Th, _ParamsStruct) ([&](){ \
auto _Fn = Runtime::StaticFindObject<UFunction>(_Pt "." # _Th); \
if (_Fn) { \
_Fn->SetNativeFunc((FNativeFuncPtr) _Th##OG); \
_Tr->ProcessEvent(_Fn, &_ParamsStruct); \
_Fn->SetNativeFunc((FNativeFuncPtr) _Th); \
} \
})()
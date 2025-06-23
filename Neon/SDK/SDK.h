#pragma once
#include "pch.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include "memcury.h"
#include <array>
#include <functional>
#include <regex>

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


class FMemory {
public:
	static inline auto InternalRealloc = ([]() {
		return (void* (*)(void*, __int64, unsigned int)) Memcury::Scanner::FindPattern("48 89 5C 24 08 48 89 74 24 10 57 48 83 EC ? 48 8B F1 41 8B D8 48 8B 0D ? ? ? ?").Get();
		})();

public:
	template <typename _Dt = uint8>
	static _Dt* Malloc(int32 Count, int32 Alignment = alignof(_Dt)) {
		return (_Dt*)InternalRealloc(nullptr, Count * sizeof(_Dt), Alignment);
	}

	template <typename _Dt = uint8>
	static _Dt* Realloc(_Dt* Ptr, int32 Count, int32 Alignment = alignof(_Dt)) {
		return (_Dt*)InternalRealloc(Ptr, Count * sizeof(_Dt), Alignment);
	}

	static void Free(void* Ptr) {
		InternalRealloc(Ptr, 0, 0);
	}
};

template <typename _Et>
class TArray {
public:
	_Et* Data;
	int32 NumElements;
	int32 MaxElements;

private:
	inline int32 GetSlack() const { return MaxElements - NumElements; }

	inline void VerifyIndex(int32 Index) const { if (!IsValidIndex(Index)) throw out_of_range("Index was out of range!"); }

	inline       _Et& GetUnsafe(int32 Index) { return Data[Index]; }
	inline const _Et& GetUnsafe(int32 Index) const { return Data[Index]; }


public:
	TArray() : Data(nullptr), NumElements(0), MaxElements(0)
	{
	}

	inline TArray(int32 Size) : NumElements(0), MaxElements(Size), Data(FMemory::Malloc<_Et>(Size))
	{
	}

	inline void Reserve(const int Num)
	{
		if (Num > GetSlack()) Data = FMemory::Realloc<_Et>(Data, MaxElements = Num + NumElements);
	}

	inline _Et& Add(const _Et& Element)
	{
		Reserve(1);

		auto& Elem = Data[NumElements] = Element;
		NumElements++;

		return Elem;
	}

	inline bool Remove(int32 Index)
	{
		if (!IsValidIndex(Index))
			return false;

		NumElements--;

		for (int i = Index; i < NumElements; i++)
		{
			Data[i] = Data[i + 1];
		}

		return true;
	}

	inline void Free()
	{
		if (Data)
			FMemory::Free(Data);

		MaxElements = 0;
		NumElements = 0;
		Data = nullptr;
	}

	inline void ResetNum() {
		NumElements = 0;
	}

	inline void Clear()
	{
		ResetNum();

		if (Data) __stosb((PBYTE)Data, 0, NumElements * sizeof(_Et));
	}

public:
	inline int32 Num() const { return NumElements; }
	inline int32 Max() const { return MaxElements; }

	inline bool IsValidIndex(int32 Index) const { return Data && Index >= 0 && Index < NumElements; }

	inline bool IsValid() const { return Data && NumElements > 0 && MaxElements >= NumElements; }

	_Et* Search(function<bool(_Et& val)> check) {
		for (auto& v : *this) {
			if (check(v)) return &v;
		}
		return nullptr;
	}


	int32_t SearchIndex(function<bool(_Et& val)> check) {
		for (int32_t i = 0; i < Num(); i++) {
			if (check((*this)[i])) return i;
		}
		return -1;
	}

public:
	inline       _Et& operator[](int32 Index) { VerifyIndex(Index); return Data[Index]; }
	inline const _Et& operator[](int32 Index) const { VerifyIndex(Index); return Data[Index]; }


	inline const _Et* GetPtr(int32 Index, int32 Size) const { VerifyIndex(Index); return (_Et*)(__int64(Data) + (Index * Size)); }
	inline const _Et& Get(int32 Index, int32 Size) const { VerifyIndex(Index); return *GetPtr(Index, Size); }

	inline explicit operator bool() const { return IsValid(); };

public:
	inline _Et* begin() { return Data; }
	inline _Et* end() { return Data + Num(); }
};

class FString : public TArray<wchar_t> {
public:
	using TArray::TArray;

	FString(const wchar_t* Str)
	{
		const uint32 Len = static_cast<uint32>(wcslen(Str) + 0x1);

		Data = const_cast<wchar_t*>(Str);
		MaxElements = NumElements = Len;
	}

public:
	inline string ToString() const
	{
		if (*this)
		{
			wstring _Wd(Data);
#pragma warning(suppress: 4244)
			return string(_Wd.begin(), _Wd.end());
		}

		return "";
	}

public:
	inline       wchar_t* c_str() { return Data; }
	inline const wchar_t* c_str() const { return Data; }

public:
	inline bool operator==(const FString& Other) const { return Other && NumElements == Other.NumElements ? wcscmp(Data, Other.Data) == 0 : false; }
	inline bool operator!=(const FString& Other) const { return Other && NumElements != Other.NumElements ? wcscmp(Data, Other.Data) != 0 : true; }
};

class TName {
public:
	int32 ComparisonIndex;
	int32 Number;
	static inline void(*AppendString)(const TName*, FString&) = ([]() {
		auto SRef = Memcury::Scanner::FindStringRef("ForwardShadingQuality_");
		constexpr std::array<const char*, 5> sigs =
		{
			"48 8D ? ? 48 8D ? ? E8",
			"48 8D ? ? ? 48 8D ? ? E8",
			"48 8D ? ? 49 8B ? E8",
			"48 8D ? ? ? 49 8B ? E8",
			"48 8D ? ? 48 8B ? E8"
		};

		for (auto& sig : sigs) {
			auto Scanner = SRef.ScanFor(sig);
			if (Scanner.Get()) {
				auto pb = Memcury::ASM::pattern2bytes(sig);
				return (void(*)(const TName*, FString&)) Scanner.RelativeOffset((uint32_t)pb.size()).Get();
			}
		}
		return (void(*)(const TName*, FString&)) nullptr;
	})();
	
	string ToString() const
	{
		thread_local FString TName_TempString = FString(1024);
		AppendString(this, TName_TempString);

		string OutputString = TName_TempString.ToString();
		TName_TempString.Clear();

		return OutputString;
	}


	wstring ToWString() const
	{
		thread_local FString TName_TempString = FString(1024);
		AppendString(this, TName_TempString);

		wstring OutputString = TName_TempString.c_str();
		TName_TempString.Clear();

		return OutputString;
	}

	wchar_t* GetWStr() const
	{
		thread_local FString TName_TempString = FString(1024);
		TName_TempString.Clear();
		AppendString(this, TName_TempString);

		return TName_TempString.c_str();
	}


	wchar_t* GetSDKWStr() const
	{
		wchar_t* OutputString = GetWStr();

		wchar_t* pos = wcsrchr(OutputString, L'/');

		if (!pos)
			return OutputString;

		return pos + 1;
	}

	string ToSDKString() const
	{
		std::string OutputString = ToString();

		size_t pos = OutputString.rfind('/');

		if (pos == std::string::npos)
			return OutputString;

		return OutputString.substr(pos + 1);
	}
};

class FName : public TName
{
public:
	int32 Number;
};


class FNameLWC : public TName
{
};

static FORCEINLINE uint32 CountLeadingZeros(uint32 Value)
{
    unsigned long Log2;
    if (_BitScanReverse(&Log2, Value) != 0)
    {
        return 31 - Log2;
    }

    return 32;
}

#define NumBitsPerDWORD ((int32)32)
#define NumBitsPerDWORDLogTwo ((int32)5)


template <int NumElements>
class TInlineAllocator
{
private:
    template <int Size, int Alignment>
    struct alignas(Alignment) TAlignedBytes
    {
        unsigned char Pad[Size];
    };

    template <typename ElementType>
    struct TTypeCompatibleBytes : public TAlignedBytes<sizeof(ElementType), alignof(ElementType)>
    {
    };

public:
    template <typename ElementType>
    class ForElementType
    {
        friend class TBitArray;

    private:
        TTypeCompatibleBytes<ElementType> InlineData[NumElements];

        ElementType* SecondaryData;

    public:

        FORCEINLINE int32 NumInlineBytes() const
        {
            return sizeof(ElementType) * NumElements;
        }
        FORCEINLINE int32 NumInlineBits() const
        {
            return NumInlineBytes() * 8;
        }

        FORCEINLINE ElementType& operator[](int32 Index)
        {
            return *(ElementType*)(&InlineData[Index]);
        }
        FORCEINLINE const ElementType& operator[](int32 Index) const
        {
            return *(ElementType*)(&InlineData[Index]);
        }

        FORCEINLINE void operator=(void* InElements)
        {
            SecondaryData = InElements;
        }

        FORCEINLINE ElementType& GetInlineElement(int32 Index)
        {
            return *(ElementType*)(&InlineData[Index]);
        }
        FORCEINLINE const ElementType& GetInlineElement(int32 Index) const
        {
            return *(ElementType*)(&InlineData[Index]);
        }
        FORCEINLINE ElementType& GetSecondaryElement(int32 Index)
        {
            return SecondaryData[Index];
        }
        FORCEINLINE const ElementType& GetSecondaryElement(int32 Index) const
        {
            return SecondaryData[Index];
        }
        ElementType* GetInlineElements() const
        {
            return (ElementType*)InlineData;
        }
        FORCEINLINE ElementType* GetAllocation() const
        {
            return IfAThenAElseB<ElementType>(SecondaryData, GetInlineElements());
        }
    };
};

class TBitArray
{
public:
    TInlineAllocator<4>::ForElementType<unsigned int> Data;
    int NumBits;
    int MaxBits;

    struct FRelativeBitReference
    {
    public:
        FORCEINLINE explicit FRelativeBitReference(int32 BitIndex)
            : DWORDIndex(BitIndex >> NumBitsPerDWORDLogTwo)
            , Mask(1 << (BitIndex & (NumBitsPerDWORD -1)))
        {
        }

        int32 DWORDIndex;
        uint32 Mask;
    };

public:
    struct FBitReference
    {
        FORCEINLINE FBitReference(uint32& InData, uint32 InMask)
            : Data(InData)
            , Mask(InMask)
        {
        }
        FORCEINLINE FBitReference(const uint32& InData, const uint32 InMask)
            : Data(const_cast<uint32&>(InData))
            , Mask(InMask)
        {
        }

        FORCEINLINE void SetBit(const bool Value)
        {
            Value ? Data |= Mask : Data &= ~Mask;

            // 10011101 - Data			 // 10011101 - Data
            // 00000010 - Mask - true |	 // 00000010 - Mask - false
            // 10011111	-  |=			 // 11111101 -  ~
            //							 // 10011111 -  &=
        }

        FORCEINLINE operator bool() const
        {
            return (Data & Mask) != 0;
        }
        FORCEINLINE void operator=(const bool Value)
        {
            this->SetBit(Value);
        }

    private:
        uint32& Data;
        uint32 Mask;
    };

public:
    class FBitIterator : public FRelativeBitReference
    {
    private:
        int32 Index;
        const TBitArray& IteratedArray;

    public:
        FORCEINLINE FBitIterator(const TBitArray& ToIterate, const int32 StartIndex) // Begin
            : IteratedArray(ToIterate)
            , Index(StartIndex)
            , FRelativeBitReference(StartIndex)
        {
        }
        FORCEINLINE FBitIterator(const TBitArray& ToIterate) // End
            : IteratedArray(ToIterate)
            , Index(ToIterate.NumBits)
            , FRelativeBitReference(ToIterate.NumBits)
        {
        }

        FORCEINLINE explicit operator bool() const
        {
            return Index < IteratedArray.Num();
        }
        FORCEINLINE FBitIterator& operator++()
        {
            ++Index;
            this->Mask <<= 1;
            if (!this->Mask)
            {
                this->Mask = 1;
                ++this->DWORDIndex;
            }
            return *this;
        }
        FORCEINLINE bool operator*() const
        {
            // Thesis: Once there are more elements in the BitArray than InlineData can hold it'll just allocate all of 
            // them through SecondaryElements, leaving InlineData all true

            if (IteratedArray.NumBits < IteratedArray.Data.NumInlineBits())
            {
                return (bool)FBitReference(IteratedArray.Data.GetInlineElement(this->DWORDIndex), this->Mask);
            }
            else
            {
                return (bool)FBitReference(IteratedArray.Data.GetSecondaryElement(this->DWORDIndex), this->Mask);
            }
        }
        FORCEINLINE bool operator==(const FBitIterator& OtherIt) const
        {
            return Index == OtherIt.Index;
        }
        FORCEINLINE bool operator!=(const FBitIterator& OtherIt) const
        {
            return Index </*=*/ OtherIt.Index;
        }
        FORCEINLINE bool operator < (const int32 Other) const
        {
            return Index < Other;
        }
        FORCEINLINE bool operator > (const int32 Other) const
        {
            return Index < Other;
        }

        FORCEINLINE int32 GetIndex() const
        {
            return Index;
        }
    };

    class FSetBitIterator : public FRelativeBitReference
    {
    private:
        const TBitArray& IteratedArray;

        uint32 UnvisitedBitMask;
        int32  CurrentBitIndex;
        int32  BaseBitIndex;

    public:
        FORCEINLINE FSetBitIterator(const TBitArray& ToIterate, int32 StartIndex)
            : FRelativeBitReference(StartIndex)
            , IteratedArray(const_cast<TBitArray&>(ToIterate))
            , UnvisitedBitMask((~0U) << (StartIndex & (NumBitsPerDWORD - 1)))
            , CurrentBitIndex(StartIndex)
            , BaseBitIndex(StartIndex & ~(NumBitsPerDWORD - 1))
        {
            if (StartIndex != IteratedArray.NumBits)
            {
                FindNextSetBit();
            }
        }
        FORCEINLINE FSetBitIterator(const TBitArray& ToIterate)
            : FRelativeBitReference(ToIterate.NumBits)
            , IteratedArray(const_cast<TBitArray&>(ToIterate))
            , UnvisitedBitMask(0)
            , CurrentBitIndex(ToIterate.NumBits)
            , BaseBitIndex(ToIterate.NumBits)
        {
        }

        FORCEINLINE FSetBitIterator& operator++()
        {
            UnvisitedBitMask &= ~this->Mask;

            FindNextSetBit();

            return *this;
        }
        FORCEINLINE bool operator*() const
        {
            return true;
        }

        FORCEINLINE bool operator==(const FSetBitIterator& Other) const
        {
            return CurrentBitIndex == Other.CurrentBitIndex;
        }
        FORCEINLINE bool operator!=(const FSetBitIterator& Other) const
        {
            return CurrentBitIndex </*=*/ Other.CurrentBitIndex;
        }

        FORCEINLINE explicit operator bool() const
        {
            return CurrentBitIndex < IteratedArray.NumBits;
        }

        FORCEINLINE int32 GetIndex() const
        {
            return CurrentBitIndex;
        }

    private:

        void FindNextSetBit()
        {
            //InlineData is the first 16-bytes of TBitArray
            const uint32* ArrayData = (IteratedArray.Data.SecondaryData ? IteratedArray.Data.SecondaryData : (uint32*)&IteratedArray.Data.InlineData);

            if (!ArrayData)
                return;

            const int32 ArrayNum = IteratedArray.NumBits;
            const int32 LastDWORDIndex = (ArrayNum - 1) / NumBitsPerDWORD;

            uint32 RemainingBitMask = ArrayData[this->DWORDIndex] & UnvisitedBitMask;

            while (!RemainingBitMask)
            {
                ++this->DWORDIndex;
                BaseBitIndex += NumBitsPerDWORD;

                if (this->DWORDIndex > LastDWORDIndex)
                {
                    CurrentBitIndex = ArrayNum;
                    return;
                }

                RemainingBitMask = ArrayData[this->DWORDIndex];
                UnvisitedBitMask = ~0;
            }

            const uint32 NewRemainingBitMask = RemainingBitMask & (RemainingBitMask - 1);

            this->Mask = NewRemainingBitMask ^ RemainingBitMask;

            CurrentBitIndex = BaseBitIndex + NumBitsPerDWORD - 1 - CountLeadingZeros(this->Mask);

            if (CurrentBitIndex > ArrayNum)
            {
                CurrentBitIndex = ArrayNum;
            }
        }
    };

public:
    FORCEINLINE FBitIterator Iterator(int32 StartIndex)
    {
        return FBitIterator(*this, StartIndex);
    }
    FORCEINLINE FSetBitIterator SetBitIterator(int32 StartIndex)
    {
        return FSetBitIterator(*this, StartIndex);
    }

    FORCEINLINE FBitIterator begin()
    {
        return FBitIterator(*this, 0);
    }
    FORCEINLINE const FBitIterator begin() const
    {
        return FBitIterator(*this, 0);
    }
    FORCEINLINE FBitIterator end()
    {
        return FBitIterator(*this);
    }
    FORCEINLINE const FBitIterator end() const
    {
        return  FBitIterator(*this);
    }

    FORCEINLINE FSetBitIterator SetBitsItBegin()
    {
        return FSetBitIterator(*this, 0);
    }
    FORCEINLINE const FSetBitIterator SetBitsItBegin() const
    {
        return FSetBitIterator(*this, 0);
    }
    FORCEINLINE const FSetBitIterator SetBitsItEnd()
    {
        return FSetBitIterator(*this);
    }
    FORCEINLINE const FSetBitIterator SetBitsItEnd() const
    {
        return FSetBitIterator(*this);
    }

    FORCEINLINE int32 Num() const
    {
        return NumBits;
    }
    FORCEINLINE int32 Max() const
    {
        return MaxBits;
    }
    FORCEINLINE bool IsSet(int32 Index) const
    {
        return *FBitIterator(*this, Index);
    }
    FORCEINLINE void Set(const int32 Index, const bool Value, bool bIsSettingAllZero = false)
    {
        const int32 DWORDIndex = (Index >> ((int32)5));
        const int32 Mask = (1 << (Index & (((int32)32) - 1)));

        if (!bIsSettingAllZero)
            NumBits = Index >= NumBits ? Index < MaxBits ? Index + 1 : NumBits : NumBits;

        FBitReference(Data[DWORDIndex], Mask).SetBit(Value);
    }
    FORCEINLINE void ZeroAll()
    {
        for (int i = 0; i < MaxBits; ++i)
        {
            Set(i, false, true);
        }
    }
};

template <typename ElementType>
class TSetElement
{
public:
    ElementType Value;
    mutable int32 HashNextId;
    mutable int32 HashIndex;

    TSetElement(ElementType InValue, int32 InHashNextId, int32 InHashIndex)
        : Value(InValue)
        , HashNextId(InHashNextId)
        , HashIndex(InHashIndex)
    {
    }

    FORCEINLINE TSetElement<ElementType>& operator=(const TSetElement<ElementType>& Other)
    {
        Value = Other.Value;
    }

    FORCEINLINE bool operator==(const TSetElement& Other) const
    {
        return Value == Other.Value;
    }
    FORCEINLINE bool operator!=(const TSetElement& Other) const
    {
        return Value != Other.Value;
    }
};

#define INDEX_NONE -1

template <typename ElementType>
union TSparseArrayElementOrListLink
{
    TSparseArrayElementOrListLink(ElementType& InElement)
        : ElementData(InElement)
    {
    }
    TSparseArrayElementOrListLink(ElementType&& InElement)
        : ElementData(InElement)
    {
    }

    TSparseArrayElementOrListLink(int32 InPrevFree, int32 InNextFree)
        : PrevFreeIndex(InPrevFree)
        , NextFreeIndex(InNextFree)
    {
    }

    TSparseArrayElementOrListLink<ElementType> operator=(const TSparseArrayElementOrListLink<ElementType>& Other)
    {
        return TSparseArrayElementOrListLink(Other.NextFreeIndex, Other.PrevFreeIndex);
    }

    /** If the element is allocated, its value is stored here. */
    ElementType ElementData;

    struct
    {
        /** If the element isn't allocated, this is a link to the previous element in the array's free list. */
        int PrevFreeIndex;

        /** If the element isn't allocated, this is a link to the next element in the array's free list. */
        int NextFreeIndex;
    };
};

template <typename ArrayType>
class TSparseArray
{
public:
    typedef TSparseArrayElementOrListLink<ArrayType> FSparseArrayElement;

    TArray<FSparseArrayElement> Data;
    TBitArray AllocationFlags;
    int32 FirstFreeIndex;
    int32 NumFreeIndices;

    FORCEINLINE int32 Num() const
    {
        return Data.Num() - NumFreeIndices;
    }

    class FBaseIterator
    {
    private:
        TSparseArray<ArrayType>& IteratedArray;
        TBitArray::FSetBitIterator BitArrayIt;

    public:
        FORCEINLINE FBaseIterator(const TSparseArray<ArrayType>& Array, const TBitArray::FSetBitIterator BitIterator)
            : IteratedArray(const_cast<TSparseArray<ArrayType>&>(Array))
            , BitArrayIt(const_cast<TBitArray::FSetBitIterator&>(BitIterator))
        {
        }

        FORCEINLINE explicit operator bool() const
        {
            return (bool)BitArrayIt;
        }
        FORCEINLINE TSparseArray<ArrayType>::FBaseIterator& operator++()
        {
            ++BitArrayIt;
            return *this;
        }
        FORCEINLINE ArrayType& operator*()
        {
            return IteratedArray[BitArrayIt.GetIndex()].ElementData;
        }
        FORCEINLINE const ArrayType& operator*() const
        {
            return IteratedArray[BitArrayIt.GetIndex()].ElementData;
        }
        FORCEINLINE ArrayType* operator->()
        {
            return &IteratedArray[BitArrayIt.GetIndex()].ElementData;
        }
        FORCEINLINE const ArrayType* operator->() const
        {
            return &IteratedArray[BitArrayIt.GetIndex()].ElementData;
        }
        FORCEINLINE bool operator==(const TSparseArray<ArrayType>::FBaseIterator& Other) const
        {
            return BitArrayIt == Other.BitArrayIt;
        }
        FORCEINLINE bool operator!=(const TSparseArray<ArrayType>::FBaseIterator& Other) const
        {
            return BitArrayIt != Other.BitArrayIt;
        }

        FORCEINLINE int32 GetIndex() const
        {
            return BitArrayIt.GetIndex();
        }
        FORCEINLINE bool IsElementValid() const
        {
            return *BitArrayIt;
        }
    };

public:
    FORCEINLINE TSparseArray<ArrayType>::FBaseIterator begin()
    {
        return TSparseArray<ArrayType>::FBaseIterator(*this, TBitArray::FSetBitIterator(AllocationFlags, 0));
    }
    FORCEINLINE const TSparseArray<ArrayType>::FBaseIterator begin() const
    {
        return TSparseArray<ArrayType>::FBaseIterator(*this, TBitArray::FSetBitIterator(AllocationFlags, 0));
    }
    FORCEINLINE TSparseArray<ArrayType>::FBaseIterator end()
    {
        return TSparseArray<ArrayType>::FBaseIterator(*this, TBitArray::FSetBitIterator(AllocationFlags));
    }
    FORCEINLINE const TSparseArray<ArrayType>::FBaseIterator end() const
    {
        return TSparseArray<ArrayType>::FBaseIterator(*this, TBitArray::FSetBitIterator(AllocationFlags));
    }

    FORCEINLINE FSparseArrayElement& operator[](uint32 Index)
    {
        return *(FSparseArrayElement*)&Data[Index].ElementData;
    }
    FORCEINLINE const FSparseArrayElement& operator[](uint32 Index) const
    {
        return *(const FSparseArrayElement*)&Data.at(Index).ElementData;
    }
    FORCEINLINE int32 GetNumFreeIndices() const
    {
        return NumFreeIndices;
    }
    FORCEINLINE int32 GetFirstFreeIndex() const
    {
        return FirstFreeIndex;
    }
    FORCEINLINE const TArray<FSparseArrayElement>& GetData() const
    {
        return Data;
    }
    FSparseArrayElement& GetData(int32 Index)
    {
        return *(FSparseArrayElement*)&Data.at(Index).ElementData;
    }

    const FSparseArrayElement& GetData(int32 Index) const
    {
        return *(const FSparseArrayElement*)&Data.at(Index).ElementData;
    }
    FORCEINLINE const TBitArray& GetAllocationFlags() const
    {
        return AllocationFlags;
    }
    FORCEINLINE bool IsIndexValid(int32 IndexToCheck) const
    {
        return AllocationFlags.IsSet(IndexToCheck);
    }

    void RemoveAt(int32 Index, int32 Count = 1)
    {
        RemoveAtUninitialized(Index, Count);
    }

    void RemoveAtUninitialized(int32 Index, int32 Count = 1)
    {
        for (; Count; --Count)
        {
            if (NumFreeIndices)
            {
                GetData(FirstFreeIndex).PrevFreeIndex = Index;
            }
            auto& IndexData = GetData(Index);
            IndexData.PrevFreeIndex = -1;
            IndexData.NextFreeIndex = NumFreeIndices > 0 ? FirstFreeIndex : INDEX_NONE;
            FirstFreeIndex = Index;
            ++NumFreeIndices;
            AllocationFlags.Set(Index, false);

            ++Index;
        }
    }
};

template <typename SetType>
class TSet
{
private:
    template <typename>
    friend class TSparseArray;

public:
    typedef TSetElement<SetType> ElementType;
    typedef TSparseArrayElementOrListLink<ElementType> ArrayElementType;

public:
    TSparseArray<ElementType> Elements;

    mutable TInlineAllocator<1>::ForElementType<int> Hash;
    mutable int32 HashSize;

public:
    class FBaseIterator
    {
    private:
        TSet<SetType>& IteratedSet;
        TSparseArray<ElementType>::FBaseIterator ElementIt;

    public:
        FORCEINLINE FBaseIterator(const TSet<SetType>& InSet, TSparseArray<TSetElement<SetType>>::FBaseIterator InElementIt)
            : IteratedSet(const_cast<TSet<SetType>&>(InSet))
            , ElementIt(InElementIt)
        {
        }

        FORCEINLINE explicit operator bool() const
        {
            return (bool)ElementIt;
        }
        FORCEINLINE TSet<SetType>::FBaseIterator& operator++()
        {
            ++ElementIt;
            return *this;
        }
        FORCEINLINE bool operator==(const TSet<SetType>::FBaseIterator& OtherIt) const
        {
            return ElementIt == OtherIt.ElementIt;
        }
        FORCEINLINE bool operator!=(const TSet<SetType>::FBaseIterator& OtherIt) const
        {
            return ElementIt != OtherIt.ElementIt;
        }
        FORCEINLINE TSet<SetType>::FBaseIterator& operator=(TSet<SetType>::FBaseIterator& OtherIt)
        {
            return ElementIt = OtherIt.ElementIt;
        }
        FORCEINLINE SetType& operator*()
        {
            return (*ElementIt).Value;
        }
        FORCEINLINE const SetType& operator*() const
        {
            return &((*ElementIt).Value);
        }
        FORCEINLINE SetType* operator->()
        {
            return &((*ElementIt).Value);
        }
        FORCEINLINE const SetType* operator->() const
        {
            return &(*ElementIt).Value;
        }
        FORCEINLINE const int32 GetIndex() const
        {
            return ElementIt.GetIndex();
        }
        FORCEINLINE ElementType& GetSetElement()
        {
            return *ElementIt;
        }
        FORCEINLINE const ElementType& GetSetElement() const
        {
            return *ElementIt;
        }
        FORCEINLINE bool IsElementValid() const
        {
            return ElementIt.IsElementValid();
        }
    };

public:
    FORCEINLINE TSet<SetType>::FBaseIterator begin()
    {
        return TSet<SetType>::FBaseIterator(*this, Elements.begin());
    }
    FORCEINLINE const TSet<SetType>::FBaseIterator begin() const
    {
        return TSet<SetType>::FBaseIterator(*this, Elements.begin());
    }
    FORCEINLINE TSet<SetType>::FBaseIterator end()
    {
        return TSet<SetType>::FBaseIterator(*this, Elements.end());
    }
    FORCEINLINE const TSet<SetType>::FBaseIterator end() const
    {
        return TSet<SetType>::FBaseIterator(*this, Elements.end());
    }

    FORCEINLINE SetType& operator[](int Index)
    {
        return Elements[Index].ElementData.Value;
    }

    FORCEINLINE int32 Num() const
    {
        return Elements.Num();
    }
    FORCEINLINE bool IsValid() const
    {
        return Elements.Data.Data != nullptr && Elements.AllocationFlags.MaxBits > 0;
    }
    FORCEINLINE TSparseArray<ElementType>& GetElements()
    {
        return Elements;
    }
    FORCEINLINE const TSparseArray<ElementType>& GetElements() const
    {
        return Elements;
    }
    FORCEINLINE const TBitArray& GetAllocationFlags() const
    {
        return Elements.GetAllocationFlags();
    }
    FORCEINLINE bool IsIndexValid(int32 IndexToCheck) const
    {
        return Elements.IsIndexValid(IndexToCheck);
    }
    FORCEINLINE const bool Contains(const SetType& ElementToLookFor) const
    {
        if (Num() <= 0)
            return false;

        for (SetType Element : *this)
        {
            if (Element == ElementToLookFor)
                return true;
        }
        return false;
    }
    FORCEINLINE const int32 Find(const SetType& ElementToLookFor) const
    {
        for (auto It = this->begin(); It != this->end(); ++It)
        {
            if (*It == ElementToLookFor)
            {
                return It.GetIndex();
            }
        }

        return -1;
    }
    FORCEINLINE bool Remove(const SetType& ElementToRemove)
    {
        auto Idx = Find(ElementToRemove);

        if (Idx == -1)
            return false;

        Elements.RemoveAt(Idx);
        return true;
    }
    FORCEINLINE bool Remove(int Index)
    {
        Elements.RemoveAt(Index);
        return true;
    }
};

template <typename KeyType, typename ValueType>
class TPair
{
public:
	KeyType First;
	ValueType Second;

    FORCEINLINE KeyType& Key()
    {
        return First;
    }
    FORCEINLINE const KeyType& Key() const
    {
        return First;
    }
    FORCEINLINE ValueType& Value()
    {
        return Second;
    }
    FORCEINLINE const ValueType& Value() const
    {
        return Second;
    }
};

template <typename KeyType, typename ValueType>
class TMap
{
public:
    typedef TPair<KeyType, ValueType> ElementType;

public:
    TSet<ElementType> Pairs;

public:
    class FBaseIterator
    {
    private:
        TMap<KeyType, ValueType>& IteratedMap;
        TSet<ElementType>::FBaseIterator SetIt;

    public:
        FBaseIterator(TMap<KeyType, ValueType>& InMap, TSet<ElementType>::FBaseIterator InSet)
            : IteratedMap(InMap)
            , SetIt(InSet)
        {
        }
        FORCEINLINE TMap<KeyType, ValueType>::FBaseIterator operator++()
        {
            ++SetIt;
            return *this;
        }
        FORCEINLINE TMap<KeyType, ValueType>::ElementType& operator*()
        {
            return *SetIt;
        }
        FORCEINLINE const TMap<KeyType, ValueType>::ElementType& operator*() const
        {
            return *SetIt;
        }
        FORCEINLINE bool operator==(const TMap<KeyType, ValueType>::FBaseIterator& Other) const
        {
            return SetIt == Other.SetIt;
        }
        FORCEINLINE bool operator!=(const TMap<KeyType, ValueType>::FBaseIterator& Other) const
        {
            return SetIt != Other.SetIt;
        }
        FORCEINLINE bool IsElementValid() const
        {
            return SetIt.IsElementValid();
        }
    };

    FORCEINLINE TMap<KeyType, ValueType>::FBaseIterator begin()
    {
        return TMap<KeyType, ValueType>::FBaseIterator(*this, Pairs.begin());
    }
    FORCEINLINE const TMap<KeyType, ValueType>::FBaseIterator begin() const
    {
        return TMap<KeyType, ValueType>::FBaseIterator(*this, Pairs.begin());
    }
    FORCEINLINE TMap<KeyType, ValueType>::FBaseIterator end()
    {
        return TMap<KeyType, ValueType>::FBaseIterator(*this, Pairs.end());
    }
    FORCEINLINE const TMap<KeyType, ValueType>::FBaseIterator end() const
    {
        return TMap<KeyType, ValueType>::FBaseIterator(*this, Pairs.end());
    }
    FORCEINLINE ValueType& operator[](const KeyType& Key)
    {
        return this->GetByKey(Key);
    }
    FORCEINLINE const ValueType& operator[](const KeyType& Key) const
    {
        return this->GetByKey(Key);
    }
    FORCEINLINE int32 Num() const
    {
        return Pairs.Num();
    }
    FORCEINLINE bool IsValid() const
    {
        return Pairs.IsValid();
    }
    FORCEINLINE bool IsIndexValid(int32 IndexToCheck) const
    {
        return Pairs.IsIndexValid(IndexToCheck);
    }
    FORCEINLINE bool Contains(const KeyType& ElementToLookFor) const
    {
        for (auto& Element : *this)
        {
            if (Element.Key() == ElementToLookFor)
                return true;
        }
        return false;
    }
    FORCEINLINE ValueType& GetByKey(const KeyType& Key, bool* wasSuccessful = nullptr)
    {
        for (auto& Pair : *this)
        {
            if (Pair.Key() == Key)
            {
                if (wasSuccessful)
                    *wasSuccessful = true;

                return Pair.Value();
            }
        }

        if (wasSuccessful)
            *wasSuccessful = false;
    }
    FORCEINLINE ValueType& Find(const KeyType& Key, bool* wasSuccessful = nullptr)
    {
        return GetByKey(Key, wasSuccessful);
    }
    FORCEINLINE ValueType GetByKeyNoRef(const KeyType& Key)
    {
        for (auto& Pair : *this)
        {
            if (Pair.Key() == Key)
            {
                return Pair.Value();
            }
        }
    }
};

inline auto VersionInfo = ([]() {
    struct {
        double FortniteVersion;
        int EngineVersion;
        int FortniteCL;
    } result;

    static FString(*GetEngineVersion)() = decltype(GetEngineVersion)(
        Memcury::Scanner::FindPattern("40 53 48 83 EC 20 48 8B D9 E8 ? ? ? ? 48 8B C8 41 B8 04 ? ? ? 48 8B D3", false).Get()
    );
    
    std::string FullVersion;
    FString toFree;
    
    if (!GetEngineVersion) {
        auto VerStr = Memcury::Scanner::FindPattern("2B 2B 46 6F 72 74 6E 69 74 65 2B 52 65 6C 65 61 73 65 2D ? ? ? ?").Get();
        FullVersion = decltype(FullVersion.c_str())(VerStr);
        result.EngineVersion = 500;
    } else {
        toFree = GetEngineVersion();
        FullVersion = toFree.ToString();
    }
    
    std::string FNVer = FullVersion;
    std::string EngineVer = FullVersion;
    std::string CLStr;
    
    if (!FullVersion.contains("Live") && !FullVersion.contains("Next") && !FullVersion.contains("Cert")) {
        if (GetEngineVersion) {
            FNVer.erase(0, FNVer.find_last_of("-", FNVer.length() - 1) + 1);
            EngineVer.erase(EngineVer.find_first_of("-", FNVer.length() - 1), 40);
            if (EngineVer.find_first_of(".") != EngineVer.find_last_of(".")) {
                EngineVer.erase(EngineVer.find_last_of("."), 2);
            }
            result.EngineVersion = std::stod(EngineVer) * 100;
        } else {
            const std::regex base_regex("-([0-9.]*)-");
            std::cmatch base_match;
            std::regex_search(FullVersion.c_str(), base_match, base_regex);
            FNVer = base_match[1];
        }
        
        result.FortniteVersion = std::stod(FNVer);
        
        if (result.FortniteVersion >= 16.00 && result.FortniteVersion <= 18.40) {
            result.EngineVersion = 427;
        }
    } else {
        CLStr = FullVersion.substr(FullVersion.find_first_of('-') + 1);
        CLStr = CLStr.substr(0, CLStr.find_first_of('+'));
        result.FortniteCL = std::stoi(CLStr);
        result.EngineVersion = result.FortniteCL <= 3775276 ? 416 : 419;
    }
    
    if (result.FortniteCL == 3700114) result.FortniteVersion = 1.72;
    if (result.FortniteCL == 3724489) result.FortniteVersion = 1.8;
    if (result.FortniteCL == 3757339) result.FortniteVersion = 1.9;
    if (result.FortniteCL == 3775276) result.FortniteVersion = 1.9; 
    if (result.FortniteCL == 3790078) result.FortniteVersion = 1.10;
    if (result.FortniteCL == 3807424) result.FortniteVersion = 1.11;
    if (result.FortniteCL == 3841827) result.FortniteVersion = 2.2;
    if (result.FortniteCL == 3847564) result.FortniteVersion = 2.3;
    if (result.FortniteCL == 3858292) result.FortniteVersion = 2.4;
    if (result.FortniteCL == 3870737) result.FortniteVersion = 2.42;
    
    if (toFree.Data) {
        toFree.Free();
    }
    
    return result;
})();

inline double FNVer = VersionInfo.FortniteVersion;
inline int Engine_Version = VersionInfo.EngineVersion;
inline int Fortnite_CL = VersionInfo.FortniteCL;
inline double Fortnite_Version = VersionInfo.FortniteVersion;

inline int Chapter = ([]() {
	if (FNVer >= 28.00) return 5;
	if (FNVer == 27.00) return 1; // does c4sog use athena?
	if (FNVer >= 23.00) return 4;
	if (FNVer >= 19.00) return 3;
	if (FNVer >= 11.00) return 2;
	return 1;
	})();


template <typename _Ot>
_Ot& GetFromOffset(void* Obj, uint32 Offset) {
	return *(_Ot*)(__int64(Obj) + Offset);
}

template <typename _Ot>
_Ot& GetFromOffset(const void* Obj, uint32 Offset) {
	return *(_Ot*)(__int64(Obj) + Offset);
}

template <typename _Ot>
_Ot* GetPtrFromOffset(const void* Obj, uint32 Offset) {
	return (_Ot*)(__int64(Obj) + Offset);
}

class ParamPair {
public:
	std::string _Fi;
	void* _Sc;

	template <typename _Vt>
	ParamPair(std::string _Nm, _Vt _Va) {
		_Fi = _Nm;
		// really scuffed way to make this work, just using & gives the same address for each param
		_Sc = FMemory::Malloc(sizeof(_Vt));
		memcpy(_Sc, &_Va, sizeof(_Vt));
	}
};


class FStructBaseChain
{
public:
	FStructBaseChain()
		: StructBaseChainArray(nullptr)
		, NumStructBasesInChainMinusOne(-1)
	{
	}
	~FStructBaseChain()
	{
		delete[] StructBaseChainArray;
	}

	FStructBaseChain(const FStructBaseChain&) = delete;
	FStructBaseChain& operator=(const FStructBaseChain&) = delete;

	__forceinline bool IsChildOfUsingStructArray(const FStructBaseChain& Parent) const
	{
		int32 NumParentStructBasesInChainMinusOne = Parent.NumStructBasesInChainMinusOne;
		return NumParentStructBasesInChainMinusOne <= NumStructBasesInChainMinusOne && StructBaseChainArray[NumParentStructBasesInChainMinusOne] == &Parent;
	}

private:
	FStructBaseChain** StructBaseChainArray;
	int32 NumStructBasesInChainMinusOne;

	friend class UStruct;
};

template<int32 _Sl>
struct ConstexprWString
{
	wchar_t _Ch[_Sl];

	consteval ConstexprWString(const wchar_t(&_St)[_Sl])
	{
		copy_n(_St, _Sl, _Ch);
	}

	operator const wchar_t* () const
	{
		return static_cast<const wchar_t*>(_Ch);
	}
};

class UObject {
public:
	void** Vft;
	int32 ObjectFlags;
	int32 Index;
	class UClass* Class;
	class FName Name;
	UObject* Outer;
    	static inline void(*ProcessEventInternal)(const UObject*, class UFunction*, void*) = ([]() {
		uintptr_t addr;
		if (FNVer < 14.00) addr = Memcury::Scanner::FindStringRef("AccessNoneNoContext").ScanFor({ 0x40, 0x55 }, true, 0, 1, 2000).Get();
		else addr = Memcury::Scanner::FindStringRef("UMeshNetworkComponent::ProcessEvent: Invalid mesh network node type: %s", true, 0, FNVer >= 19.00).ScanFor({ 0xE8 }, true, FNVer < 19.00 ? 1 : 3, 0, 2000).RelativeOffset(1).Get();
		return (void(*)(const UObject*, class UFunction*, void*)) addr;
	})();
public:
	const class UField* GetProperty(const wchar_t* Name) const;
	uint32 GetOffset(const wchar_t* Name) const {
		auto OffsetOff = FNVer >= 12.10 && FNVer < 20 ? 0x4c : 0x44;
		auto Prop = GetProperty(Name);
		if (!Prop) return -1;
		return GetFromOffset<uint32>(Prop, OffsetOff);
	}

	template <ConstexprWString Name, typename T = UObject*>
	T& Get() const {
		auto Off = GetOffset(Name);
		if (Off == -1) {
			//printf("Failed to get offset for %ls!\n", Name._Ch);
			while (true);
		}
		return GetFromOffset<T>(this, Off);
	}


	template <typename T = UObject*>
	T& Get(const wchar_t* Name) const {
		auto Off = GetOffset(Name);
		if (Off == -1) {
			//printf(_("Failed to get offset for %ls!\n"), Name);
			while (true);
		}
		return GetFromOffset<T>(this, Off);
	}

	template <ConstexprWString Name>
	void SetBitfield(bool Value) const {
		static auto OffsetOff = FNVer >= 12.10 && FNVer < 20 ? 0x4c : 0x44;
		static auto Prop = GetProperty(Name);
		uint8& Field = GetFromOffset<uint8>(Prop, OffsetOff);
		if (Value) {
			Field |= Prop->GetFieldMask();
		}
		else {
			Field &= ~Prop->GetFieldMask();
		}
	}

	template <ConstexprString Name>
	bool Has() const {
		return GetOffset(Name) != -1;
	}

	template <ConstexprString Name, typename T = UObject*>
	T* GetPtr() const {
		auto Off = GetOffset(Name);
		if (Off == -1) return nullptr;
		return GetPtrFromOffset<T>(this, Off);
	}

	bool IsA(const class UClass* Clss) const {
		if (!this || !Clss)
			return false;
		if (FNVer >= 7)
		{
			auto& BaseChain = GetFromOffset<FStructBaseChain>(Class, 0x30);
			auto& BaseChainOther = GetFromOffset<FStructBaseChain>(Clss, 0x30);

			return BaseChain.IsChildOfUsingStructArray(BaseChainOther);
		}
		auto SuperOff = FNVer >= 7 ? 0x40 : 0x30;
		for (auto _Cl = Class; _Cl; _Cl = GetFromOffset<UClass*>(_Cl, SuperOff)) {
			if (_Cl == Clss) return true;
		}
		return false;
	}

	UObject* Cast(const class UClass* Class)
	{
		return IsA(Class) ? this : nullptr;
	}

	class UFunction* GetFunction(const wchar_t* Name) const {
		return (UFunction*)GetProperty(Name);
	}

	inline 	void ProcessEvent(class UFunction* Function, void* Params) const {
		ProcessEventInternal(this, Function, Params);
	}

template <ConstexprString Name>
void Call(void* Params = nullptr) const {
auto Function = GetFunction(reinterpret_cast<const wchar_t*>(Name._Ch));
ProcessEvent(Function, Params);
}

	void Call(const wchar_t* Name, void* Params = nullptr) const {
		auto Function = GetFunction(Name);
		ProcessEvent(Function, Params);
	}

	template <ConstexprString Name>
	void Call(std::vector<ParamPair> Params) const;
};

class UField : public UObject {
public:
	const UField* GetNext(bool bNewFields = false) const {
		auto NextOff = bNewFields ? 0x20 : 0x28;
		return GetFromOffset<UField*>(this, NextOff);
	}


	FName& GetName(bool bNewFields = false) const {
		auto NameOff = bNewFields ? 0x28 : 0x18;
		return GetFromOffset<FName>(this, NameOff);
	}

	const uint8 GetFieldMask() const {
		static auto FieldMaskOff = FNVer >= 12.21 && FNVer < 20 ? 0x7b : 0x73;
		return GetFromOffset<uint8>(this, FieldMaskOff);
	}
};

class UStruct : public UField {
public:
	const UStruct* GetSuper() const {
		auto SuperOff = FNVer >= 7 ? 0x40 : 0x30;
		return GetFromOffset<UStruct*>(this, SuperOff);
	}
	const int32 GetPropertiesSize() const {
		auto ChildrenOff = FNVer >= 12.10 ? 0x58 : (FNVer >= 7 ? 0x50 : 0x40);
		return GetFromOffset<int32>(this, ChildrenOff);
	}

	const UField* GetChildren(bool bNewFields = false) const {
		auto ChildrenOff = bNewFields ? 0x50 : (FNVer >= 7 ? 0x48 : 0x38);
		return GetFromOffset<UField*>(this, ChildrenOff);
	}
};

class UClass : public UStruct {
public:
UObject* GetClassDefaultObject() const;

	const UField* GetProperty(const wchar_t* Name) const {
		for (const UClass* _Cl = this; _Cl; _Cl = (const UClass*)_Cl->GetSuper()) {
			if (FNVer >= 12.10) {
				for (const UField* _Pr = _Cl->GetChildren(true); _Pr; _Pr = _Pr->GetNext(true))
					if (wcscmp(_Pr->GetName(true).GetSDKWStr(), Name) == 0) return _Pr;
			}
			for (const UField* _Pr = _Cl->GetChildren(false); _Pr; _Pr = _Pr->GetNext(false))
				if (wcscmp(_Pr->GetName(false).GetSDKWStr(), Name) == 0) return _Pr;
		}
		return nullptr;
	}
};


inline const UField* UObject::GetProperty(const wchar_t* Name) const {
	return Class->GetProperty(Name);
}

class UFunction : public UStruct {
public:
	void Call(const UObject* obj, void* Params) {
		if (this) obj->ProcessEvent(this, Params);
	}

	void Call(const UObject* obj, std::vector<ParamPair> Params) {
		if (this) obj->ProcessEvent(this, CreateParams(Params));
	}

	void operator()(const UObject* obj, void* Params) {
		return Call(obj, Params);
	}

	void*& GetNativeFunc() {
		if (FNVer <= 6.31) return GetFromOffset<void*>(this, 0xB0);
		else if (FNVer > 7.00 && FNVer < 12.00) return GetFromOffset<void*>(this, 0xC0);
		else if (FNVer >= 12.00 && FNVer < 12.10) return GetFromOffset<void*>(this, 0xC8);
		else if (FNVer >= 12.10 && FNVer <= 12.61) return GetFromOffset<void*>(this, 0xF0);
		else return GetFromOffset<void*>(this, 0xD8);
	}

	int32 GetVTableIndex() {
		if (!this) return -1;
		auto ValidateName = Name.ToString() + "_Validate";
		auto ValidateRef = Memcury::Scanner::FindStringRef(ValidateName.c_str(), false);

		auto Addr = ValidateRef.Get();

		if (!Addr) {
			Addr = __int64(GetNativeFunc());
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

	struct Param {
		std::string Name;
		uint32 Offset;
	};
	class Params {
	public:
		std::vector<Param> NameOffsetMap;
		uint32 Size;
	};

	Params GetParams() {
		Params p;
		static auto OffsetOff = FNVer >= 12.10 && FNVer < 20 ? 0x4c : 0x44;

		if (FNVer >= 12.10) {
			for (const UField* _Pr = GetChildren(true); _Pr; _Pr = _Pr->GetNext(true)) {
				p.NameOffsetMap.push_back({ _Pr->GetName(true).ToSDKString(), GetFromOffset<uint32>(_Pr, OffsetOff) });
			}
		}
		for (const UField* _Pr = GetChildren(false); _Pr; _Pr = _Pr->GetNext(false)) {
			p.NameOffsetMap.push_back({ _Pr->GetName(false).ToSDKString(), GetFromOffset<uint32>(_Pr, OffsetOff) });
		}

		p.Size = GetPropertiesSize();
		return p;
	}

	void* CreateParams(std::vector<ParamPair> InputParams) {
		auto Mem = FMemory::Malloc(GetParams().Size);
		auto Params = GetParams();

		for (auto& _Pa : InputParams) {
			Param FoundParam;
			int i = 0;
			uint32 Size = 0;
			for (auto& _Mp : Params.NameOffsetMap) {
				if (_Mp.Name == _Pa._Fi) {
					FoundParam = _Mp;
					Size = i == Params.NameOffsetMap.size() - 1 ? Params.Size - _Mp.Offset : Params.NameOffsetMap[i + 1].Offset - _Mp.Offset;
					break;
				}
				i++;
			}

			if (Size) {
				memcpy((PBYTE)Mem + FoundParam.Offset, _Pa._Sc, Size);
			}
		}
		return Mem;
	}

	template <typename _Rt>
	_Rt* GetValueFromParams(void* Params, const char* Name) {
		auto Params = GetParams();

		for (auto& _Mp : Params.NameOffsetMap) {
			if (_Mp.Name == Name) {
				return GetPtrFromOffset<_Rt>(Params, _Mp.Offset);
			}
		}

		return nullptr;
	}
};

template <ConstexprString Name>
void UObject::Call(std::vector<ParamPair> Params) const {
	static auto Function = GetFunction(Name);
	ProcessEvent(Function, Function->CreateParams(Params));
}

struct FUObjectItem final
{
public:
	class UObject* Object;
	int32 Flags;
	int32 ClusterRootIndex;
	int32 SerialNumber;
};

class TUObjectArrayUnchunked
{
private:
	const FUObjectItem* Objects;
	const int32 MaxElements;
	const int32 NumElements;

public:
	inline int Num() const
	{
		return NumElements;
	}

	inline int Max() const
	{
		return MaxElements;
	}

	inline const FUObjectItem* GetItemByIndex(const int32 Index) const
	{
		if (Index < 0 || Index > NumElements)
			return nullptr;

		return Objects + Index;
	}
};

class TUObjectArrayChunked
{
private:
	const FUObjectItem** Objects;
	const FUObjectItem* PreAllocatedObjects;
	const int32 MaxElements;
	const int32 NumElements;
	const int32 MaxChunks;
	const int32 NumChunks;

public:
	inline int Num() const
	{
		return NumElements;
	}

	inline int Max() const
	{
		return MaxElements;
	}

	inline const FUObjectItem* GetItemByIndex(const int32 Index) const
	{
		const int32 ChunkIndex = Index / 0x10000;
		const int32 ChunkOffset = Index % 0x10000;

		return Objects[ChunkIndex] + ChunkOffset;
	}
};

class TUObjectArray {
public:
	static inline TUObjectArrayChunked* GObjectsChunked = ([]() {
		if (FNVer >= 5.00) {
			return (TUObjectArrayChunked*)Memcury::Scanner::FindPattern(FNVer <= 6.02 ? "48 8B 05 ? ? ? ? 48 8B 0C C8 48 8D 04 D1" : "48 8B 05 ? ? ? ? 48 8B 0C C8 48 8B 04 D1").RelativeOffset(3).Get();
		}
		return (TUObjectArrayChunked*)nullptr;
		})();
	static inline TUObjectArrayUnchunked* GObjectsUnchunked = ([]() {
		if (FNVer < 5.00) {
			auto Addr = Memcury::Scanner::FindPattern("48 8B 05 ? ? ? ? 48 8D 14 C8 EB 03 49 8B D6 8B 42 08 C1 E8 1D A8 01 0F 85 ? ? ? ? F7 86 ? ? ? ? ? ? ? ?", false);
			if (!Addr.Get()) {
				Addr = Memcury::Scanner::FindPattern("48 8B 05 ? ? ? ? 48 8D 1C C8 81 4B ? ? ? ? ? 49 63 76 30", false);
			}
			return (TUObjectArrayUnchunked*)Addr.RelativeOffset(3).Get();
		}
		return (TUObjectArrayUnchunked*)nullptr;
		})();
public:
	static const int32 Num() {
		return GObjectsChunked ? GObjectsChunked->Num() : GObjectsUnchunked->Num();
	}

	static const int32 Max() {
		return GObjectsChunked ? GObjectsChunked->Max() : GObjectsUnchunked->Max();
	}

	static const FUObjectItem* GetItemByIndex(const int32 Index) {
		return GObjectsChunked ? GObjectsChunked->GetItemByIndex(Index) : GObjectsUnchunked->GetItemByIndex(Index);
	}

	static const UObject* GetObjectByIndex(const int32 Index) {
		const FUObjectItem* Item = GetItemByIndex(Index);
		return Item ? Item->Object : nullptr;
	}

	static const UObject* FindObject(const char* Name, uint32 Flags = 0) {
		for (int i = 0; i < Num(); i++) {
			const UObject* Obj = GetObjectByIndex(i);
			if (Obj && (Obj->ObjectFlags & Flags) == Flags && Obj->Name.ToString() == Name) [[unlikely]]
				return Obj;
		}
		return nullptr;
	}

	template <typename _Et = UObject>
	static const _Et* FindObject(const char* Name) {
		return (const _Et*)FindObject(Name);
	}

	template <typename _Et = UObject>
	static const _Et* FindObject(const std::string& Name) {
		return FindObject<_Et>(Name.c_str());
	}

	static const UObject* FindObject(const wchar_t* Name, uint32 Flags = 0) {
		for (int i = 0; i < Num(); i++) {
			const UObject* Obj = GetObjectByIndex(i);
			if (Obj && (Obj->ObjectFlags & Flags) == Flags && wcscmp(Obj->Name.GetWStr(), Name) == 0) [[unlikely]]
				return Obj;
		}
		return nullptr;
	}

	template <typename _Et = UObject>
	static const _Et* FindObject(const wchar_t* Name) {
		return (const _Et*)FindObject(Name);
	}

	template <typename _Et = UObject>
	static const _Et* FindObject(const std::wstring& Name) {
		return FindObject<_Et>(Name.c_str());
	}

	static const UObject* FindFirstObject(const wchar_t* Name) {
		UClass* TargetClass = (UClass*)FindObject(Name);
		for (int i = 0; i < Num(); i++) {
			const UObject* Obj = GetObjectByIndex(i);
			if (Obj && !(Obj->ObjectFlags & 16) && Obj->IsA(TargetClass)) [[unlikely]]
				return Obj;
		}
		return nullptr;
	}
};

inline UObject* UClass::GetClassDefaultObject() const {
    std::string Name = "Default__" + this->Name.ToString();
    return (UObject*)TUObjectArray::FindObject(Name);
}

template <typename T = UObject*, typename _St>
T& StructGet(_St*& StructInstance, const char* StructName, const wchar_t* Name) {
	auto Struct = TUObjectArray::FindObject<UClass>(StructName);

	static auto OffsetOff = FNVer >= 12.10 && FNVer < 20 ? 0x4c : 0x44;
	auto Off = GetFromOffset<uint32>(Struct->GetProperty(Name), OffsetOff);
	return GetFromOffset<T>(StructInstance, Off);
}

template <DefaultObjChars Name>
const UObject* GetDefaultObj() {
	static auto Obj = TUObjectArray::FindObject(Name);
	return Obj;
}

class WrapperBase {
private:
	virtual const UObject* Get() const = 0;

public:
	operator const UObject* () const {
		return Get();
	}
operator UObject* () const {
return const_cast<UObject*>(Get());
}

	const UObject* operator->() const {
		return Get();
	}

	operator bool() const {
		return Get() != nullptr;
	}
};

class EngineWrapper : public WrapperBase {
public:
	const UObject* Get() const {
		return TUObjectArray::FindFirstObject(L"FortEngine");
	}
};

inline EngineWrapper Engine;

class WorldWrapper : public WrapperBase {
public:
	const UObject* Get() const {
		auto _Engine = Engine.Get();
		if (!_Engine)
			return nullptr;

		auto VPOffset = _Engine->GetOffset(L"GameViewport");
		if (VPOffset == -1)
			return nullptr;

		auto _Viewport = GetFromOffset<UObject*>(_Engine, VPOffset);
		if (!_Viewport)
			return nullptr;

		auto WOffset = _Viewport->GetOffset(L"World");
		if (WOffset == -1)
			return nullptr;

		return GetFromOffset<UObject*>(_Viewport, WOffset);
	}
};

inline WorldWrapper World;

class GameModeWrapper : public WrapperBase {
public:
	const UObject* Get() const {
		auto _Wr = World.Get();
		if (!_Wr)
			return nullptr;

		auto GMOffset = _Wr->GetOffset(L"AuthorityGameMode");
		if (GMOffset == -1)
			return nullptr;


		return GetFromOffset<UObject*>(_Wr, GMOffset);
	}
};

inline GameModeWrapper GameMode;

class GameStateWrapper : public WrapperBase {
public:
	const UObject* Get() const {
		return World ? World->Get(L"GameState") : nullptr;
	}
};

inline GameStateWrapper GameState;

inline void* nullptrForHook;

template<DefaultObjChars ClassName, typename T = void*>
__forceinline void Hook(int idx, void* detour, T& og = nullptrForHook) {
	auto VTable = (void**)GetDefaultObj<ClassName>()->Vft;
	if (!is_same_v<T, void*>)
		og = (T)VTable[idx];

	DWORD vpog;
	VirtualProtect(VTable + idx, 8, PAGE_EXECUTE_READWRITE, &vpog);
	VTable[idx] = detour;
	VirtualProtect(VTable + idx, 8, vpog, &vpog);
}


template<DefaultObjChars ClassName, ConstexprString FuncName, typename T = void*>
__forceinline void Hook(void* detour, T& og = nullptrForHook) {
	auto DefaultObj = GetDefaultObj<ClassName>();
	auto VTable = (void**)DefaultObj->Vft;
	int idx = DefaultObj->GetFunction(FuncName)->GetVTableIndex();
	if (!is_same_v<T, void*>)
		og = (T)VTable[idx];

	DWORD vpog;
	VirtualProtect(VTable + idx, 8, PAGE_EXECUTE_READWRITE, &vpog);
	VTable[idx] = detour;
	VirtualProtect(VTable + idx, 8, vpog, &vpog);
}

struct FFastArraySerializerItem : public UObject
{
public:
	int32 ReplicationID;
	int32 ReplicationKey;
	int32 MostRecentArrayReplicationKey;
};

struct alignas(0x08) FFastArraySerializer : public UObject
{
public:
	char ItemMap[0x50];
	int32 IDCounter;
	int32 ArrayReplicationKey;
	char GuidReferencesMap[0x50];

	int32& GetCachedItems() {
		return GetFromOffset<int32>(this, FNVer >= 8.30 ? 0xf8 : 0xa8);
	}

	int32& GetCachedItemsToConsiderForWriting() {
		return GetFromOffset<int32>(this, FNVer >= 8.30 ? 0xfc : 0xac);
	}

	/** This must be called if you add or change an item in the array */
	void MarkItemDirty(FFastArraySerializerItem& Item)
	{
		if (Item.ReplicationID == -1)
		{
			Item.ReplicationID = ++IDCounter;
			if (IDCounter == -1)
			{
				IDCounter++;
			}
		}

		Item.ReplicationKey++;
		MarkArrayDirty();
	}

	/** This must be called if you just remove something from the array */
	void MarkArrayDirty()
	{
		// ItemMap.Reset();        // This allows to clients to add predictive elements to arrays without affecting replication.
		IncrementArrayReplicationKey();

		// Invalidate the cached item counts so that they're recomputed during the next write
		GetCachedItems() = -1;
		GetCachedItemsToConsiderForWriting() = -1;
	}

	void IncrementArrayReplicationKey()
	{
		ArrayReplicationKey++;
		if (ArrayReplicationKey == -1)
		{
			ArrayReplicationKey++;
		}
	}
};

template<typename _Ct>
class TSubclassOf
{
	const UClass* ClassPtr;

public:
	TSubclassOf() = default;

	inline TSubclassOf(const UClass* Class)
		: ClassPtr(Class)
	{
	}

	inline UClass* Get()
	{
		return const_cast<UClass*>(ClassPtr);
	}

	inline UClass* operator->()
	{
		return const_cast<UClass*>(ClassPtr);
	}
	
	template<typename Target, typename = std::enable_if<std::is_base_of_v<Target, _Ct>, bool>::type>
	inline operator TSubclassOf<Target>() const
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

struct GameplayStatics_GetAllActorsOfClass final
{
public:
	const class UObject* WorldContextObject;
	TSubclassOf<class AActor> ActorClass;
	TArray<class AActor*> OutActors;
};

class AActor;

static TArray<AActor*> GetAll(const UClass* Class) {
	GameplayStatics_GetAllActorsOfClass Params{ World, Class };
	GetDefaultObj<"GameplayStatics">()->Call<"GetAllActorsOfClass">(&Params);
	return Params.OutActors;
}

template <ConstexprString Str>
static TArray<AActor*> GetAll() {
	static auto Class = TUObjectArray::FindObject<UClass>(Str); // we can do this because of the template
	return GetAll(Class);
}

template <typename UnderlayingType>
struct TVector
{
public:
	UnderlayingType X;
	UnderlayingType Y;
	UnderlayingType Z;

public:
	TVector(UnderlayingType _X = 0, UnderlayingType _Y = 0, UnderlayingType _Z = 0)
		: X(_X), Y(_Y), Z(_Z)
	{
	}

	TVector<UnderlayingType>& Normalize()
	{
		*this /= Magnitude();
		return *this;
	}
	TVector<UnderlayingType>& operator*=(const TVector<UnderlayingType>& Other)
	{
		*this = *this * Other;
		return *this;
	}
	TVector<UnderlayingType>& operator*=(UnderlayingType Scalar)
	{
		*this = *this * Scalar;
		return *this;
	}
	TVector<UnderlayingType>& operator+=(const TVector<UnderlayingType>& Other)
	{
		*this = *this + Other;
		return *this;
	}
	TVector<UnderlayingType>& operator-=(const TVector<UnderlayingType>& Other)
	{
		*this = *this - Other;
		return *this;
	}
	TVector<UnderlayingType>& operator/=(const TVector<UnderlayingType>& Other)
	{
		*this = *this / Other;
		return *this;
	}
	TVector<UnderlayingType>& operator/=(UnderlayingType Scalar)
	{
		*this = *this / Scalar;
		return *this;
	}

	UnderlayingType Dot(const TVector<UnderlayingType>& Other) const
	{
		return (X * Other.X) + (Y * Other.Y) + (Z * Other.Z);
	}
	UnderlayingType GetDistanceTo(const TVector<UnderlayingType>& Other) const
	{
		TVector<UnderlayingType> DiffVector = Other - *this;
		return DiffVector.Magnitude();
	}
	UnderlayingType GetDistanceToInMeters(const TVector<UnderlayingType>& Other) const
	{
		return GetDistanceTo(Other) * 0.01f;
	}
	TVector<UnderlayingType> GetNormalized() const
	{
		return *this / Magnitude();
	}
	bool IsZero() const
	{
		return X == 0.0 && Y == 0.0 && Z == 0.0;
	}
	UnderlayingType Magnitude() const
	{
		return std::sqrt((X * X) + (Y * Y) + (Z * Z));
	}
	bool operator!=(const TVector<UnderlayingType>& Other) const
	{
		return X != Other.X || Y != Other.Y || Z != Other.Z;
	}
	TVector<UnderlayingType> operator*(const TVector<UnderlayingType>& Other) const
	{
		return { X * Other.X, Y * Other.Y, Z * Other.Z };
	}
	TVector<UnderlayingType> operator*(UnderlayingType Scalar) const
	{
		return { X * Scalar, Y * Scalar, Z * Scalar };
	}
	TVector<UnderlayingType> operator+(const TVector<UnderlayingType>& Other) const
	{
		return { X + Other.X, Y + Other.Y, Z + Other.Z };
	}
	TVector<UnderlayingType> operator-(const TVector<UnderlayingType>& Other) const
	{
		return { X - Other.X, Y - Other.Y, Z - Other.Z };
	}
	TVector<UnderlayingType> operator/(const TVector<UnderlayingType>& Other) const
	{
		if (Other.X == 0.0f || Other.Y == 0.0f || Other.Z == 0.0f)
			return *this;

		return { X / Other.X, Y / Other.Y, Z / Other.Z };
	}
	TVector<UnderlayingType> operator/(UnderlayingType Scalar) const
	{
		if (Scalar == 0.0f)
			return *this;

		return { X / Scalar, Y / Scalar, Z / Scalar };
	}
	bool operator==(const TVector<UnderlayingType>& Other) const
	{
		return X == Other.X && Y == Other.Y && Z == Other.Z;
	}
};

template <typename UnderlayingType>
struct alignas(0x10) TQuat
{
public:
	UnderlayingType X;
	UnderlayingType Y;
	UnderlayingType Z;
	UnderlayingType W;
};

template <typename UnderlayingType>
struct TRotator
{
public:
	UnderlayingType Pitch;
	UnderlayingType Yaw;
	UnderlayingType Roll;

	operator TQuat<UnderlayingType>() {
		UnderlayingType halfOfARadian = (UnderlayingType)0.008726646259971648;
		UnderlayingType sinPitch = sin(Pitch * halfOfARadian),
			sinYaw = sin(Yaw * halfOfARadian),
			sinRoll = sin(Roll * halfOfARadian);
		UnderlayingType cosPitch = cos(Pitch * halfOfARadian),
			cosYaw = cos(Yaw * halfOfARadian),
			cosRoll = cos(Roll * halfOfARadian);

		TQuat<UnderlayingType> out{};
		out.X = cosRoll * sinPitch * sinYaw - sinRoll * cosPitch * cosYaw;
		out.Y = -cosRoll * sinPitch * cosYaw - sinRoll * cosPitch * sinYaw;
		out.Z = cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw;
		out.W = cosRoll * cosPitch * cosYaw + sinRoll * sinPitch * sinYaw;
		return out;
	}

	operator TVector<UnderlayingType>()
	{
		UnderlayingType oneRadian = (UnderlayingType)0.017453292519943295;
		UnderlayingType cosPitch = cos(Pitch * oneRadian), cosYaw = cos(Yaw * oneRadian);
		UnderlayingType sinPitch = sin(Pitch * oneRadian), sinYaw = sin(Yaw * oneRadian);

		return TVector<UnderlayingType>(cosPitch * cosYaw, cosPitch * sinYaw, sinPitch);
	}
};



template <typename UnderlayingType>
struct TTransform
{
public:
	struct TQuat<UnderlayingType> Rotation;
	struct TVector<UnderlayingType> Translation;
	uint8 _Padding1[0x4];
	struct TVector<UnderlayingType> Scale3D;
	uint8 _Padding2[0x4];

	TTransform() {}
	TTransform(TVector<UnderlayingType> loc, TQuat<UnderlayingType> rot, TVector<UnderlayingType> scale = { 1, 1, 1 }) : Translation(loc), Rotation(rot), Scale3D(scale) {}
};

struct KismetStringLibrary_Conv_StringToName
{
public:
	class FString InString;
	class FName ReturnValue;
};

inline FName Conv_StringToName(FString Str) {
	KismetStringLibrary_Conv_StringToName Params{ Str };
	TUObjectArray::FindObject(L"Default__KismetStringLibrary")->Call(L"Conv_StringToName", &Params);
	return Params.ReturnValue;
}

struct UScriptStruct {

};

struct FURL
{
public:
	class FString Protocol;
	class FString Host;
	int32 Port;
	uint8 Pad_BC4[0x4];
	class FString Map;
	class FString RedirectUrl;
	TArray<class FString> Op;
	class FString Portal;
	int32 Valid;
	uint8 Pad_BC5[0x4];
};

enum class ELogLevel { Fatal, Error, Warning, Log, Verbose, VeryVerbose };

inline constexpr ELogLevel Fatal = ELogLevel::Fatal;
inline constexpr ELogLevel Error = ELogLevel::Error;
inline constexpr ELogLevel Warning = ELogLevel::Warning;
inline constexpr ELogLevel Log = ELogLevel::Log;
inline constexpr ELogLevel Verbose = ELogLevel::Verbose;
inline constexpr ELogLevel VeryVerbose = ELogLevel::VeryVerbose;


inline const char *ToString( ELogLevel level ) {
        switch ( level ) {
        case ELogLevel::Verbose:
                return "Verbose";
        case ELogLevel::VeryVerbose:
                return "VeryVerbose";
        case ELogLevel::Log:
                return "Log";
        case ELogLevel::Warning:
                return "Warning";
        case ELogLevel::Error:
                return "Error";
        case ELogLevel::Fatal:
                return "Fatal";
        default:
                return "Unknown";
        }
}

inline const wchar_t *ToWString( ELogLevel level ) {
        switch ( level ) {
        case ELogLevel::Verbose:
                return L"Verbose";
        case ELogLevel::VeryVerbose:
                return L"VeryVerbose";
        case ELogLevel::Log:
                return L"Log";
        case ELogLevel::Warning:
                return L"Warning";
        case ELogLevel::Error:
                return L"Error";
        case ELogLevel::Fatal:
                return L"Fatal";
        default:
                return L"Unknown";
        }
}

inline void SetConsoleColorByLogLevel( ELogLevel Level ) {
        HANDLE hConsole = GetStdHandle( STD_OUTPUT_HANDLE );

        WORD color = FOREGROUND_RED | FOREGROUND_GREEN |
                     FOREGROUND_BLUE; // default white

        switch ( Level ) {
        case ELogLevel::Warning:
                color = FOREGROUND_RED | FOREGROUND_GREEN; // yellow
                break;
        case ELogLevel::Error:
                color = FOREGROUND_RED; // red
                break;
        case ELogLevel::Fatal:
                color = FOREGROUND_RED | FOREGROUND_INTENSITY; // bright red
                break;
        case ELogLevel::Verbose:
                color = FOREGROUND_GREEN | FOREGROUND_BLUE; // cyan
                break;
        case ELogLevel::VeryVerbose:
                color = FOREGROUND_BLUE | FOREGROUND_INTENSITY; // bright blue
                break;
        case ELogLevel::Log:
        default:
                color = FOREGROUND_RED | FOREGROUND_GREEN |
                        FOREGROUND_BLUE; // white
                break;
        }

        SetConsoleTextAttribute( hConsole, color );
}

inline void ResetConsoleColor() {
        SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ),
                                 FOREGROUND_RED | FOREGROUND_GREEN |
                                     FOREGROUND_BLUE );
}


struct FLogCategory {
        const char *Name;
        ELogLevel RuntimeVerbosity;
};


#if _DEBUG
#define DEFINE_LOG_CATEGORY( CategoryName )                                    \
        inline FLogCategory CategoryName = { #CategoryName, ELogLevel::VeryVerbose }
#else
#define DEFINE_LOG_CATEGORY( CategoryName )                                    \
        inline FLogCategory CategoryName = { #CategoryName, ELogLevel::Log }
#endif



inline std::string GetTimestampA() {
        auto now = std::chrono::system_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      now.time_since_epoch() ) %
                  1000;
        std::time_t now_c = std::chrono::system_clock::to_time_t( now );
        std::tm timeinfo{};
        localtime_s( &timeinfo, &now_c );
        std::ostringstream oss;
        oss << std::put_time( &timeinfo, "%Y.%m.%d-%H.%M.%S" ) << ':'
            << std::setfill( '0' ) << std::setw( 3 ) << ms.count();
        return oss.str();
}

inline std::wstring GetTimestampW() {
        auto now = std::chrono::system_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      now.time_since_epoch() ) %
                  1000;
        std::time_t now_c = std::chrono::system_clock::to_time_t( now );
        std::tm timeinfo{};
        localtime_s( &timeinfo, &now_c );
        std::wostringstream oss;
        oss << std::put_time( &timeinfo, L"%Y.%m.%d-%H.%M.%S" ) << ':'
            << std::setfill( L'0' ) << std::setw( 3 ) << ms.count();
        return oss.str();
}

inline void UELogImpl( FLogCategory &Category, ELogLevel Level, const char *fmt,
                       ... ) {
        if ( (int)Level > (int)Category.RuntimeVerbosity )
                return;

        char buffer[2048];

        va_list args;
        va_start( args, fmt );
        vsnprintf( buffer, sizeof( buffer ), fmt, args );
        va_end( args );

        SetConsoleColorByLogLevel( Level );

        std::cout << "[" << GetTimestampA() << "]" << Category.Name << ": "
                  << ToString( Level ) << ": " << buffer << std::endl;

        ResetConsoleColor();
}

inline std::string WStringToUTF8( const std::wstring &wstr ) {
        return std::string( wstr.begin(), wstr.end() );
}

inline void UELogWImpl( FLogCategory &Category, ELogLevel Level, const wchar_t *fmt,
                       ... ) {
        if ( (int)Level > (int)Category.RuntimeVerbosity )
                return;
        
        wchar_t wbuffer[2048];

        va_list args;
        va_start( args, fmt );
        vswprintf( wbuffer, sizeof( wbuffer ) / sizeof( wchar_t ), fmt, args );
        va_end( args );

        std::string Categorydih = std::string( Category.Name );
        std::wstring WDih =
            std::wstring( Categorydih.begin(), Categorydih.end() );
        
        std::wstring wfinal = std::wstring( wbuffer );
        std::string finalbuf = WStringToUTF8( wfinal );

        SetConsoleColorByLogLevel( Level );

        std::cout << "[" << GetTimestampA() << "] " << Category.Name << ": "
                  << ToString( Level ) << ": " << finalbuf << std::endl;

        ResetConsoleColor();
}

#define UE_LOG( Category, Verbosity, Format, ... )                             \
        UELogImpl( Category, Verbosity, Format, ##__VA_ARGS__ )

#define UE_LOG_W( Category, VerbosityLevel, Format, ... )                      \
        UELogWImpl( Category, VerbosityLevel, Format, ##__VA_ARGS__)

#define DEFINE_MEMBER(Ret, ClassName, PropName) \
inline Ret& Get##PropName() { \
    static int PropOffset = -1; \
    if (PropOffset == -1) { \
        PropOffset = this->GetOffset(L#PropName); \
    } \
    return *reinterpret_cast<Ret*>(uintptr_t(this) + PropOffset); \
} \
inline void Set##PropName(Ret& Value) { \
    static int PropOffset = -1; \
    if (PropOffset == -1) { \
        PropOffset = this->GetOffset(L#PropName); \
    } \
    *reinterpret_cast<Ret*>(uintptr_t(this) + PropOffset) = Value; \
}

__forceinline UClass* StaticClassImpl(const char* ClassName) {
        static std::unordered_map<std::string, UClass *> ClassCache;
        auto It = ClassCache.find( ClassName );
        if ( It != ClassCache.end() ) {
                return It->second;
        }
        auto Class = reinterpret_cast<UClass *>(
           TUObjectArray::FindObject( ClassName )->Class );
        if ( !Class ) {
                return nullptr;
        }
        ClassCache[ClassName] = Class;
        return Class;
}

    #define DECLARE_STATIC_CLASS(ClassName) \
static UClass* StaticClass() \
{ \
return StaticClassImpl(#ClassName + 1); \
}

#define DECLARE_DEFAULT_OBJECT(ClassName) \
static UObject* GetDefaultObj() \
{ \
return ClassName::StaticClass()->GetClassDefaultObject(); \
}

#define DEFINE_BOOL(ClassName, PropName) \
inline bool Get##PropName() const { \
    static int PropOffset = -1; \
    if (PropOffset == -1) { \
        PropOffset = this->GetOffset(L#PropName); \
    } \
    return *reinterpret_cast<bool*>(uintptr_t(this) + PropOffset); \
} \
inline void Set##PropName(bool Value) { \
    static int PropOffset = -1; \
    if (PropOffset == -1) { \
        PropOffset = this->GetOffset(L#PropName); \
    } \
    *reinterpret_cast<bool*>(uintptr_t(this) + PropOffset) = Value; \
}
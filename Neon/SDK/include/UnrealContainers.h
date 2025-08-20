#pragma once
#include <cassert>
#include <cmath>

template <typename A, typename B> struct TAreTypesEqual;

template <typename, typename> struct TAreTypesEqual {
        enum { Value = false };
};

template <typename A> struct TAreTypesEqual<A, A> {
        enum { Value = true };
};

#define ARE_TYPES_EQUAL( A, B ) TAreTypesEqual<A, B>::Value

inline void CheckHandler( const char *expr, const char *file, int line,
                          const std::string &msg = "" ) {
        std::cerr << "CHECK FAILED: (" << expr << ")" << " at " << file << ":"
                  << line;
        if ( !msg.empty() )
                std::cerr << " - " << msg;
        std::cerr << std::endl;

        std::abort();
}

inline void CheckHandler( const char *expr, const char *file, int line,
                          const std::wstring &msg = L"" ) {
        std::wcerr << L"CHECK FAILED: (" << expr << L")" << L" at " << file
                   << L":" << line;
        if ( !msg.empty() )
                std::wcerr << " - " << msg;
        std::cerr << std::endl;

        std::abort();
}

inline void CheckHandlerWithNoStr( const char *expr, const char *file,
                                   int line ) {
        std::wcerr << L"CHECK FAILED: (" << expr << L")" << L" at " << file
                   << L":" << line;
        std::cerr << std::endl;

        std::abort();
}

inline void CheckSlowHandler( const char *expr, const char *file, int line ) {
        std::wcerr << L"CHECK FAILED: (" << expr << L")" << L" at " << file
                   << L":" << line;
        std::cerr << std::endl;

        std::abort();
}

#define DO_CHECK_SLOW 0

#define check( expr )                                                          \
        ( ( expr ) ? static_cast<void>( 0 )                                    \
                   : CheckHandlerWithNoStr( #expr, __FILE__, __LINE__ ) )

#define checkf( expr, message, ... )                                           \
        ( ( expr ) ? static_cast<void>( 0 )                                    \
                   : CheckHandler( #expr, __FILE__, __LINE__,                  \
                                   std::format( message, __VA_ARGS__ ) ) )

#if DO_CHECK_SLOW
#define checkSlow( expr )                                                      \
        ( ( expr ) ? static_cast<void>( 0 )                                    \
                   : CheckSlowHandler( #expr, __FILE__, __LINE__ ) )
#else
#define checkSlow( expr ) ( (void)0 )
#endif

inline bool EnsureFailed( const char *expr, const char *file, int line,
                          const std::string &msg, bool &triggered ) {
        if ( !triggered ) {
                triggered = true;

                std::cerr << "ENSURE FAILED: (" << expr << ")"
                          << " at " << file << ":" << line << " - " << msg
                          << std::endl;
        }
        return false;
}

#define ensureMsgf( expr, message )                                            \
        ( []() -> bool {                                                       \
                static bool triggered = false;                                 \
                return ( expr ) ? true                                         \
                                : EnsureFailed( #expr, __FILE__, __LINE__,     \
                                                ( message ), triggered );      \
        } )()

#define RESTRICT __restrict
#define UE_ASSUME( x ) __assume( x )

enum { INDEX_NONE = -1 };
enum { UNICODE_BOM = 0xfeff };

template <typename T> struct TRemoveReference {
        typedef T Type;
};
template <typename T> struct TRemoveReference<T &> {
        typedef T Type;
};
template <typename T> struct TRemoveReference<T &&> {
        typedef T Type;
};

template <typename T>
FORCEINLINE T &&Forward( typename TRemoveReference<T>::Type &Obj ) {
        return static_cast<T &&>( Obj );
}

template <typename T>
FORCEINLINE T &&Forward( typename TRemoveReference<T>::Type &&Obj ) {
        static_assert( !std::is_lvalue_reference<T>::value,
                       "Do not forward an rvalue as an lvalue." );
        return static_cast<T &&>( Obj );
}

/**
 * MoveTemp will cast a reference to an rvalue reference.
 * This is UE's equivalent of std::move.  It doesn't static assert like
 * MoveTemp, because it is useful in templates or macros where it's not obvious
 * what the argument is, but you want to take advantage of move semantics where
 * you can but not stop compilation.
 */
template <typename T>
FORCEINLINE typename TRemoveReference<T>::Type &&MoveTempIfPossible( T &&Obj ) {
        typedef typename TRemoveReference<T>::Type CastType;
        return (CastType &&)Obj;
}

template <typename FuncType, typename... ArgTypes>
FORCEINLINE auto Invoke( FuncType &&Func, ArgTypes &&...Args )
    -> decltype( Forward<FuncType>( Func )( Forward<ArgTypes>( Args )... ) ) {
        return Forward<FuncType>( Func )( Forward<ArgTypes>( Args )... );
}

FORCEINLINE size_t DefaultQuantizeSize( size_t Count, uint32_t Alignment ) {
        return Count;
}

FORCEINLINE int32_t DefaultCalculateSlackGrow( int32_t NumElements,
                                               int32_t NumAllocatedElements,
                                               size_t BytesPerElement,
                                               bool bAllowQuantize,
                                               uint32_t Alignment = 8 ) {
        const size_t FirstGrow = 4;
        const size_t ConstantGrow = 16;
        int32_t Retval;

        size_t Grow = FirstGrow;

        if ( NumAllocatedElements || size_t( NumElements ) > Grow ) {
                Grow = size_t( NumElements ) + 3 * size_t( NumElements ) / 8 +
                       ConstantGrow;
        }
        if ( bAllowQuantize ) {
                Retval = int32_t(
                    DefaultQuantizeSize( Grow * BytesPerElement, Alignment ) /
                    BytesPerElement );
        } else {
                Retval = int32_t( Grow );
        }
        if ( NumElements > Retval ) {
                Retval = INT32_MAX;
        }

        printf( "RetVal: %d\n", Retval );

        return Retval;
}

namespace SDK {
typedef uint32_t uint32;
typedef int32_t int32;
typedef uint64_t uint64;
typedef uint8_t uint8;
typedef int64_t int64;

template <typename ContainerType, typename ElementType, typename SizeType>
class TIndexedContainerIterator {
      public:
        TIndexedContainerIterator( ContainerType &InContainer,
                                   SizeType StartIndex = 0 )
            : Container( InContainer ), Index( StartIndex ) {}

        /** Advances iterator to the next element in the container. */
        TIndexedContainerIterator &operator++() {
                ++Index;
                return *this;
        }
        TIndexedContainerIterator operator++( int ) {
                TIndexedContainerIterator Tmp( *this );
                ++Index;
                return Tmp;
        }

        /** Moves iterator to the previous element in the container. */
        TIndexedContainerIterator &operator--() {
                --Index;
                return *this;
        }
        TIndexedContainerIterator operator--( int ) {
                TIndexedContainerIterator Tmp( *this );
                --Index;
                return Tmp;
        }

        /** iterator arithmetic support */
        TIndexedContainerIterator &operator+=( SizeType Offset ) {
                Index += Offset;
                return *this;
        }

        TIndexedContainerIterator operator+( SizeType Offset ) const {
                TIndexedContainerIterator Tmp( *this );
                return Tmp += Offset;
        }

        TIndexedContainerIterator &operator-=( SizeType Offset ) {
                return *this += -Offset;
        }

        TIndexedContainerIterator operator-( SizeType Offset ) const {
                TIndexedContainerIterator Tmp( *this );
                return Tmp -= Offset;
        }

        ElementType &operator*() const { return Container[Index]; }

        ElementType *operator->() const { return &Container[Index]; }

        /** conversion to "bool" returning true if the iterator has not reached
         * the last element. */
        FORCEINLINE explicit operator bool() const {
                return Container.IsValidIndex( Index );
        }

        /** Returns an index to the current element. */
        SizeType GetIndex() const { return Index; }

        /** Resets the iterator to the first element. */
        void Reset() { Index = 0; }

        /** Sets the iterator to one past the last element. */
        void SetToEnd() { Index = Container.Num(); }

        /** Removes current element in array. This invalidates the current
         * iterator value and it must be incremented */
        void RemoveCurrent() {
                Container.RemoveAt( Index );
                Index--;
        }

        FORCEINLINE friend bool
        operator==( const TIndexedContainerIterator &Lhs,
                    const TIndexedContainerIterator &Rhs ) {
                return &Lhs.Container == &Rhs.Container &&
                       Lhs.Index == Rhs.Index;
        }
        FORCEINLINE friend bool
        operator!=( const TIndexedContainerIterator &Lhs,
                    const TIndexedContainerIterator &Rhs ) {
                return &Lhs.Container != &Rhs.Container ||
                       Lhs.Index != Rhs.Index;
        }

      private:
        ContainerType &Container;
        SizeType Index;
};

/**
 * Pointer-like iterator type for ranged-for loops which checks that the
 * container hasn't been resized during iteration.
 */
template <typename ElementType, typename SizeType>
struct TCheckedPointerIterator {
        // This iterator type only supports the minimal functionality needed to
        // support C++ ranged-for syntax.  For example, it does not provide
        // post-increment ++ nor ==.
        //
        // We do add an operator-- to help FString implementation

        explicit TCheckedPointerIterator( const SizeType &InNum,
                                          ElementType *InPtr )
            : Ptr( InPtr ), CurrentNum( InNum ), InitialNum( InNum ) {}

        FORCEINLINE ElementType &operator*() const { return *Ptr; }

        FORCEINLINE TCheckedPointerIterator &operator++() {
                ++Ptr;
                return *this;
        }

        FORCEINLINE TCheckedPointerIterator &operator--() {
                --Ptr;
                return *this;
        }

      private:
        ElementType *Ptr;
        const SizeType &CurrentNum;
        SizeType InitialNum;

        FORCEINLINE friend bool
        operator!=( const TCheckedPointerIterator &Lhs,
                    const TCheckedPointerIterator &Rhs ) {
                // We only need to do the check in this operator, because no
                // other operator will be called until after this one returns.
                //
                // Also, we should only need to check one side of this
                // comparison - if the other iterator isn't even from the same
                // array then the compiler has generated bad code.
                return Lhs.Ptr != Rhs.Ptr;
        }
};

template <typename InElementType> class TArray {

      public:
        typedef InElementType ElementType;
        typedef int32_t SizeType;
        friend class FString;

      public:
        /**
         * Constructor, initializes element number counters.
         */
        FORCEINLINE TArray() : Data( nullptr ), ArrayNum( 0 ), ArrayMax( 0 ) {}

    TArray(int32 Size)
    :ArrayNum(0), ArrayMax(Size), Data(reinterpret_cast<ElementType*>(FMemory::Realloc(0, sizeof(ElementType) *  Size, 0)))
        {

        }
    
      protected:
        ElementType *Data;
        int32 ArrayNum;
        int32 ArrayMax;

      public:
        FORCEINLINE ElementType *GetData() { return (ElementType *)Data; }

        FORCEINLINE const ElementType *GetData() const {
                return (const ElementType *)Data;
        }

        FORCEINLINE ElementType &GetFirstData() { return Data[0]; }
        template <class PT>
        ElementType* Search(PT Predicate, int32 ElementSize = sizeof(ElementType)) {
            for (int i = 0; i < Num(); i++) {
                auto& Element = *reinterpret_cast<ElementType*>(__int64(GetData()) + (i * ElementSize));
                if (Predicate(Element)) return &Element;
            }
            return nullptr;
        }

        /**
         * Helper function for returning a typed pointer to the first array
         * entry.
         *
         * @returns Pointer to first array entry or nullptr if ArrayMax == 0.
         */
        FORCEINLINE const ElementType &GetFirstData() const {
                return (const ElementType &)Data[0];
        }

        FORCEINLINE ElementType &GetData( int32 Index ) { return Data[Index]; }

        FORCEINLINE const ElementType &GetData( int32 Index ) const {
                return (const ElementType &)Data[Index];
        }

        /**
         * Helper function returning the size of the inner type.
         *
         * @returns Size in bytes of array type.
         */
        FORCEINLINE uint32 GetTypeSize() const { return sizeof( ElementType ); }

        /**
         * Returns the amount of slack in this array in elements.
         *
         * @see Num, Shrink
         */
        FORCEINLINE int32 GetSlack() const { return ArrayMax - ArrayNum; }

        /**
         * Tests if index is valid, i.e. greater than or equal to zero, and less
         * than the number of elements in the array.
         *
         * @param Index Index to test.
         * @returns True if index is valid. False otherwise.
         */
        FORCEINLINE bool IsValidIndex( int32 Index ) const {
                return Index >= 0 && Index < ArrayNum;
        }

        /**
         * Returns number of elements in array.
         *
         * @returns Number of elements in array.
         * @see GetSlack
         */
        FORCEINLINE int32 Num() const { return ArrayNum; }

        /**
         * Returns maximum number of elements in array.
         *
         * @returns Maximum number of elements in array.
         * @see GetSlack
         */
        FORCEINLINE int32 Max() const { return ArrayMax; }

        /**
         * Array bracket operator. Returns reference to element at give index.
         *
         * @returns Reference to indexed element.
         */
        FORCEINLINE ElementType &operator[]( int32 Index ) {
                return GetData( Index );
        }

        /**
         * Array bracket operator. Returns reference to element at give index.
         *
         * Const version of the above.
         *
         * @returns Reference to indexed element.
         */
        FORCEINLINE const ElementType &operator[]( int32 Index ) const {
                return GetData( Index );
        }

        /**
         * Finds element within the array.
         *
         * @param Item Item to look for.
         * @param Index Will contain the found index.
         * @returns True if found. False otherwise.
         * @see FindLast, FindLastByPredicate
         */

        FORCEINLINE bool Find( const ElementType &Item, int32 Index ) const {
                Index = this->Find( Item );
                return Index != -1;
        }

        /**
         * Finds element within the array.
         *
         * @param Item Item to look for.
         * @returns Index of the found element. INDEX_NONE otherwise.
         * @see FindLast, FindLastByPredicate
         */
        int32 Find( const ElementType &Item ) const {
                for ( int i = 0; i < ArrayNum; i++ ) {
                        if ( Data[i] == Item ) {
                                return static_cast<int32>( i );
                        }
                }

                return -1;
        }

        FORCEINLINE bool FindLast( const ElementType &Item,
                                   int32 Index ) const {
                Index = this->FindLast( Item );
                return Index != -1;
        }

        int32 FindLast( const ElementType &Item ) const {
                for ( int i = ArrayNum; i > 0; i-- ) {
                        if ( Data[i] == Item ) {
                                return static_cast<int32>( i );
                        }
                }

                return -1;
        }

        bool Contains( const InElementType &Item ) const {
                for ( int i = 0; i < ArrayNum; i++ ) {
                        if ( Data[i] == Item ) {
                                return true;
                        }
                }

                return false;
        }

        FORCEINLINE void ResizeGrow(int32 ElementSize = sizeof(ElementType)) {
            Data = (ElementType*)FMemory::Realloc(Data, (ArrayMax = 1 + ArrayNum) * ElementSize, 0);
        }

        FORCEINLINE int32 AddUnitalized(int32 Count = 1, int32 ElementSize = sizeof(ElementType)) {
            if (Count >= 0) {
                if ((ArrayNum + Count) > ArrayMax) {
                    ResizeGrow(ElementSize);
                }

                return ArrayNum;
            }
        }

        FORCEINLINE int32 Emplace(InElementType& Item,
            int32 ElementSize = sizeof(ElementType)) {
            const int32 Index = AddUnitalized(1, ElementSize);
            memcpy_s((InElementType*)(__int64(Data) +
                (ArrayNum * ElementSize)),
                ElementSize, (void*)&Item, ElementSize);
            ArrayNum++;

            return ArrayNum -1;
        }

        FORCEINLINE int32 Emplace(const InElementType& Item,
            int32 ElementSize = sizeof(ElementType)) {
            const int32 Index = AddUnitalized(1, ElementSize);
            memcpy_s((InElementType*)(__int64(Data) +
                (ArrayNum * ElementSize)),
                ElementSize, (void*)&Item, ElementSize);
            ArrayNum++;

            return ArrayNum - 1;
        }

        FORCEINLINE ElementType&
            Emplace_GetRef(InElementType& Item,
                int32 ElementSize = sizeof(ElementType)) {
            const int32 Index = AddUnitalized(1, ElementSize);
            memcpy_s((InElementType*)(__int64(Data) +
                (ArrayNum * ElementSize)),
                ElementSize, (void*)&Item, ElementSize);
            return Data[ArrayNum - 1];
        }

        FORCEINLINE int32 Add( InElementType &Item,
                               int32 ElementSize = sizeof( ElementType ) ) {
                if ( &Item )
                        return Emplace( Item, ElementSize );
        }

        FORCEINLINE int32 Add( const ElementType &Item, int32 ElementSize = sizeof(ElementType) ) {
                if ( &Item )
                        return Emplace( Item,ElementSize );
        }

        FORCEINLINE ElementType &
        Add_GetRef( ElementType &Item,
                    int32 ElementSize = sizeof( ElementType ) ) {
                if ( &Item ) {
                        return Emplace_GetRef( Item,ElementSize );
                }
        }

        inline bool Remove( int32 Index ) {
                if ( !IsValidIndex( Index ) )
                        return false;

                ArrayNum--;

                for ( int i = Index; i < ArrayNum; i++ ) {
                        int32 NextIndex = i + 1;
                        Data[i] = Data[NextIndex];
                }
        }

        int32 RemoveSingle( const ElementType &Item ) {
                int32 Index = Find( Item );

                if ( Index == -1 ) {
                        return 0;
                }

                --ArrayNum;

                for ( int i = Index; i < ArrayNum; i++ ) {
                        int32 NextIndex = i + 1;
                        Data[i] = Data[NextIndex];
                }

                return 1;
        }

        inline void Free() {
                ArrayMax = 0;
                ArrayNum = 0;
                Data = nullptr;
        }

        typedef TIndexedContainerIterator<TArray, ElementType, SizeType>
            TIterator;
        typedef TIndexedContainerIterator<const TArray, const ElementType,
                                          SizeType>
            TConstIterator;

        /**
         * Creates an iterator for the contents of this array
         *
         * @returns The iterator.
         */
        TIterator CreateIterator() { return TIterator( *this ); }

        /**
         * Creates a const iterator for the contents of this array
         *
         * @returns The const iterator.
         */
        TConstIterator CreateConstIterator() const {
                return TConstIterator( *this );
        }

        typedef TCheckedPointerIterator<ElementType, SizeType>
            RangedForIteratorType;
        typedef TCheckedPointerIterator<const ElementType, SizeType>
            RangedForConstIteratorType;

        FORCEINLINE RangedForIteratorType begin() {
                return RangedForIteratorType( ArrayNum, GetData() );
        }
        FORCEINLINE RangedForConstIteratorType begin() const {
                return RangedForConstIteratorType( ArrayNum, GetData() );
        }
        FORCEINLINE RangedForIteratorType end() {
                return RangedForIteratorType( ArrayNum, GetData() + Num() );
        }
        FORCEINLINE RangedForConstIteratorType end() const {
                return RangedForConstIteratorType( ArrayNum,
                                                   GetData() + Num() );
        }
};
        
template <typename T> struct TTupleBaseElement {
        TTupleBaseElement( TTupleBaseElement && ) = default;
        TTupleBaseElement( const TTupleBaseElement & ) = default;
        TTupleBaseElement &operator=( TTupleBaseElement && ) = default;
        TTupleBaseElement &operator=( const TTupleBaseElement & ) = default;

        TTupleBaseElement() : Value() {}

        T Value;
};

        template <typename... Types> 
        struct TTupleBase : TTupleBaseElement<Types>... { 
                TTupleBase() = default;
                TTupleBase(TTupleBase&& Other) = default;
                TTupleBase(const TTupleBase& Other) = default;
                TTupleBase& operator=(TTupleBase&& Other) = default;
                TTupleBase& operator=(const TTupleBase& Other) = default;
    
                template<typename... Args>
                TTupleBase(Args&&... args) : TTupleBaseElement<Types>(std::forward<Args>(args))... {}
        };
        template <typename... Types> 
        struct TTuple : TTupleBase<Types...> {  
        public:
                TTuple() = default;
                TTuple(TTuple&&) = default;
                TTuple(const TTuple&) = default;
                TTuple& operator=(TTuple&&) = default;
                TTuple& operator=(const TTuple&) = default;
    
                template<typename... Args>
                TTuple(Args&&... args) : TTupleBase<Types...>(std::forward<Args>(args)...) {}

                template<int Index>
  auto& Get() {
                        if constexpr (Index == 0) {
                                return static_cast<TTupleBaseElement<typename std::tuple_element<0, std::tuple<Types...>>::type>*>(this)->Value;
                        } else if constexpr (Index == 1) {
                                return static_cast<TTupleBaseElement<typename std::tuple_element<1, std::tuple<Types...>>::type>*>(this)->Value;
                        }
                }
        };

        template <typename KeyType, typename ValueType>
        struct TPair {
        KeyType Key;
        ValueType Value;

        TPair() = default;
        TPair(const KeyType& InKey, const ValueType& InValue) : Key(InKey), Value(InValue) {}
        TPair(TPair&&) = default;
        TPair(const TPair&) = default;
        TPair& operator=(TPair&&) = default;
        TPair& operator=(const TPair&) = default;

        bool operator==(const TPair& Other) const {
                return Key == Other.Key && Value == Other.Value;
        }

        bool operator!=(const TPair& Other) const {
                return !(*this == Other);
        }
};

/** Allocated elements are overlapped with free element info in the element
 * list. */
template <typename ElementType> union TSparseArrayElementOrFreeListLink {
        /** If the element is allocated, its value is stored here. */
        ElementType ElementData;

        struct {
                /** If the element isn't allocated, this is a link to the
                 * previous element in the array's free list. */
                int32 PrevFreeIndex;

                /** If the element isn't allocated, this is a link to the next
                 * element in the array's free list. */
                int32 NextFreeIndex;
        };
};

template <int32 Size, uint32 Alignment> struct TAlignedBytes {
        alignas( Alignment ) uint8 Pad[Size];
};

template <uint32 NumInlineElements> class TInlineAllocator {
      public:
        template <typename ElementType> class ForElementType {
              private:
                static constexpr int32 ElementSize = sizeof( ElementType );
                static constexpr int32 ElementAlign = alignof( ElementType );

                static constexpr int32 InlineDataSizeBytes =
                    NumInlineElements * ElementSize;

              private:
                TAlignedBytes<ElementSize, ElementAlign>
                    InlineData[NumInlineElements];
                ElementType *SecondaryData;

              public:
                ForElementType()
                    : InlineData{ 0x0 }, SecondaryData( nullptr ) {}

                ForElementType( ForElementType && ) = default;
                ForElementType( const ForElementType & ) = default;

              public:
                ForElementType &operator=( ForElementType && ) = default;
                ForElementType &operator=( const ForElementType & ) = default;

              public:
                inline const ElementType *GetAllocation() const {
                        return SecondaryData
                                   ? SecondaryData
                                   : reinterpret_cast<const ElementType *>(
                                         &InlineData );
                }

                inline uint32 GetInitialCapacity() const {
                        return NumInlineElements;
                }
        };
};

#define NumBitsPerDWORD ( (int32)32 )
#define NumBitsPerDWORDLogTwo ( (int32)5 )

struct FBitSet {
        /** Clears the next set bit in the mask and returns its index. */
        static FORCEINLINE uint32 GetAndClearNextBit( uint32 &Mask ) {
                const uint32 LowestBitMask = ( Mask ) & ( -(int32)Mask );
                const uint32 BitIndex = FMath::FloorLog2( LowestBitMask );
                Mask ^= LowestBitMask;
                return BitIndex;
        }

        // Clang generates 7 instructions for int32 DivideAndRoundUp but only 2
        // for uint32
        static constexpr uint32 BitsPerWord = NumBitsPerDWORD;

        FORCEINLINE static uint32 CalculateNumWords( int32 NumBits ) {
                checkSlow( NumBits >= 0 );
                return FMath::DivideAndRoundUp( static_cast<uint32>( NumBits ),
                                                BitsPerWord );
        }
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

class TBitArray {
        typedef uint32 WordType;
        static constexpr WordType FullWordMask = (WordType)-1;

      public:
        TBitArray()
            : NumBits( 0 ), MaxBits( AllocatorInstance.GetInitialCapacity() *
                                     NumBitsPerDWORD ) {
                // ClearPartialSlackBits is already satisfied since final word
                // does not exist when NumBits == 0
        }

        /**
         * Minimal initialization constructor.
         * @param Value - The value to initial the bits to.
         * @param InNumBits - The initial number of bits in the array.
         */
        FORCEINLINE explicit TBitArray( bool bValue, int32 InNumBits )
            : MaxBits( AllocatorInstance.GetInitialCapacity() *
                       NumBitsPerDWORD ) {}

        FORCEINLINE void ZeroAll()
        {
            for (int i = 0; i < MaxBits; i++)
            {
                Set(i, false, true);
            }
        }
        
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
        void CheckInvariants() const {

                checkf( NumBits <= MaxBits,
                        TEXT( "TBitArray::NumBits (%d) should never be greater "
                              "than MaxBits (%d)" ),
                        NumBits, MaxBits );
                checkf(
                    NumBits >= 0 && MaxBits >= 0,
                    TEXT(
                        "NumBits (%d) and MaxBits (%d) should always be >= 0" ),
                    NumBits, MaxBits );

                // Verify the ClearPartialSlackBits invariant
                const int32 UsedBits = ( NumBits % NumBitsPerDWORD );
                if ( UsedBits != 0 ) {
                        const int32 LastWordIndex = NumBits / NumBitsPerDWORD;
                        const uint32 SlackMask = FullWordMask << UsedBits;

                        const uint32 LastWord = *( GetData() + LastWordIndex );
                        checkf( ( LastWord & SlackMask ) == 0,
                                TEXT( "TBitArray slack bits are non-zero, this "
                                      "will result in undefined behavior." ) );
                }
        }

        FORCEINLINE const uint32 *GetData() const {
                return (uint32 *)AllocatorInstance.GetAllocation();
        }

        FORCEINLINE uint32 *GetData() {
                return (uint32 *)AllocatorInstance.GetAllocation();
        }

        FORCEINLINE bool IsValidIndex(int32 Index) const {
                return Index >= 0 && Index < NumBits;
        }

        FORCEINLINE bool operator[](int32 Index) const {
                check(IsValidIndex(Index));
                const int32 WordIndex = Index / NumBitsPerDWORD;
                const int32 BitIndex = Index % NumBitsPerDWORD;
                const uint32 Word = GetData()[WordIndex];
                return (Word & (1u << BitIndex)) != 0;
        }

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
            return Index < IteratedArray.NumBits;
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

            if (IteratedArray.NumBits < IteratedArray.NumBits)
            {
                return (bool)FBitReference(IteratedArray.GetData()[this->DWORDIndex], this->Mask);
            }
            else
            {
                return (bool)FBitReference(IteratedArray.GetData()[this->DWORDIndex], this->Mask);
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
            const uint32* ArrayData = IteratedArray.GetData();

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

            FBitReference(GetData()[DWORDIndex], Mask).SetBit(Value);
        }
    
      private:
        TInlineAllocator<4>::ForElementType<int32> AllocatorInstance;
        int32 NumBits;
        int32 MaxBits;

        /**
         * Clears the slack bits within the final partially relevant Word
         */
        void ClearPartialSlackBits() {
                // TBitArray has a contract about bits outside of the active
                // range - the bits in the final word past NumBits are
                // guaranteed to be 0 This prevents easy-to-make determinism
                // errors from users of TBitArray that do not carefully mask the
                // final word. It also allows us optimize some operations which
                // would otherwise require us to mask the last word.
                const int32 UsedBits = NumBits % NumBitsPerDWORD;
                if ( UsedBits != 0 ) {
                        const int32 LastWordIndex = NumBits / NumBitsPerDWORD;
                        const uint32 SlackMask =
                            FullWordMask >> ( NumBitsPerDWORD - UsedBits );

                        uint32 *LastWord = ( GetData() + LastWordIndex );
                        *LastWord = *LastWord & SlackMask;
                }
        }

        FORCEINLINE uint32 GetNumWords() const {
                return FBitSet::CalculateNumWords( NumBits );
        }

        FORCEINLINE uint32 GetMaxWords() const {
                return FBitSet::CalculateNumWords( MaxBits );
        }

        FORCEINLINE static void SetWords( uint32 *Words, int32 NumWords,
                                          bool bValue ) {
                if ( NumWords > 8 ) {
                        memset( Words, bValue ? 0xff : 0,
                                NumWords * sizeof( uint32 ) );
                } else {
                        uint32 Word = bValue ? ~0u : 0u;
                        for ( int32 Idx = 0; Idx < NumWords; ++Idx ) {
                                Words[Idx] = Word;
                        }
                }
        }

        /**
         * Removes all bits from the array retaining any space already
         * allocated.
         */
        void Reset() { NumBits = 0; }
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
        return *(FSparseArrayElement*)&Data[(Index)].ElementData;
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
        // return ((FSparseArrayElement*)Data.Data)[Index];
    }

    /** Accessor for the element or free list data. */
    const FSparseArrayElement& GetData(int32 Index) const
    {
        return *(const FSparseArrayElement*)&Data.at(Index).ElementData;
        // return ((FSparseArrayElement*)Data.Data)[Index];
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
        /* if (!TIsTriviallyDestructible<ElementType>::Value)
        {
            for (int32 It = Index, ItCount = Count; ItCount; ++It, --ItCount)
            {
                ((ElementType&)GetData(It).ElementData).~ElementType();
            }
        } */

        RemoveAtUninitialized(Index, Count);
    }

    /** Removes Count elements from the array, starting from Index, without destructing them. */
    void RemoveAtUninitialized(int32 Index, int32 Count = 1)
    {
        for (; Count; --Count)
        {
            // check(AllocationFlags[Index]);

            // Mark the element as free and add it to the free element list.
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
            // AllocationFlags[Index] = false;

            ++Index;
        }
    }

    FORCEINLINE int32_t Add(ArrayType InElement)
    {
        FSparseArrayElement Element(InElement);

        int32_t NextFree;
        int32_t OutIndex;
        if (FirstFreeIndex >= 1)
        {
            NextFree = Data[FirstFreeIndex].NextFreeIndex;
            Data[FirstFreeIndex] = Element;
            --NumFreeIndices;

            AllocationFlags.Set(FirstFreeIndex, true);

            if (NumFreeIndices >= 1)
            {
                OutIndex = NextFree;
                FirstFreeIndex = NextFree;
                Data[NextFree].PrevFreeIndex = -1;

                return OutIndex;
            }
        }
        else
        {
            Data.Add(Element);
            AllocationFlags.Set(Data.Num() - 1, true);

            return Data.Num() - 1;
        }
    }

    FORCEINLINE void Reset()
    {
        Data.Free();

        AllocationFlags.ZeroAll();

        NumFreeIndices = 0;
        FirstFreeIndex = -1;
    }
};
    
template <typename InElementType> class TSetElement {
      private:
        template <typename SetDataType> friend class TSet;

      private:
        InElementType Value;
        int32 HashNextId;
        int32 HashIndex;
};


template <typename InElementType> 
class TSet {
public:
    typedef InElementType ElementType;

private:
    typedef TSetElement<InElementType> SetElementType;

public:
    FORCEINLINE TSet() : HashSize(0) {}

private:
    typedef TSparseArray<SetElementType> ElementArrayType;
    using HashType = TInlineAllocator<1>::ForElementType<int32>;

    ElementArrayType Elements;
    mutable HashType Hash;
    mutable int32 HashSize;

public:
    class Iterator
    {
    private:
        ElementArrayType* ElementsPtr;
        int32 CurrentIndex;

    public:
        Iterator(ElementArrayType* InElements, int32 InIndex) 
            : ElementsPtr(InElements), CurrentIndex(InIndex)
        {
            while (CurrentIndex < ElementsPtr->Num() && !ElementsPtr->IsIndexValid(CurrentIndex))
            {
                ++CurrentIndex;
            }
        }

        InElementType& operator*()
        {
            return (*ElementsPtr)[CurrentIndex].ElementData.Value;
        }

        const InElementType& operator*() const
        {
            return (*ElementsPtr)[CurrentIndex].ElementData.Value;
        }

        InElementType* operator->()
        {
            return &((*ElementsPtr)[CurrentIndex].ElementData.Value);
        }

        const InElementType* operator->() const
        {
            return &((*ElementsPtr)[CurrentIndex].ElementData.Value);
        }

            Iterator& operator++()
        {
                ++CurrentIndex;
                while (CurrentIndex < ElementsPtr->Num() && !ElementsPtr->IsIndexValid(CurrentIndex))
                {
                        ++CurrentIndex;
                }
                return *this;
        }


        Iterator operator++(int)
        {
            Iterator temp = *this;
            ++(*this);
            return temp;
        }

        bool operator==(const Iterator& Other) const
        {
            return CurrentIndex == Other.CurrentIndex;
        }

        bool operator!=(const Iterator& Other) const
        {
            return !(*this == Other);
        }
    };

    class ConstIterator
    {
    private:
        const ElementArrayType* ElementsPtr;
        int32 CurrentIndex;

    public:
        ConstIterator(const ElementArrayType* InElements, int32 InIndex) 
            : ElementsPtr(InElements), CurrentIndex(InIndex)
        {
            while (CurrentIndex < ElementsPtr->GetMaxIndex() && !ElementsPtr->IsAllocated(CurrentIndex))
            {
                ++CurrentIndex;
            }
        }

        const InElementType& operator*() const
        {
            return (*ElementsPtr)[CurrentIndex].ElementData.Value;
        }

        const InElementType* operator->() const
        {
            return &((*ElementsPtr)[CurrentIndex].ElementData.Value);
        }

            ConstIterator& operator++()
        {
                ++CurrentIndex;
                while (CurrentIndex < ElementsPtr->Num() && !ElementsPtr->IsValidIndex(CurrentIndex))
                {
                        ++CurrentIndex;
                }
                return *this;
        }

        ConstIterator operator++(int)
        {
            ConstIterator temp = *this;
            ++(*this);
            return temp;
        }

        bool operator==(const ConstIterator& Other) const
        {
            return CurrentIndex == Other.CurrentIndex;
        }

        bool operator!=(const ConstIterator& Other) const
        {
            return !(*this == Other);
        }
    };

    Iterator begin()
    {
        return Iterator(&Elements, 0);
    }

    Iterator end()
    {
        return Iterator(&Elements, Elements.Data.Num());
    }

    ConstIterator begin() const
    {
        return ConstIterator(&Elements, 0);
    }

    ConstIterator end() const
    {
        return ConstIterator(&Elements, Elements.GetMaxIndex());
    }

    int32 Num() const
    {
        return Elements.Num();
    }

    bool IsEmpty() const
    {
        return Elements.Num() == 0;
    }
};

template <typename KeyType, typename ValueType> class TMapBase {
        template <typename OtherKeyType, typename OtherValueType>
        friend class TMapBase;

      public:
        typedef TPair<KeyType, ValueType> ElementType;

      protected:
        TMapBase() = default;
        TMapBase( TMapBase && ) = default;
        TMapBase( const TMapBase & ) = default;
        TMapBase &operator=( TMapBase && ) = default;
        TMapBase &operator=( const TMapBase & ) = default;

      protected:
        typedef TSet<ElementType> ElementSetType;

        /** A set of the key-value pairs in the map. */
        ElementSetType Pairs;
};

        template <typename InKeyType, typename InValueType>
        class TMap : TMapBase<InKeyType, InValueType> {
        public:
                typedef InKeyType KeyType;
                typedef InValueType ValueType;
                typedef typename TMapBase<InKeyType, InValueType>::ElementType ElementType;

                TMap() = default;
                TMap( TMap && ) = default;
                TMap( const TMap & ) = default;
                TMap &operator=( TMap && ) = default;
                TMap &operator=( const TMap & ) = default;

                auto begin() -> decltype(this->Pairs.begin()) {
                        return this->Pairs.begin();
                }
    
                auto end() -> decltype(this->Pairs.end()) {
                        return this->Pairs.end();
                }
    
                auto begin() const -> decltype(this->Pairs.begin()) {
                        return this->Pairs.begin();
                }
    
                auto end() const -> decltype(this->Pairs.end()) {
                        return this->Pairs.end();
                }

                int32 Num() const {
                        return this->Pairs.Num();
                }
        };

#define UE_PTRDIFF_TO_INT32( argument ) static_cast<int32>( argument )
#define UE_PTRDIFF_TO_UINT32( argument ) static_cast<uint32>( argument )

#define _tcslen wcslen
#define _tcsstr wcsstr
static FORCEINLINE int32 Strlen( const TCHAR *String ) {
        return _tcslen( String );
}

typedef TCHAR WIDECHAR;

FORCEINLINE const typename TCHAR *Strstr( const TCHAR *String,
                                          const TCHAR *Find ) {
        return _tcsstr( String, Find );
}

static std::vector<std::string> SplitString( const std::string &input,
                                             const char *delimiter,
                                             size_t maxParts = 3 ) {
        std::vector<std::string> parts;
        size_t pos = 0;
        size_t delimLen = std::strlen( delimiter );
        size_t start = 0;

        while ( ( pos = input.find( delimiter, start ) ) != std::string::npos &&
                parts.size() < maxParts - 1 ) {
                parts.emplace_back( input.substr( start, pos - start ) );
                start = pos + delimLen;
        }

        parts.emplace_back( input.substr( start ) ); // Remainder
        return parts;
}

class FString {

      private:
        typedef TArray<wchar_t> DataType;
        DataType Data;

      public:
        using ElementType = wchar_t;

        FString() = default;
        FString( FString && ) = default;
        FString( const FString & ) = default;
        FString &operator=( FString && ) = default;
        FString &operator=( const FString & ) = default;

      public:
        FString( const wchar_t *Str ) {
                const uint32 NullTerminatedLength =
                    static_cast<uint32>( wcslen( Str ) + 0x1 );

                Data.Data = ( const_cast<wchar_t *>( Str ) );
                Data.ArrayNum = NullTerminatedLength;
                Data.ArrayMax = NullTerminatedLength;
        }

        FString( const std::string &Str )
            : FString( std::wstring( Str.begin(), Str.end() ).c_str() ) {}

        FString( const std::wstring &Str ) : FString( Str.c_str() ) {}

      public:
        bool IsValid() const
        {
                return Data.Num() > 0;
        }
        
        DataType GetData()
        {
                return Data;
        }
        std::string ToString() {
                wchar_t *ProcessedData = Data.GetData();
                std::wstring WideDataString( ProcessedData );

                return std::string( WideDataString.begin(),
                                    WideDataString.end() );
        }

        std::string ToString() const {
                const wchar_t *ProcessedData = Data.GetData();
                std::wstring WideDataString( ProcessedData );

                return std::string( WideDataString.begin(),
                                    WideDataString.end() );
        }

        std::wstring ToWideString() {
                wchar_t *ProcessedData = Data.GetData();
                std::wstring WideDataString( ProcessedData );

                return WideDataString;
        }

        FORCEINLINE const TCHAR *operator*() const {
                const wchar_t *t = L"";
                return Data.Num() ? Data.GetData() : t;
        }

        bool operator!=( std::string &Other ) {
                return this->ToString() != Other;
        }
        bool operator!=( std::wstring &Other ) {
                return this->ToWideString() != Other;
        }
        bool operator!=( FString &Other ) {
                return this->Data.Data != Other.Data.Data;
        }
        bool operator!=( const char *Other ) {
                int result = std::strcmp( this->ToString().c_str(), Other );
                return !!result;
        }
        bool operator!=( const wchar_t *Other ) {
                int result = std::wcscmp( this->Data.Data, Other );
                return !!result;
        }

        bool operator==( std::string &Other ) {
                return this->ToString() == Other;
        }
        bool operator==( std::wstring &Other ) {
                return this->ToWideString() == Other;
        }
        bool operator==( FString &Other ) {
                return this->Data.Data == Other.Data.Data;
        }
        bool operator==( const char *Other ) {
                int result = std::strcmp( this->ToString().c_str(), Other );
                return !result;
        }
        bool operator==( const wchar_t *Other ) {
                int result = std::wcscmp( this->Data.Data, Other );
                return !result;
        }

        friend std::ostream &operator<<( std::ostream &Stream,
                                         FString &Str ) {
                return Stream << Str.ToString();
        }
        friend std::istream &operator>>( std::istream &Stream,
                                         FString &Str ) {
                std::string string = Str.ToString();
                return Stream >> string;
        }
        friend std::ostream &operator<<( std::ofstream &Stream,
                                         FString &Str ) {
                return Stream << Str.ToString();
        }
        friend std::ostream &operator<<( std::ostream &Stream,
                                         FString Str ) {
                return Stream << Str.ToString();
        }
        friend std::istream &operator>>( std::istream &Stream,
                                         FString Str ) {
                std::string string = Str.ToString();
                return Stream >> string;
        }
        friend std::ostream &operator<<( std::ofstream &Stream,
                                         FString Str ) {
                return Stream << Str.ToString();
        }

        void ParseIntoArray( std::vector<std::string> &OutArray,
                             FString &Delimiter, bool bCullEmpty = true ) {
                size_t Start = 0;
                size_t End;
                std::string Source = this->ToString();
                std::string Delim = Delimiter.ToString();

                UE_LOG( LogFortSDK, VeryVerbose, "Source: %s, Delim : %s",
                        Source.c_str(), Delim.c_str() );

                auto Dih = SplitString( Source, Delim.c_str() );

                if ( Dih.size() == 3 ) {
                        for ( std::string Str : Dih ) {
                                OutArray.push_back( Str );
                        }
                }
        }
};

        
};


#pragma once
namespace SDK
{


class FKismetMemoryLibrary {
      public:
        FKismetMemoryLibrary( const char* pattern, 
             uint64_t ModuleBaseAddress);

        FKismetMemoryLibrary( const wchar_t* stringRef,
                              const char *moduleName = 0 );

        FKismetMemoryLibrary(void* Data, int Size, int32_t OldSize);

        FKismetMemoryLibrary( uint64_t Address );

      private:
        uint64_t FindPattern( const std::string &pattern,
                              uint64_t m_ModuleBase );

        uint64_t
        FindStringRef( const wchar_t* target, 
            HMODULE m_ModuleBase );

       void
    ResizeVirtualMemory(int32_t OldSize, int32_t NewSize);

       

      protected:
        uint64_t m_address = 0;  
        void *m_data = nullptr;
        uint32_t m_status;

      public:
        template <typename T> T *GetInitalizedMemory()
            {
            if (m_data == nullptr)
                return nullptr;
            return static_cast<T *>(m_data);
        };

    // In header:
    template <typename T>
    T* AtPtr(int32_t Size) {
        if (m_data == nullptr) {
            m_status = 0;
            return nullptr;
        }
    
        if (Size <= 0) {
            m_status = 0;
            return nullptr;
        }
    
        if (Size > sizeof(T)) {
            ResizeVirtualMemory(sizeof(T), Size);
            if (m_data == nullptr) {
                m_status = 0;
                return nullptr;
            }
        }
    
        m_status = 1;
        return static_cast<T*>(m_data);
    }

    
        uint64_t GetAddress() const;
        template <typename T> T GetAddress() const {
                return *reinterpret_cast<T *>( m_address );
        }

        uint32_t GetStatus() const;

        void SwitchPattern( const std::string &pattern,
                                uint64_t m_ModuleBase = uint64_t(GetModuleHandle(0)) );

        void SwitchStringRef( const wchar_t* stringRef,
                              const char *moduleName );
        void SwitchAddress( uint64_t m_NewAddress );

        void ScanFor( const std::vector<BYTE> &BytesToFind, bool forward = true,
                      size_t skip = 0 );

        void AbsoluteOffset( int Offset );
        void RelativeOffset( int Offset );
};


}
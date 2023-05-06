#include <Windows.h>

#include <iostream>
#include <cstdint>
#include <string>
#include "../utils/vector.h"

#pragma comment (lib, "ntdll.lib")

namespace memory {
    EXTERN_C NTSTATUS NTAPI NtReadVirtualMemory(HANDLE, PVOID, PVOID, ULONG, PULONG);
    EXTERN_C NTSTATUS NTAPI NtWriteVirtualMemory(HANDLE, PVOID, PVOID, ULONG, PULONG);

    inline int process_id = 0;
    inline void* handle = nullptr;
    inline uintptr_t client = 0;
    inline uintptr_t engine = 0;

    void GetProcess(const std::string name);

    uintptr_t GetModuleAddress(const std::string name);

    bool OpenPHandle();
    void ClosePHandle();

    template <typename T>
    constexpr T Read(const uintptr_t& address) {
        T value{};

        NTSTATUS status = NtReadVirtualMemory(
            handle, 
            reinterpret_cast<PVOID>(address),
            &value,
            sizeof(T), 
            0
        );

        return value;
    }

    template <typename T>
    constexpr void Write(const uintptr_t& address, const T& value) {
        NTSTATUS status = NtWriteVirtualMemory(
            handle, 
            reinterpret_cast<void*>(address), 
            const_cast<void*>(static_cast<const void*>(&value)), 
            sizeof(T), 
            0
        );
    }
}
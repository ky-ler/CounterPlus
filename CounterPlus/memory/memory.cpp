#include "memory.h"

#include <TlHelp32.h>

void memory::GetProcess(const std::string process_name) {
    auto entry = PROCESSENTRY32{};
    entry.dwSize = sizeof(PROCESSENTRY32);

    const auto snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    while (Process32Next(snap, &entry)) {
        if (!process_name.compare(entry.szExeFile)) {
            process_id = entry.th32ProcessID;
            break;
        }
    }

    if (snap) {
        CloseHandle(snap);
    }
}

uintptr_t memory::GetModuleAddress(const std::string name) {
    auto entry = MODULEENTRY32{};
    entry.dwSize = sizeof(MODULEENTRY32);

    const auto snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, process_id);

    uintptr_t addr = 0;

    // loop through modules
    do {
        if (!name.compare(entry.szModule)) {
            addr = reinterpret_cast <uintptr_t> (entry.modBaseAddr);
            break;
        }
    } while (Module32Next(snap, &entry));

    // Close the snapshot handle.
    if (snap) {
        CloseHandle(snap);
    }

    return addr;
}

bool memory::OpenPHandle() {
    return handle = OpenProcess(PROCESS_ALL_ACCESS, 0, process_id);
}

void memory::ClosePHandle() {
    if (handle) {
        CloseHandle(handle);
    }
}
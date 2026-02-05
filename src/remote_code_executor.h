#pragma once

#include <string>
#include <string_view>
#include <Windows.h>

class RemoteCodeExecutor {
public:
    RemoteCodeExecutor() = default;
    RemoteCodeExecutor(const RemoteCodeExecutor& other) = delete;
    RemoteCodeExecutor(RemoteCodeExecutor&& other) = delete;
    RemoteCodeExecutor operator=(const RemoteCodeExecutor& other) = delete;
    RemoteCodeExecutor operator=(RemoteCodeExecutor&& other) = delete;

    static void InjectDLL(std::wstring_view dll_path, std::wstring_view victim_proc_name);
    static void InjectShellcode(const uint8_t* shelcode, 
        SIZE_T shellcode_size, 
        std::wstring_view victim_proc_name);

private:
    static DWORD GetProcessId(std::wstring_view victim_name);
    static HANDLE OpenVictimProcess(DWORD victim_pid);

    static LPVOID AllocateMemoryInVictim(HANDLE hVictim, LPVOID address, SIZE_T size);
    static void MakeVictimMemoryExecutable(HANDLE hVictim, LPVOID address, SIZE_T size);
    static void FreeMemoryInVictim(HANDLE hVictim, LPVOID allocated_memory, HANDLE hThread);
    static void WriteToVictimMemory(HANDLE hVictim, LPVOID address
        , LPCVOID data_buffer, SIZE_T data_size);

    static HANDLE CreateThreadInVictim(HANDLE hVictim, LPVOID entry, LPVOID params);

    static LPVOID GetLoadLibraryFunc();
};
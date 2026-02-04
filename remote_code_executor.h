#include <string>
#include <string_view>


class RemoteCodeExecutor {
public:
    RemoteCodeExecutor() = default;
    RemoteCodeExecutor(const RemoteCodeExecutor& other) = delete;
    RemoteCodeExecutor(RemoteCodeExecutor&& other) = delete;
    RemoteCodeExecutor operator=(const RemoteCodeExecutor& other) = delete;
    RemoteCodeExecutor operator=(RemoteCodeExecutor&& other) = delete;

    static void InjectDLL(std::wstring_view dll_path, std::wstring_view victim_proc_name) {
        
    }

    static void InjectShellcode(const char* shelcode, std::wstring_view victim_proc_name) {

    }

private:

    static LPVOID AllocateMemoryInVictim(HANDLE hVictim, LPVOID address, SIZE_T size) {
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

    static void FreeMemoryInVictim(HANDLE hVictim, LPVOID allocated_memory) {
        if (allocated_memory && hVictim) {
            VirtualFreeEx(hVictim, allocated_memory, 0, MEM_RELEASE);
            CloseHandle(hVictim);
            return;
        }
        if (hVictim) {
            CloseHandle(hVictim);
        }
    }

    static HANDLE OpenVictimProcess(DWORD victim_pid) {
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

    static DWORD GetProcessId(std::wstring_view victim_name) {
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

    static std::string WideCharToString(const WCHAR* wstr) {
        if (!wstr) {
            return "";
        }

        int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, nullptr, 0, nullptr, nullptr);
        std::string str(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstr, -1, &str[0], size_needed, nullptr, nullptr);

        if (!str.empty() && str.back() == '\0') {
            str.pop_back();
        }
        return str;
    }

}
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

private:

}
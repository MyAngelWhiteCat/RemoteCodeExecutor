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

}
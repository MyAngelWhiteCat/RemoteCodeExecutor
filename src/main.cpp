#include <iostream>
#include <filesystem>
#include <string>
#include <exception>

#include "remote_code_executor.h"

int main() {
    std::wstring proc_name = L"test.exe";
    std::wstring dll_name = L"test.dll";
    std::filesystem::path dll_path = std::filesystem::current_path() / dll_name;
    if (!std::filesystem::exists(dll_path)) {
        std::wcout << dll_name << " is not exist" << std::endl;
        return 1;
    }

    try {
        RemoteCodeExecutor::InjectDLL(dll_path.wstring(), proc_name);
        std::wcout << "Successfully injected " << dll_name << " to " << proc_name << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}
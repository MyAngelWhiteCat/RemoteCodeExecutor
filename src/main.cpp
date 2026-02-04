#include <iostream>
#include <filesystem>
#include <string>
#include <exception>

#include "remote_code_executor.h"

int main(int argc, wchar_t** argv) {
    if (argc != 3) {
        std::wcout << "Usage: " << argv[0] << " <dll> <process>" << std::endl;
    }
    std::filesystem::path dll_path(argv[1]);
    if (!std::filesystem::exists(dll_path)) {
        std::wcout << argv[1] << " is not exist" << std::endl;
        return 1;
    }

    try {
        RemoteCodeExecutor::InjectDLL(dll_path.wstring(), argv[2]);
        std::wcout << "Successfully injected " << argv[1] << " to " << argv[2] << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}
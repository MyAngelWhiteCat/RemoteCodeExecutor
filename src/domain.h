#pragma once

#include <windows.h>
#include <string>
#include <string_view>


namespace domain {

    std::string WideCharToString(const WCHAR* wstr);
    std::wstring StringToWideChar(std::string_view str);

}
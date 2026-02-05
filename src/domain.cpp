#include "domain.h"

#include <string>
#include <string_view>
#include <stringapiset.h>
#include <Windows.h>


namespace domain {

    std::string WideCharToString(const WCHAR* wstr) {
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

    std::wstring StringToWideChar(std::string_view str) {
        if (str.empty()) {
            return L"";
        }
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.data(), -1, nullptr, 0);
        wchar_t* wide_str = new wchar_t[size_needed];
        MultiByteToWideChar(CP_UTF8, 0, str.data(), -1, wide_str, size_needed);
        return wide_str;
    }

}
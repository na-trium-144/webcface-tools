#define TOOLS_VERSION_DISP(name) name " (webcface-tools " TOOLS_VERSION ")"

#ifdef WIN32
#include <windows.h>
#include <string>

inline std::string acpToUTF8(const char *bytes, int n) {
    auto length = MultiByteToWideChar(CP_ACP, 0, bytes, n, nullptr, 0);
    std::wstring result(length, '\0');
    MultiByteToWideChar(CP_ACP, 0, bytes, n, result.data(),
                        static_cast<int>(result.length()));
    auto length_utf8 = WideCharToMultiByte(CP_UTF8, 0, result.data(),
                                           static_cast<int>(result.length()),
                                           nullptr, 0, nullptr, nullptr);
    std::string result_utf8(length_utf8, '\0');
    WideCharToMultiByte(CP_UTF8, 0, result.data(),
                        static_cast<int>(result.length()), result_utf8.data(),
                        static_cast<int>(result_utf8.length()), nullptr,
                        nullptr);
    return result_utf8;
}
inline std::string acpToUTF8(const std::string &str) {
    return acpToUTF8(str.c_str(), static_cast<int>(str.size()));
}
#endif
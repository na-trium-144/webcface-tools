#pragma once
#include <string>

inline std::string &defaultStatus() {
    static std::string s = "↓/↑ or J/K = move, Ctrl+C = quit";
    return s;
}

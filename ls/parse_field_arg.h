#pragma once
#include "./main.h"
#include "webcface/client.h"
#include <algorithm>
#include <ftxui/dom/elements.hpp>
#include <iostream>

// 引数のfieldsをパースしてmember名とfield名にわけ、それぞれコールバックを呼ぶ
template <typename F>
void parseFieldArgs(webcface::Client &wcli,
                    const std::vector<std::string> &fields, F func) {
    auto wcli_members = wcli.members();
    for (const std::string &fp : fields) {
        auto fp_colon = fp.find(':');
        std::string member_name;
        std::string field_name;
        if (fp_colon == std::string::npos) {
            member_name = fp;
        } else {
            member_name = fp.substr(0, fp_colon);
            field_name = fp.substr(fp_colon + 1);
        }
        if (std::find_if(wcli_members.begin(), wcli_members.end(),
                         [&](const webcface::Member &m) {
                             return m.name() == member_name;
                         }) == wcli_members.end()) {
            std::cerr << "Member '" << member_name << "': not found"
                      << std::endl;
            ok = false;
            continue;
        }
        func(member_name, field_name);
    }
}

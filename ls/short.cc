#include "./main.h"
#include "./find_field.h"
#include "./parse_field_arg.h"

ftxui::Element lsFlex(std::vector<ftxui::Element> &&elements) {
    // lsの表示は縦に並ぶけど、ftxuiでそれを実現するのめんどくさそう
    // とりあえず動けばいいので横並びにしている
    ftxui::FlexboxConfig config;
    config.direction = ftxui::FlexboxConfig::Direction::Row;
    config.wrap = ftxui::FlexboxConfig::Wrap::Wrap;
    config.justify_content = ftxui::FlexboxConfig::JustifyContent::FlexStart;
    config.align_items = ftxui::FlexboxConfig::AlignItems::FlexStart;
    config.align_content = ftxui::FlexboxConfig::AlignContent::FlexStart;
    for (auto &e : elements) {
        e = ftxui::hbox(e, elemSpace1());
    }
    return ftxui::flexbox(std::move(elements), config);
}

ftxui::Element listMemberShort(webcface::Client &wcli) {
    std::vector<ftxui::Element> elem_members;
    for (const auto &m : wcli.members()) {
        elem_members.push_back(elemMember(m));
    }
    return lsFlex(std::move(elem_members));
}

ftxui::Element listMemberShortRecurse(webcface::Client &wcli) {
    std::vector<ftxui::Element> elem_members;
    for (const auto &m : wcli.members()) {
        elem_members.push_back(fieldInfoShort(m, "", true, true));
    }
    return ftxui::vbox(elem_members);
}
ftxui::Element listFieldsShort(webcface::Client &wcli,
                               const std::vector<std::string> &fields,
                               bool recursive) {
    std::vector<ftxui::Element> elem_members;
    parseFieldArgs(
        wcli, fields,
        [&](const std::string &member_name, const std::string &field_name) {
            auto m = wcli.member(member_name);
            elem_members.push_back(fieldInfoShort(
                m, field_name, recursive, recursive || fields.size() > 1));
        });
    return ftxui::vbox(elem_members);
}

ftxui::Element fieldInfoShort(const webcface::Member &m,
                              const std::string &field_prefix, bool recursive,
                              bool with_header) {
    std::vector<ftxui::Element> elem_members;
    if (with_header) {
        elem_members.push_back(ftxui::hbox({
            elemMember(m),
            elemColon(),
            ftxui::text(field_prefix),
        }));
    }
    auto [elem_fields, folders] = findFields(
        m, field_prefix,
        [&](const std::string &name, const auto &v) {
            return ftxui::text(name) | ftxui::color(ftxui::Color::Blue);
        },
        [&](const std::string &name, const auto &v) {
            return ftxui::text(name) | ftxui::color(ftxui::Color::Cyan);
        },
        [&](const std::string &name, const auto &v) {
            return ftxui::text(name) | ftxui::color(ftxui::Color::Green);
        },
        [&](const std::string &name, const auto &v) {
            return ftxui::text(name) | ftxui::color(ftxui::Color::Red);
        },
        [&](const std::string &name, const auto &v) {
            return ftxui::text(name) | ftxui::color(ftxui::Color::Yellow);
        },
        [&](const std::string &name, const auto &v) {
            return ftxui::text(name) | ftxui::color(ftxui::Color::Yellow);
        },
        [&](const std::string &name, const auto &v) {
            return ftxui::text(name) | ftxui::color(ftxui::Color::Yellow);
        },
        [&](const std::string &name, const auto &v) {
            return ftxui::text(name) | ftxui::color(ftxui::Color::Yellow);
        });

    for (const auto &fn : folders) {
        elem_fields.emplace(fn, ftxui::text(fn));
    }

    std::vector<ftxui::Element> elem_fields_v;
    for (const auto &it : elem_fields) {
        elem_fields_v.push_back(it.second);
    }
    if (!elem_fields_v.empty()) {
        elem_members.push_back(ftxui::hbox({
            ftxui::emptyElement() |
                ftxui::size(ftxui::WIDTH, ftxui::EQUAL, with_header ? 1 : 0),
            lsFlex(std::move(elem_fields_v)),
        }));
    }

    if (recursive) {
        for (const auto &fn : folders) {
            std::string fn_absolute;
            if (field_prefix.empty()) {
                fn_absolute = fn;
            } else {
                fn_absolute = field_prefix + "." + fn;
            }
            elem_members.push_back(fieldInfoShort(m, fn_absolute, true, true));
        }
    }

    return ftxui::vbox(elem_members);
}

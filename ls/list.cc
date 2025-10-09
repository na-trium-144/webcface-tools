#include "./main.h"
#include "./find_field.h"
#include "./parse_field_arg.h"

ftxui::Element listMemberAll(webcface::Client &wcli, bool recursive) {
    std::vector<ftxui::Element> elem_members;
    for (const auto &m : wcli.members()) {
        elem_members.push_back(memberInfo(m));
        if (recursive) {
            for (const auto &it : fieldInfo(m, "", recursive, 1)) {
                elem_members.push_back(it.second);
            }
        }
    }
    return ftxui::vbox(elem_members);
}

ftxui::Element listFields(webcface::Client &wcli,
                          const std::vector<std::string> &fields,
                          bool recursive) {
    std::vector<ftxui::Element> elem_members;
    parseFieldArgs(
        wcli, fields,
        [&](const std::string &member_name, const std::string &field_name) {
            auto m = wcli.member(member_name);
            int tab = 0;
            if (fields.size() > 1) {
                elem_members.push_back(ftxui::hbox({
                    elemMember(m),
                    elemColon(),
                    ftxui::text(field_name),
                }));
                tab = 1;
            }
            for (const auto &it : fieldInfo(m, field_name, recursive, tab)) {
                elem_members.push_back(it.second);
            }
        });
    return ftxui::vbox(elem_members);
}


ftxui::Element memberInfo(const webcface::Member &m) {
    ftxui::Decorator ping_color;
    std::string ping_str = std::to_string(m.pingStatus().value_or(0));
    if (!m.pingStatus()) {
        ping_color = ftxui::color(ftxui::Color::Red);
        ping_str = "?";
    } else if (*m.pingStatus() < 10) {
        ping_color = ftxui::color(ftxui::Color::Green);
    } else if (*m.pingStatus() < 100) {
        ping_color = ftxui::color(ftxui::Color::Yellow);
    } else {
        ping_color = ftxui::color(ftxui::Color::Red);
    }
    return ftxui::hbox({
        elemMember(m),
        elemColon(),
        elemSpace1(),
        ftxui::text(m.libName()) | ftxui::dim,
        elemSpace1(),
        ftxui::text(m.libVersion()) | ftxui::dim,
        elemSpace1(),
        ftxui::text("[" + ping_str + " ms]") | ping_color,
    });
}

std::multimap<std::string, ftxui::Element>
fieldInfo(const webcface::Member &m, const std::string &field_prefix,
          bool recursive, int tab) {
    auto [elem_fields, folders] = findFields(
        m, field_prefix,
        [&](const std::string &name, auto &v) {
            return ftxui::hbox({
                ftxui::text("[vl]") | ftxui::dim,
                elemSpace1(),
                ftxui::text(name) | ftxui::color(ftxui::Color::Blue),
            });
        },
        [&](const std::string &name, auto &v) {
            return ftxui::hbox({
                ftxui::text("[tx]") | ftxui::dim,
                elemSpace1(),
                ftxui::text(name) | ftxui::color(ftxui::Color::Cyan),
            });
        },
        [&](const std::string &name, auto &v) {
            std::vector<ftxui::Element> fn_args;
            for (const auto &a : v.args()) {
                if (!fn_args.empty()) {
                    fn_args.push_back(ftxui::text(", "));
                }
                if (!a.name().empty()) {
                    fn_args.push_back(ftxui::text(std::string(a.name())));
                    fn_args.push_back(ftxui::text(": ") | ftxui::dim);
                }
                switch (a.type()) {
                case webcface::ValType::int_:
                    fn_args.push_back(ftxui::text("int") | ftxui::dim);
                    break;
                case webcface::ValType::float_:
                    fn_args.push_back(ftxui::text("float") | ftxui::dim);
                    break;
                case webcface::ValType::bool_:
                    fn_args.push_back(ftxui::text("bool") | ftxui::dim);
                    break;
                case webcface::ValType::string_:
                    fn_args.push_back(ftxui::text("str") | ftxui::dim);
                    break;
                default:
                    fn_args.push_back(ftxui::text("?") | ftxui::dim);
                    break;
                }
            }
            return ftxui::hbox({
                ftxui::text("[fn]") | ftxui::dim,
                elemSpace1(),
                ftxui::text(name) | ftxui::color(ftxui::Color::Green),
                ftxui::text("("),
                ftxui::hbox(fn_args),
                ftxui::text(")"),
            });
        },
        [&](const std::string &name, auto &v) {
            return ftxui::hbox({
                ftxui::text("[vi]") | ftxui::dim,
                elemSpace1(),
                ftxui::text(name) | ftxui::color(ftxui::Color::Red),
            });
        },
        [&](const std::string &name, auto &v) {
            return ftxui::hbox({
                ftxui::text("[2d]") | ftxui::dim,
                elemSpace1(),
                ftxui::text(name) | ftxui::color(ftxui::Color::Yellow),
            });
        },
        [&](const std::string &name, auto &v) {
            return ftxui::hbox({
                ftxui::text("[im]") | ftxui::dim,
                elemSpace1(),
                ftxui::text(name) | ftxui::color(ftxui::Color::Yellow),
            });
        },
        [&](const std::string &name, auto &v) {
            return ftxui::hbox({
                ftxui::text("[3d]") | ftxui::dim,
                elemSpace1(),
                ftxui::text(name) | ftxui::color(ftxui::Color::Yellow),
            });
        },
        [&](const std::string &name, auto &v) {
            return ftxui::hbox({
                ftxui::text("[rm]") | ftxui::dim,
                elemSpace1(),
                ftxui::text(name) | ftxui::color(ftxui::Color::Yellow),
            });
        });
    for (const auto &fn : folders) {
        elem_fields.emplace(fn, ftxui::hbox({
                                    ftxui::text("-   "),
                                    elemSpace1(),
                                    ftxui::text(fn),
                                }));
        if (recursive) {
            std::string fn_absolute;
            if (field_prefix.empty()) {
                fn_absolute = fn;
            } else {
                fn_absolute = field_prefix + "." + fn;
            }
            for (const auto &it_subdir :
                 fieldInfo(m, fn_absolute, recursive, 1)) {
                elem_fields.emplace(fn + "." + it_subdir.first,
                                    it_subdir.second);
            }
        }
    }
    if (tab > 0) {
        for (auto it = elem_fields.begin(); it != elem_fields.end(); it++) {
            it->second = ftxui::hbox({
                ftxui::emptyElement() |
                    ftxui::size(ftxui::WIDTH, ftxui::EQUAL, tab),
                it->second,
            });
        }
    }
    if (elem_fields.empty()) {
        ok = false;
    }
    return elem_fields;
}

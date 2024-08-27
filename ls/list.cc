#include "./main.h"
#include <map>
#include <webcface/value.h>
#include <webcface/text.h>
#include <webcface/view.h>
#include <webcface/func.h>
#include <webcface/canvas2d.h>
#include <webcface/canvas3d.h>
#include <webcface/image.h>
#include <webcface/robot_model.h>

ftxui::Element listMember(webcface::Client &wcli, bool recursive) {
    std::vector<ftxui::Element> elem_members;
    for (const auto &m : wcli.members()) {
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
        elem_members.push_back(ftxui::hbox({
            elemMember(m),
            elemColon(),
            elemSpace1(),
            ftxui::text(m.libName()) | ftxui::dim,
            elemSpace1(),
            ftxui::text(m.libVersion()) | ftxui::dim,
            elemSpace1(),
            ftxui::text("[" + ping_str + " ms]") | ping_color,
        }));

        if (recursive) {
            std::multimap<std::string, ftxui::Element> elem_fields;
            if (!m.valueEntries().empty()) {
                for (const auto &v : m.valueEntries()) {
                    elem_fields.emplace(
                        v.name(), ftxui::hbox({
                                      ftxui::text(" [vl]") | ftxui::dim,
                                      ftxui::text(v.name()) |
                                          ftxui::color(ftxui::Color::Blue),
                                  }));
                }
            }
            if (!m.textEntries().empty()) {
                for (const auto &v : m.textEntries()) {
                    elem_fields.emplace(
                        v.name(), ftxui::hbox({
                                      ftxui::text(" [tx]") | ftxui::dim,
                                      ftxui::text(v.name()) |
                                          ftxui::color(ftxui::Color::Cyan),
                                  }));
                }
            }
            if (!m.funcEntries().empty()) {
                for (const auto &v : m.funcEntries()) {
                    std::vector<ftxui::Element> fn_args;
                    for (const auto &a : v.args()) {
                        if (!fn_args.empty()) {
                            fn_args.push_back(ftxui::text(", "));
                        }
                        if (!a.name().empty()) {
                            fn_args.push_back(ftxui::text(a.name()));
                            fn_args.push_back(ftxui::text(": ") | ftxui::dim);
                        }
                        switch (a.type()) {
                        case webcface::ValType::int_:
                            fn_args.push_back(ftxui::text("int") | ftxui::dim);
                            break;
                        case webcface::ValType::float_:
                            fn_args.push_back(ftxui::text("float") |
                                              ftxui::dim);
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
                    elem_fields.emplace(
                        v.name(), ftxui::hbox({
                                      ftxui::text(" [fn]") | ftxui::dim,
                                      ftxui::text(v.name()) |
                                          ftxui::color(ftxui::Color::Green),
                                      ftxui::text("("),
                                      ftxui::hbox(fn_args),
                                      ftxui::text(")"),
                                  }));
                }
            }
            if (!m.viewEntries().empty()) {
                for (const auto &v : m.viewEntries()) {
                    elem_fields.emplace(v.name(),
                                        ftxui::hbox({
                                            ftxui::text(" [vi]") | ftxui::dim,
                                            ftxui::text(v.name()) |
                                                ftxui::color(ftxui::Color::Red),
                                        }));
                }
            }
            if (!m.canvas2DEntries().empty()) {
                for (const auto &v : m.canvas2DEntries()) {
                    elem_fields.emplace(
                        v.name(), ftxui::hbox({
                                      ftxui::text(" [2d]") | ftxui::dim,
                                      ftxui::text(v.name()) |
                                          ftxui::color(ftxui::Color::Yellow),
                                  }));
                }
            }
            if (!m.imageEntries().empty()) {
                for (const auto &v : m.imageEntries()) {
                    elem_fields.emplace(
                        v.name(), ftxui::hbox({
                                      ftxui::text(" [im]") | ftxui::dim,
                                      ftxui::text(v.name()) |
                                          ftxui::color(ftxui::Color::Yellow),
                                  }));
                }
            }
            if (!m.canvas3DEntries().empty()) {
                for (const auto &v : m.canvas3DEntries()) {
                    elem_fields.emplace(
                        v.name(), ftxui::hbox({
                                      ftxui::text(" [3d]") | ftxui::dim,
                                      ftxui::text(v.name()) |
                                          ftxui::color(ftxui::Color::Yellow),
                                  }));
                }
            }
            if (!m.robotModelEntries().empty()) {
                for (const auto &v : m.robotModelEntries()) {
                    elem_fields.emplace(
                        v.name(), ftxui::hbox({
                                      ftxui::text(" [rm]") | ftxui::dim,
                                      ftxui::text(v.name()) |
                                          ftxui::color(ftxui::Color::Yellow),
                                  }));
                }
            }
            for (const auto &it : elem_fields) {
                elem_members.push_back(it.second);
            }
        }
    }
    return ftxui::vbox(elem_members);
}
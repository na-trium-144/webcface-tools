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
        elem_members.push_back(ftxui::text(m.name()) |
                               (m.pingStatus() ? ftxui::bold : ftxui::nothing));
    }
    return lsFlex(std::move(elem_members));
}

ftxui::Element listMemberShortRecurse(webcface::Client &wcli) {
    std::vector<ftxui::Element> elem_members;
    for (const auto &m : wcli.members()) {
        elem_members.push_back(ftxui::hbox({
            ftxui::text(m.name()) |
                (m.pingStatus() ? ftxui::bold : ftxui::nothing),
            ftxui::text(":"),
        }));
        std::multimap<std::string, ftxui::Element> elem_fields;
        for (const auto &v : m.valueEntries()) {
            elem_fields.emplace(v.name(), ftxui::text(v.name()) |
                                              ftxui::color(ftxui::Color::Blue));
        }
        for (const auto &v : m.textEntries()) {
            elem_fields.emplace(v.name(), ftxui::text(v.name()) |
                                              ftxui::color(ftxui::Color::Cyan));
        }
        for (const auto &v : m.funcEntries()) {
            elem_fields.emplace(v.name(),
                                ftxui::text(v.name()) |
                                    ftxui::color(ftxui::Color::Green));
        }
        for (const auto &v : m.viewEntries()) {
            elem_fields.emplace(v.name(), ftxui::text(v.name()) |
                                              ftxui::color(ftxui::Color::Red));
        }
        for (const auto &v : m.canvas2DEntries()) {
            elem_fields.emplace(v.name(),
                                ftxui::text(v.name()) |
                                    ftxui::color(ftxui::Color::Yellow));
        }
        for (const auto &v : m.imageEntries()) {
            elem_fields.emplace(v.name(),
                                ftxui::text(v.name()) |
                                    ftxui::color(ftxui::Color::Yellow));
        }
        for (const auto &v : m.canvas3DEntries()) {
            elem_fields.emplace(v.name(),
                                ftxui::text(v.name()) |
                                    ftxui::color(ftxui::Color::Yellow));
        }
        for (const auto &v : m.robotModelEntries()) {
            elem_fields.emplace(v.name(),
                                ftxui::text(v.name()) |
                                    ftxui::color(ftxui::Color::Yellow));
        }
        std::vector<ftxui::Element> elem_fields_v;
        for (const auto &it : elem_fields) {
            elem_fields_v.push_back(it.second);
        }
        if (!elem_fields_v.empty()) {
            elem_members.push_back(ftxui::hbox({
                ftxui::emptyElement() |
                    ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 1),
                lsFlex(std::move(elem_fields_v)),
            }));
        }
    }
    return ftxui::vbox(elem_members);
}

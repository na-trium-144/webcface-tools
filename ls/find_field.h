#pragma once
#include <webcface/member.h>
#include <webcface/value.h>
#include <webcface/text.h>
#include <webcface/view.h>
#include <webcface/func.h>
#include <webcface/canvas2d.h>
#include <webcface/canvas3d.h>
#include <webcface/image.h>
#include <webcface/robot_model.h>
#include <ftxui/dom/elements.hpp>
#include <map>
#include <string>
#include <unordered_set>
#include <utility>

// memberの中で指定したfield名で始まるものを列挙し、
// 型にあわせてそれぞれコールバックを呼んでelement化したものをmultimapで返す
// グループ化できるものは別で名前だけunordered_setにまとめられる
template <typename FVal, typename FText, typename FFunc, typename FView,
          typename F2D, typename FIm, typename F3D, typename FRMdl>
std::pair<std::multimap<std::string, ftxui::Element>,
          std::unordered_set<std::string>>
findFields(const webcface::Member &m, const std::string &field_prefix,
           FVal render_val, FText render_text, FFunc render_func,
           FView render_view, F2D render_2d, FIm render_image, F3D render_3d,
           FRMdl render_rmodel) {
    std::multimap<std::string, ftxui::Element> elem_fields;
    std::unordered_set<std::string> folders;
    auto check_field_name = [&](const auto &v, const auto &renderer) {
        std::string name{v.name()};
        if (!field_prefix.empty()) {
            if (name.substr(0, field_prefix.size() + 1) == field_prefix + ".") {
                name = name.substr(field_prefix.size() + 1);
            } else {
                return;
            }
        }
        if (name.find('.') != std::string::npos) {
            folders.emplace(name.substr(0, name.find('.')));
        } else {
            elem_fields.emplace(name, renderer(name, v));
        }
    };
    for (const auto &v : m.valueEntries()) {
        check_field_name(v, render_val);
    }
    for (const auto &v : m.textEntries()) {
        check_field_name(v, render_text);
    }
    for (const auto &v : m.funcEntries()) {
        check_field_name(v, render_func);
    }
    for (const auto &v : m.viewEntries()) {
        check_field_name(v, render_view);
    }
    for (const auto &v : m.canvas2DEntries()) {
        check_field_name(v, render_2d);
    }
    for (const auto &v : m.imageEntries()) {
        check_field_name(v, render_image);
    }
    for (const auto &v : m.canvas3DEntries()) {
        check_field_name(v, render_3d);
    }
    for (const auto &v : m.robotModelEntries()) {
        check_field_name(v, render_rmodel);
    }
    return std::make_pair(elem_fields, folders);
}

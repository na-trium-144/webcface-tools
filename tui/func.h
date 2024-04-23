#pragma once
#include <webcface/member.h>
#include <webcface/func.h>

template <typename... Args>
inline void runAsync(const webcface::Func &func,
                     std::shared_ptr<ftxui::Element> result, Args &&...args) {
    auto res = func.runAsync(args...);
    auto name = func.member().name() + ":" + func.name();
    std::thread([result, res, name = std::move(name)] {
        using namespace std::string_literals;
        *result = ftxui::text("Connecting... (" + name + ")") |
                  ftxui::color(ftxui::Color::Green);
        res.started.get();
        *result = ftxui::text("Running... (" + name + ")") |
                  ftxui::color(ftxui::Color::Blue);
        try {
            auto result_val = res.result.get();
            if (!result_val.asStringRef().empty()) {
                *result = ftxui::text("ok, \"" + result_val.asStringRef() +
                                      "\" (" + name + ")") |
                          ftxui::color(ftxui::Color::Black);
            } else {
                *result = ftxui::text("ok (" + name + ")") |
                          ftxui::color(ftxui::Color::Black);
            }
        } catch (const std::exception &e) {
            *result = ftxui::text("Error: "s + e.what() + " (" + name + ")") |
                      ftxui::color(ftxui::Color::Red);
        }
    }).detach();
}

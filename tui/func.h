#pragma once
#include <webcface/member.h>
#include <webcface/func.h>

static int call_num = 0;

template <typename... Args>
inline void runAsync(const webcface::Func &func,
                     std::shared_ptr<ftxui::Element> result, Args &&...args) {
    auto res = func.runAsync(args...);
    auto name = func.member().name() + ":" + func.name();
    using namespace std::string_literals;
    auto num = ftxui::hbox({
        ftxui::text("["),
        ftxui::text(std::to_string(++call_num)),
        ftxui::text("] "),
    });
    *result = ftxui::hbox({
        num,
        ftxui::text("Connecting... (" + name + ")") |
            ftxui::color(ftxui::Color::Green),
    });
    res.onStarted().append([=](bool started) {
        *result = ftxui::hbox({
            num,
            ftxui::text("Running... (" + name + ")") |
                ftxui::color(ftxui::Color::Blue),
        });
    });
    res.onResult().append([=](std::shared_future<webcface::ValAdaptor> res_f) {
        try {
            auto result_val = res_f.get();
            if (!result_val.asStringRef().empty()) {
                *result = ftxui::hbox({
                    num,
                    ftxui::text("ok, \"" + result_val.asStringRef() + "\" (" +
                                name + ")") |
                        ftxui::color(ftxui::Color::Black),
                });
            } else {
                *result = ftxui::hbox({
                    num,
                    ftxui::text("ok (" + name + ")") |
                        ftxui::color(ftxui::Color::Black),
                });
            }
        } catch (const std::exception &e) {
            *result = ftxui::hbox({
                num,
                ftxui::text("Error: "s + e.what() + " (" + name + ")") |
                    ftxui::color(ftxui::Color::Red),
            });
        }
    });
}

#pragma once

inline void runAsync(const webcface::Func &func,
                     std::shared_ptr<ftxui::Element> status) {
  auto res = func.runAsync();
  auto name = func.member().name() + ":" + func.name();
  std::thread([status, res, name = std::move(name)] {
    using namespace std::string_literals;
    *status = ftxui::text("Connecting... (" + name + ")") |
              ftxui::color(ftxui::Color::Green);
    res.started.get();
    *status = ftxui::text("Running... (" + name + ")") |
              ftxui::color(ftxui::Color::Blue);
    try {
      auto result = res.result.get();
      if (!result.asStringRef().empty()) {
        *status =
            ftxui::text("ok, \"" + result.asStringRef() + "\" (" + name + ")") |
            ftxui::color(ftxui::Color::Black);
      } else {
        *status = ftxui::text("ok (" + name + ")") |
                  ftxui::color(ftxui::Color::Black);
      }
    } catch (const std::exception &e) {
      *status = ftxui::text("Error: "s + e.what() + " (" + name + ")") |
                ftxui::color(ftxui::Color::Red);
    }
  }).detach();
}

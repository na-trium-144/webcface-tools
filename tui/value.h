#pragma once

inline std::string &defaultStatus() {
    static std::string s = "↓/↑ or J/K = move, Ctrl+C = quit";
    return s;
}
inline ftxui::Component valueComponent(const webcface::Value &value,
                                       std::shared_ptr<std::string> help,
                                       std::shared_ptr<ftxui::Element> result) {
    return ftxui::Renderer([value, help](bool focused) {
        if (focused) {
            *help = defaultStatus();
        }
        return ftxui::hbox({
                   ftxui::text(value.member().name()),
                   ftxui::text(":"),
                   ftxui::text(value.name()),
                   ftxui::text(" = "),
                   ftxui::text(std::to_string(value.get())) |
                       (focused ? ftxui::bold : ftxui::nothing) | ftxui::xflex,
               }) |
               ftxui::xflex | (focused ? ftxui::focus : ftxui::nothing);
    });
}
inline void addValueComponent(ftxui::ScreenInteractive &screen,
                              ftxui::Component &container,
                              const webcface::Value &value,
                              std::shared_ptr<std::string> help,
                              std::shared_ptr<ftxui::Element> result) {
    auto handle = std::make_shared<webcface::Value::EventHandle>();
    *handle = value.prependListener(
        [&screen, &container, value, handle, help, result] {
            screen.Post([&container, value, help, result] {
                container->Add(valueComponent(value, help, result));
            });
            value.removeListener(*handle);
        });
}

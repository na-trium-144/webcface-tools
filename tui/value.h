#pragma once

inline ftxui::Component valueComponent(const webcface::Value &value,
                                       std::shared_ptr<ftxui::Element> status) {
    return ftxui::Renderer([value, status](bool focused) {
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
                              std::shared_ptr<ftxui::Element> status) {
    auto handle = std::make_shared<webcface::Value::EventHandle>();
    *handle =
        value.prependListener([&screen, &container, value, handle, status] {
            screen.Post([&container, value, status] {
                container->Add(valueComponent(value, status));
            });
            value.removeListener(*handle);
        });
}

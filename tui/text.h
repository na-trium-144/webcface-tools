#pragma once

inline ftxui::Component textComponent(const webcface::Text &text,
                                      std::shared_ptr<std::string> help,
                                      std::shared_ptr<ftxui::Element> result) {
    return ftxui::Renderer([text, help](bool focused) {
        if (focused) {
            *help = defaultStatus();
        }
        return ftxui::hbox({
                   ftxui::text(text.member().name()),
                   ftxui::text(":"),
                   ftxui::text(text.name()),
                   ftxui::text(" = "),
                   ftxui::text(text.get()) |
                       (focused ? ftxui::bold : ftxui::nothing) | ftxui::xflex,
               }) |
               ftxui::xflex | (focused ? ftxui::focus : ftxui::nothing);
    });
}
inline void addTextComponent(ftxui::ScreenInteractive &screen,
                             ftxui::Component &container,
                             const webcface::Text &text,
                             std::shared_ptr<std::string> help,
                             std::shared_ptr<ftxui::Element> result) {
    auto handle = std::make_shared<webcface::Text::EventHandle>();
    *handle =
        text.prependListener([&screen, &container, text, handle, help, result] {
            screen.Post([&container, text, help, result] {
                container->Add(textComponent(text, help, result));
            });
            text.removeListener(*handle);
        });
}

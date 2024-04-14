#pragma once

inline ftxui::Component textComponent(const webcface::Text &text,
                                      std::shared_ptr<ftxui::Element> status) {
    return ftxui::Renderer([text, status](bool focused) {
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
                             std::shared_ptr<ftxui::Element> status) {
    auto handle = std::make_shared<webcface::Text::EventHandle>();
    *handle = text.prependListener([&screen, &container, text, handle, status] {
        screen.Post([&container, text, status] {
            container->Add(textComponent(text, status));
        });
        text.removeListener(*handle);
    });
}

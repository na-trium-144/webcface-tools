#pragma once

inline ftxui::Component valueComponent(const webcface::Value &value) {
    return ftxui::Renderer([value] {
        return ftxui::hbox({
                   ftxui::text(value.member().name()),
                   ftxui::text(":"),
                   ftxui::text(value.name()),
                   ftxui::text(" = "),
                   ftxui::text(std::to_string(value.get())) | ftxui::xflex,
               }) |
               ftxui::xflex;
    });
}
inline void addValueComponent(ftxui::ScreenInteractive &screen,
                              ftxui::Component &container,
                              const webcface::Value &value) {
    if (value.tryGet()) {
        container->Add(valueComponent(value));
    } else {
        auto handle = std::make_shared<webcface::Value::EventHandle>();
        *handle = value.prependListener([&screen, &container, value, handle] {
            screen.Post(
                [&container, value] { container->Add(valueComponent(value)); });
            value.removeListener(*handle);
        });
    }
}

inline ftxui::Component textComponent(const webcface::Text &text) {
    return ftxui::Renderer([text] {
        return ftxui::hbox({
                   ftxui::text(text.member().name()),
                   ftxui::text(":"),
                   ftxui::text(text.name()),
                   ftxui::text(" = "),
                   ftxui::text(text.get()) | ftxui::xflex,
               }) |
               ftxui::xflex;
    });
}
inline void addTextComponent(ftxui::ScreenInteractive &screen,
                             ftxui::Component &container,
                             const webcface::Text &text) {
    if (text.tryGet()) {
        container->Add(textComponent(text));
    } else {
        auto handle = std::make_shared<webcface::Text::EventHandle>();
        *handle = text.prependListener([&screen, &container, text, handle] {
            screen.Post(
                [&container, text] { container->Add(textComponent(text)); });
            text.removeListener(*handle);
        });
    }
}

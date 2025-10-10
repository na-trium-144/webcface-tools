#include "value.h"
#include "status.h"
#include <webcface/member.h>

ftxui::Component valueComponent(const webcface::Value &value,
                                std::shared_ptr<std::string> help,
                                std::shared_ptr<ftxui::Element> result,
                                bool /*light*/) {
    return ftxui::Renderer([=](bool focused) {
        if (focused) {
            *help = defaultStatus();
        }
        return ftxui::hbox({
                   ftxui::text(std::string(value.member().name())),
                   ftxui::text(":"),
                   ftxui::text(std::string(value.name())),
                   ftxui::text(" = "),
                   ftxui::text(std::to_string(value.get())) |
                       (focused ? ftxui::bold : ftxui::nothing) | ftxui::xflex,
               }) |
               ftxui::xflex | (focused ? ftxui::focus : ftxui::nothing);
    });
}
void addValueComponent(ftxui::ScreenInteractive &screen,
                       ftxui::Component &container,
                       const webcface::Value &value,
                       std::shared_ptr<std::string> help,
                       std::shared_ptr<ftxui::Element> result, bool light) {
    value.onChange([=, &screen, &container]() {
        value.onChange([] {});
        screen.Post([=, &container] {
            container->Add(valueComponent(value, help, result, light));
        });
    });
}

#include "text.h"
#include "status.h"
#include <webcface/member.h>
#include <eventpp/utilities/counterremover.h>

ftxui::Component textComponent(const webcface::Text &text,
                               std::shared_ptr<std::string> help,
                               std::shared_ptr<ftxui::Element> result,
                               bool /*light*/) {
    return ftxui::Renderer([=](bool focused) {
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
void addTextComponent(ftxui::ScreenInteractive &screen,
                      ftxui::Component &container, const webcface::Text &text,
                      std::shared_ptr<std::string> help,
                      std::shared_ptr<ftxui::Element> result, bool light) {
    eventpp::counterRemover(text.callbackList())
        .prepend(
            [=, &screen, &container](const webcface::Text &) {
                screen.Post([=, &container] {
                    container->Add(textComponent(text, help, result, light));
                });
            },
            1);
}

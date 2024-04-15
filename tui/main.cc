#include <webcface/webcface.h>
#include <CLI/CLI.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <string>
#include <vector>
#include "../common/common.h"
#include "value.h"
#include "text.h"
#include "view.h"

int main(int argc, char **argv) {
    CLI::App app{TOOLS_VERSION_DISP("WebCFace TUI")};

    std::string wcli_host = "127.0.0.1";
    int wcli_port = WEBCFACE_DEFAULT_PORT;
    app.add_option("-a,--address", wcli_host,
                   "Server address (default: 127.0.0.1)");
    app.add_option("-p,--port", wcli_port,
                   "Server port (default: " WEBCFACE_DEFAULT_PORT_S ")");

    std::vector<std::string> fields;
    app.add_option("fields", fields,
                   "Fields to display as 'memberName:name/of/field'");

    CLI11_PARSE(app, argc, argv);

    webcface::Client wcli("", wcli_host, wcli_port);
    auto screen = ftxui::ScreenInteractive::Fullscreen();
    auto container = ftxui::Container::Vertical({});
    auto help = std::make_shared<std::string>();
    auto result = std::make_shared<ftxui::Element>();

    for (auto &fp : fields) {
        auto fp_colon = fp.find(':');
        std::string member_name = fp.substr(0, fp_colon);
        std::string field_name = fp.substr(fp_colon + 1);
        auto value = wcli.member(member_name).value(field_name);
        value.appendListener(
            [&screen] { screen.PostEvent(ftxui::Event::Custom); });
        addValueComponent(screen, container, value, help, result);

        auto text = wcli.member(member_name).text(field_name);
        text.appendListener(
            [&screen] { screen.PostEvent(ftxui::Event::Custom); });
        addTextComponent(screen, container, text, help, result);

        auto view = wcli.member(member_name).view(field_name);
        view.appendListener(
            [&screen] { screen.PostEvent(ftxui::Event::Custom); });
        addViewComponent(screen, container, view, help, result);
    }
    wcli.waitConnection();

    std::string help_prev = "";
    ftxui::Element result_prev = nullptr;
    ftxui::Element status = nullptr;
    screen.Loop(ftxui::Renderer(container, [&] {
        if(*help != help_prev){
            help_prev = *help;
            status = ftxui::text(*help) | ftxui::color(ftxui::Color::Black);
        }
        if(*result != result_prev){
            status = result_prev = *result;
        }
        return ftxui::vbox({container->Render() | ftxui::vscroll_indicator |
                                ftxui::yframe | ftxui::yflex,
                            (status ? status : ftxui::emptyElement()) |
                                ftxui::bgcolor(ftxui::Color::White) |
                                ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 1)});
    }));
}

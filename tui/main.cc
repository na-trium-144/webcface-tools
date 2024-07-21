#include <webcface/client.h>
#include <CLI/CLI.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <string>
#include <vector>
#include <unordered_set>
#include "../common/common.h"
#include "status.h"
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
    bool light = false;
    app.add_flag("-w,--white", light,
                 "Light mode: display black text and white background "
                 "(otherwise black and white are inverted)");

    std::vector<std::string> fields;
    app.add_option("fields", fields,
                   "Fields to display as 'memberName:name/of/field'");

    CLI11_PARSE(app, argc, argv);

    webcface::Client wcli("", wcli_host, wcli_port);
    auto screen = ftxui::ScreenInteractive::Fullscreen();
    auto container = ftxui::Container::Vertical({});
    auto help = std::make_shared<std::string>();
    auto result = std::make_shared<ftxui::Element>();
    std::unordered_set<std::string> members;
    for (auto &fp : fields) {
        auto fp_colon = fp.find(':');
        std::string member_name = fp.substr(0, fp_colon);
        std::string field_name = fp.substr(fp_colon + 1);
        if(!members.count(member_name)){
            wcli.member(member_name).onSync().appendListener(
                [&screen] { screen.PostEvent(ftxui::Event::Custom); });
            members.emplace(member_name);
        }
        auto value = wcli.member(member_name).value(field_name);
        addValueComponent(screen, container, value, help, result, light);

        auto text = wcli.member(member_name).text(field_name);
        addTextComponent(screen, container, text, help, result, light);

        auto view = wcli.member(member_name).view(field_name);
        addViewComponent(screen, container, view, help, result, light);
    }
    wcli.waitConnection();

    std::string help_prev = "";
    ftxui::Element result_prev = nullptr;
    ftxui::Element status = nullptr;
    screen.Loop(ftxui::Renderer(container, [&] {
        if (*help != help_prev) {
            help_prev = *help;
            status = ftxui::text(*help) | ftxui::color(ftxui::Color::Black);
        }
        if (*result != result_prev) {
            status = result_prev = *result;
        }
        return ftxui::vbox({container->Render() | ftxui::vscroll_indicator |
                                ftxui::yframe | ftxui::yflex,
                            (status ? status : ftxui::emptyElement()) |
                                ftxui::bgcolor(ftxui::Color::White) |
                                ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 1)});
    }));
}

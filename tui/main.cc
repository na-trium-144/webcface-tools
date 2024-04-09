#include <webcface/webcface.h>
#include <CLI/CLI.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <string>
#include <vector>
#include "../common/common.h"
#include "components.h"

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

    for (auto &fp : fields) {
        auto fp_colon = fp.find(':');
        std::string member_name = fp.substr(0, fp_colon);
        std::string field_name = fp.substr(fp_colon + 1);
        auto value = wcli.member(member_name).value(field_name);
        value.appendListener(
            [&screen] { screen.PostEvent(ftxui::Event::Custom); });
        addValueComponent(screen, container, value);

        auto text = wcli.member(member_name).text(field_name);
        text.appendListener(
            [&screen] { screen.PostEvent(ftxui::Event::Custom); });
        addTextComponent(screen, container, text);

        auto view = wcli.member(member_name).view(field_name);
        view.appendListener(
            [&screen] { screen.PostEvent(ftxui::Event::Custom); });
        addViewComponent(screen, container, view);
    }
    wcli.waitConnection();

    screen.Loop(ftxui::Renderer(container, [&] {
        return container->Render() | ftxui::vscroll_indicator | ftxui::yframe;
    }));
}

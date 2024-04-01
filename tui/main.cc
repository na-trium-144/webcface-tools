#include <webcface/webcface.h>
#include <CLI/CLI.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <string>
#include <vector>
#include "../common/common.h"

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
    wcli.waitConnection();
    auto screen = ftxui::ScreenInteractive::Fullscreen();

    std::vector<ftxui::Component> components;
    for (auto &fp : fields) {
        auto fp_colon = fp.find(':');
        std::string member_name = fp.substr(0, fp_colon);
        std::string field_name = fp.substr(fp_colon + 1);
        auto value_field = wcli.member(member_name).value(field_name);
        value_field.appendListener(
            [&screen] { screen.RequestAnimationFrame(); });
        components.push_back(
            ftxui::Renderer([member_name, field_name, value_field] {
                return ftxui::hbox({
                           ftxui::text(member_name),
                           ftxui::text(":"),
                           ftxui::text(field_name),
                           ftxui::text(" = "),
                           ftxui::text(std::to_string(value_field.get())) |
                               ftxui::xflex,
                       }) |
                       ftxui::xflex;
            }));
    }

    auto layout = ftxui::Container::Vertical(components);
    screen.Loop(layout);
}
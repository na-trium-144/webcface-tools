#include <webcface/client.h>
#include <CLI/CLI.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <string>
#include <vector>
#include "../common/common.h"

int main(int argc, char **argv) {
    CLI::App app{TOOLS_VERSION_DISP("WebCFace ls")};

    std::string wcli_host = "127.0.0.1";
    int wcli_port = WEBCFACE_DEFAULT_PORT;
    app.add_option("-a,--address", wcli_host,
                   "Server address (default: 127.0.0.1)");
    app.add_option("-p,--port", wcli_port,
                   "Server port (default: " WEBCFACE_DEFAULT_PORT_S ")");

    CLI11_PARSE(app, argc, argv);

    webcface::Client wcli("", wcli_host, wcli_port);

    // Define the document
    ftxui::Element document = ftxui::hbox({
        ftxui::text("left") | ftxui::border,
        ftxui::text("middle") | ftxui::border | ftxui::flex,
        ftxui::text("right") | ftxui::border,
    });

    auto screen =
        ftxui::Screen::Create(ftxui::Dimension::Full(),       // Width
                              ftxui::Dimension::Fit(document) // Height
        );
    ftxui::Render(screen, document);
    screen.Print();
}

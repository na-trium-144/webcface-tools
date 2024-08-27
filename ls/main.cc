#include <webcface/client.h>
#include <CLI/CLI.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <spdlog/spdlog.h>
#include <string>
#include "../common/common.h"
#include "./main.h"

bool ok = true;

int main(int argc, char **argv) {
    CLI::App app{TOOLS_VERSION_DISP("WebCFace ls")};

    std::string wcli_host = "127.0.0.1";
    int wcli_port = WEBCFACE_DEFAULT_PORT;
    app.add_option("-a,--address", wcli_host,
                   "Server address (default: 127.0.0.1)");
    app.add_option("-p,--port", wcli_port,
                   "Server port (default: " WEBCFACE_DEFAULT_PORT_S ")");

    bool list = false, recursive = false;
    app.add_flag("-l", list, "Long listing format");
    app.add_flag("-r", recursive, "Recursively list fields");

    std::vector<std::string> fields;
    app.add_option("fields", fields,
                   "Fields to display as 'memberName:name/of/field'");

    CLI11_PARSE(app, argc, argv);

    webcface::Client wcli("", wcli_host, wcli_port);
    wcli.autoReconnect(false);
    wcli.pingStatus(); // request
    wcli.waitConnection();

    if (!wcli.connected()) {
        spdlog::error("could not connect to webcface server");
        return 1;
    }

    ftxui::Element document;
    if (list) {
        if (fields.empty()) {
            document = listMemberAll(wcli, recursive);
        } else {
            document = listFields(wcli, fields, recursive);
        }
    } else {
        if (!recursive) {
            document = listMemberShort(wcli);
        } else {
            document = listMemberShortRecurse(wcli);
        }
    }

    auto screen = ftxui::Screen::Create(ftxui::Dimension::Full(),
                                        ftxui::Dimension::Fit(document));
    ftxui::Render(screen, document);
    screen.Print();

    return !ok;
}

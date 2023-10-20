#include <webcface/webcface.h>
#include <toml++/toml.hpp>
#include <CLI/CLI.hpp>
#include <string>
#include "launcher.h"

#define DEFAULT_TOML "webcface-launcher.toml"

int main(int argc, char **argv) {
    CLI::App app{"WebCFace Launcher " TOOLS_VERSION};
    app.allow_windows_style_options();

    std::string wcli_host = "", wcli_name = "";
    int wcli_port = 0;
    app.add_option("-a,--address", wcli_host,
                   "Server address (default: 127.0.0.1)");
    app.add_option("-p,--port", wcli_port,
                   "Server port (default: " WEBCFACE_DEFAULT_PORT_S ")");
    app.add_option("-m,--member", wcli_name, "Client member name");

    std::string toml_path = DEFAULT_TOML;
    auto config_opt =
        app.add_option("config_path", toml_path,
                       "Path of config file (default: " DEFAULT_TOML ")")
            ->check(CLI::ExistingFile);

    CLI11_PARSE(app, argc, argv);

    auto config = toml::parse_file(toml_path);
    if (wcli_name.empty()) {
        wcli_name = config["init"]["name"].value_or("webcface-launcher");
    }
    if (wcli_host.empty()) {
        wcli_host = config["init"]["address"].value_or("127.0.0.1");
    }
    if (wcli_port == 0) {
        wcli_port = config["init"]["port"].value_or(WEBCFACE_DEFAULT_PORT);
    }
    WebCFace::Client wcli(wcli_name, wcli_host, wcli_port);

    launcher(wcli, config);
}
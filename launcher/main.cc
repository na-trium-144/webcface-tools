#include <webcface/webcface.h>
#include <toml++/toml.hpp>
#include <tclap/CmdLine.h>
#include <string>
#include "launcher.h"

#define TOOLS_VERSION "1.0.1"
#define DEFAULT_TOML "webcface-launcher.toml"

int main(int argc, char **argv) {
    try {
        TCLAP::CmdLine cmd("WebCFace Launcher", ' ', TOOLS_VERSION);
        TCLAP::ValueArg<std::string> hostArg(
            "a", "address", "Server address (default: 127.0.0.1)", false, "",
            "address");
        cmd.add(hostArg);
        TCLAP::ValueArg<int> portArg(
            "p", "port", "Server port (default: " WEBCFACE_DEFAULT_PORT_S ")",
            false, 0, "number");
        cmd.add(portArg);
        TCLAP::ValueArg<std::string> nameArg(
            "m", "member_name", "Client member name", false, "", "string");
        cmd.add(nameArg);

        TCLAP::UnlabeledValueArg<std::string> tomlPathArg(
            "config_path", "Path of config file (default: " DEFAULT_TOML ")",
            false, DEFAULT_TOML, "path");
        cmd.add(tomlPathArg);

        cmd.parse(argc, argv);

        auto config = toml::parse_file(tomlPathArg.getValue());
        std::string wcli_name = nameArg.getValue();
        if (wcli_name.empty()) {
            wcli_name = config["init"]["name"].value_or("webcface-launcher");
        }
        std::string wcli_host = hostArg.getValue();
        if (wcli_host.empty()) {
            wcli_host = config["init"]["host"].value_or("127.0.0.1");
        }
        int wcli_port = portArg.getValue();
        if (wcli_port == 0) {
            wcli_port = config["init"]["port"].value_or(WEBCFACE_DEFAULT_PORT);
        }
        WebCFace::Client wcli(wcli_name, wcli_host, wcli_port);

        launcher(wcli, config);

        return 0;
    } catch (TCLAP::ArgException &e) {
        std::cerr << "error: " << e.error() << " for arg " << e.argId()
                  << std::endl;
        return 1;
    }

    // todo: オプションで変えられるようにする
}
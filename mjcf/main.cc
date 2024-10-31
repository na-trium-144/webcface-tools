#include <webcface/client.h>
#include <CLI/CLI.hpp>
#include <pugixml.hpp>
#include <spdlog/spdlog.h>
#include "../common/common.h"
#include "./main.h"

int main(int argc, char **argv) {
    CLI::App app{TOOLS_VERSION_DISP("WebCFace MJCF")};

    std::string wcli_host = "127.0.0.1", wcli_name = "webcface-mjcf";
    int wcli_port = WEBCFACE_DEFAULT_PORT;
    app.add_option("-a,--address", wcli_host,
                   "Server address (default: 127.0.0.1)");
    app.add_option("-p,--port", wcli_port,
                   "Server port (default: " WEBCFACE_DEFAULT_PORT_S ")");
    app.add_option("-m,--member", wcli_name, "Client member name");

    std::string filename;
    app.add_option("file", filename, "MJCF xml filename");

    CLI11_PARSE(app, argc, argv);

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(filename.c_str());
    if (!result) {
        spdlog::error("XML Parse Error: {} (at offset {})",
                      result.description(), result.offset);
        return 1;
    }
    auto mujoco = doc.child("mujoco");
    if (!mujoco) {
        spdlog::error("No <mujoco> tag found in the xml file");
        return 1;
    }
    auto model_name = mujoco.attribute("model");
    parseBody(mujoco.child("worldbody"));
}

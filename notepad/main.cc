#include <webcface/webcface.h>
#include <spdlog/spdlog.h>
#include <CLI/CLI.hpp>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <iostream>
#include "../common/common.h"
#include "../common/logger_sink.h"

int main(int argc, char **argv) {
    CLI::App app{TOOLS_VERSION_DISP("WebCFace NotePad")};

    std::string wcli_host = "127.0.0.1", wcli_name = "webcface-notepad";
    int wcli_port = WEBCFACE_DEFAULT_PORT;
    app.add_option("-a,--address", wcli_host,
                   "Server address (default: 127.0.0.1)");
    app.add_option("-p,--port", wcli_port,
                   "Server port (default: " WEBCFACE_DEFAULT_PORT_S ")");
    app.add_option("-m,--member", wcli_name, "Client member name");

    std::string filename;
    app.add_option("filename", filename, "File path");

    // #ifdef WIN32
    //     bool utf8 = false;
    //     app.add_flag("-8,--utf8", utf8,
    //                  "Treat input as UTF-8 (windows only, default: false)");
    // #endif

    CLI11_PARSE(app, argc, argv);

    std::string init_content;
    {
        std::ifstream fs(filename);
        if (fs) {
            init_content = std::string(std::istream_iterator<char>(fs), {});
        } else {
            spdlog::error("Failed to open {} for reading: {}", filename,
                          std::strerror(errno));
        }
    }

    webcface::Client wcli(wcli_name, wcli_host, wcli_port);
    auto v = wcli.view("notepad");
    v << webcface::textInput().init(init_content).width(20).height(10).onChange(
        [&](const std::string &content) {
            std::ofstream fs(filename);
            if (fs) {
                fs << content;
                spdlog::info("{} bytes written", content.size());
            } else {
                spdlog::error("Failed to open {} for writing: {}", filename,
                              std::strerror(errno));
            }
            wcli.sync();
        });
    v.sync();
    wcli.loopSync();
}

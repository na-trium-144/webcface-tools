#include <webcface/webcface.h>
#include <CLI/CLI.hpp>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <iostream>

int main(int argc, char **argv) {
    CLI::App app{"WebCFace Send " TOOLS_VERSION};
    app.allow_windows_style_options();

    std::string wcli_host = "127.0.0.1", wcli_name = "webcface-send";
    int wcli_port = WEBCFACE_DEFAULT_PORT;
    app.add_option("-a,--address", wcli_host,
                   "Server address (default: 127.0.0.1)");
    app.add_option("-p,--port", wcli_port,
                   "Server port (default: " WEBCFACE_DEFAULT_PORT_S ")");
    app.add_option("-m,--member", wcli_name, "Client member name");

    std::string data_type = "value", field = "data";
    app.add_option("-t,--type", data_type,
                   "Message type (default: value)")
        ->check(CLI::IsMember({"value", "text", "log"}, CLI::ignore_case));
    app.add_option("field", field, "Field name to send (default: data)");

    CLI11_PARSE(app, argc, argv);

    WebCFace::Client wcli(wcli_name, wcli_host, wcli_port);

    while (!std::cin.eof()) {
        if (data_type == "value") {
            static double val;
            std::cin >> val;
            wcli.value(field).set(val);
        } else if (data_type == "text") {
            static std::string val;
            std::getline(std::cin, val);
            wcli.text(field).set(val);
        } else if (data_type == "log") {
            static std::string val;
            std::getline(std::cin, val);
            wcli.logger()->info(val);
        }
        wcli.sync();
    }
    if (!wcli.connected()) {
        while (!wcli.connected()) {
            std::this_thread::yield();
        }
        wcli.sync();
    }

}
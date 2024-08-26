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
    CLI::App app{TOOLS_VERSION_DISP("WebCFace Send")};

    std::string wcli_host = "127.0.0.1", wcli_name = "webcface-send";
    int wcli_port = WEBCFACE_DEFAULT_PORT;
    app.add_option("-a,--address", wcli_host,
                   "Server address (default: 127.0.0.1)");
    app.add_option("-p,--port", wcli_port,
                   "Server port (default: " WEBCFACE_DEFAULT_PORT_S ")");
    app.add_option("-m,--member", wcli_name, "Client member name");

    bool echo = false;
    app.add_flag("-e,--echo", echo, "Echo input to stdout");
    std::string data_type = "value", field = "data";
    app.add_option("-t,--type", data_type, "Message type (default: value)")
        ->check(CLI::IsMember({"value", "text", "log"}, CLI::ignore_case));
    app.add_option("field", field, "Field name to send (default: data)");

#ifdef WIN32
    bool utf8 = false;
    app.add_flag("-8,--utf8", utf8,
                 "Treat input as UTF-8 (windows only, default: false)");
#endif

    CLI11_PARSE(app, argc, argv);

    WebCFace::Client wcli(wcli_name, wcli_host, wcli_port);

    auto logger = std::make_shared<spdlog::logger>(
        "webcface-send", std::make_shared<LoggerSink>(wcli));
    while (!std::cin.eof()) {
        std::string input, input_org;
        std::getline(std::cin, input_org);
        input = input_org;
#ifdef WIN32
        if (!utf8) {
            input = acpToUTF8(input_org);
        }
#endif
        if (data_type == "value") {
            try {
                wcli.value(field).set(std::stod(input));
                if (echo) {
                    std::cout << input_org << std::endl;
                }
            } catch (const std::invalid_argument &) {
            } catch (const std::out_of_range &) {
            }
        } else if (data_type == "text") {
            wcli.text(field).set(input);
            if (echo) {
                std::cout << input_org << std::endl;
            }
        } else if (data_type == "log") {
            logger->info(input);
            if (echo) {
                std::cout << input_org << std::endl;
            }
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

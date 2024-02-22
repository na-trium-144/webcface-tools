#include <webcface/webcface.h>
#include <spdlog/spdlog.h>
#include <toml++/toml.hpp>
#include <CLI/CLI.hpp>
#include <string>
#include <stdexcept>
#include "launcher.h"
#include "../common/common.h"
#ifndef _WIN32
#include "signal_handler_unix.h"
#endif

#define DEFAULT_TOML "webcface-launcher.toml"


int main(int argc, char **argv) {
    CLI::App app{TOOLS_VERSION_DISP("WebCFace Launcher")};

    std::string wcli_host = "", wcli_name = "";
    int wcli_port = 0;
    app.add_option("-a,--address", wcli_host,
                   "Server address (default: 127.0.0.1)");
    app.add_option("-p,--port", wcli_port,
                   "Server port (default: " WEBCFACE_DEFAULT_PORT_S ")");
    app.add_option("-m,--member", wcli_name, "Client member name");

    bool use_stdin = false;
    app.add_flag("-s,--stdin", use_stdin,
                 "Read config from stdin instead of file");

    std::string toml_path = DEFAULT_TOML;
    auto config_opt =
        app.add_option("config_path", toml_path,
                       "Path of config file (default: " DEFAULT_TOML ")");

    CLI11_PARSE(app, argc, argv);

    toml::parse_result config;
    if (use_stdin) {
        std::string config_str = "";
        while (!std::cin.eof()) {
            std::string input;
            std::getline(std::cin, input);
            config_str += input + "\n";
        }
        config = toml::parse(config_str);
    } else {
        try {
            config = toml::parse_file(toml_path);
        } catch (const toml::parse_error &e) {
            spdlog::error("Error reading config file {}: {} ({})", toml_path,
                          std::string(e.description()),
                          tomlSourceInfo(e.source()));
            std::exit(1);
        } catch (const std::exception &e) {
            spdlog::error("Error reading config file {}: {}", toml_path,
                          e.what());
            std::exit(1);
        }
    }
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
    std::vector<std::shared_ptr<Command>> commands = parseToml(wcli, config);

    initHandler();

    while (!shouldStop()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        launcherLoop(wcli, commands);
    }

#ifndef _WIN32
    int sig_send = 0;
    do {
        if (shouldStop()) {
            spdlog::warn("Signal {} received.", sig_received);
            if (sig_send == 0) {
                if (sig_received == SIGINT) {
                    sig_send = SIGINT;
                } else {
                    sig_send = SIGTERM;
                }
            } else if (sig_send != SIGTERM) {
                // 再度シグナルが送られたら、sigtermにしてもう1回送る
                sig_send = SIGTERM;
                spdlog::warn("Escalating to {}.", sig_send);
            } else {
                // termでも止まらなかったらkill?
                sig_send = SIGKILL;
                spdlog::warn("Escalating to {}.", sig_send);
            }
            for (auto &cmd : commands) {
                if (cmd->is_running()) {
                    cmd->kill(sig_send);
                }
            }
            sig_received = 0;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    } while (std::any_of(commands.begin(), commands.end(),
                         [](const auto &cmd) { return cmd->is_running(); }));
    return 1;
#endif
}
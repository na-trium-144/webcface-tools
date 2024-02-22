#include <webcface/webcface.h>
#include <toml++/toml.hpp>
#include <process.hpp>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <functional>
#include <stdexcept>
#include <iostream>
#include <spdlog/spdlog.h>
#include "command.h"

void launcher(WebCFace::Client &wcli, toml::parse_result &config) {
    std::vector<std::shared_ptr<Command>> commands;

    auto config_commands = config["command"].as_array();
    for (auto &&v : *config_commands) {
        std::unordered_map<std::string, std::string> env;
        if(v[toml::path("env")].is_table()){
            for (auto &e : *v[toml::path("env")].as_table()) {
                env.emplace(e.first.str(), e.second.value_or(""));
            }
        }
        auto cmd = std::make_shared<Command>(
            wcli, v[toml::path("name")].value_or(""),
            v[toml::path("exec")].value_or(""),
            v[toml::path("workdir")].value_or("."),
            v[toml::path("stdout_capture")].value_or("onerror"),
            v[toml::path("stdout_utf8")].value_or(false), env);
        commands.push_back(cmd);
        spdlog::info("Command '{}': '{}' (workdir: {})", cmd->name, cmd->exec,
                     cmd->workdir);
    }

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        auto v = wcli.view("launcher");
        for (auto c : commands) {
            v << c->name << " ";
            auto start = webcface::button("start", c->start);
            auto stop = webcface::button("stop", c->terminate);
            if (c->is_running()) {
                // todo: button.disable がほしい
                start.bgColor(WebCFace::ViewColor::gray);
                stop.bgColor(WebCFace::ViewColor::orange);
            } else {
                start.bgColor(WebCFace::ViewColor::green);
                stop.bgColor(WebCFace::ViewColor::gray);
            }
            v << start << stop;
            if (!c->is_running() && c->exit_status != 0) {
                v << webcface::text("(" + std::to_string(c->exit_status) + ") ")
                         .textColor(WebCFace::ViewColor::red);
            }
            if (!c->is_running() &&
                (c->exit_status != 0 ||
                 c->capture_stdout == CaptureMode::always)) {
                std::string logs = c->logs;
                if (!logs.empty()) {
                    v << webcface::button("Clear Logs",
                                          [c] { c->logs.clear(); })
                             .bgColor(webcface::ViewColor::cyan)
                      << std::endl;
                    for (int i;
                         (i = logs.find_first_of("\n")) != std::string::npos;) {
                        v << "　　" << logs.substr(0, i) << std::endl;
                        logs = logs.substr(i + 1);
                    }
                    if (!logs.empty()) {
                        v << "　　" << logs << std::endl;
                    }
                } else {
                    v << std::endl;
                }
            } else {
                v << std::endl;
            }
        }
        v.sync();
        wcli.sync();
    }
}
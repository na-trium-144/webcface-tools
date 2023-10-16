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
#include "command.h"

void launcher(WebCFace::Client &wcli, toml::parse_result &config){
    std::vector<std::shared_ptr<Command>> commands;

    auto config_commands = config["command"].as_array();
    for (auto &&v : *config_commands) {
        commands.push_back(
            std::make_shared<Command>(wcli, v[toml::path("name")].value_or(""),
                                      v[toml::path("exec")].value_or(""),
                                      v[toml::path("workdir")].value_or(".")));
    }

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        auto v = wcli.view("launcher");
        for (auto c : commands) {
            v << c->name << " ";
            if (c->is_running()) {
                v << WebCFace::ViewComponents::button("stop", c->terminate)
                         .bgColor(WebCFace::ViewColor::orange);
            } else {
                v << WebCFace::ViewComponents::button("start", c->start);
                if (c->exit_status != 0) {
                    v << WebCFace::ViewComponents::text(
                             "(" + std::to_string(c->exit_status) + ") ")
                             .textColor(WebCFace::ViewColor::red);
                }
            }
            v << std::endl;
        }
        v.sync();
        wcli.sync();
    }
}
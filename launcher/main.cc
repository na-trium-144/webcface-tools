#include <webcface/webcface.h>
#include <toml++/toml.hpp>
#include <process.hpp>
#include <string>
#include <memory>
#include <vector>
#include <thread>
#include <chrono>
#include <functional>
#include <stdexcept>
#include <iostream>

struct Command {
    Command(const Command&) = delete;
    Command& operator=(const Command&) = delete;
    Command(WebCFace::Client& wcli, const std::string& name, const std::string& exec,
        const std::string& workdir)
        : name(name), exec(exec), workdir(workdir)
    {
        start
            = wcli.func(name + "_start")
                  .set([this] {
                      if (is_running()) {
                          throw std::runtime_error("already started");
                      } else {
                          p = std::make_shared<TinyProcessLib::Process>(this->exec, this->workdir);
                      }
                  })
                  .hidden(true);
        terminate = wcli.func(name + "_terminate")
                        .set([this] {
                            if (p) {
                                p->kill(false);
                                p.reset();
                            } else {
                                throw std::runtime_error("already stopped");
                            }
                        })
                        .hidden(true);
    }
    std::string name;
    std::string exec;
    std::string workdir;
    int exit_status = 0;
    std::shared_ptr<TinyProcessLib::Process> p;
    WebCFace::Func start, terminate;
    bool is_running() { return p && !p->try_get_exit_status(exit_status); }
};
std::vector<std::shared_ptr<Command>> commands;
int main()
{
    // todo: オプションで変えられるようにする
    auto config = toml::parse_file("webcface-launcher.toml");
    std::string wcli_name = config["init"]["name"].value_or("webcface-launcher");
    std::string wcli_host = config["init"]["host"].value_or("127.0.0.1");
    int wcli_port = config["init"]["port"].value_or(WEBCFACE_DEFAULT_PORT);
    WebCFace::Client wcli(wcli_name, wcli_host, wcli_port);

    auto config_commands = config["command"].as_array();
    for (auto&& v : *config_commands) {
        commands.push_back(std::make_shared<Command>(wcli, v[toml::path("name")].value_or(""),
            v[toml::path("exec")].value_or(""), v[toml::path("workdir")].value_or(".")));
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
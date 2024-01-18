#pragma once
#include <webcface/webcface.h>
#include <string>
#include <memory>

struct Command {
    Command(const Command &) = delete;
    Command &operator=(const Command &) = delete;
    Command(WebCFace::Client &wcli, const std::string &name,
            const std::string &exec, const std::string &workdir)
        : name(name), exec(exec), workdir(workdir) {
        auto read_log = [this](const char *bytes, std::size_t n) {
            this->logs.append(bytes, n);
        };
        start = wcli.func(name + "_start")
                    .set([this, read_log] {
                        if (is_running()) {
                            spdlog::warn("Command '{}' is already started.",
                                         this->name);
                            throw std::runtime_error("already started");
                        } else {
                            spdlog::info("Starting command '{}'.", this->name);
                            this->logs.clear();
                            p = std::make_shared<TinyProcessLib::Process>(
                                this->exec, this->workdir, read_log, read_log);
                        }
                    })
                    .hidden(true);
        terminate =
            wcli.func(name + "_terminate")
                .set([this] {
                    if (p) {
                        spdlog::info("Stopping command '{}'.", this->name);
                        p->kill(false);
                        p.reset();
                    } else {
                        spdlog::warn("Command '{}' is already stopped.",
                                     this->name);
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
    std::string logs;
    bool is_running() { return p && !p->try_get_exit_status(exit_status); }
};
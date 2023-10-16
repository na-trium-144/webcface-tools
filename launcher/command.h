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
        start = wcli.func(name + "_start")
                    .set([this] {
                        if (is_running()) {
                            throw std::runtime_error("already started");
                        } else {
                            p = std::make_shared<TinyProcessLib::Process>(
                                this->exec, this->workdir);
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
#pragma once
#include <webcface/webcface.h>
#include <spdlog/spdlog.h>
#include <process.hpp>
#include <string>
#include <memory>
#include <unordered_map>
#include "../common/common.h"

enum class CaptureMode {
    never,
    onerror,
    always,
};
struct Command {
    Command(const Command &) = delete;
    Command &operator=(const Command &) = delete;
    Command(WebCFace::Client &wcli, const std::string &name,
            const std::string &exec, const std::string &workdir,
            CaptureMode capture_stdout, bool stdout_is_utf8,
            const std::unordered_map<std::string, std::string> &env)
        : name(name), exec(exec), workdir(workdir),
          capture_stdout(capture_stdout), stdout_is_utf8(stdout_is_utf8),
          env(env) {
        auto logger = spdlog::stdout_color_mt(name);
        logger->set_pattern("[%n] %v");
        auto read_log = [this, logger](const char *bytes, std::size_t n) {
#ifdef WIN32
            if (!this->stdout_is_utf8) {
                this->logs += acpToUTF8(bytes, static_cast<int>(n));
            } else {
                this->logs.append(bytes, n);
            }
#else
            this->logs.append(bytes, n);
#endif
            logger->info(std::string(bytes, n));
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
                            if (this->capture_stdout != CaptureMode::never) {
                                p = std::make_shared<TinyProcessLib::Process>(
                                    this->exec, this->workdir, this->env,
                                    read_log, read_log);
                            } else {
                                p = std::make_shared<TinyProcessLib::Process>(
                                    this->exec, this->workdir, this->env);
                            }
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
    CaptureMode capture_stdout;
    bool stdout_is_utf8;
    std::unordered_map<std::string, std::string> env;

    int exit_status = 0;
    std::shared_ptr<TinyProcessLib::Process> p;
    WebCFace::Func start, terminate;
    std::string logs;
    bool is_running() { return p && !p->try_get_exit_status(exit_status); }
};
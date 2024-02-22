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
struct Command : std::enable_shared_from_this<Command> {
    Command(const Command &) = delete;
    Command &operator=(const Command &) = delete;
    Command(const std::string &name, const std::string &exec,
            const std::string &workdir, CaptureMode capture_stdout,
            bool stdout_is_utf8,
            const std::unordered_map<std::string, std::string> &env)
        : std::enable_shared_from_this<Command>(), name(name), exec(exec),
          workdir(workdir), capture_stdout(capture_stdout),
          stdout_is_utf8(stdout_is_utf8), env(env),
          logger(spdlog::stdout_color_mt(name)) {
        logger->set_pattern("[%n] %v");
    }
    // shared_from_thisを使うためコンストラクタと別
    void initFunc(WebCFace::Client &wcli) {
        start_f = wcli.func(name + "_start")
                    .set([cmd = shared_from_this()] { cmd->start(); })
                    .hidden(true);
        terminate_f = wcli.func(name + "_terminate")
                        .set([cmd = shared_from_this()] { cmd->kill(); })
                        .hidden(true);
    }
    void start() {
        auto read_log = [cmd = shared_from_this()](const char *bytes,
                                                   std::size_t n) {
#ifdef _WIN32
            if (!cmd->stdout_is_utf8) {
                cmd->logs += acpToUTF8(bytes, static_cast<int>(n));
            } else {
                cmd->logs.append(bytes, n);
            }
#else
            cmd->logs.append(bytes, n);
#endif
            cmd->logger->info(std::string(bytes, n));
        };
        if (is_running()) {
            spdlog::warn("Command '{}' is already started.", this->name);
            throw std::runtime_error("already started");
        } else {
            spdlog::info("Starting command '{}'.", this->name);
            this->logs.clear();
            if (this->capture_stdout != CaptureMode::never) {
                p = std::make_shared<TinyProcessLib::Process>(
                    this->exec, this->workdir, this->env, read_log, read_log);
            } else {
                p = std::make_shared<TinyProcessLib::Process>(
                    this->exec, this->workdir, this->env);
            }
        }
    }
    void kill() {
        if (is_running()) {
            spdlog::info("Stopping command '{}'.", this->name);
            p->kill(false);
        } else {
            spdlog::warn("Command '{}' is already stopped.", this->name);
            throw std::runtime_error("already stopped");
        }
    }
#ifndef _WIN32
    void kill(int sig) {
        if (p) {
            spdlog::info("Sending signal {} to command '{}'.", sig, this->name);
            p->signal(sig);
        }
    }
#endif

    std::string name;
    std::string exec;
    std::string workdir;
    CaptureMode capture_stdout;
    bool stdout_is_utf8;
    std::unordered_map<std::string, std::string> env;

    int exit_status = 0;
    std::shared_ptr<TinyProcessLib::Process> p;
    std::shared_ptr<spdlog::logger> logger;
    webcface::Func start_f, terminate_f;
    std::string logs;
    bool is_running() { return p && !p->try_get_exit_status(exit_status); }
};
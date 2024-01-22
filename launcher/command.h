#pragma once
#include <webcface/webcface.h>
#include <string>
#include <memory>
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
            const std::string &capture_stdout, bool stdout_is_utf8)
        : name(name), exec(exec), workdir(workdir),
          stdout_is_utf8(stdout_is_utf8) {
        if (capture_stdout == "never") {
            this->capture_stdout = CaptureMode::never;
        } else if (capture_stdout == "onerror") {
            this->capture_stdout = CaptureMode::onerror;
        } else if (capture_stdout == "always") {
            this->capture_stdout = CaptureMode::always;
        } else {
            spdlog::error(
                "'stdout_capture' must be 'never', 'onerror' or 'always'");
        }
        auto read_log = [this](const char *bytes, std::size_t n) {
#ifdef WIN32
            if (!this->stdout_is_utf8) {
                this->logs += acpToUTF8(bytes, static_cast<int>(n));
            } else {
                this->logs.append(bytes, n);
            }
#else
            this->logs.append(bytes, n);
#endif
        };
        start =
            wcli.func(name + "_start")
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
                                this->exec, this->workdir, read_log, read_log);
                        } else {
                            p = std::make_shared<TinyProcessLib::Process>(
                                this->exec, this->workdir);
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

    int exit_status = 0;
    std::shared_ptr<TinyProcessLib::Process> p;
    WebCFace::Func start, terminate;
    std::string logs;
    bool is_running() { return p && !p->try_get_exit_status(exit_status); }
};
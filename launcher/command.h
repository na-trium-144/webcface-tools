#pragma once
#include <webcface/log.h>
#include <webcface/func.h>
#include <webcface/client.h>
#include <webcface/value.h>
#include <webcface/view.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <process.hpp>
#include <string>
#include <memory>
#include <unordered_map>
#include "../common/common.h"

struct Process : std::enable_shared_from_this<Process> {
    std::string name;
    std::string exec;
    std::string workdir;
    bool capture_stdout;
    bool stdout_is_utf8;
    std::unordered_map<std::string, std::string> env;
    int exit_status = 0;
    std::shared_ptr<TinyProcessLib::Process> p;
    std::shared_ptr<spdlog::logger> logger;
    std::optional<webcface::Log> send_logs = std::nullopt;
    std::string logs;
    std::size_t logs_last_pos = 0;

    Process(const Process &) = delete;
    Process &operator=(const Process &) = delete;
    Process(const std::string &name, const std::string &exec,
            const std::string &workdir, bool capture_stdout,
            bool stdout_is_utf8,
            const std::unordered_map<std::string, std::string> &env)
        : std::enable_shared_from_this<Process>(), name(name), exec(exec),
          workdir(workdir), capture_stdout(capture_stdout),
          stdout_is_utf8(stdout_is_utf8), env(env),
          logger(spdlog::stdout_color_mt(name)) {
        logger->set_pattern("[%n] %v");
    }

    void start() {
        logs_last_pos = 0;
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
            // 改行で区切り、出力
            while (cmd->logs.find('\n') != std::string_view::npos) {
                auto ln_pos = cmd->logs.find('\n');
                auto logs_line = std::string_view(cmd->logs).substr(0, ln_pos);
                cmd->logger->info("{}", logs_line);
                if (cmd->send_logs.has_value()) {
                    cmd->send_logs->append(webcface::level::info, logs_line);
                }

                cmd->logs = cmd->logs.substr(ln_pos + 1);
                cmd->logs_last_pos = 0;
            }
        };
        if (is_running()) {
            spdlog::warn("Command '{}' is already started.", this->name);
            // throw std::runtime_error("already started");
        } else {
            spdlog::info("Starting command '{}'.", this->name);
            this->logs.clear();
            if (this->capture_stdout) {
                p = std::make_shared<TinyProcessLib::Process>(
                    this->exec, this->workdir, this->env, read_log, read_log);
            } else {
                p = std::make_shared<TinyProcessLib::Process>(
                    this->exec, this->workdir, this->env);
            }
        }
    }
    void kill([[maybe_unused]] int sig) {
        if (is_running()) {
#ifdef _WIN32
            spdlog::info("Stopping command '{}'.", this->name);
            p->kill(false);
#else
            spdlog::info("Sending signal {} to command '{}'.", sig, this->name);
            p->signal(sig);
#endif
        } else {
            spdlog::warn("Command '{}' is already stopped.", this->name);
            // throw std::runtime_error("already stopped");
        }
    }

    bool is_running() { return p && !p->try_get_exit_status(exit_status); }
};

// ProcessにStart/Stopボタンの実装を追加したもの
struct Command : std::enable_shared_from_this<Command> {
    webcface::Func start_f, stop_f;
    std::optional<webcface::CallHandle> stop_h, run_h;
    std::shared_ptr<Process> start_p;
    using StopOption =
        std::variant<std::nullopt_t, std::shared_ptr<Process>, int>;
    StopOption stop_p;

    Command(const Command &) = delete;
    Command &operator=(const Command &) = delete;
    Command(const std::shared_ptr<Process> &start_p, const StopOption &stop_p)
        : start_p(start_p), stop_p(stop_p) {}

    // shared_from_thisを使うためコンストラクタと別
    void initFunc(webcface::Client &wcli) {
        start_f =
            wcli.func(start_p->name + "/start").set([cmd = shared_from_this()] {
                if (cmd->start_p->is_running()) {
                    throw std::runtime_error("already started");
                } else {
                    cmd->start_p->start();
                }
            });
        wcli.func(start_p->name + "/run")
            .set([cmd = shared_from_this()](webcface::CallHandle h) {
                if (cmd->start_p->is_running()) {
                    h.reject("already started");
                } else {
                    cmd->start_p->start();
                    cmd->run_h = h;
                }
            });
        stop_f = wcli.func(start_p->name + "/stop")
                     .set([cmd = shared_from_this()](webcface::CallHandle h) {
                         if (cmd->start_p->is_running()) {
                             switch (cmd->stop_p.index()) {
                             case 1:
                                 std::get<1>(cmd->stop_p)->start();
                                 break;
                             case 2:
                                 cmd->start_p->kill(std::get<2>(cmd->stop_p));
                                 break;
                             default:
                                 // throw std::runtime_error("stop signal
                                 // disabled");
                                 h.reject("stop signal disabled");
                             }
                             cmd->stop_h = h;
                         } else {
                             h.reject("already stopped");
                         }
                     });
    }

    void update(webcface::Client &wcli) {
        wcli.value(start_p->name).child("running") = start_p->is_running();
        wcli.value(start_p->name).child("exit_status") = start_p->exit_status;
        if (stop_h.has_value() && !start_p->is_running()) {
            stop_h->respond();
            stop_h.reset();
        }
        if (run_h.has_value() && !start_p->is_running()) {
            run_h->respond();
            run_h.reset();
        }
    }
    void updateView(webcface::View &v) {
        v << start_p->name << " ";
        auto start = webcface::button("start", start_f);
        auto stop = webcface::button("stop", stop_f);
        if (start_p->is_running()) {
            // todo: button.disable がほしい
            start.bgColor(webcface::ViewColor::gray);
            stop.bgColor(webcface::ViewColor::orange);
        } else {
            start.bgColor(webcface::ViewColor::green);
            stop.bgColor(webcface::ViewColor::gray);
        }
        v << start;
        if (stop_p.index() != 0) {
            v << stop;
        }
        if (!start_p->is_running() && start_p->exit_status != 0) {
            v << webcface::text("(" + std::to_string(start_p->exit_status) +
                                ") ")
                     .textColor(webcface::ViewColor::red);
        }
        v << std::endl;
    }
};

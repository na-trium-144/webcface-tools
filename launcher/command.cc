#include "command.h"

Process::Process(const std::string &name, const std::string &exec,
                 const std::string &workdir, bool capture_stdout,
                 bool stdout_is_utf8,
                 const std::unordered_map<std::string, std::string> &env)
    : std::enable_shared_from_this<Process>(), name(name), exec(exec),
      workdir(workdir), capture_stdout(capture_stdout),
      stdout_is_utf8(stdout_is_utf8), env(env),
      logger(spdlog::stdout_color_mt(name)) {
    logger->set_pattern("[%n] %v");
}

void Process::readLog(const char *bytes, std::size_t n, bool is_stderr) {
    std::string &logs = is_stderr ? logs_err : logs_out;

#ifdef _WIN32
    if (!this->stdout_is_utf8) {
        logs += acpToUTF8(bytes, static_cast<int>(n));
    } else {
        logs.append(bytes, n);
    }
#else
    logs.append(bytes, n);
#endif
    // 改行で区切り、出力
    while (logs.find('\n') != std::string_view::npos) {
        auto ln_pos = logs.find('\n');
        auto logs_line = std::string_view(logs).substr(0, ln_pos);
        if (is_stderr) {
            this->logger->warn("{}", logs_line);
        } else {
            this->logger->info("{}", logs_line);
        }
        if (this->send_logs.has_value()) {
            this->send_logs->append(is_stderr ? webcface::level::warn
                                              : webcface::level::info,
                                    logs_line);
        }

        logs = logs.substr(ln_pos + 1);
    }
}
void Process::start() {
    auto read_out = [cmd = shared_from_this()](const char *bytes,
                                               std::size_t n) {
        cmd->readLog(bytes, n, false);
    };
    auto read_err = [cmd = shared_from_this()](const char *bytes,
                                               std::size_t n) {
        cmd->readLog(bytes, n, true);
    };
    if (is_running()) {
        spdlog::warn("Command '{}' is already started.", this->name);
        // throw std::runtime_error("already started");
    } else {
        spdlog::info("Starting command '{}'.", this->name);
        this->logs_out.clear();
        this->logs_err.clear();
        if (this->send_logs.has_value()) {
            this->send_logs->append(webcface::level::debug,
                                    "<Starting command '" + this->name + "'>");
        }
        if (this->capture_stdout) {
            p = std::make_shared<TinyProcessLib::Process>(
                this->exec, this->workdir, this->env, read_out, read_err);
        } else {
            p = std::make_shared<TinyProcessLib::Process>(
                this->exec, this->workdir, this->env);
        }
    }
}
void Process::kill([[maybe_unused]] int sig) {
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
bool Process::is_running() {
    bool running = p && !p->try_get_exit_status(exit_status);
    if (running_prev == true && running == false) {
        if (this->send_logs.has_value()) {
            if (!this->logs_out.empty()) {
                this->send_logs->append(webcface::level::info, this->logs_out);
                this->logs_out.clear();
            }
            if (!this->logs_err.empty()) {
                this->send_logs->append(webcface::level::warn, this->logs_err);
                this->logs_err.clear();
            }
            this->send_logs->append(webcface::level::debug,
                                    "<Command '" + this->name +
                                        "' has stopped with exit code " +
                                        std::to_string(exit_status) + ">");
        }
    }
    running_prev = running;
    return running;
}

void Command::initFunc(webcface::Client &wcli) {
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
                cmd->stop_h.push_back(h);
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
                         cmd->stop_h.push_back(h);
                     } else {
                         h.reject("already stopped");
                     }
                 });
    kill_f = wcli.func(start_p->name + "/kill")
                 .set([cmd = shared_from_this()](webcface::CallHandle h) {
                     if (cmd->start_p->is_running()) {
                         switch (cmd->kill_p.index()) {
                         case 1:
                             std::get<1>(cmd->kill_p)->start();
                             break;
                         case 2:
                             cmd->start_p->kill(std::get<2>(cmd->kill_p));
                             break;
                         default:
                             // throw std::runtime_error("stop signal
                             // disabled");
                             h.reject("kill signal disabled");
                         }
                         cmd->stop_h.push_back(h);
                     } else {
                         h.reject("already stopped");
                     }
                 });
}

void Command::update(webcface::Client &wcli) {
    wcli.value(start_p->name).child("running") = start_p->is_running();
    wcli.value(start_p->name).child("exit_status") = start_p->exit_status;
    if (!start_p->is_running()) {
        for (const auto &h : stop_h) {
            h.respond();
        }
        stop_h.clear();
    }
}
void Command::updateView(webcface::View &v) {
    v << start_p->name << " ";
    auto start = webcface::button("start", start_f);
    auto stop = webcface::button("stop", stop_f);
    auto kill = webcface::button("kill", kill_f);
    if (start_p->is_running()) {
        // todo: button.disable がほしい
        start.bgColor(webcface::ViewColor::gray);
        stop.bgColor(webcface::ViewColor::orange);
        kill.bgColor(webcface::ViewColor::red);
    } else {
        start.bgColor(webcface::ViewColor::green);
        stop.bgColor(webcface::ViewColor::gray);
        kill.bgColor(webcface::ViewColor::gray);
    }
    v << start;
    if (stop_p.index() != 0) {
        v << stop;
    }
    if (kill_p.index() != 0) {
        v << kill;
    }
    if (!start_p->is_running() && start_p->exit_status != 0) {
        v << webcface::text("(" + std::to_string(start_p->exit_status) + ") ")
                 .textColor(webcface::ViewColor::red);
    }
    v << std::endl;
}

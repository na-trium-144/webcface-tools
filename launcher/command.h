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
    bool running_prev = false;
    std::shared_ptr<spdlog::logger> logger;
    std::optional<webcface::Log> send_logs = std::nullopt;
    std::string logs_out, logs_err;

    Process(const Process &) = delete;
    Process &operator=(const Process &) = delete;
    Process(const std::string &name, const std::string &exec,
            const std::string &workdir, bool capture_stdout,
            bool stdout_is_utf8,
            const std::unordered_map<std::string, std::string> &env);

    void readLog(const char *bytes, std::size_t n, bool is_stderr);
    void start();
    void kill([[maybe_unused]] int sig);

    bool is_running();
};

// ProcessにStart/Stopボタンの実装を追加したもの
struct Command : std::enable_shared_from_this<Command> {
    webcface::Func start_f, stop_f, kill_f;
    std::vector<webcface::CallHandle> stop_h;
    std::shared_ptr<Process> start_p;
    using StopOption =
        std::variant<std::nullopt_t, std::shared_ptr<Process>, int>;
    StopOption stop_p, kill_p;

    Command(const Command &) = delete;
    Command &operator=(const Command &) = delete;
    Command(const std::shared_ptr<Process> &start_p, const StopOption &stop_p,
            const StopOption &kill_p)
        : start_p(start_p), stop_p(stop_p), kill_p(kill_p) {}

    // shared_from_thisを使うためコンストラクタと別
    void initFunc(webcface::Client &wcli);

    void update(webcface::Client &wcli);
    void updateView(webcface::View &v);
};

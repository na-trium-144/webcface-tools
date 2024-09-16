#include <webcface/log.h>
#include <webcface/client.h>
#include <spdlog/sinks/base_sink.h>

class LoggerSink final : public spdlog::sinks::base_sink<std::mutex> {
    webcface::Log wcli_log;

  protected:
    void sink_it_(const spdlog::details::log_msg &msg) override {
        if (auto *buf_ptr = msg.payload.data()) {
            std::string log_text(buf_ptr, buf_ptr + msg.payload.size());
            if (log_text.size() > 0 && log_text.back() == '\n') {
                log_text.pop_back();
            }
            if (log_text.size() > 0 && log_text.back() == '\r') {
                log_text.pop_back();
            }
            wcli_log.append(msg.level, msg.time, log_text);
        }
    }
    void flush_() override {}

  public:
    explicit LoggerSink(webcface::Client &wcli)
        : spdlog::sinks::base_sink<std::mutex>(), wcli_log(wcli.log()) {}
    void set_pattern_(const std::string &) override {}
    void set_formatter_(std::unique_ptr<spdlog::formatter>) override {}
};

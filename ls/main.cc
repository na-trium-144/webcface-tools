#include <webcface/webcface.h>
#include <CLI/CLI.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <spdlog/spdlog.h>
#include <eventpp/utilities/counterremover.h>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include "../common/common.h"

int main(int argc, char **argv) {
    CLI::App app{TOOLS_VERSION_DISP("WebCFace ls")};

    std::string wcli_host = "127.0.0.1";
    int wcli_port = WEBCFACE_DEFAULT_PORT;
    int timeout = 10;
    app.add_option("-a,--address", wcli_host,
                   "Server address (default: 127.0.0.1)");
    app.add_option("-p,--port", wcli_port,
                   "Server port (default: " WEBCFACE_DEFAULT_PORT_S ")");
    app.add_option("-t,--timeout", timeout,
                   "Timeout for receiving entry information (default: 10)");

    CLI11_PARSE(app, argc, argv);

    webcface::Client wcli("", wcli_host, wcli_port);
    wcli.autoReconnect(false);

    static std::mutex timeout_m;
    static auto timeout_begin = std::chrono::steady_clock::now();
    wcli.onMemberEntry().appendListener([](const webcface::Member &m) {
        eventpp::counterRemover(m.onPing().callbackList())
            .append([](const auto &) {
                std::lock_guard lock(timeout_m);
                timeout_begin = std::chrono::steady_clock::now();
            });
        m.onValueEntry().callbackList().append([](const auto &) {
            std::lock_guard lock(timeout_m);
            timeout_begin = std::chrono::steady_clock::now();
        });
        m.onTextEntry().callbackList().append([](const auto &) {
            std::lock_guard lock(timeout_m);
            timeout_begin = std::chrono::steady_clock::now();
        });
        m.onFuncEntry().callbackList().append([](const auto &) {
            std::lock_guard lock(timeout_m);
            timeout_begin = std::chrono::steady_clock::now();
        });
        m.onViewEntry().callbackList().append([](const auto &) {
            std::lock_guard lock(timeout_m);
            timeout_begin = std::chrono::steady_clock::now();
        });
        m.onImageEntry().callbackList().append([](const auto &) {
            std::lock_guard lock(timeout_m);
            timeout_begin = std::chrono::steady_clock::now();
        });
        m.onCanvas2DEntry().callbackList().append([](const auto &) {
            std::lock_guard lock(timeout_m);
            timeout_begin = std::chrono::steady_clock::now();
        });
        m.onCanvas3DEntry().callbackList().append([](const auto &) {
            std::lock_guard lock(timeout_m);
            timeout_begin = std::chrono::steady_clock::now();
        });
        m.onRobotModelEntry().callbackList().append([](const auto &) {
            std::lock_guard lock(timeout_m);
            timeout_begin = std::chrono::steady_clock::now();
        });
    });
    wcli.waitConnection();

    if (!wcli.connected()) {
        spdlog::error("could not connect to webcface server");
        return 1;
    }

    std::chrono::milliseconds wait_dur;
    do {
        {
            std::lock_guard lock(timeout_m);
            wait_dur = std::chrono::milliseconds(timeout) -
                       std::chrono::duration_cast<std::chrono::milliseconds>(
                           std::chrono::steady_clock::now() - timeout_begin);
        }
        std::this_thread::sleep_for(wait_dur);
    } while (wait_dur.count() > 0);

    std::vector<ftxui::Element> elem_members;
    for (const auto &m : wcli.members()) {
        ftxui::Decorator ping_color;
        std::string ping_str = std::to_string(m.pingStatus().value_or(0));
        if (!m.pingStatus()) {
            ping_color = ftxui::color(ftxui::Color::Red);
            ping_str = "?";
        } else if (*m.pingStatus() < 10) {
            ping_color = ftxui::color(ftxui::Color::Green);
        } else if (*m.pingStatus() < 100) {
            ping_color = ftxui::color(ftxui::Color::Yellow);
        } else {
            ping_color = ftxui::color(ftxui::Color::Red);
        }
        auto row_name = ftxui::hbox({
            ftxui::text(m.name()) |
                (m.pingStatus() ? ftxui::bold : ftxui::nothing),
            ftxui::text(": "),
            ftxui::text(m.libName()) | ftxui::dim,
            ftxui::text(" "),
            ftxui::text(m.libVersion()) | ftxui::dim,
            ftxui::text(" "),
            ftxui::text("[" + ping_str + " ms]") | ping_color,
        });
        elem_members.push_back(row_name);
    }

    // Define the document
    ftxui::Element document = ftxui::vbox(elem_members);
    auto screen = ftxui::Screen::Create(ftxui::Dimension::Full(),
                                        ftxui::Dimension::Fit(document));
    ftxui::Render(screen, document);
    screen.Print();
}

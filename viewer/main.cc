#include <CLI/CLI.hpp>
#include <string>
#include <algorithm>
#include "main.h"

int main(int argc, char **argv) {
    CLI::App app{"WebCFace Viewer " TOOLS_VERSION};
    app.allow_windows_style_options();

    std::string wcli_host = "127.0.0.1", wcli_name = "";
    int wcli_port = WEBCFACE_DEFAULT_PORT;
    app.add_option("-a,--address", wcli_host,
                   "Server address (default: 127.0.0.1)");
    app.add_option("-p,--port", wcli_port,
                   "Server port (default: " WEBCFACE_DEFAULT_PORT_S ")");
    // app.add_option("-m,--member", wcli_name, "Client member name");

    CLI11_PARSE(app, argc, argv);

    wcli = std::make_shared<WebCFace::Client>("", wcli_host, wcli_port);

    init_display();

    wcli->onMemberEntry().appendListener([](WebCFace::Member m) {
        m.onValueEntry().appendListener([](WebCFace::Value v) {
            {
                std::lock_guard lock(mtx);
                if (std::none_of(data.begin(), data.end(), [&](auto &d) {
                        if (std::holds_alternative<WebCFace::Value>(d)) {
                            auto dd = std::get<WebCFace::Value>(d);
                            return dd.member().name() == v.member().name() &&
                                   dd.name() == v.name();
                        }
                        return false;
                    })) {
                    data.push_back(v);
                }
            }
            v.appendListener([](auto) { update_display(); });
            wcli->sync();
        });
        m.onTextEntry().appendListener([](WebCFace::Text v) {
            {
                std::lock_guard lock(mtx);
                if (std::none_of(data.begin(), data.end(), [&](auto &d) {
                        if (std::holds_alternative<WebCFace::Text>(d)) {
                            auto dd = std::get<WebCFace::Text>(d);
                            return dd.member().name() == v.member().name() &&
                                   dd.name() == v.name();
                        }
                        return false;
                    })) {
                    data.push_back(v);
                }
            }
            v.appendListener([](auto) { update_display(); });
            wcli->sync();
        });
    });

    while (!wcli->connected()) {
        std::this_thread::yield();
    }
    wcli->sync();

    loop_display();
    exit_display();
}
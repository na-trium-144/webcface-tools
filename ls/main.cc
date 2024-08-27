#include <webcface/webcface.h>
#include <CLI/CLI.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <spdlog/spdlog.h>
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

    bool list = false, recursive = false;
    app.add_flag("-l", list, "Long listing format");
    app.add_flag("-r", recursive, "Recursively list fields");

    CLI11_PARSE(app, argc, argv);

    webcface::Client wcli("", wcli_host, wcli_port);
    wcli.autoReconnect(false);
    wcli.pingStatus(); // request
    wcli.waitConnection();

    if (!wcli.connected()) {
        spdlog::error("could not connect to webcface server");
        return 1;
    }

    ftxui::Element document;
    if (list) {
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
            elem_members.push_back(ftxui::hbox({
                ftxui::text(m.name()) |
                    (m.pingStatus() ? ftxui::bold : ftxui::nothing),
                ftxui::text(": "),
                ftxui::text(m.libName()) | ftxui::dim,
                ftxui::text(" "),
                ftxui::text(m.libVersion()) | ftxui::dim,
                ftxui::text(" "),
                ftxui::text("[" + ping_str + " ms]") | ping_color,
            }));

            if (recursive) {
                if (!m.valueEntries().empty()) {
                    for (const auto &v : m.valueEntries()) {
                        elem_members.push_back(ftxui::hbox({
                            ftxui::text(" [vl]") | ftxui::dim,
                            ftxui::text(v.name()) |
                                ftxui::color(ftxui::Color::Blue),
                        }));
                    }
                }
                if (!m.textEntries().empty()) {
                    for (const auto &v : m.textEntries()) {
                        elem_members.push_back(ftxui::hbox({
                            ftxui::text(" [tx]") | ftxui::dim,
                            ftxui::text(v.name()) |
                                ftxui::color(ftxui::Color::Cyan),
                        }));
                    }
                }
                if (!m.funcEntries().empty()) {
                    for (const auto &v : m.funcEntries()) {
                        std::vector<ftxui::Element> fn_args;
                        for (const auto &a : v.args()) {
                            if (!fn_args.empty()) {
                                fn_args.push_back(ftxui::text(", "));
                            }
                            if (!a.name().empty()) {
                                fn_args.push_back(ftxui::text(a.name()));
                                fn_args.push_back(ftxui::text(": ") |
                                                  ftxui::dim);
                            }
                            switch (a.type()) {
                            case webcface::ValType::int_:
                                fn_args.push_back(ftxui::text("int") |
                                                  ftxui::dim);
                                break;
                            case webcface::ValType::float_:
                                fn_args.push_back(ftxui::text("float") |
                                                  ftxui::dim);
                                break;
                            case webcface::ValType::bool_:
                                fn_args.push_back(ftxui::text("bool") |
                                                  ftxui::dim);
                                break;
                            case webcface::ValType::string_:
                                fn_args.push_back(ftxui::text("str") |
                                                  ftxui::dim);
                                break;
                            default:
                                fn_args.push_back(ftxui::text("?") |
                                                  ftxui::dim);
                                break;
                            }
                        }
                        elem_members.push_back(ftxui::hbox({
                            ftxui::text(" [fn]") | ftxui::dim,
                            ftxui::text(v.name()) |
                                ftxui::color(ftxui::Color::Green),
                            ftxui::text("("),
                            ftxui::hbox(fn_args),
                            ftxui::text(")"),
                        }));
                    }
                }
                if (!m.viewEntries().empty()) {
                    for (const auto &v : m.viewEntries()) {
                        elem_members.push_back(ftxui::hbox({
                            ftxui::text(" [vi]") | ftxui::dim,
                            ftxui::text(v.name()) |
                                ftxui::color(ftxui::Color::Red),
                        }));
                    }
                }
                if (!m.canvas2DEntries().empty()) {
                    for (const auto &v : m.canvas2DEntries()) {
                        elem_members.push_back(ftxui::hbox({
                            ftxui::text(" [2d]") | ftxui::dim,
                            ftxui::text(v.name()) |
                                ftxui::color(ftxui::Color::Yellow),
                        }));
                    }
                }
                if (!m.imageEntries().empty()) {
                    for (const auto &v : m.imageEntries()) {
                        elem_members.push_back(ftxui::hbox({
                            ftxui::text(" [im]") | ftxui::dim,
                            ftxui::text(v.name()) |
                                ftxui::color(ftxui::Color::Yellow),
                        }));
                    }
                }
                if (!m.canvas3DEntries().empty()) {
                    for (const auto &v : m.canvas3DEntries()) {
                        elem_members.push_back(ftxui::hbox({
                            ftxui::text(" [3d]") | ftxui::dim,
                            ftxui::text(v.name()) |
                                ftxui::color(ftxui::Color::Yellow),
                        }));
                    }
                }
                if (!m.robotModelEntries().empty()) {
                    for (const auto &v : m.robotModelEntries()) {
                        elem_members.push_back(ftxui::hbox({
                            ftxui::text(" [rm]") | ftxui::dim,
                            ftxui::text(v.name()) |
                                ftxui::color(ftxui::Color::Yellow),
                        }));
                    }
                }
            }
        }

        // Define the document
        document = ftxui::vbox(elem_members);
    } else {
        if (!recursive) {
            std::vector<ftxui::Element> elem_members;
            for (const auto &m : wcli.members()) {
                elem_members.push_back(
                    ftxui::text(m.name() + " ") |
                    (m.pingStatus() ? ftxui::bold : ftxui::nothing));
            }
            // lsの表示は縦に並ぶけど、ftxuiでそれを実現するのめんどくさそう
            // とりあえず動けばいいので横並びにしている
            ftxui::FlexboxConfig config;
            config.direction = ftxui::FlexboxConfig::Direction::Row;
            config.wrap = ftxui::FlexboxConfig::Wrap::Wrap;
            config.justify_content =
                ftxui::FlexboxConfig::JustifyContent::FlexStart;
            config.align_items = ftxui::FlexboxConfig::AlignItems::FlexStart;
            config.align_content =
                ftxui::FlexboxConfig::AlignContent::FlexStart;
            document = ftxui::flexbox(elem_members, config);
        } else {
            std::vector<ftxui::Element> elem_members;
            for (const auto &m : wcli.members()) {
                elem_members.push_back(ftxui::hbox({
                    ftxui::text(m.name()) |
                        (m.pingStatus() ? ftxui::bold : ftxui::nothing),
                    ftxui::text(":"),
                }));
                std::vector<ftxui::Element> elem_fields;
                for (const auto &v : m.valueEntries()) {
                    elem_fields.push_back(ftxui::text(v.name() + " ") |
                                          ftxui::color(ftxui::Color::Blue));
                }
                for (const auto &v : m.textEntries()) {
                    elem_fields.push_back(ftxui::text(v.name() + " ") |
                                          ftxui::color(ftxui::Color::Cyan));
                }
                for (const auto &v : m.funcEntries()) {
                    elem_fields.push_back(ftxui::text(v.name() + " ") |
                                          ftxui::color(ftxui::Color::Green));
                }
                for (const auto &v : m.viewEntries()) {
                    elem_fields.push_back(ftxui::text(v.name() + " ") |
                                          ftxui::color(ftxui::Color::Red));
                }
                for (const auto &v : m.canvas2DEntries()) {
                    elem_fields.push_back(ftxui::text(v.name() + " ") |
                                          ftxui::color(ftxui::Color::Yellow));
                }
                for (const auto &v : m.imageEntries()) {
                    elem_fields.push_back(ftxui::text(v.name() + " ") |
                                          ftxui::color(ftxui::Color::Yellow));
                }
                for (const auto &v : m.canvas3DEntries()) {
                    elem_fields.push_back(ftxui::text(v.name() + " ") |
                                          ftxui::color(ftxui::Color::Yellow));
                }
                for (const auto &v : m.robotModelEntries()) {
                    elem_fields.push_back(ftxui::text(v.name() + " ") |
                                          ftxui::color(ftxui::Color::Yellow));
                }
                if (!elem_fields.empty()) {
                    ftxui::FlexboxConfig config;
                    config.direction = ftxui::FlexboxConfig::Direction::Row;
                    config.wrap = ftxui::FlexboxConfig::Wrap::Wrap;
                    config.justify_content =
                        ftxui::FlexboxConfig::JustifyContent::FlexStart;
                    config.align_items =
                        ftxui::FlexboxConfig::AlignItems::FlexStart;
                    config.align_content =
                        ftxui::FlexboxConfig::AlignContent::FlexStart;
                    elem_members.push_back(ftxui::hbox({
                        ftxui::emptyElement() |
                            ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 1),
                        ftxui::flexbox(elem_fields, config),
                    }));
                }
            }
            document = ftxui::vbox(elem_members);
        }
    }

    auto screen = ftxui::Screen::Create(ftxui::Dimension::Full(),
                                        ftxui::Dimension::Fit(document));
    ftxui::Render(screen, document);
    screen.Print();
}

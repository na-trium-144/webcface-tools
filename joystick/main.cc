#include <CLI/CLI.hpp>
#include <webcface/client.h>
#include <webcface/text.h>
#include <webcface/value.h>
#include <webcface/logger.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <SDL.h>
#include "../common/common.h"
#include "../common/logger_sink.h"
#undef main

int SDL_main(int argc, char **argv) {
    CLI::App app{TOOLS_VERSION_DISP("WebCFace Joystick")};

    std::string wcli_host = "127.0.0.1", wcli_name = "webcface-joystick";
    int wcli_port = WEBCFACE_DEFAULT_PORT;
    app.add_option("-a,--address", wcli_host,
                   "Server address (default: 127.0.0.1)");
    app.add_option("-p,--port", wcli_port,
                   "Server port (default: " WEBCFACE_DEFAULT_PORT_S ")");
    app.add_option("-m,--member", wcli_name, "Client member name");

    CLI11_PARSE(app, argc, argv);

    webcface::Client wcli(wcli_name, wcli_host, wcli_port);
    wcli.waitConnection();

    auto logger = std::make_shared<spdlog::logger>("webcface-joystick");
    logger->sinks() = {std::make_shared<spdlog::sinks::stderr_color_sink_mt>(),
                       std::make_shared<LoggerSink>(wcli)};

    if (SDL_Init(SDL_INIT_JOYSTICK) < 0) {
        logger->critical("Error in SDL_Init: {}", SDL_GetError());
        wcli.sync();
        return 1;
    }
    int joystick_num = SDL_NumJoysticks();
    if (joystick_num < 0) {
        logger->critical("Error in SDL_NumJoysticks: {}", SDL_GetError());
        wcli.sync();
        return 1;
    }

    std::unordered_map<int, SDL_Joystick *> joystick;
    for (int n = 0; n < joystick_num; n++) {
        if (SDL_JoystickGetDeviceType(n) == SDL_JOYSTICK_TYPE_GAMECONTROLLER) {
            SDL_ClearError();
            logger->info("[{}]:", n);
            const char *name = SDL_JoystickNameForIndex(n);
            if (!name) {
                name = SDL_GetError();
            }
            logger->info("Name: {}", name);
            wcli.text(std::to_string(n)).child("name") = name;

            SDL_ClearError();
            SDL_Joystick *j = SDL_JoystickOpen(n);
            if (!j) {
                logger->error("Error in SDL_JoystickOpen: {}", SDL_GetError());
                continue;
            }
            joystick[n] = j;
        }
    }
    if (joystick.size() == 0) {
        logger->error("No joysticks found");
        wcli.sync();
        return 1;
    }
    SDL_JoystickEventState(SDL_IGNORE);

    while (true) {
        SDL_JoystickUpdate();
        for (auto [n, j] : joystick) {
            auto wcli_v = wcli.child(n);
            int buttons_num = SDL_JoystickNumButtons(j);
            auto buttons = wcli_v.value("buttons").resize(0);
            for (int i = 0; i < buttons_num; i++) {
                buttons.push_back(SDL_JoystickGetButton(j, i));
            }
            int axes_num = SDL_JoystickNumAxes(j);
            auto axes = wcli_v.value("axes").resize(0);
            for (int i = 0; i < axes_num; i++) {
                axes.push_back(SDL_JoystickGetAxis(j, i));
            }
            int hats_num = SDL_JoystickNumHats(j);
            for (int i = 0; i < hats_num; i++) {
                int hat = SDL_JoystickGetHat(j, i);
                auto hat_v = wcli_v.value("hats")[i];
                hat_v["up"] = !!(hat & SDL_HAT_UP);
                hat_v["down"] = !!(hat & SDL_HAT_DOWN);
                hat_v["left"] = !!(hat & SDL_HAT_LEFT);
                hat_v["right"] = !!(hat & SDL_HAT_RIGHT);
            }
            wcli_v.value("power") = SDL_JoystickCurrentPowerLevel(j);
        }
        wcli.sync();
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                SDL_Quit();
                return 0;
            default:
                break;
            }
        }
        SDL_Delay(1);
    }
}

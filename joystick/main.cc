#include <CLI/CLI.hpp>
#include <webcface/client.h>
#include <webcface/text.h>
#include <webcface/value.h>
#include <webcface/logger.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <SDL.h>
#include "../common/common.h"
#undef main

int main(int argc, char **argv) {
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

    std::vector<spdlog::sink_ptr> sinks = {
        std::make_shared<spdlog::sinks::stderr_color_sink_mt>(),
        wcli.loggerSink(),
    };
    auto logger = std::make_shared<spdlog::logger>("webcface-joystick",
                                                   sinks.begin(), sinks.end());

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
            auto wcli_v = wcli.value(std::to_string(n));
            int buttons_num = SDL_JoystickNumButtons(j);
            std::vector<bool> buttons;
            for (int i = 0; i < buttons_num; i++) {
                buttons.push_back(SDL_JoystickGetButton(j, i));
            }
            wcli_v.child("buttons") = buttons;
            int axes_num = SDL_JoystickNumAxes(j);
            std::vector<std::int16_t> axes;
            for (int i = 0; i < axes_num; i++) {
                axes.push_back(SDL_JoystickGetAxis(j, i));
            }
            wcli_v.child("axes") = axes;
            int hats_num = SDL_JoystickNumHats(j);
            for (int i = 0; i < hats_num; i++) {
                int hat = SDL_JoystickGetHat(j, i);
                wcli_v.child("hats").child(std::to_string(i)) = {
                    {"up", !!(hat & SDL_HAT_UP)},
                    {"down", !!(hat & SDL_HAT_DOWN)},
                    {"left", !!(hat & SDL_HAT_LEFT)},
                    {"right", !!(hat & SDL_HAT_RIGHT)},
                };
            }
            wcli_v.child("power") = SDL_JoystickCurrentPowerLevel(j);
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

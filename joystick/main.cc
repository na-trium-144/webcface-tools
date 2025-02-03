#include <CLI/CLI.hpp>
#include <webcface/client.h>
#include <webcface/text.h>
#include <webcface/value.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <SDL.h>
#include <iostream>
#include "../common/common.h"
#include "../common/logger_sink.h"
#undef main

std::string getJoystickTypeName(SDL_JoystickType type) {
    switch (type) {
#define define_joystick_type(TYPE)                                             \
    case SDL_JOYSTICK_TYPE_##TYPE:                                             \
        return #TYPE;
        define_joystick_type(UNKNOWN);
        define_joystick_type(GAMECONTROLLER);
        define_joystick_type(WHEEL);
        define_joystick_type(ARCADE_STICK);
        define_joystick_type(FLIGHT_STICK);
        define_joystick_type(DANCE_PAD);
        define_joystick_type(GUITAR);
        define_joystick_type(DRUM_KIT);
        define_joystick_type(ARCADE_PAD);
        define_joystick_type(THROTTLE);
    default:
        return std::to_string(static_cast<int>(type));
    }
}

extern "C" int main(int argc, char *argv[]) {
    CLI::App app{TOOLS_VERSION_DISP("WebCFace Joystick")};

    std::string wcli_host = "127.0.0.1", wcli_name = "webcface-joystick";
    int wcli_port = WEBCFACE_DEFAULT_PORT;
    app.add_option("-a,--address", wcli_host,
                   "Server address (default: 127.0.0.1)");
    app.add_option("-p,--port", wcli_port,
                   "Server port (default: " WEBCFACE_DEFAULT_PORT_S ")");
    app.add_option("-m,--member", wcli_name, "Client member name");

    bool list = false;
    app.add_flag("-l,--list", list, "List information of connected joysticks");

    std::string id;
    app.add_option("GUID", id, "Joystick GUID to use");
    std::size_t index = 0;
    app.add_option(
        "index", index,
        "Joystick index, when there are multiple joystick with same GUID. "
        "Specify number from 0 to (number of joysticks with same GUID) - 1.");

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

    if (joystick_num == 0) {
        logger->error("No joysticks found");
        wcli.sync();
        return 1;
    }

    if (list) {
        std::vector<std::string> names;
        std::size_t max_name_len = 0;
        for (int n = 0; n < joystick_num; n++) {
            const char *name = SDL_JoystickNameForIndex(n);
            if (name) {
                names.push_back(name);
            } else {
                names.push_back("");
                logger->warn("Failed to get name of joystick {}: {}", n,
                             SDL_GetError());
            }
            std::size_t name_len = strlen(name);
            if (max_name_len < name_len) {
                max_name_len = name_len;
            }
        }
        std::size_t max_index_len =
            1 + static_cast<std::size_t>(std::log10(joystick_num));
        std::cout << std::string(max_index_len + 1, ' ') << "GUID"
                  << std::string(32 - 4 + 1, ' ') << "Type"
                  << std::string(14 - 4 + 1, ' ') << "Name" << std::endl;
        for (int n = 0; n < joystick_num; n++) {
            char guid_buf[33];
            SDL_JoystickGetGUIDString(SDL_JoystickGetDeviceGUID(n), guid_buf,
                                      sizeof(guid_buf));
            std::cout << std::setw(max_index_len) << n << " " << std::setw(32)
                      << guid_buf << " " << std::setw(14)
                      << getJoystickTypeName(SDL_JoystickGetDeviceType(n))
                      << " " << names[n] << std::endl;
        }
        return 0;
    }
    std::size_t j_index;
    if (id.empty()) {
        if (joystick_num == 1) {
            logger->warn("Joystick index not specified, but only one joystick "
                         "found so assuming index 0.");
            j_index = 0;
        } else {
            logger->error("Specify joystick GUID. Available joysticks can be "
                          "listed with -l option.");
            return 1;
        }
    } else {
        std::vector<std::size_t> j_indexes;
        for (int n = 0; n < joystick_num; n++) {
            char guid_buf[33];
            SDL_JoystickGetGUIDString(SDL_JoystickGetDeviceGUID(n), guid_buf,
                                      sizeof(guid_buf));
            if (id == guid_buf) {
                j_indexes.push_back(n);
            }
        }
        if (j_indexes.empty()) {
            logger->error("Joystick with the specified GUID not found. "
                          "Available joysticks can be "
                          "listed with -l option.");
            return 1;
        } else if (index >= j_indexes.size()) {
            logger->error("Only {} joysticks with the specified GUID found. "
                          "Index {} is out of range.",
                          j_indexes.size(), index);
            return 1;
        } else {
            j_index = j_indexes[index];
        }
    }

    SDL_ClearError();
    SDL_Joystick *joystick = SDL_JoystickOpen(j_index);
    if (!joystick) {
        logger->error("Error in SDL_JoystickOpen: {}", SDL_GetError());
        return 1;
    }
    SDL_ClearError();
    const char *name = SDL_JoystickNameForIndex(j_index);
    if (!name) {
        name = SDL_GetError();
    }
    logger->info("Name: {}", name);
    logger->info("Type: {}",
                 getJoystickTypeName(SDL_JoystickGetDeviceType(j_index)));
    wcli.text("name") = name;

    SDL_JoystickEventState(SDL_IGNORE);

    while (true) {
        SDL_JoystickUpdate();
        int buttons_num = SDL_JoystickNumButtons(joystick);
        auto buttons = wcli.value("buttons").resize(0);
        for (int i = 0; i < buttons_num; i++) {
            buttons.push_back(SDL_JoystickGetButton(joystick, i));
        }
        int axes_num = SDL_JoystickNumAxes(joystick);
        auto axes = wcli.value("axes").resize(0);
        for (int i = 0; i < axes_num; i++) {
            axes.push_back(SDL_JoystickGetAxis(joystick, i));
        }
        int hats_num = SDL_JoystickNumHats(joystick);
        auto hats = wcli.value("hats").resize(0);
        for (int i = 0; i < hats_num; i++) {
            int hat = SDL_JoystickGetHat(joystick, i);
            hats.push_back(!!(hat & SDL_HAT_UP));
            hats.push_back(!!(hat & SDL_HAT_DOWN));
            hats.push_back(!!(hat & SDL_HAT_LEFT));
            hats.push_back(!!(hat & SDL_HAT_RIGHT));
        }
        wcli.value("power") = SDL_JoystickCurrentPowerLevel(joystick);

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

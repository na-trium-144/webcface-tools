#include <CLI/CLI.hpp>
#include <webcface/client.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <webcface/text.h>
#include "../common/common.h"
#include "../common/logger_sink.h"
#include "main.h"

int main(int argc, char *argv[]) {
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
    wcli.start();

    logger = std::make_shared<spdlog::logger>("webcface-joystick");
    logger->sinks() = {std::make_shared<spdlog::sinks::stderr_color_sink_mt>(),
                       std::make_shared<LoggerSink>(wcli)};

    if (SDL_Init(SDL_INIT_JOYSTICK) < 0) {
        logger->critical("Error in SDL_Init: {}", SDL_GetError());
        wcli.sync();
        return 1;
    }
    joystick_num = SDL_NumJoysticks();
    if (joystick_num < 0) {
        logger->critical("Error in SDL_NumJoysticks: {}", SDL_GetError());
        wcli.sync();
        return 1;
    }
    if (joystick_num == 0) {
        logger->error("No joysticks found");
        return 1;
    }

    if (list) {
        listJoySticks();
        wcli.sync();
        return 0;
    }

    std::optional<std::size_t> j_index = selectJoyStick(id, index);
    if (!j_index) {
        wcli.sync();
        return 1;
    }

    SDL_ClearError();
    SDL_Joystick *joystick = SDL_JoystickOpen(*j_index);
    if (!joystick) {
        logger->error("Error in SDL_JoystickOpen: {}", SDL_GetError());
        return 1;
    }
    SDL_ClearError();
    const char *name = SDL_JoystickNameForIndex(*j_index);
    if(name){
        logger->info("Name: {}", name);
        wcli.text("name") = name;
    }else{
        logger->warn("Failed to get name of joystick: {}", SDL_GetError());
    }
    logger->info("Type: {}",
                 getJoystickTypeName(SDL_JoystickGetDeviceType(*j_index)));

    SDL_JoystickEventState(SDL_IGNORE);

    buttons_state.resize(SDL_JoystickNumButtons(joystick));
    logger->info("Number of buttons: {}", buttons_state.size());
    axes_state.resize(SDL_JoystickNumAxes(joystick));
    logger->info("Number of axes: {}", axes_state.size());
    hats_state.resize(SDL_JoystickNumHats(joystick) * 4);
    logger->info("Number of hats: {}", hats_state.size());
    balls_state.resize(SDL_JoystickNumBalls(joystick) * 2);
    logger->info("Number of balls: {}", balls_state.size());

    while (true) {
        SDL_JoystickUpdate();
        readJoystick(wcli, joystick);

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

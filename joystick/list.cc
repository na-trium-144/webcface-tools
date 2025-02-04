#include "main.h"
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

void listJoySticks() {
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
    std::cout << "GUID" << std::string(32 - 4 + 1, ' ') << "Type"
              << std::string(len_type_max - 4 + 1, ' ') << "Name" << std::endl;
    for (int n = 0; n < joystick_num; n++) {
        char guid_buf[33];
        SDL_JoystickGetGUIDString(SDL_JoystickGetDeviceGUID(n), guid_buf,
                                  sizeof(guid_buf));
        std::cout << std::left << std::setw(32) << guid_buf << " ";
        std::cout << std::left << std::setw(len_type_max)
                  << getTypeName(SDL_JoystickGetDeviceType(n),
                                 SDL_GameControllerTypeForIndex(n))
                  << " ";
        std::cout << names[n] << std::endl;
    }
}

std::string getTypeName(SDL_JoystickType type,
                        SDL_GameControllerType gamecon_type) {
    switch (type) {
    case SDL_JOYSTICK_TYPE_GAMECONTROLLER:
        switch (gamecon_type) {
        case SDL_CONTROLLER_TYPE_UNKNOWN:
            static_assert(
                sizeof("UNKNOWN GAMECONTROLLER") <= len_type_max,
                "len_type_max is smaller than sizeof UNKNOWN GAMECONTROLER");
            return "UNKNOWN GAMECONTROLLER";
#define define_gamecon_type(e, TYPE)                                           \
    case static_cast<SDL_GameControllerType>(e):                               \
        static_assert(sizeof(#TYPE) <= len_type_max + 1,                       \
                      "len_type_max is smaller than sizeof " #TYPE);           \
        return #TYPE;
            define_gamecon_type(1, XBOX360);
            define_gamecon_type(2, XBOXONE);
            define_gamecon_type(3, PS3);
            define_gamecon_type(4, PS4);
            define_gamecon_type(5, NINTENDO_SWITCH_PRO);
            define_gamecon_type(6, VIRTUAL);
            define_gamecon_type(7, PS5);
            define_gamecon_type(8, AMAZON_LUNA);
            define_gamecon_type(9, GOOGLE_STADIA);
            define_gamecon_type(10, NVIDIA_SHIELD);
            define_gamecon_type(11, NINTENDO_SWITCH_JOYCON_LEFT);
            define_gamecon_type(12, NINTENDO_SWITCH_JOYCON_RIGHT);
            define_gamecon_type(13, NINTENDO_SWITCH_JOYCON_PAIR);
        default:
            return "GAMECONTROLLER " + std::to_string(static_cast<int>(type));
        }

#define define_joystick_type(TYPE)                                             \
    case SDL_JOYSTICK_TYPE_##TYPE:                                             \
        return #TYPE;
        define_joystick_type(UNKNOWN);
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

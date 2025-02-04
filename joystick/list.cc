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
              << std::string(14 - 4 + 1, ' ') << "Name" << std::endl;
    for (int n = 0; n < joystick_num; n++) {
        char guid_buf[33];
        SDL_JoystickGetGUIDString(SDL_JoystickGetDeviceGUID(n), guid_buf,
                                  sizeof(guid_buf));
        std::cout << std::setw(32) << guid_buf << " " << std::setw(14)
                  << getJoystickTypeName(SDL_JoystickGetDeviceType(n)) << " "
                  << names[n] << std::endl;
    }
}

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

#include "main.h"

std::optional<std::size_t> selectJoyStick(const std::string &id,
                                          std::size_t index) {
    if (id.empty()) {
        if (joystick_num == 1) {
            logger->warn("Joystick index not specified, but only one joystick "
                         "found so assuming index 0.");
            return 0;
        } else {
            logger->error("Specify joystick GUID. Available joysticks can be "
                          "listed with -l option.");
            return std::nullopt;
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
            return std::nullopt;
        } else if (index >= j_indexes.size()) {
            logger->error("Only {} joysticks with the specified GUID found. "
                          "Index {} is out of range.",
                          j_indexes.size(), index);
            return std::nullopt;
        } else {
            return j_indexes[index];
        }
    }
}

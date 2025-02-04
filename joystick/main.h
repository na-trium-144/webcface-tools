#include <memory>
#include <optional>
#include <vector>
#include <spdlog/logger.h>
#include <webcface/member.h>
#define SDL_MAIN_HANDLED
#include <SDL.h>

inline std::shared_ptr<spdlog::logger> logger;
inline int joystick_num;
inline std::vector<int> buttons_state, axes_state, hats_state, balls_state;

void listJoySticks();
std::string getJoystickTypeName(SDL_JoystickType type);
std::optional<std::size_t> selectJoyStick(const std::string &id, std::size_t index);
void readJoystick(const webcface::Member &wcli, SDL_Joystick* joystick);

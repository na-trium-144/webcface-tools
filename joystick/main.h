#include <memory>
#include <optional>
#include <vector>
#include <spdlog/logger.h>
#include <webcface/member.h>
#include <SDL.h>

inline std::shared_ptr<spdlog::logger> logger;
inline int joystick_num;
inline std::vector<int> buttons_state, axes_state, hats_state, balls_state;

void listJoySticks();
constexpr std::size_t len_type_max = 28;
void getVendorProduct(std::ostream &os, int n);
std::string getTypeName(SDL_JoystickType type,
                        SDL_GameControllerType gamecon_type);
std::string getGameControllerTypeName(SDL_GameControllerType type);
std::optional<std::size_t> selectJoyStick(const std::string &id,
                                          std::size_t index);
void readJoystick(const webcface::Member &wcli, SDL_GameController *gamecon,
                  SDL_Joystick *joystick);

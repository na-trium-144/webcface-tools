#include "main.h"
#include <webcface/text.h>
#include <webcface/value.h>

void readJoystick(const webcface::Member &wcli, SDL_GameController *gamecon,
                  SDL_Joystick *joystick) {
    if (gamecon) {
        auto g_buttons = wcli.child("game_buttons");
        for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i++) {
            auto b = static_cast<SDL_GameControllerButton>(i);
            if (SDL_GameControllerHasButton(gamecon, b)) {
                g_buttons.value(SDL_GameControllerGetStringForButton(b)) =
                    SDL_GameControllerGetButton(gamecon, b);
            }
        }
        auto g_axes = wcli.child("game_axes");
        for (int i = 0; i < SDL_CONTROLLER_AXIS_MAX; i++) {
            auto a = static_cast<SDL_GameControllerAxis>(i);
            if (SDL_GameControllerHasAxis(gamecon, a)) {
                g_axes.value(SDL_GameControllerGetStringForAxis(a)) =
                    static_cast<double>(SDL_GameControllerGetAxis(gamecon, a)) /
                    32768;
            }
        }
    }

    if (buttons_state.size() != SDL_JoystickNumButtons(joystick)) {
        buttons_state.resize(SDL_JoystickNumButtons(joystick));
        logger->info("Number of buttons changed: {}", buttons_state.size());
    }
    auto buttons = wcli.value("buttons").resize(0);
    for (int i = 0; i < buttons_state.size(); i++) {
        auto button = SDL_JoystickGetButton(joystick, i);
        if (buttons_state[i] != button) {
            buttons_state[i] = button;
            logger->info("Button {}: {}", i, button);
        }
        buttons.push_back(button);
    }

    if (axes_state.size() != SDL_JoystickNumAxes(joystick)) {
        axes_state.resize(SDL_JoystickNumAxes(joystick));
        logger->info("Number of axes changed: {}", axes_state.size());
    }
    auto axes = wcli.value("axes").resize(0);
    for (int i = 0; i < axes_state.size(); i++) {
        double axis =
            static_cast<double>(SDL_JoystickGetAxis(joystick, i)) / 32768;
        int axis_int = static_cast<int>(axis * 100); // for displaying purpose
        if (axes_state[i] != axis_int) {
            axes_state[i] = axis_int;
            logger->info("Axis {}: {:.2f}", i, axis);
        }
        axes.push_back(axis);
    }

    if (hats_state.size() != SDL_JoystickNumHats(joystick) * 4) {
        hats_state.resize(SDL_JoystickNumHats(joystick) * 4);
        logger->info("Number of hats changed: {}", hats_state.size());
    }
    auto hats = wcli.value("hats").resize(0);
    for (int i = 0; i < hats_state.size(); i += 4) {
        int hat = SDL_JoystickGetHat(joystick, i / 4);
        std::array<int, 4> hat_a = {
            !!(hat & SDL_HAT_UP), !!(hat & SDL_HAT_DOWN),
            !!(hat & SDL_HAT_LEFT), !!(hat & SDL_HAT_RIGHT)};
        for (int j = 0; j < 4; j++) {
            if (hats_state[i + j] != hat_a[j]) {
                hats_state[i + j] = hat_a[j];
                logger->info("Hat {}: {}", i + j, hat_a[j]);
            }
            hats.push_back(hat_a[j]);
        }
    }

    if (balls_state.size() != SDL_JoystickNumBalls(joystick) * 2) {
        balls_state.resize(SDL_JoystickNumBalls(joystick) * 2);
        logger->info("Number of balls changed: {}", balls_state.size());
    }
    auto balls = wcli.value("balls").resize(0);
    for (int i = 0; i < balls_state.size(); i += 2) {
        int dx, dy;
        auto ball = SDL_JoystickGetBall(joystick, i / 2, &dx, &dy);
        if (balls_state[i] != dx) {
            balls_state[i] = dx;
            logger->info("Ball {}: {}", i, dx);
        }
        if (balls_state[i + 1] != dy) {
            balls_state[i + 1] = dy;
            logger->info("Ball {}: {}", i + 1, dy);
        }
        balls.push_back(dx);
        balls.push_back(dy);
    }

    wcli.value("power") = SDL_JoystickCurrentPowerLevel(joystick);
}

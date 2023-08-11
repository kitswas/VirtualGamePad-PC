#pragma once
#ifndef SETTINGS_KEY_VARIABLES_H
#define SETTINGS_KEY_VARIABLES_H
#include <windows.h>
#endif // SETTINGS_KEY_VARIABLES_H

#include <map>
#include "../VGP_Data_Exchange/C/GameButtons.h"

extern int mouse_sensivity;
extern int port;
extern std::map<GamepadButtons, WORD> GAMEPAD_BUTTONS;

enum Thumbstick
{
    LeftThumbstickUp,
    LeftThumbstickDown,
    LeftThumbstickLeft,
    LeftThumbstickRight,
    RightThumbstickUp,
    RightThumbstickDown,
    RightThumbstickLeft,
    RightThumbstickRight
};

extern std::map<Thumbstick, WORD> THUMBSTICK_KEYS;

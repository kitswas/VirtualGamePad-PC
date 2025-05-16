#pragma once

#include "input_types.hpp"

#include <QList>
#include <QMap>
#include <QString>
#include <QVariant>
#include <map>
#include <windows.h>

extern std::map<GamepadButtons, ButtonInput> GAMEPAD_BUTTONS;
extern std::map<Thumbstick, ThumbstickInput> THUMBSTICK_INPUTS;
extern std::map<WORD, const char *> vk_maps;
extern const QList<WORD> MOUSE_BUTTONS;
bool is_mouse_button(WORD vk);

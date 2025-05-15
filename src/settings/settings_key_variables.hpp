#pragma once

#include "input_types.hpp"
#include <QList>
#include <QMap>
#include <QString>
#include <QVariant>
#include <map>
#include <windows.h>

extern std::map<GamepadButtons, Input> GAMEPAD_BUTTONS;
extern std::map<Thumbstick, WORD> THUMBSTICK_KEYS;
extern std::map<WORD, const char *> vk_maps;
extern const QList<UINT> MOUSE_BUTTONS;
uint is_mouse_button(UINT vk);

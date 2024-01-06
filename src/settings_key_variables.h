#pragma once
#ifndef SETTINGS_KEY_VARIABLES_H
#define SETTINGS_KEY_VARIABLES_H
#include <QList>
#include <windows.h>

#include "../VGP_Data_Exchange/C/GameButtons.h"
#include <map>

/**
 * A structure to hold the button mappings.
 */
struct Input
{
	WORD vk{};
	uint is_mouse_key = 0;
};

extern int mouse_sensitivity;
extern int port;
extern std::map<GamepadButtons, Input> GAMEPAD_BUTTONS;

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
extern std::map<WORD, const char *> vk_maps;

extern const QList<UINT> MOUSE_BUTTONS;

#endif // SETTINGS_KEY_VARIABLES_H

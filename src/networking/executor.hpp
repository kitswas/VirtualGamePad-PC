#pragma once

#include "../../VGP_Data_Exchange/C/Colfer.h"
#include "../../VGP_Data_Exchange/C/GameButtons.h"
#include "../simulation/simulate.hpp"

#include <map>

inline int mouse_sensivity =  1000;

vgp_data_exchange_gamepad_reading parse_gamepad_state(const char *data, size_t len);
bool inject_gamepad_state(vgp_data_exchange_gamepad_reading reading);

// Map of gamepad buttons to keyboard keys
const std::map<GamepadButtons, WORD> GAMEPAD_BUTTONS = {{GamepadButtons::GamepadButtons_Menu, VK_MENU},
														{GamepadButtons::GamepadButtons_View, VK_TAB},
														{GamepadButtons::GamepadButtons_A, VK_RETURN},
														{GamepadButtons::GamepadButtons_B, VK_ESCAPE},
														{GamepadButtons::GamepadButtons_X, VK_SHIFT},
														{GamepadButtons::GamepadButtons_Y, VK_CONTROL},
														{GamepadButtons::GamepadButtons_DPadUp, VK_UP},
														{GamepadButtons::GamepadButtons_DPadDown, VK_DOWN},
														{GamepadButtons::GamepadButtons_DPadLeft, VK_LEFT},
														{GamepadButtons::GamepadButtons_DPadRight, VK_RIGHT},
														{GamepadButtons::GamepadButtons_LeftShoulder, 'E'},
														{GamepadButtons::GamepadButtons_RightShoulder, VK_SPACE},
														{GamepadButtons::GamepadButtons_LeftThumbstick, VK_PRIOR},
														{GamepadButtons::GamepadButtons_RightThumbstick, VK_NEXT}};

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

const std::map<Thumbstick, WORD> THUMBSTICK_KEYS = {
	{Thumbstick::LeftThumbstickUp, 'W'},		{Thumbstick::LeftThumbstickDown, 'S'},
	{Thumbstick::LeftThumbstickLeft, 'A'},		{Thumbstick::LeftThumbstickRight, 'D'},
	{Thumbstick::RightThumbstickUp, VK_UP},		{Thumbstick::RightThumbstickDown, VK_DOWN},
	{Thumbstick::RightThumbstickLeft, VK_LEFT}, {Thumbstick::RightThumbstickRight, VK_RIGHT}};

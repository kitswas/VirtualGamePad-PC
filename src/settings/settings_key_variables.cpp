#include "settings_key_variables.hpp"
#include "input_types.hpp"
#include "settings_singleton.hpp"

std::map<GamepadButtons, Input> GAMEPAD_BUTTONS = {
	{GamepadButtons::GamepadButtons_Menu, Input{VK_MENU, 0}},
	{GamepadButtons::GamepadButtons_View, Input{VK_TAB, 0}},
	{GamepadButtons::GamepadButtons_A, Input{VK_RETURN, 0}},
	{GamepadButtons::GamepadButtons_B, Input{'B', 0}},
	{GamepadButtons::GamepadButtons_X, Input{VK_SHIFT, 0}},
	{GamepadButtons::GamepadButtons_Y, Input{VK_CONTROL, 0}},
	{GamepadButtons::GamepadButtons_DPadUp, Input{VK_UP, 0}},
	{GamepadButtons::GamepadButtons_DPadDown, Input{VK_DOWN, 0}},
	{GamepadButtons::GamepadButtons_DPadLeft, Input{VK_LEFT, 0}},
	{GamepadButtons::GamepadButtons_DPadRight, Input{VK_RIGHT, 0}},
	{GamepadButtons::GamepadButtons_LeftShoulder, Input{VK_LBUTTON, 1}},
	{GamepadButtons::GamepadButtons_RightShoulder, Input{VK_NEXT, 0}}};

std::map<Thumbstick, WORD> THUMBSTICK_KEYS = {
	{Thumbstick::LeftThumbstickUp, 'W'},		{Thumbstick::LeftThumbstickDown, 'S'},
	{Thumbstick::LeftThumbstickLeft, 'A'},		{Thumbstick::LeftThumbstickRight, 'D'},
	{Thumbstick::RightThumbstickUp, VK_UP},		{Thumbstick::RightThumbstickDown, VK_DOWN},
	{Thumbstick::RightThumbstickLeft, VK_LEFT}, {Thumbstick::RightThumbstickRight, VK_RIGHT}};

std::map<WORD, const char *> vk_maps = {{VK_LBUTTON, "LMButton"},
										{VK_RBUTTON, "RMButton"},
										{VK_MBUTTON, "MMButton"},
										{VK_BACK, "BACKSPACE"},
										{VK_TAB, "TAB"},
										{VK_RETURN, "ENTER"},
										{VK_SHIFT, "SHIFT"},
										{VK_CONTROL, "CTRL"},
										{VK_CAPITAL, "CAPITAL"},
										{VK_ESCAPE, "ESCAPE"},
										{VK_SPACE, "SPACE"},
										{VK_PRIOR, "PageUP"},
										{VK_NEXT, "PageDOWN"},
										{VK_END, "END"},
										{VK_HOME, "HOME"},
										{VK_LEFT, "LEFT"},
										{VK_UP, "UP"},
										{VK_RIGHT, "RIGHT"},
										{VK_DOWN, "DOWN"},
										{VK_INSERT, "INS"},
										{VK_DELETE, "DEL"},
										{VK_OEM_PERIOD, "."},
										{VK_OEM_COMMA, ","},
										{VK_OEM_MINUS, "-"},
										{VK_OEM_PLUS, "+"},
										{VK_MENU, "MENU"}};

const QList<UINT> MOUSE_BUTTONS = {VK_LBUTTON, VK_RBUTTON, VK_MBUTTON};

uint is_mouse_button(UINT vk)
{
	for (uint i = 0; i < 3; ++i)
	{
		if (MOUSE_BUTTONS[i] == vk)
			return 1;
	}
	return 0;
}

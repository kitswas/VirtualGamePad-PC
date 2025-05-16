#include "settings_key_variables.hpp"

#include "input_types.hpp"
#include "settings_singleton.hpp"

std::map<GamepadButtons, ButtonInput> GAMEPAD_BUTTONS = {
	{GamepadButtons::GamepadButtons_Menu, ButtonInput{VK_MENU, false}},
	{GamepadButtons::GamepadButtons_View, ButtonInput{VK_TAB, false}},
	{GamepadButtons::GamepadButtons_A, ButtonInput{VK_RETURN, false}},
	{GamepadButtons::GamepadButtons_B, ButtonInput{VK_ESCAPE, false}},
	{GamepadButtons::GamepadButtons_X, ButtonInput{VK_SHIFT, false}},
	{GamepadButtons::GamepadButtons_Y, ButtonInput{VK_CONTROL, false}},
	{GamepadButtons::GamepadButtons_DPadUp, ButtonInput{VK_UP, false}},
	{GamepadButtons::GamepadButtons_DPadDown, ButtonInput{VK_DOWN, false}},
	{GamepadButtons::GamepadButtons_DPadLeft, ButtonInput{VK_LEFT, false}},
	{GamepadButtons::GamepadButtons_DPadRight, ButtonInput{VK_RIGHT, false}},
	{GamepadButtons::GamepadButtons_LeftShoulder, ButtonInput{VK_LBUTTON, true}},
	{GamepadButtons::GamepadButtons_RightShoulder, ButtonInput{VK_RBUTTON, true}}};

std::map<Thumbstick, ThumbstickInput> THUMBSTICK_INPUTS = {
	{Thumbstick_Left, {false, {'W', false}, {'S', false}, {'A', false}, {'D', false}}},
	{Thumbstick_Right,
	 {false, {VK_UP, false}, {VK_DOWN, false}, {VK_LEFT, false}, {VK_RIGHT, false}}}};

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

const QList<WORD> MOUSE_BUTTONS = {VK_LBUTTON, VK_RBUTTON, VK_MBUTTON};

bool is_mouse_button(WORD vk)
{
	for (uint i = 0; i < 3; ++i)
	{
		if (MOUSE_BUTTONS[i] == vk)
			return true;
	}
	return false;
}

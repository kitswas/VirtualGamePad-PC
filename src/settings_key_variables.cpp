#include "settings_key_variables.h"

int mouse_sensivity = 1000; // the mouse_sensivity or the cursor speed.
int port = 7878;			// the port on which the server runs on.

/**
 * @brief GAMEPAD_BUTTONS
 * A map for the gamepad button and the corresponding input.
 */
std::map<GamepadButtons, TRIAL> GAMEPAD_BUTTONS = {{GamepadButtons::GamepadButtons_Menu, TRIAL{VK_MENU, 0}},
												   {GamepadButtons::GamepadButtons_View, TRIAL{VK_TAB, 0}},
												   {GamepadButtons::GamepadButtons_A, TRIAL{VK_RETURN, 0}},
												   {GamepadButtons::GamepadButtons_B, TRIAL{'B', 0}},
												   {GamepadButtons::GamepadButtons_X, TRIAL{VK_SHIFT, 0}},
												   {GamepadButtons::GamepadButtons_Y, TRIAL{VK_CONTROL, 0}},
												   {GamepadButtons::GamepadButtons_DPadUp, TRIAL{VK_UP, 0}},
												   {GamepadButtons::GamepadButtons_DPadDown, TRIAL{VK_DOWN, 0}},
												   {GamepadButtons::GamepadButtons_DPadLeft, TRIAL{VK_LEFT, 0}},
												   {GamepadButtons::GamepadButtons_DPadRight, TRIAL{VK_RIGHT, 0}},
												   {GamepadButtons::GamepadButtons_LeftShoulder, TRIAL{VK_LBUTTON, 1}},
												   {GamepadButtons::GamepadButtons_RightShoulder, TRIAL{VK_NEXT, 0}}};
// {GamepadButtons::GamepadButtons_LeftThumbstick, TRIAL{VK_LBUTTON, 1}},
// {GamepadButtons::GamepadButtons_RightThumbstick, TRIAL{VK_NEXT, 0}}};

/**
 * @brief THUMBSTICK_KEYS
 * Maps the joystick movements.
 */
std::map<Thumbstick, WORD> THUMBSTICK_KEYS = {
	{Thumbstick::LeftThumbstickUp, 'W'},		{Thumbstick::LeftThumbstickDown, 'S'},
	{Thumbstick::LeftThumbstickLeft, 'A'},		{Thumbstick::LeftThumbstickRight, 'D'},
	{Thumbstick::RightThumbstickUp, VK_UP},		{Thumbstick::RightThumbstickDown, VK_DOWN},
	{Thumbstick::RightThumbstickLeft, VK_LEFT}, {Thumbstick::RightThumbstickRight, VK_RIGHT}};

/**
 * @brief vk_maps
 * A std::map to map the virtual key codes to corresponding key names
 */
std::map<UINT, const char *> vk_maps = {{VK_LBUTTON, "LMButton"},
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

/**
 * @brief MOUSE_BUTTONS
 * A list containing the Mouse Buttons.
 */
const QList<UINT> MOUSE_BUTTONS = {VK_LBUTTON, VK_RBUTTON, VK_MBUTTON};

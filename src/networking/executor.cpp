#include "executor.hpp"

#include "../settings/input_types.hpp"
#include "../settings/settings_singleton.hpp"
#include "../simulation/keyboardSim.hpp"
#include "../simulation/mouseSim.hpp"

#include <QApplication>
#include <algorithm>
#include <cmath>
#include <vector>

constexpr double THRESHOLD = 0.5;

vgp_data_exchange_gamepad_reading parse_gamepad_state(const char *data, size_t len)
{
	vgp_data_exchange_gamepad_reading reading;
	reading.buttons_up = 0;
	reading.buttons_down = 0;
	reading.left_trigger = 0;
	reading.right_trigger = 0;
	reading.left_thumbstick_x = 0;
	reading.left_thumbstick_y = 0;
	reading.right_thumbstick_x = 0;
	reading.right_thumbstick_y = 0;

	// Deserialize the data
	size_t decoded_octects = vgp_data_exchange_gamepad_reading_unmarshal(&reading, data, len);
	if (decoded_octects == 0)
	{
		qWarning() << "Failed to deserialize data";
		return reading;
	}

#ifdef QT_DEBUG
	// Log the gamepad state
	qDebug() << "Gamepad state:"
			 << "\nButtons up: " << reading.buttons_up << "\nButtons down: " << reading.buttons_down
			 << "\nLeft trigger: " << reading.left_trigger
			 << "\nRight trigger: " << reading.right_trigger
			 << "\nLeft thumbstick x: " << reading.left_thumbstick_x
			 << "\nLeft thumbstick y: " << reading.left_thumbstick_y
			 << "\nRight thumbstick x: " << reading.right_thumbstick_x
			 << "\nRight thumbstick y: " << reading.right_thumbstick_y;
#endif

	return reading;
}

// Helper functions to handle mouse button input

static void handleButtonDown(const ButtonInput &buttonInput)
{
	if (buttonInput.is_mouse_button)
	{
		if (buttonInput.vk == VK_LBUTTON)
			leftDown();
		else if (buttonInput.vk == VK_RBUTTON)
			rightDown();
		else if (buttonInput.vk == VK_MBUTTON)
			middleDown();
	}
	else
		keyDown(buttonInput.vk);
}

static void handleButtonUp(const ButtonInput &buttonInput)
{
	if (buttonInput.is_mouse_button)
	{
		if (buttonInput.vk == VK_LBUTTON)
			leftUp();
		else if (buttonInput.vk == VK_RBUTTON)
			rightUp();
		else if (buttonInput.vk == VK_MBUTTON)
			middleUp();
	}
	else
		keyUp(buttonInput.vk);
}

// Helper function to handle thumbstick input
void handleThumbstickInput(const ThumbstickInput &thumbstick, float x_value, float y_value,
						   double threshold)
{
	if (thumbstick.is_mouse_move)
	{
		// Mouse movement code
		int offsetX = x_value * SettingsSingleton::instance().mouseSensitivity();
		int offsetY = y_value * SettingsSingleton::instance().mouseSensitivity();
		int scaleX =
			abs(offsetX) < (threshold * SettingsSingleton::instance().mouseSensitivity()) ? 0 : 1;
		int scaleY =
			abs(offsetY) < (threshold * SettingsSingleton::instance().mouseSensitivity()) ? 0 : 1;

		for (int count = 1; count <= std::max(abs(offsetX), abs(offsetY)); ++count)
		{
			int stepX = std::copysign(scaleX, offsetX);
			int stepY = std::copysign(scaleY, offsetY);
			moveMouseByOffset(stepX, stepY);
		}
	}
	else
	{
		// Direction handling
		if (x_value > threshold)
			handleButtonDown(thumbstick.right);
		else if (x_value < -threshold)
			handleButtonDown(thumbstick.left);
		else
		{
			handleButtonUp(thumbstick.right);
			handleButtonUp(thumbstick.left);
		}

		if (y_value > threshold)
			handleButtonDown(thumbstick.down);
		else if (y_value < -threshold)
			handleButtonDown(thumbstick.up);
		else
		{
			handleButtonUp(thumbstick.down);
			handleButtonUp(thumbstick.up);
		}
	}
}

bool inject_gamepad_state(vgp_data_exchange_gamepad_reading const &reading)
{
	// Handle button input using active keymap profile
	const auto &profile = SettingsSingleton::instance().activeKeymapProfile();
	static const std::vector<GamepadButtons> buttons = {GamepadButtons_Menu,
														GamepadButtons_View,
														GamepadButtons_A,
														GamepadButtons_B,
														GamepadButtons_X,
														GamepadButtons_Y,
														GamepadButtons_DPadUp,
														GamepadButtons_DPadDown,
														GamepadButtons_DPadLeft,
														GamepadButtons_DPadRight,
														GamepadButtons_LeftShoulder,
														GamepadButtons_RightShoulder};
	for (auto button : buttons)
	{
		WORD vk = profile.buttonMap(button);
		if (vk == 0)
			continue;
		ButtonInput input{vk, is_mouse_button(vk)};
		if (reading.buttons_down & button)
			handleButtonDown(input);
		if (reading.buttons_up & button)
			handleButtonUp(input);
	}

	handleThumbstickInput(
		SettingsSingleton::instance().activeKeymapProfile().thumbstickInput(Thumbstick_Left),
		reading.left_thumbstick_x, reading.left_thumbstick_y, THRESHOLD);

	handleThumbstickInput(
		SettingsSingleton::instance().activeKeymapProfile().thumbstickInput(Thumbstick_Right),
		reading.right_thumbstick_x, reading.right_thumbstick_y, THRESHOLD);

	return true;
}

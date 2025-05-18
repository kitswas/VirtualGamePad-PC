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

	// Log the gamepad state
	qDebug() << "Gamepad state:"
			 << "\nButtons up: " << reading.buttons_up << "\nButtons down: " << reading.buttons_down
			 << "\nLeft trigger: " << reading.left_trigger
			 << "\nRight trigger: " << reading.right_trigger
			 << "\nLeft thumbstick x: " << reading.left_thumbstick_x
			 << "\nLeft thumbstick y: " << reading.left_thumbstick_y
			 << "\nRight thumbstick x: " << reading.right_thumbstick_x
			 << "\nRight thumbstick y: " << reading.right_thumbstick_y;

	return reading;
}

// Helper function to handle mouse button input
void handleMouseButtonInput(WORD vk)
{
	if (vk == VK_LBUTTON)
		leftClick();
	else if (vk == VK_RBUTTON)
		rightClick();
	else if (vk == VK_MBUTTON)
		middleClick();
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
		{
			if (thumbstick.right.is_mouse_button)
				handleMouseButtonInput(thumbstick.right.vk);
			else
				keyDown(thumbstick.right.vk);
		}
		else if (x_value < -threshold)
		{
			if (thumbstick.left.is_mouse_button)
				handleMouseButtonInput(thumbstick.left.vk);
			else
				keyDown(thumbstick.left.vk);
		}
		else
		{
			if (!thumbstick.right.is_mouse_button)
				keyUp(thumbstick.right.vk);
			if (!thumbstick.left.is_mouse_button)
				keyUp(thumbstick.left.vk);
		}

		if (y_value > threshold)
		{
			if (thumbstick.down.is_mouse_button)
				handleMouseButtonInput(thumbstick.down.vk);
			else
				keyDown(thumbstick.down.vk);
		}
		else if (y_value < -threshold)
		{
			if (thumbstick.up.is_mouse_button)
				handleMouseButtonInput(thumbstick.up.vk);
			else
				keyDown(thumbstick.up.vk);
		}
		else
		{
			if (!thumbstick.down.is_mouse_button)
				keyUp(thumbstick.down.vk);
			if (!thumbstick.up.is_mouse_button)
				keyUp(thumbstick.up.vk);
		}
	}
}

bool inject_gamepad_state(vgp_data_exchange_gamepad_reading reading)
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
		{
			if (input.is_mouse_button)
			{
				handleMouseButtonInput(vk);
			}
			else
			{
				keyDown(vk);
			}
		}
		if (reading.buttons_up & button)
		{
			if (!input.is_mouse_button)
				keyUp(vk);
		}
	}

	handleThumbstickInput(
		SettingsSingleton::instance().activeKeymapProfile().thumbstickInput(Thumbstick_Left),
		reading.left_thumbstick_x, reading.left_thumbstick_y, THRESHOLD);

	handleThumbstickInput(
		SettingsSingleton::instance().activeKeymapProfile().thumbstickInput(Thumbstick_Right),
		reading.right_thumbstick_x, reading.right_thumbstick_y, THRESHOLD);

	return true;
}

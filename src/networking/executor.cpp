#include "executor.hpp"

#include "../settings/input_types.hpp"
#include "../settings/settings_key_variables.hpp"
#include "../settings/settings_singleton.hpp"
#include "../simulation/keyboardSim.hpp"
#include "../simulation/mouseSim.hpp"

#include <QApplication>

constexpr double THRESHOLD = 0.5;
constexpr int MOUSE_MOVE_SCALE = 10; // Tune as needed
constexpr int SCROLL_AMOUNT = 1;	 // Tune as needed

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

bool inject_gamepad_state(vgp_data_exchange_gamepad_reading reading)
{
	// Handle button input
	for (auto const &[button, input] : GAMEPAD_BUTTONS)
	{
		if (reading.buttons_down & button)
		{
			if (input.is_mouse_button)
			{
				if (input.vk == VK_LBUTTON)
				{
					leftClick();
				}
				else if (input.vk == VK_RBUTTON)
				{
					rightClick();
				}
				else
				{
					middleClick();
				}
			}
			else
			{
				keyDown(input.vk);
			}
		}
		if (reading.buttons_up & button)
		{
			if (!input.is_mouse_button) // Only use keyUp for keyboard keys
			{
				keyUp(input.vk);
			}
		}
	}

	// Handle left thumbstick
	const auto &leftThumbstick = THUMBSTICK_INPUTS[Thumbstick_Left];
	if (leftThumbstick.is_mouse_move)
	{
		// Left thumbstick is configured for mouse movement
		int offsetX = reading.left_thumbstick_x * SettingsSingleton::instance().mouseSensitivity();
		int offsetY = reading.left_thumbstick_y * SettingsSingleton::instance().mouseSensitivity();
		int scaleX =
			abs(offsetX) < (THRESHOLD * SettingsSingleton::instance().mouseSensitivity()) ? 0 : 1;
		int scaleY =
			abs(offsetY) < (THRESHOLD * SettingsSingleton::instance().mouseSensitivity()) ? 0 : 1;

		for (int count = 1; count <= std::max(abs(offsetX), abs(offsetY)); ++count)
		{
			int stepX = std::copysign(scaleX, offsetX);
			int stepY = std::copysign(scaleY, offsetY);
			moveMouseByOffset(stepX, stepY);
		}
	}
	else
	{
		// Left thumbstick is configured for keyboard input
		if (reading.left_thumbstick_x > THRESHOLD)
		{
			if (leftThumbstick.right.is_mouse_button)
			{
				if (leftThumbstick.right.vk == VK_LBUTTON)
					leftClick();
				else if (leftThumbstick.right.vk == VK_RBUTTON)
					rightClick();
				else if (leftThumbstick.right.vk == VK_MBUTTON)
					middleClick();
			}
			else
				keyDown(leftThumbstick.right.vk);
		}
		else if (reading.left_thumbstick_x < -THRESHOLD)
		{
			if (leftThumbstick.left.is_mouse_button)
			{
				if (leftThumbstick.left.vk == VK_LBUTTON)
					leftClick();
				else if (leftThumbstick.left.vk == VK_RBUTTON)
					rightClick();
				else if (leftThumbstick.left.vk == VK_MBUTTON)
					middleClick();
			}
			else
				keyDown(leftThumbstick.left.vk);
		}
		else
		{
			if (!leftThumbstick.right.is_mouse_button)
				keyUp(leftThumbstick.right.vk);
			if (!leftThumbstick.left.is_mouse_button)
				keyUp(leftThumbstick.left.vk);
		}

		if (reading.left_thumbstick_y > THRESHOLD)
		{
			if (leftThumbstick.down.is_mouse_button)
			{
				if (leftThumbstick.down.vk == VK_LBUTTON)
					leftClick();
				else if (leftThumbstick.down.vk == VK_RBUTTON)
					rightClick();
				else if (leftThumbstick.down.vk == VK_MBUTTON)
					middleClick();
			}
			else
				keyDown(leftThumbstick.down.vk);
		}
		else if (reading.left_thumbstick_y < -THRESHOLD)
		{
			if (leftThumbstick.up.is_mouse_button)
			{
				if (leftThumbstick.up.vk == VK_LBUTTON)
					leftClick();
				else if (leftThumbstick.up.vk == VK_RBUTTON)
					rightClick();
				else if (leftThumbstick.up.vk == VK_MBUTTON)
					middleClick();
			}
			else
				keyDown(leftThumbstick.up.vk);
		}
		else
		{
			if (!leftThumbstick.down.is_mouse_button)
				keyUp(leftThumbstick.down.vk);
			if (!leftThumbstick.up.is_mouse_button)
				keyUp(leftThumbstick.up.vk);
		}
	}

	// Handle right thumbstick
	const auto &rightThumbstick = THUMBSTICK_INPUTS[Thumbstick_Right];
	if (rightThumbstick.is_mouse_move)
	{
		// Right thumbstick is configured for mouse movement
		int offsetX = reading.right_thumbstick_x * SettingsSingleton::instance().mouseSensitivity();
		int offsetY = reading.right_thumbstick_y * SettingsSingleton::instance().mouseSensitivity();
		int scaleX =
			abs(offsetX) < (THRESHOLD * SettingsSingleton::instance().mouseSensitivity()) ? 0 : 1;
		int scaleY =
			abs(offsetY) < (THRESHOLD * SettingsSingleton::instance().mouseSensitivity()) ? 0 : 1;

		for (int count = 1; count <= std::max(abs(offsetX), abs(offsetY)); ++count)
		{
			int stepX = std::copysign(scaleX, offsetX);
			int stepY = std::copysign(scaleY, offsetY);
			moveMouseByOffset(stepX, stepY);
		}
	}
	else
	{
		// Right thumbstick is configured for keyboard input
		if (reading.right_thumbstick_x > THRESHOLD)
		{
			if (rightThumbstick.right.is_mouse_button)
			{
				if (rightThumbstick.right.vk == VK_LBUTTON)
					leftClick();
				else if (rightThumbstick.right.vk == VK_RBUTTON)
					rightClick();
				else if (rightThumbstick.right.vk == VK_MBUTTON)
					middleClick();
			}
			else
				keyDown(rightThumbstick.right.vk);
		}
		else if (reading.right_thumbstick_x < -THRESHOLD)
		{
			if (rightThumbstick.left.is_mouse_button)
			{
				if (rightThumbstick.left.vk == VK_LBUTTON)
					leftClick();
				else if (rightThumbstick.left.vk == VK_RBUTTON)
					rightClick();
				else if (rightThumbstick.left.vk == VK_MBUTTON)
					middleClick();
			}
			else
				keyDown(rightThumbstick.left.vk);
		}
		else
		{
			if (!rightThumbstick.right.is_mouse_button)
				keyUp(rightThumbstick.right.vk);
			if (!rightThumbstick.left.is_mouse_button)
				keyUp(rightThumbstick.left.vk);
		}

		if (reading.right_thumbstick_y > THRESHOLD)
		{
			if (rightThumbstick.down.is_mouse_button)
			{
				if (rightThumbstick.down.vk == VK_LBUTTON)
					leftClick();
				else if (rightThumbstick.down.vk == VK_RBUTTON)
					rightClick();
				else if (rightThumbstick.down.vk == VK_MBUTTON)
					middleClick();
			}
			else
				keyDown(rightThumbstick.down.vk);
		}
		else if (reading.right_thumbstick_y < -THRESHOLD)
		{
			if (rightThumbstick.up.is_mouse_button)
			{
				if (rightThumbstick.up.vk == VK_LBUTTON)
					leftClick();
				else if (rightThumbstick.up.vk == VK_RBUTTON)
					rightClick();
				else if (rightThumbstick.up.vk == VK_MBUTTON)
					middleClick();
			}
			else
				keyDown(rightThumbstick.up.vk);
		}
		else
		{
			if (!rightThumbstick.down.is_mouse_button)
				keyUp(rightThumbstick.down.vk);
			if (!rightThumbstick.up.is_mouse_button)
				keyUp(rightThumbstick.up.vk);
		}
	}

	return true;
}

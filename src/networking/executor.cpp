#include "executor.hpp"
#include "../settings/input_types.hpp"
#include "../settings/settings_key_variables.hpp"
#include "../settings/settings_singleton.hpp"

#include <QApplication>

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

bool inject_gamepad_state(vgp_data_exchange_gamepad_reading reading)
{
	for (auto const &[button, key] : GAMEPAD_BUTTONS)
	{
		if (reading.buttons_down & button)
		{
			if (key.is_mouse_key)
			{ // checking if the input key is a mouse key.
				if (key.vk == VK_LBUTTON)
				{ // if it's a left mouse click execute a left click
					leftClick();
				}
				else if (key.vk == VK_RBUTTON)
				{ // if it's a right mouse click execute a right mouse
					rightClick();
				}

				else
				{ // else execute a middle mouse click.
					middleClick();
				}
			}
			else
				keyDown(key.vk);
		}
		if (reading.buttons_up & button)
		{
			keyUp(key.vk);
		}
	}

	// Handle thumbstick input
	if (reading.left_thumbstick_x > THRESHOLD)
	{
		keyDown(THUMBSTICK_KEYS.at(Thumbstick::LeftThumbstickRight));
	}
	else if (reading.left_thumbstick_x < -THRESHOLD)
	{
		keyDown(THUMBSTICK_KEYS.at(Thumbstick::LeftThumbstickLeft));
	}
	else
	{
		keyUp(THUMBSTICK_KEYS.at(Thumbstick::LeftThumbstickRight));
		keyUp(THUMBSTICK_KEYS.at(Thumbstick::LeftThumbstickLeft));
	}
	if (reading.left_thumbstick_y > THRESHOLD)
	{
		keyDown(THUMBSTICK_KEYS.at(Thumbstick::LeftThumbstickDown));
	}
	else if (reading.left_thumbstick_y < -THRESHOLD)
	{
		keyDown(THUMBSTICK_KEYS.at(Thumbstick::LeftThumbstickUp));
	}
	else
	{
		keyUp(THUMBSTICK_KEYS.at(Thumbstick::LeftThumbstickDown));
		keyUp(THUMBSTICK_KEYS.at(Thumbstick::LeftThumbstickUp));
	}

	// Use the right thumbstick to move the mouse
	int offsetX = reading.right_thumbstick_x * SettingsSingleton::instance().mouseSensitivity();
	int offsetY = reading.right_thumbstick_y * SettingsSingleton::instance().mouseSensitivity();
	int scaleX =
		abs(offsetX) < (THRESHOLD * SettingsSingleton::instance().mouseSensitivity()) ? 0 : 1;
	int scaleY =
		abs(offsetY) < (THRESHOLD * SettingsSingleton::instance().mouseSensitivity()) ? 0 : 1;

	qDebug() << "Moving mouse by" << offsetX << ", " << offsetY;
	for (int count = 1; count <= std::max(abs(offsetX), abs(offsetY)); ++count)
	{
		int stepX = std::copysign(scaleX, offsetX);
		int stepY = std::copysign(scaleY, offsetY);
		moveMouseByOffset(stepX, stepY);
	}

	return false;
}

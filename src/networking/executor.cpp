#include "executor.hpp"

#include <QApplication>

#define THRESHOLD 0.5

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
		qDebug() << "Failed to deserialize data";
		return reading;
	}

	return reading;
}

bool inject_gamepad_state(vgp_data_exchange_gamepad_reading reading)
{
	for (auto const &[button, key] : GAMEPAD_BUTTONS)
	{
		if (reading.buttons_down & button)
		{
			keyDown(key);
		}
		else if (reading.buttons_up & button)
		{
			keyUp(key);
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

	if (reading.right_thumbstick_x > THRESHOLD)
	{
		keyDown(THUMBSTICK_KEYS.at(Thumbstick::RightThumbstickRight));
	}
	else if (reading.right_thumbstick_x < -THRESHOLD)
	{
		keyDown(THUMBSTICK_KEYS.at(Thumbstick::RightThumbstickLeft));
	}
	else
	{
		keyUp(THUMBSTICK_KEYS.at(Thumbstick::RightThumbstickRight));
		keyUp(THUMBSTICK_KEYS.at(Thumbstick::RightThumbstickLeft));
	}
	if (reading.right_thumbstick_y > THRESHOLD)
	{
		keyDown(THUMBSTICK_KEYS.at(Thumbstick::RightThumbstickDown));
	}
	else if (reading.right_thumbstick_y < -THRESHOLD)
	{
		keyDown(THUMBSTICK_KEYS.at(Thumbstick::RightThumbstickUp));
	}
	else
	{
		keyUp(THUMBSTICK_KEYS.at(Thumbstick::RightThumbstickDown));
		keyUp(THUMBSTICK_KEYS.at(Thumbstick::RightThumbstickUp));
	}

	return false;
}

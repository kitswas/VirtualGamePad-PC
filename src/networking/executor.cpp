#include "executor.hpp"

#include <QApplication>

#define THRESHOLD 0.5
#define MOUSE_SENSITIVITY 10

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

	// Use the right thumbstick to move the mouse
	// if (abs(reading.right_thumbstick_x) > THRESHOLD || abs(reading.right_thumbstick_y) > THRESHOLD)
	moveMouseByOffset(reading.right_thumbstick_x * MOUSE_SENSITIVITY, reading.right_thumbstick_y * MOUSE_SENSITIVITY);

	return false;
}

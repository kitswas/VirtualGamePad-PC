#include "executor.hpp"

#include "../settings/input_types.hpp"
#include "../settings/settings_singleton.hpp"
#include "../simulation/keyboardSim.hpp"
#include "../simulation/mouseSim.hpp"

#include <QApplication>
#include <algorithm>
#include <cmath>
#include <errno.h>
#include <sstream>
#include <string>
#include <vector>

constexpr double THRESHOLD = 0.5;

// Helper function to convert button flags to readable names
std::string getButtonNames(uint32_t buttons)
{
	std::vector<std::string> buttonNames;

	if (buttons & GamepadButtons_Menu)
		buttonNames.emplace_back("Menu");
	if (buttons & GamepadButtons_View)
		buttonNames.emplace_back("View");
	if (buttons & GamepadButtons_A)
		buttonNames.emplace_back("A");
	if (buttons & GamepadButtons_B)
		buttonNames.emplace_back("B");
	if (buttons & GamepadButtons_X)
		buttonNames.emplace_back("X");
	if (buttons & GamepadButtons_Y)
		buttonNames.emplace_back("Y");
	if (buttons & GamepadButtons_DPadUp)
		buttonNames.emplace_back("DPadUp");
	if (buttons & GamepadButtons_DPadDown)
		buttonNames.emplace_back("DPadDown");
	if (buttons & GamepadButtons_DPadLeft)
		buttonNames.emplace_back("DPadLeft");
	if (buttons & GamepadButtons_DPadRight)
		buttonNames.emplace_back("DPadRight");
	if (buttons & GamepadButtons_LeftShoulder)
		buttonNames.emplace_back("LeftShoulder");
	if (buttons & GamepadButtons_RightShoulder)
		buttonNames.emplace_back("RightShoulder");
	if (buttons & GamepadButtons_LeftThumbstick)
		buttonNames.emplace_back("LeftThumbstick");
	if (buttons & GamepadButtons_RightThumbstick)
		buttonNames.emplace_back("RightThumbstick");

	if (buttonNames.empty())
	{
		return "None";
	}

	std::ostringstream oss;
	for (size_t i = 0; i < buttonNames.size(); ++i)
	{
		if (i > 0)
			oss << " | ";
		oss << buttonNames[i];
	}

	return oss.str();
}

ParseResult parse_gamepad_state(const char *data, size_t len)
{
	ParseResult result;
	result.reading.buttons_up = 0;
	result.reading.buttons_down = 0;
	result.reading.left_trigger = 0;
	result.reading.right_trigger = 0;
	result.reading.left_thumbstick_x = 0;
	result.reading.left_thumbstick_y = 0;
	result.reading.right_thumbstick_x = 0;
	result.reading.right_thumbstick_y = 0;
	result.bytes_consumed = 0;
	result.success = false;

	// Deserialize the data
	size_t decoded_octects =
		vgp_data_exchange_gamepad_reading_unmarshal(&result.reading, data, len);

	// When the return is zero then errno is set to one of the following 3 values:
	// EWOULDBLOCK on incomplete data, EFBIG on a breach of either colfer_size_max
	// or colfer_list_max and EILSEQ on schema mismatch.
	if (decoded_octects == 0)
	{
		if (errno == EWOULDBLOCK)
		{
			result.failure_reason = ParseResult::FailureReason::IncompleteData;
		}
		else if (errno == EFBIG)
		{
			result.failure_reason = ParseResult::FailureReason::DataTooLarge;
			qWarning() << "Data too large to process";
		}
		else if (errno == EILSEQ)
		{
			result.failure_reason = ParseResult::FailureReason::SchemaMismatch;
			qWarning() << "Schema mismatch detected";
		}
		else
		{
			qWarning() << "Unknown error occurred during deserialization";
		}
		return result;
	}

	result.bytes_consumed = decoded_octects;
	result.success = true;

#ifdef QT_DEBUG
	// Log the gamepad state
	qDebug() << "Gamepad state:"
			 << "\nButtons up: " << getButtonNames(result.reading.buttons_up).c_str()
			 << "\nButtons down: " << getButtonNames(result.reading.buttons_down).c_str()
			 << "\nLeft trigger: " << result.reading.left_trigger
			 << "\nRight trigger: " << result.reading.right_trigger
			 << "\nLeft thumbstick x: " << result.reading.left_thumbstick_x
			 << "\nLeft thumbstick y: " << result.reading.left_thumbstick_y
			 << "\nRight thumbstick x: " << result.reading.right_thumbstick_x
			 << "\nRight thumbstick y: " << result.reading.right_thumbstick_y;
#endif

	return result;
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

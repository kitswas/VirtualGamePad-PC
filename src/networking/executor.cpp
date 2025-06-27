#include "executor.hpp"

#include "../settings/input_types.hpp"
#include "../settings/settings_singleton.hpp"
#include "../simulation/gamepadSim.hpp"
#include "../simulation/keyboardSim.hpp"
#include "../simulation/mouseSim.hpp"

#include <QApplication>
#include <algorithm>
#include <cmath>
#include <errno.h>
#include <sstream>
#include <string>
#include <vector>

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

void KeyboardMouseExecutor::handleButtonDown(const ButtonInput &buttonInput)
{
	if (buttonInput.is_mouse_button)
	{
		if (buttonInput.vk == VK_LBUTTON)
			MouseInjector::leftDown();
		else if (buttonInput.vk == VK_RBUTTON)
			MouseInjector::rightDown();
		else if (buttonInput.vk == VK_MBUTTON)
			MouseInjector::middleDown();
	}
	else
		KeyboardInjector::keyDown(buttonInput.vk);
}

void KeyboardMouseExecutor::handleButtonUp(const ButtonInput &buttonInput)
{
	if (buttonInput.is_mouse_button)
	{
		if (buttonInput.vk == VK_LBUTTON)
			MouseInjector::leftUp();
		else if (buttonInput.vk == VK_RBUTTON)
			MouseInjector::rightUp();
		else if (buttonInput.vk == VK_MBUTTON)
			MouseInjector::middleUp();
	}
	else
		KeyboardInjector::keyUp(buttonInput.vk);
}

void KeyboardMouseExecutor::handleThumbstickInput(const ThumbstickInput &thumbstick, float x_value,
												  float y_value, double threshold)
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
			MouseInjector::moveMouseByOffset(stepX, stepY);
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

bool KeyboardMouseExecutor::inject_gamepad_state(vgp_data_exchange_gamepad_reading const &reading)
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

bool GamepadExecutor::inject_gamepad_state(vgp_data_exchange_gamepad_reading const &reading)
{
	using enum winrt::Windows::Gaming::Input::GamepadButtons;
	// Create a new state to update thumbsticks and triggers
	InjectedInputGamepadInfo newState;
	newState.Buttons(None);

	// Set thumbstick values, invert Y-axis
	newState.LeftThumbstickX(reading.left_thumbstick_x);
	newState.LeftThumbstickY(-reading.left_thumbstick_y);
	newState.RightThumbstickX(reading.right_thumbstick_x);
	newState.RightThumbstickY(-reading.right_thumbstick_y);

	// Set trigger values
	newState.LeftTrigger(reading.left_trigger);
	newState.RightTrigger(reading.right_trigger);

	// Update the gamepad state
	m_injector.update(newState);

	// Then call the button handling logic, because it affects the state

	// Handle button presses (mapping from our buttons to WinRT buttons)
	if (reading.buttons_down & GamepadButtons_Menu)
		m_injector.pressButton(Menu);
	if (reading.buttons_down & GamepadButtons_View)
		m_injector.pressButton(View);
	if (reading.buttons_down & GamepadButtons_A)
		m_injector.pressButton(A);
	if (reading.buttons_down & GamepadButtons_B)
		m_injector.pressButton(B);
	if (reading.buttons_down & GamepadButtons_X)
		m_injector.pressButton(X);
	if (reading.buttons_down & GamepadButtons_Y)
		m_injector.pressButton(Y);
	if (reading.buttons_down & GamepadButtons_DPadUp)
		m_injector.pressButton(DPadUp);
	if (reading.buttons_down & GamepadButtons_DPadDown)
		m_injector.pressButton(DPadDown);
	if (reading.buttons_down & GamepadButtons_DPadLeft)
		m_injector.pressButton(DPadLeft);
	if (reading.buttons_down & GamepadButtons_DPadRight)
		m_injector.pressButton(DPadRight);
	if (reading.buttons_down & GamepadButtons_LeftShoulder)
		m_injector.pressButton(LeftShoulder);
	if (reading.buttons_down & GamepadButtons_RightShoulder)
		m_injector.pressButton(RightShoulder);
	if (reading.buttons_down & GamepadButtons_LeftThumbstick)
		m_injector.pressButton(LeftThumbstick);
	if (reading.buttons_down & GamepadButtons_RightThumbstick)
		m_injector.pressButton(RightThumbstick);

	// Handle button releases
	if (reading.buttons_up & GamepadButtons_Menu)
		m_injector.releaseButton(Menu);
	if (reading.buttons_up & GamepadButtons_View)
		m_injector.releaseButton(View);
	if (reading.buttons_up & GamepadButtons_A)
		m_injector.releaseButton(A);
	if (reading.buttons_up & GamepadButtons_B)
		m_injector.releaseButton(B);
	if (reading.buttons_up & GamepadButtons_X)
		m_injector.releaseButton(X);
	if (reading.buttons_up & GamepadButtons_Y)
		m_injector.releaseButton(Y);
	if (reading.buttons_up & GamepadButtons_DPadUp)
		m_injector.releaseButton(DPadUp);
	if (reading.buttons_up & GamepadButtons_DPadDown)
		m_injector.releaseButton(DPadDown);
	if (reading.buttons_up & GamepadButtons_DPadLeft)
		m_injector.releaseButton(DPadLeft);
	if (reading.buttons_up & GamepadButtons_DPadRight)
		m_injector.releaseButton(DPadRight);
	if (reading.buttons_up & GamepadButtons_LeftShoulder)
		m_injector.releaseButton(LeftShoulder);
	if (reading.buttons_up & GamepadButtons_RightShoulder)
		m_injector.releaseButton(RightShoulder);
	if (reading.buttons_up & GamepadButtons_LeftThumbstick)
		m_injector.releaseButton(LeftThumbstick);
	if (reading.buttons_up & GamepadButtons_RightThumbstick)
		m_injector.releaseButton(RightThumbstick);

	// Inject the current state
	m_injector.inject();

	return true;
}

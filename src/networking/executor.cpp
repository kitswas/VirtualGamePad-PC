#include "executor.hpp"

#include "../settings/input_types.hpp"
#include "../settings/settings_singleton.hpp"
#include "../simulation/gamepadSim.hpp"

#include <QApplication>
#include <algorithm>
#include <cmath>
#include <errno.h>
#include <sstream>
#include <string>
#include <vector>

// Create a global GamepadInjector instance
static GamepadInjector g_gamepadInjector;

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

bool inject_gamepad_state(vgp_data_exchange_gamepad_reading const &reading)
{
	// Create a new state to update thumbsticks and triggers
	InjectedInputGamepadInfo newState;
	newState.Buttons(WinRTGamepadButtons::None);

	// Set thumbstick values, invert Y-axis
	newState.LeftThumbstickX(reading.left_thumbstick_x);
	newState.LeftThumbstickY(-reading.left_thumbstick_y);
	newState.RightThumbstickX(reading.right_thumbstick_x);
	newState.RightThumbstickY(-reading.right_thumbstick_y);

	// Set trigger values
	newState.LeftTrigger(reading.left_trigger);
	newState.RightTrigger(reading.right_trigger);

	// Update the gamepad state
	g_gamepadInjector.Update(newState);

	// Then call the button handling logic, because it affects the state

	// Handle button presses (mapping from our buttons to WinRT buttons)
	if (reading.buttons_down & GamepadButtons_Menu)
		g_gamepadInjector.PressButton(WinRTGamepadButtons::Menu);
	if (reading.buttons_down & GamepadButtons_View)
		g_gamepadInjector.PressButton(WinRTGamepadButtons::View);
	if (reading.buttons_down & GamepadButtons_A)
		g_gamepadInjector.PressButton(WinRTGamepadButtons::A);
	if (reading.buttons_down & GamepadButtons_B)
		g_gamepadInjector.PressButton(WinRTGamepadButtons::B);
	if (reading.buttons_down & GamepadButtons_X)
		g_gamepadInjector.PressButton(WinRTGamepadButtons::X);
	if (reading.buttons_down & GamepadButtons_Y)
		g_gamepadInjector.PressButton(WinRTGamepadButtons::Y);
	if (reading.buttons_down & GamepadButtons_DPadUp)
		g_gamepadInjector.PressButton(WinRTGamepadButtons::DPadUp);
	if (reading.buttons_down & GamepadButtons_DPadDown)
		g_gamepadInjector.PressButton(WinRTGamepadButtons::DPadDown);
	if (reading.buttons_down & GamepadButtons_DPadLeft)
		g_gamepadInjector.PressButton(WinRTGamepadButtons::DPadLeft);
	if (reading.buttons_down & GamepadButtons_DPadRight)
		g_gamepadInjector.PressButton(WinRTGamepadButtons::DPadRight);
	if (reading.buttons_down & GamepadButtons_LeftShoulder)
		g_gamepadInjector.PressButton(WinRTGamepadButtons::LeftShoulder);
	if (reading.buttons_down & GamepadButtons_RightShoulder)
		g_gamepadInjector.PressButton(WinRTGamepadButtons::RightShoulder);
	if (reading.buttons_down & GamepadButtons_LeftThumbstick)
		g_gamepadInjector.PressButton(WinRTGamepadButtons::LeftThumbstick);
	if (reading.buttons_down & GamepadButtons_RightThumbstick)
		g_gamepadInjector.PressButton(WinRTGamepadButtons::RightThumbstick);

	// Handle button releases
	if (reading.buttons_up & GamepadButtons_Menu)
		g_gamepadInjector.ReleaseButton(WinRTGamepadButtons::Menu);
	if (reading.buttons_up & GamepadButtons_View)
		g_gamepadInjector.ReleaseButton(WinRTGamepadButtons::View);
	if (reading.buttons_up & GamepadButtons_A)
		g_gamepadInjector.ReleaseButton(WinRTGamepadButtons::A);
	if (reading.buttons_up & GamepadButtons_B)
		g_gamepadInjector.ReleaseButton(WinRTGamepadButtons::B);
	if (reading.buttons_up & GamepadButtons_X)
		g_gamepadInjector.ReleaseButton(WinRTGamepadButtons::X);
	if (reading.buttons_up & GamepadButtons_Y)
		g_gamepadInjector.ReleaseButton(WinRTGamepadButtons::Y);
	if (reading.buttons_up & GamepadButtons_DPadUp)
		g_gamepadInjector.ReleaseButton(WinRTGamepadButtons::DPadUp);
	if (reading.buttons_up & GamepadButtons_DPadDown)
		g_gamepadInjector.ReleaseButton(WinRTGamepadButtons::DPadDown);
	if (reading.buttons_up & GamepadButtons_DPadLeft)
		g_gamepadInjector.ReleaseButton(WinRTGamepadButtons::DPadLeft);
	if (reading.buttons_up & GamepadButtons_DPadRight)
		g_gamepadInjector.ReleaseButton(WinRTGamepadButtons::DPadRight);
	if (reading.buttons_up & GamepadButtons_LeftShoulder)
		g_gamepadInjector.ReleaseButton(WinRTGamepadButtons::LeftShoulder);
	if (reading.buttons_up & GamepadButtons_RightShoulder)
		g_gamepadInjector.ReleaseButton(WinRTGamepadButtons::RightShoulder);
	if (reading.buttons_up & GamepadButtons_LeftThumbstick)
		g_gamepadInjector.ReleaseButton(WinRTGamepadButtons::LeftThumbstick);
	if (reading.buttons_up & GamepadButtons_RightThumbstick)
		g_gamepadInjector.ReleaseButton(WinRTGamepadButtons::RightThumbstick);

	// Inject the current state
	g_gamepadInjector.Inject();

	return true;
}

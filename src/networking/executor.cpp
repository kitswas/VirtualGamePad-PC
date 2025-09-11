#include "executor.hpp"

#include "../settings/input_types.hpp"
#include "../settings/settings_singleton.hpp"
#include "../simulation/gamepadSim.hpp"
#include "../simulation/keyboardSim.hpp"
#include "../simulation/mouseSim.hpp"

#include <QApplication>
#include <QDebug>
#include <algorithm>
#include <cmath>
#include <errno.h>
#include <sstream>
#include <string>
#include <vector>

/**
 * Converts a circular position (x,y with radius=1) to square coordinates.
 * This allows diagonal movement to reach (1,1) instead of (0.71,0.71).
 */
std::pair<float, float> circleToSquare(float x, float y)
{
	// Fast path for common cases
	if (x == 0.0f && y == 0.0f)
		return {0.0f, 0.0f};

	float magnitude = std::sqrt(x * x + y * y);
	if (magnitude == 0.0f)
		return {0.0f, 0.0f};

	// Normalize coordinates
	float nx = x / magnitude;
	float ny = y / magnitude;

	// Calculate scaling factor
	float scale = (std::abs(nx) > std::abs(ny)) ? (1.0f / std::abs(nx)) : (1.0f / std::abs(ny));

	// Apply magnitude limits and return
	float clampedMagnitude = std::min(magnitude, 1.0f);
	return {nx * scale * clampedMagnitude, ny * scale * clampedMagnitude};
}

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

KeyboardMouseExecutor::KeyboardMouseExecutor()
{
	try
	{
		m_keyboardInjector = std::make_unique<KeyboardInjector>();
		m_mouseInjector = std::make_unique<MouseInjector>();
	}
	catch (const std::exception &e)
	{
		qCritical() << "Failed to initialize input injectors:" << e.what();
		throw;
	}
}

void KeyboardMouseExecutor::handleButtonDown(const ButtonInput &buttonInput)
{
	if (buttonInput.is_mouse_button)
	{
#ifdef WIN32
		if (buttonInput.vk == VK_LBUTTON)
			m_mouseInjector->leftDown();
		else if (buttonInput.vk == VK_RBUTTON)
			m_mouseInjector->rightDown();
		else if (buttonInput.vk == VK_MBUTTON)
			m_mouseInjector->middleDown();
#elif defined(__linux__)
		if (buttonInput.vk == BTN_LEFT)
			m_mouseInjector->leftDown();
		else if (buttonInput.vk == BTN_RIGHT)
			m_mouseInjector->rightDown();
		else if (buttonInput.vk == BTN_MIDDLE)
			m_mouseInjector->middleDown();
#endif
		else [[unlikely]] // Unknown mouse button, do nothing
			qWarning() << "Unknown mouse button pressed: " << buttonInput.vk;
	}
	else
	{
		m_keyboardInjector->keyDown(buttonInput.vk);
	}
}

void KeyboardMouseExecutor::handleButtonUp(const ButtonInput &buttonInput)
{
	if (buttonInput.is_mouse_button)
	{
#ifdef WIN32
		if (buttonInput.vk == VK_LBUTTON)
			m_mouseInjector->leftUp();
		else if (buttonInput.vk == VK_RBUTTON)
			m_mouseInjector->rightUp();
		else if (buttonInput.vk == VK_MBUTTON)
			m_mouseInjector->middleUp();
#elif defined(__linux__)
		if (buttonInput.vk == BTN_LEFT)
			m_mouseInjector->leftUp();
		else if (buttonInput.vk == BTN_RIGHT)
			m_mouseInjector->rightUp();
		else if (buttonInput.vk == BTN_MIDDLE)
			m_mouseInjector->middleUp();
#endif
		else [[unlikely]] // Unknown mouse button, do nothing
			qWarning() << "Unknown mouse button released: " << buttonInput.vk;
	}
	else
	{
		m_keyboardInjector->keyUp(buttonInput.vk);
	}
}

void KeyboardMouseExecutor::handleThumbstickInput(const ThumbstickInput &thumbstick, float x_value,
												  float y_value, double threshold)
{
	// Convert circular area to square area
	auto [squareX, squareY] = circleToSquare(x_value, y_value);

	if (thumbstick.is_mouse_move)
	{
		// Mouse movement code
		int offsetX = squareX * SettingsSingleton::instance().mouseSensitivity();
		int offsetY = squareY * SettingsSingleton::instance().mouseSensitivity();

		// Only move if offset is above threshold
		if (double thresholdPixels = threshold * SettingsSingleton::instance().mouseSensitivity();
			std::abs(offsetX) < thresholdPixels && std::abs(offsetY) < thresholdPixels)
			return;

		// qDebug() << "Moving mouse by offset:" << offsetX << "," << offsetY;

		// Break movement into steps for smooth movement
		int maxSteps = std::max(std::abs(offsetX), std::abs(offsetY));
		if (maxSteps > 0)
		{
			for (int step = 1; step <= maxSteps; ++step)
			{
				int stepX = (offsetX * step) / maxSteps - (offsetX * (step - 1)) / maxSteps;
				int stepY = (offsetY * step) / maxSteps - (offsetY * (step - 1)) / maxSteps;
				// qDebug() << "Moving mouse by step:" << stepX << "," << stepY;
				m_mouseInjector->moveMouseByOffset(stepX, stepY);
			}
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

void KeyboardMouseExecutor::handleTriggerInput(const TriggerInput &trigger, float trigger_value)
{
	if (trigger_value >= trigger.threshold)
	{
		handleButtonDown(trigger.button_input);
	}
	else
	{
		handleButtonUp(trigger.button_input);
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
		InputKeyCode vk = profile.buttonMap(button);
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

	handleTriggerInput(
		SettingsSingleton::instance().activeKeymapProfile().triggerInput(Trigger::Left),
		reading.left_trigger);

	handleTriggerInput(
		SettingsSingleton::instance().activeKeymapProfile().triggerInput(Trigger::Right),
		reading.right_trigger);

	return true;
}

bool GamepadExecutor::inject_gamepad_state(vgp_data_exchange_gamepad_reading const &reading)
{
#ifdef _WIN32
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

#elif defined(__linux__)
	// Linux implementation using libevdev

	// Set thumbstick and trigger values
	m_injector.setThumbsticks(reading.left_thumbstick_x, -reading.left_thumbstick_y,
							  reading.right_thumbstick_x, -reading.right_thumbstick_y);
	m_injector.setTriggers(reading.left_trigger, reading.right_trigger);

	// Handle button presses (mapping from our buttons to Linux input codes)
	if (reading.buttons_down & GamepadButtons_Menu)
		m_injector.pressButton(BTN_START);
	if (reading.buttons_down & GamepadButtons_View)
		m_injector.pressButton(BTN_SELECT);
	if (reading.buttons_down & GamepadButtons_A)
		m_injector.pressButton(BTN_A);
	if (reading.buttons_down & GamepadButtons_B)
		m_injector.pressButton(BTN_B);
	if (reading.buttons_down & GamepadButtons_X)
		m_injector.pressButton(BTN_X);
	if (reading.buttons_down & GamepadButtons_Y)
		m_injector.pressButton(BTN_Y);
	if (reading.buttons_down & GamepadButtons_DPadUp)
		m_injector.pressButton(BTN_DPAD_UP);
	if (reading.buttons_down & GamepadButtons_DPadDown)
		m_injector.pressButton(BTN_DPAD_DOWN);
	if (reading.buttons_down & GamepadButtons_DPadLeft)
		m_injector.pressButton(BTN_DPAD_LEFT);
	if (reading.buttons_down & GamepadButtons_DPadRight)
		m_injector.pressButton(BTN_DPAD_RIGHT);
	if (reading.buttons_down & GamepadButtons_LeftShoulder)
		m_injector.pressButton(BTN_TL);
	if (reading.buttons_down & GamepadButtons_RightShoulder)
		m_injector.pressButton(BTN_TR);
	if (reading.buttons_down & GamepadButtons_LeftThumbstick)
		m_injector.pressButton(BTN_THUMBL);
	if (reading.buttons_down & GamepadButtons_RightThumbstick)
		m_injector.pressButton(BTN_THUMBR);

	// Handle button releases
	if (reading.buttons_up & GamepadButtons_Menu)
		m_injector.releaseButton(BTN_START);
	if (reading.buttons_up & GamepadButtons_View)
		m_injector.releaseButton(BTN_SELECT);
	if (reading.buttons_up & GamepadButtons_A)
		m_injector.releaseButton(BTN_A);
	if (reading.buttons_up & GamepadButtons_B)
		m_injector.releaseButton(BTN_B);
	if (reading.buttons_up & GamepadButtons_X)
		m_injector.releaseButton(BTN_X);
	if (reading.buttons_up & GamepadButtons_Y)
		m_injector.releaseButton(BTN_Y);
	if (reading.buttons_up & GamepadButtons_DPadUp)
		m_injector.releaseButton(BTN_DPAD_UP);
	if (reading.buttons_up & GamepadButtons_DPadDown)
		m_injector.releaseButton(BTN_DPAD_DOWN);
	if (reading.buttons_up & GamepadButtons_DPadLeft)
		m_injector.releaseButton(BTN_DPAD_LEFT);
	if (reading.buttons_up & GamepadButtons_DPadRight)
		m_injector.releaseButton(BTN_DPAD_RIGHT);
	if (reading.buttons_up & GamepadButtons_LeftShoulder)
		m_injector.releaseButton(BTN_TL);
	if (reading.buttons_up & GamepadButtons_RightShoulder)
		m_injector.releaseButton(BTN_TR);
	if (reading.buttons_up & GamepadButtons_LeftThumbstick)
		m_injector.releaseButton(BTN_THUMBL);
	if (reading.buttons_up & GamepadButtons_RightThumbstick)
		m_injector.releaseButton(BTN_THUMBR);
#endif

	// Inject the current state
	m_injector.inject();

	return true;
}

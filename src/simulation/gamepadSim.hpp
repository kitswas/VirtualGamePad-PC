#pragma once

#include <QDebug>
#include <cmath>

#ifdef _WIN32
// Windows Runtime includes for gamepad injection
#include <winrt/Windows.Gaming.Input.h>
#include <winrt/Windows.UI.Input.Preview.Injection.h>
#include <winrt/base.h>

using winrt::Windows::UI::Input::Preview::Injection::InjectedInputGamepadInfo;
using winrt::Windows::UI::Input::Preview::Injection::InputInjector;
using WinRTGamepadButtons = winrt::Windows::Gaming::Input::GamepadButtons;
#elif defined(__linux__)
// Linux includes for gamepad injection
#include <libevdev/libevdev-uinput.h>
#include <libevdev/libevdev.h>
#include <linux/input.h>
#include <memory>
#endif

/**
 * @brief Behaves like a virtual device (gamepad).
 *
 * @details
 * Platform-specific implementations:
 * - Windows: Uses the Windows UI Input Injection API (Preview) from WinRT
 *   - Requires Windows 10 or later
 *   - Requires Admin privileges (at least when running unpackaged)
 * - Linux: Uses libevdev and uinput for gamepad simulation
 *   - Requires access to /dev/uinput
 *   - User must be in 'input' group or run as root
 *
 * Not movable or copyable
 */
class GamepadInjector
{
  private:
#ifdef _WIN32
	InjectedInputGamepadInfo gamepadState;
	InputInjector injector;
#elif defined(__linux__)
	std::unique_ptr<libevdev, void (*)(libevdev *)> dev;
	std::unique_ptr<libevdev_uinput, void (*)(libevdev_uinput *)> uidev;
	int fd;
	// Button states for tracking press/release
	bool buttonStates[BTN_GAMEPAD - BTN_JOYSTICK + 16]; // Enough for all gamepad buttons
#endif

  public:
	GamepadInjector();
	~GamepadInjector();

	// Delete copy and move operations due to platform-specific restrictions
	GamepadInjector(const GamepadInjector &) = delete;
	GamepadInjector &operator=(const GamepadInjector &) = delete;
	GamepadInjector(GamepadInjector &&) = delete;
	GamepadInjector &operator=(GamepadInjector &&) = delete;

#ifdef _WIN32
	/**
	 * @brief Updates the gamepad state (Windows only).
	 *
	 * @param state The new gamepad state.
	 */
	void update(const InjectedInputGamepadInfo &state);

	void pressButton(WinRTGamepadButtons button);

	void releaseButton(WinRTGamepadButtons button);
#elif defined(__linux__)
	/**
	 * @brief Sets thumbstick values (Linux).
	 *
	 * @param leftX Left thumbstick X (-1.0 to 1.0)
	 * @param leftY Left thumbstick Y (-1.0 to 1.0)
	 * @param rightX Right thumbstick X (-1.0 to 1.0)
	 * @param rightY Right thumbstick Y (-1.0 to 1.0)
	 */
	void setThumbsticks(float leftX, float leftY, float rightX, float rightY);

	/**
	 * @brief Sets trigger values (Linux).
	 *
	 * @param leftTrigger Left trigger (0.0 to 1.0)
	 * @param rightTrigger Right trigger (0.0 to 1.0)
	 */
	void setTriggers(float leftTrigger, float rightTrigger);

	/**
	 * @brief Press a gamepad button (Linux).
	 *
	 * @param buttonCode Linux input event code (BTN_A, BTN_B, etc.)
	 */
	void pressButton(int buttonCode);

	/**
	 * @brief Release a gamepad button (Linux).
	 *
	 * @param buttonCode Linux input event code (BTN_A, BTN_B, etc.)
	 */
	void releaseButton(int buttonCode);
#endif

	/**
	 * @brief Injects the current gamepad state into the system.
	 *
	 * @note
	 * This method sends the current gamepad state to the system for processing.
	 * None of the other methods will take effect until this method is called.
	 */
	void inject();
};

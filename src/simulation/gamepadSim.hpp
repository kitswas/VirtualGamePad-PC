#pragma once

#include <QDebug>
#include <cmath>

// Windows Runtime includes for gamepad injection
#include <winrt/Windows.Gaming.Input.h>
#include <winrt/Windows.UI.Input.Preview.Injection.h>
#include <winrt/base.h>

using winrt::Windows::UI::Input::Preview::Injection::InjectedInputGamepadInfo;
using winrt::Windows::UI::Input::Preview::Injection::InputInjector;
using WinRTGamepadButtons = winrt::Windows::Gaming::Input::GamepadButtons;

/**
 * @brief Behaves like a virtual device (gamepad).
 *
 * @details
 * Uses the Windows UI Input Injection API (Preview) from WinRT.
 *
 * - Requires Windows 10 or later
 * - Requires Admin privileges (atleast when running unpackaged)
 * - Not movable or copyable
 */
class GamepadInjector
{
  private:
	InjectedInputGamepadInfo gamepadState;
	InputInjector injector;

  public:
	GamepadInjector();
	~GamepadInjector();

	// Delete copy and move operations due to injector being non-movable
	GamepadInjector(const GamepadInjector &) = delete;
	GamepadInjector &operator=(const GamepadInjector &) = delete;
	GamepadInjector(GamepadInjector &&) = delete;
	GamepadInjector &operator=(GamepadInjector &&) = delete;

	/**
	 * @brief Updates the gamepad state.
	 *
	 * @param state The new gamepad state.
	 */
	void update(const InjectedInputGamepadInfo &state);

	void pressButton(WinRTGamepadButtons button);

	void releaseButton(WinRTGamepadButtons button);

	/**
	 * @brief Injects the current gamepad state into the system.
	 *
	 * @note
	 * This method sends the current gamepad state to the system for processing.
	 * None of the other methods will take effect until this method is called.
	 */
	void inject();
};

#pragma once

#include <QDebug>
#include <cmath>

// Windows Runtime includes for gamepad injection
#include <windows.h>
#include <winrt/Windows.Gaming.Input.h>
#include <winrt/Windows.UI.Input.Preview.Injection.h>
#include <winrt/base.h>

using namespace winrt;
using namespace Windows::UI::Input::Preview::Injection;
using WinRTGamepadButtons = Windows::Gaming::Input::GamepadButtons;

class GamepadInjector
{
  private:
	InjectedInputGamepadInfo gamepadState;
	InputInjector injector;

  public:
	GamepadInjector();
	~GamepadInjector();

	void Update(const InjectedInputGamepadInfo &state);
	void PressButton(WinRTGamepadButtons button);
	void ReleaseButton(WinRTGamepadButtons button);
	void Inject();
};

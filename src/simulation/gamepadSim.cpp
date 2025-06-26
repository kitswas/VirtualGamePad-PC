#include "gamepadSim.hpp"

#include <QDebug>
#include <cmath>

GamepadInjector::GamepadInjector() : injector(nullptr)
{
	try
	{
		injector = InputInjector::TryCreate();
		injector.InitializeGamepadInjection();

		gamepadState = InjectedInputGamepadInfo{};
	}
	catch (std::exception &e)
	{
		qDebug() << "Failed to create InputInjector:" << e.what();
	}
	catch (...)
	{
		qDebug() << "Failed to create InputInjector";
	}
}

GamepadInjector::~GamepadInjector()
{
	try
	{
		injector.UninitializeGamepadInjection();
	}
	catch (...)
	{
		qDebug() << "Failed to uninitialize gamepad injection";
	}
}

void GamepadInjector::Update(const InjectedInputGamepadInfo &state)
{
	gamepadState = state;
}

void GamepadInjector::PressButton(WinRTGamepadButtons button)
{
	gamepadState.Buttons(static_cast<WinRTGamepadButtons>(
		static_cast<uint32_t>(gamepadState.Buttons()) | static_cast<uint32_t>(button)));
}

void GamepadInjector::ReleaseButton(WinRTGamepadButtons button)
{
	gamepadState.Buttons(static_cast<WinRTGamepadButtons>(
		static_cast<uint32_t>(gamepadState.Buttons()) & ~static_cast<uint32_t>(button)));
}

void GamepadInjector::Inject()
{
	if (injector)
	{
		try
		{
			injector.InjectGamepadInput(gamepadState);
		}
		catch (...)
		{
			qDebug() << "Failed to inject gamepad input";
		}
	}
}

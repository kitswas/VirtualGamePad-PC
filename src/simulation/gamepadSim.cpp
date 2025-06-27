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

void GamepadInjector::update(const InjectedInputGamepadInfo &state)
{
	gamepadState = state;
}

void GamepadInjector::pressButton(WinRTGamepadButtons button)
{
	gamepadState.Buttons(static_cast<WinRTGamepadButtons>(
		static_cast<uint32_t>(gamepadState.Buttons()) | static_cast<uint32_t>(button)));
}

void GamepadInjector::releaseButton(WinRTGamepadButtons button)
{
	gamepadState.Buttons(static_cast<WinRTGamepadButtons>(
		static_cast<uint32_t>(gamepadState.Buttons()) & ~static_cast<uint32_t>(button)));
}

void GamepadInjector::inject()
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

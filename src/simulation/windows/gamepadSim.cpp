#include "../gamepadSim.hpp"

#include <QDebug>
#include <cmath>

GamepadInjector::GamepadInjector() : injector(nullptr)
{
	qDebug() << "GamepadInjector constructor called";

	// Initialize gamepadState with default values
	gamepadState = InjectedInputGamepadInfo{};
	gamepadState.Buttons(static_cast<WinRTGamepadButtons>(0));
	gamepadState.LeftThumbstickX(0.0);
	gamepadState.LeftThumbstickY(0.0);
	gamepadState.RightThumbstickX(0.0);
	gamepadState.RightThumbstickY(0.0);
	gamepadState.LeftTrigger(0.0);
	gamepadState.RightTrigger(0.0);

	try
	{
		injector = InputInjector::TryCreate();
		if (injector == nullptr)
		{
			qCritical() << "Failed to create input injector.";
			// Throw an exception to signal the failure to the caller
			throw std::runtime_error("Failed to create input injector.");
		}
		else
		{
			injector.InitializeGamepadInjection();
			qInfo() << "Input injector created successfully and initialized for gamepad injection.";
		}
	}
	catch (const winrt::hresult_error &ex)
	{
		qCritical() << "Exception while creating gamepad injector:"
					<< QString::fromWCharArray(ex.message().c_str());
		injector = nullptr;
		throw std::runtime_error("WinRT error while creating gamepad injector: " +
								 QString::fromWCharArray(ex.message().c_str()).toStdString());
	}
}

GamepadInjector::~GamepadInjector()
{
	qDebug() << "GamepadInjector destructor called";
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

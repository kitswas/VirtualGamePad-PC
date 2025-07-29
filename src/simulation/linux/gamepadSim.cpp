#include "../gamepadSim.hpp"

#include <QDebug>
#include <cmath>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

GamepadInjector::GamepadInjector()
	: dev(nullptr, libevdev_free), uidev(nullptr, libevdev_uinput_destroy), fd(-1)
{
	qDebug() << "GamepadInjector constructor called (Linux)";

	// Initialize button states
	memset(buttonStates, false, sizeof(buttonStates));

	// Create a new libevdev device
	libevdev *rawDev = libevdev_new();
	if (!rawDev)
	{
		qCritical() << "Failed to create libevdev device";
		throw std::runtime_error("Failed to create libevdev device");
	}
	dev.reset(rawDev);

	// Set device properties
	libevdev_set_name(dev.get(), "Virtual Gamepad PC");
	libevdev_set_id_bustype(dev.get(), BUS_USB);
	libevdev_set_id_vendor(dev.get(), 0x045e);	// Microsoft vendor ID
	libevdev_set_id_product(dev.get(), 0x028e); // Xbox 360 controller product ID
	libevdev_set_id_version(dev.get(), 0x0110);

	// Enable gamepad events
	libevdev_enable_event_type(dev.get(), EV_KEY);
	libevdev_enable_event_type(dev.get(), EV_ABS);
	libevdev_enable_event_type(dev.get(), EV_FF); // Force feedback

	// Enable gamepad buttons (using Xbox controller layout)
	libevdev_enable_event_code(dev.get(), EV_KEY, BTN_A, nullptr);		// A button
	libevdev_enable_event_code(dev.get(), EV_KEY, BTN_B, nullptr);		// B button
	libevdev_enable_event_code(dev.get(), EV_KEY, BTN_X, nullptr);		// X button
	libevdev_enable_event_code(dev.get(), EV_KEY, BTN_Y, nullptr);		// Y button
	libevdev_enable_event_code(dev.get(), EV_KEY, BTN_TL, nullptr);		// Left shoulder
	libevdev_enable_event_code(dev.get(), EV_KEY, BTN_TR, nullptr);		// Right shoulder
	libevdev_enable_event_code(dev.get(), EV_KEY, BTN_SELECT, nullptr); // View/Back button
	libevdev_enable_event_code(dev.get(), EV_KEY, BTN_START, nullptr);	// Menu/Start button
	libevdev_enable_event_code(dev.get(), EV_KEY, BTN_THUMBL, nullptr); // Left thumbstick click
	libevdev_enable_event_code(dev.get(), EV_KEY, BTN_THUMBR, nullptr); // Right thumbstick click
	libevdev_enable_event_code(dev.get(), EV_KEY, BTN_MODE,
							   nullptr); // Unused, just increments button id

	// D-pad (digital)
	libevdev_enable_event_code(dev.get(), EV_KEY, BTN_DPAD_UP, nullptr);
	libevdev_enable_event_code(dev.get(), EV_KEY, BTN_DPAD_DOWN, nullptr);
	libevdev_enable_event_code(dev.get(), EV_KEY, BTN_DPAD_LEFT, nullptr);
	libevdev_enable_event_code(dev.get(), EV_KEY, BTN_DPAD_RIGHT, nullptr);

	// Analog sticks and triggers
	struct input_absinfo absinfo;
	memset(&absinfo, 0, sizeof(absinfo));

	// Left stick X/Y (-32768 to 32767)
	absinfo.minimum = -32768;
	absinfo.maximum = 32767;
	absinfo.fuzz = 16;
	absinfo.flat = 128;
	libevdev_enable_event_code(dev.get(), EV_ABS, ABS_X, &absinfo);
	libevdev_enable_event_code(dev.get(), EV_ABS, ABS_Y, &absinfo);

	// D-pad (analog)
	// The [-1, 1] range causes SDL to assume that the D-pad is actually digital
	absinfo.minimum = -1;
	absinfo.maximum = 1;
	absinfo.fuzz = 0;
	absinfo.flat = 0;
	libevdev_enable_event_code(dev.get(), EV_ABS, ABS_HAT0X, &absinfo); // Left/Right
	libevdev_enable_event_code(dev.get(), EV_ABS, ABS_HAT0Y, &absinfo); // Up/Down

	// Right stick X/Y
	libevdev_enable_event_code(dev.get(), EV_ABS, ABS_RX, &absinfo);
	libevdev_enable_event_code(dev.get(), EV_ABS, ABS_RY, &absinfo);

	// Triggers (0 to 255)
	absinfo.minimum = 0;
	absinfo.maximum = 255;
	absinfo.fuzz = 0;
	absinfo.flat = 0;
	libevdev_enable_event_code(dev.get(), EV_ABS, ABS_Z, &absinfo);	 // Left trigger
	libevdev_enable_event_code(dev.get(), EV_ABS, ABS_RZ, &absinfo); // Right trigger

	// Create uinput device
	libevdev_uinput *rawUidev;
	int ret =
		libevdev_uinput_create_from_device(dev.get(), LIBEVDEV_UINPUT_OPEN_MANAGED, &rawUidev);
	if (ret < 0)
	{
		qCritical() << "Failed to create uinput device:" << strerror(-ret);
		if (ret == -EACCES)
		{
			qCritical() << "Permission denied. You may need to:";
			qCritical() << "1. Add your user to the 'input' group: sudo usermod -a -G input $USER";
			qCritical() << "2. Load the uinput module: sudo modprobe uinput";
			qCritical() << "3. Set uinput permissions: sudo chmod 666 /dev/uinput";
			qCritical() << "4. Or run the application as root (not recommended)";
		}
		throw std::runtime_error("Failed to create uinput device: " + std::string(strerror(-ret)));
	}
	uidev.reset(rawUidev);

	qInfo() << "Virtual gamepad created successfully on Linux";
}

GamepadInjector::~GamepadInjector()
{
	qDebug() << "GamepadInjector destructor called (Linux)";
	// Cleanup is handled by smart pointers
}

void GamepadInjector::setThumbsticks(float leftX, float leftY, float rightX, float rightY)
{
	// Convert from [-1.0, 1.0] to [-32768, 32767]
	int leftXInt = static_cast<int>(leftX * 32767);
	int leftYInt = static_cast<int>(-leftY * 32767); // Invert Y axis to match Windows behavior
	int rightXInt = static_cast<int>(rightX * 32767);
	int rightYInt = static_cast<int>(-rightY * 32767); // Invert Y axis to match Windows behavior

	// Send the events
	libevdev_uinput_write_event(uidev.get(), EV_ABS, ABS_X, leftXInt);
	libevdev_uinput_write_event(uidev.get(), EV_ABS, ABS_Y, leftYInt);
	libevdev_uinput_write_event(uidev.get(), EV_ABS, ABS_RX, rightXInt);
	libevdev_uinput_write_event(uidev.get(), EV_ABS, ABS_RY, rightYInt);
}

void GamepadInjector::setTriggers(float leftTrigger, float rightTrigger)
{
	// Convert from [0.0, 1.0] to [0, 255]
	int leftInt = static_cast<int>(leftTrigger * 255);
	int rightInt = static_cast<int>(rightTrigger * 255);

	libevdev_uinput_write_event(uidev.get(), EV_ABS, ABS_Z, leftInt);
	libevdev_uinput_write_event(uidev.get(), EV_ABS, ABS_RZ, rightInt);
}

void GamepadInjector::pressButton(int buttonCode)
{
	libevdev_uinput_write_event(uidev.get(), EV_KEY, buttonCode, 1);
	if (buttonCode == BTN_DPAD_LEFT)
	{
		libevdev_uinput_write_event(uidev.get(), EV_ABS, ABS_HAT0X, -1);
	}
	else if (buttonCode == BTN_DPAD_RIGHT)
	{
		libevdev_uinput_write_event(uidev.get(), EV_ABS, ABS_HAT0X, 1);
	}
	else if (buttonCode == BTN_DPAD_UP)
	{
		libevdev_uinput_write_event(uidev.get(), EV_ABS, ABS_HAT0Y, -1);
	}
	else if (buttonCode == BTN_DPAD_DOWN)
	{
		libevdev_uinput_write_event(uidev.get(), EV_ABS, ABS_HAT0Y, 1);
	}
}

void GamepadInjector::releaseButton(int buttonCode)
{
	libevdev_uinput_write_event(uidev.get(), EV_KEY, buttonCode, 0);
	if (buttonCode == BTN_DPAD_LEFT || buttonCode == BTN_DPAD_RIGHT)
	{
		libevdev_uinput_write_event(uidev.get(), EV_ABS, ABS_HAT0X, 0);
	}
	else if (buttonCode == BTN_DPAD_UP || buttonCode == BTN_DPAD_DOWN)
	{
		libevdev_uinput_write_event(uidev.get(), EV_ABS, ABS_HAT0Y, 0);
	}
}

void GamepadInjector::inject()
{
	// Send sync event to commit all changes
	libevdev_uinput_write_event(uidev.get(), EV_SYN, SYN_REPORT, 0);
}

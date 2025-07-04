#include "../keyboardSim.hpp"

#include <QDebug>
#include <QKeySequence>
#include <QThread>
#include <Qt>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

KeyboardInjector::KeyboardInjector() : m_keyboardDevice(nullptr, libevdev_uinput_destroy)
{
	if (m_keyboardDevice)
	{
		return;
	}

	// Create a new libevdev device for keyboard
	libevdev *dev = libevdev_new();
	if (!dev)
	{
		qCritical() << "Failed to create keyboard libevdev device";
		return;
	}

	// Set device properties
	libevdev_set_name(dev, "Virtual Gamepad PC Keyboard");
	libevdev_set_id_bustype(dev, BUS_USB);
	libevdev_set_id_vendor(dev, 0x1d6b);  // Linux Foundation
	libevdev_set_id_product(dev, 0x0001); // Generic keyboard
	libevdev_set_id_version(dev, 0x0100);

	// Enable keyboard events
	libevdev_enable_event_type(dev, EV_KEY);
	libevdev_enable_event_type(dev, EV_REP); // Key repeat

	// Enable all keyboard keys that we might use
	for (int i = 0; i < KEY_MAX; i++)
	{
		if (i >= KEY_ESC && i <= KEY_COMPOSE)
		{
			libevdev_enable_event_code(dev, EV_KEY, i, nullptr);
		}
	}

	// Create uinput device
	libevdev_uinput *uidev;
	int ret = libevdev_uinput_create_from_device(dev, LIBEVDEV_UINPUT_OPEN_MANAGED, &uidev);
	libevdev_free(dev);

	if (ret < 0)
	{
		qCritical() << "Failed to create keyboard uinput device:" << strerror(-ret);
		return;
	}

	m_keyboardDevice.reset(uidev);
	qDebug() << "Virtual keyboard device created successfully";
}

KeyboardInjector::~KeyboardInjector()
{
	// Cleanup is handled by smart pointer
}

void KeyboardInjector::pressKey(quint32 nativeKeyCode)
{
	if (!m_keyboardDevice)
		return;

	// Use native Linux key code directly
	int linuxKey = static_cast<int>(nativeKeyCode);
	if (linuxKey == KEY_RESERVED)
		return;

	// Press key
	libevdev_uinput_write_event(m_keyboardDevice.get(), EV_KEY, linuxKey, 1);
	libevdev_uinput_write_event(m_keyboardDevice.get(), EV_SYN, SYN_REPORT, 0);

	// Wait
	QThread::msleep(PRESS_INTERVAL);

	// Release key
	libevdev_uinput_write_event(m_keyboardDevice.get(), EV_KEY, linuxKey, 0);
	libevdev_uinput_write_event(m_keyboardDevice.get(), EV_SYN, SYN_REPORT, 0);
}

void KeyboardInjector::pressKeyCombo(std::vector<quint32> nativeKeys)
{
	if (!m_keyboardDevice)
		return;

	// Press all keys
	for (quint32 nativeKeyCode : nativeKeys)
	{
		int linuxKey = static_cast<int>(nativeKeyCode);
		if (linuxKey != KEY_RESERVED)
		{
			libevdev_uinput_write_event(m_keyboardDevice.get(), EV_KEY, linuxKey, 1);
		}
	}
	libevdev_uinput_write_event(m_keyboardDevice.get(), EV_SYN, SYN_REPORT, 0);

	// Wait
	QThread::msleep(PRESS_INTERVAL);

	// Release all keys
	for (quint32 nativeKeyCode : nativeKeys)
	{
		int linuxKey = static_cast<int>(nativeKeyCode);
		if (linuxKey != KEY_RESERVED)
		{
			libevdev_uinput_write_event(m_keyboardDevice.get(), EV_KEY, linuxKey, 0);
		}
	}
	libevdev_uinput_write_event(m_keyboardDevice.get(), EV_SYN, SYN_REPORT, 0);
}

void KeyboardInjector::keyDown(quint32 nativeKeyCode)
{
	if (!m_keyboardDevice)
		return;

	int linuxKey = static_cast<int>(nativeKeyCode);
	if (linuxKey == KEY_RESERVED)
		return;

	libevdev_uinput_write_event(m_keyboardDevice.get(), EV_KEY, linuxKey, 1);
	libevdev_uinput_write_event(m_keyboardDevice.get(), EV_SYN, SYN_REPORT, 0);
}

void KeyboardInjector::keyUp(quint32 nativeKeyCode)
{
	if (!m_keyboardDevice)
		return;

	int linuxKey = static_cast<int>(nativeKeyCode);
	if (linuxKey == KEY_RESERVED)
		return;

	libevdev_uinput_write_event(m_keyboardDevice.get(), EV_KEY, linuxKey, 0);
	libevdev_uinput_write_event(m_keyboardDevice.get(), EV_SYN, SYN_REPORT, 0);
}

void KeyboardInjector::keyComboUp(std::vector<quint32> nativeKeys)
{
	if (!m_keyboardDevice)
		return;

	for (quint32 nativeKeyCode : nativeKeys)
	{
		int linuxKey = static_cast<int>(nativeKeyCode);
		if (linuxKey != KEY_RESERVED)
		{
			libevdev_uinput_write_event(m_keyboardDevice.get(), EV_KEY, linuxKey, 0);
		}
	}
	libevdev_uinput_write_event(m_keyboardDevice.get(), EV_SYN, SYN_REPORT, 0);
}

void KeyboardInjector::keyComboDown(std::vector<quint32> nativeKeys)
{
	if (!m_keyboardDevice)
		return;

	for (quint32 nativeKeyCode : nativeKeys)
	{
		int linuxKey = static_cast<int>(nativeKeyCode);
		if (linuxKey != KEY_RESERVED)
		{
			libevdev_uinput_write_event(m_keyboardDevice.get(), EV_KEY, linuxKey, 1);
		}
	}
	libevdev_uinput_write_event(m_keyboardDevice.get(), EV_SYN, SYN_REPORT, 0);
}

void KeyboardInjector::typeUnicodeString(const QString &str)
{
	// For Unicode string typing in Linux, we would need to implement
	// a more complex system involving composition or input method
	// For now, we'll handle basic ASCII characters
	if (!m_keyboardDevice)
		return;

	for (const QChar &c : str)
	{
		if (c.isLetter())
		{
			// Handle letters - map to Linux key codes
			QChar upper = c.toUpper();
			if (upper >= 'A' && upper <= 'Z')
			{
				int keyCode = KEY_A + (upper.unicode() - 'A');
				if (c.isUpper())
				{
					// Upper case - press shift + letter
					std::vector<quint32> combo = {KEY_LEFTSHIFT, static_cast<quint32>(keyCode)};
					pressKeyCombo(combo);
				}
				else
				{
					// Lower case - just press the letter
					pressKey(static_cast<quint32>(keyCode));
				}
			}
		}
		else if (c.isDigit())
		{
			// Handle digits
			int digitValue = c.digitValue();
			if (digitValue >= 0 && digitValue <= 9)
			{
				int keyCode = KEY_0 + digitValue;
				pressKey(static_cast<quint32>(keyCode));
			}
		}
		else if (c.isSpace())
		{
			pressKey(KEY_SPACE);
		}
		else if (c == QChar::LineFeed || c == QChar::CarriageReturn)
		{
			pressKey(KEY_ENTER);
		}
		else if (c == QChar::Tabulation)
		{
			pressKey(KEY_TAB);
		}
	}
}

/**
 * @file keyboardSim.hpp
 * @brief Simulates keyboard input across platforms.
 */
#pragma once

#include <QString>
#include <string>
#include <vector>

#ifdef _WIN32
// Exclude rarely-used stuff from Windows headers. Required for WinSoc2 to work.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif defined(__linux__)
#include <libevdev/libevdev-uinput.h>
#include <libevdev/libevdev.h>
#include <linux/input.h>
#include <memory>
#endif

/**
 * @brief Keyboard input injector using platform-specific APIs.
 *
 * @details
 * Platform-specific implementations:
 * - Windows: Uses the Windows SendInput API for keyboard simulation
 * - Linux: Uses libevdev and uinput for keyboard simulation
 *
 * Both platforms use instance methods for consistent interface.
 */
class KeyboardInjector
{
  public:
	/**
	 * The time in milliseconds to wait between pressing and releasing a key.
	 */
	static constexpr int PRESS_INTERVAL = 10;

	KeyboardInjector();
	~KeyboardInjector();

	// Delete copy and move operations due to platform-specific restrictions
	KeyboardInjector(const KeyboardInjector &) = delete;
	KeyboardInjector &operator=(const KeyboardInjector &) = delete;
	KeyboardInjector(KeyboardInjector &&) = delete;
	KeyboardInjector &operator=(KeyboardInjector &&) = delete;

	void pressKey(quint32 nativeKeyCode);
	void pressKeyCombo(std::vector<quint32> nativeKeys);
	void keyUp(quint32 nativeKeyCode);
	void keyDown(quint32 nativeKeyCode);
	void keyComboUp(std::vector<quint32> nativeKeys);
	void keyComboDown(std::vector<quint32> nativeKeys);
	void typeUnicodeString(const QString &str);

  private:
#ifdef _WIN32
	void addScanCode(INPUT &input, WORD key);
#elif defined(__linux__)
	std::unique_ptr<libevdev_uinput, void (*)(libevdev_uinput *)> m_keyboardDevice;
#endif
};

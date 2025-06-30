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
#include <libevdev/libevdev.h>
#include <libevdev/libevdev-uinput.h>
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
 * All methods are static as no instance state is required.
 */
class KeyboardInjector
{
  public:
	/**
	 * The time in milliseconds to wait between pressing and releasing a key.
	 */
	static constexpr int PRESS_INTERVAL = 10;

	// These methods take Qt::Key codes and convert to platform-specific codes
	static void pressKey(int qtKeyCode);
	static void pressKeyCombo(std::vector<int> qtKeys);
	static void keyUp(int qtKeyCode);
	static void keyDown(int qtKeyCode);
	static void keyComboUp(std::vector<int> qtKeys);
	static void keyComboDown(std::vector<int> qtKeys);
	static void typeUnicodeString(const QString& str);

  private:

#ifdef _WIN32
	static void addScanCode(INPUT &input, WORD key);
	static WORD qtKeyToWindowsVK(int qtKey);
#elif defined(__linux__)
	static std::unique_ptr<libevdev_uinput, void(*)(libevdev_uinput*)> getKeyboardDevice();
	static std::unique_ptr<libevdev_uinput, void(*)(libevdev_uinput*)> s_keyboardDevice;
	static int qtKeyToLinuxKey(int qtKey);
#endif
};

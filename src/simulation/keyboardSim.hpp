/**
 * @file keyboardSim.hpp
 * @brief Simulates keyboard input in Windows.
 *
 * @note Do not include this file directly. Use @link simulate.hpp @endlink instead.
 */
#pragma once

// Exclude rarely-used stuff from Windows headers. Required for WinSoc2 to work.
#define WIN32_LEAN_AND_MEAN
#include <string>
#include <vector>
#include <windows.h>

/**
 * @brief Keyboard input injector using Windows Input API.
 *
 * @details
 * Uses the Windows SendInput API for keyboard simulation.
 * All methods are static as no instance state is required.
 */
class KeyboardInjector
{
  public:
	/**
	 * The time in milliseconds to wait between pressing and releasing a key.
	 */
	static constexpr int PRESS_INTERVAL = 10;

	static void pressKey(WORD key);
	static void pressKeyCombo(std::vector<WORD> keys);
	static void keyUp(WORD key);
	static void keyDown(WORD key);
	static void keyComboUp(std::vector<WORD> keys);
	static void keyComboDown(std::vector<WORD> keys);
	static void typeUnicodeString(std::wstring str);

  private:
	static void addScanCode(INPUT &input, WORD key);
};

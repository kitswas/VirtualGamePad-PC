/**
 * @file keyboardSim.hpp
 * @brief Simulates keyboard input in Windows.
 *
 * @note Do not include this file directly. Use @link simulate.hpp @endlink instead.
 */
#pragma once

// Exclude rarely-used stuff from Windows headers. Required for WinSoc2 to work.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <string>
#include <vector>

/**
 * The time in milliseconds to wait between pressing and releasing a key.
 */
constexpr int PRESS_INTERVAL=10;

void pressKey(WORD key);
void pressKeyCombo(std::vector<WORD> keys);
void keyUp(WORD key);
void keyDown(WORD key);
void keyComboUp(std::vector<WORD> keys);
void keyComboDown(std::vector<WORD> keys);
void typeUnicodeString(std::wstring str);

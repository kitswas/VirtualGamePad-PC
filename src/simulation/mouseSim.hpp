/**
 * @file mouseSim.hpp
 * @brief Simulates mouse input in Windows.
 * 
 * @note Do not include this file directly. Use @link simulate.hpp @endlink instead.
 */
#pragma once

// Exclude rarely-used stuff from Windows headers. Required for WinSoc2 to work.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/**
 * @brief Move the mouse to the specified coordinates.
 *
 * @param x from 0 to screen width
 * @param y from 0 to screen height
 */
void moveMouseToPosition(int x, int y);

/**
 * @brief Move the mouse by the specified offset.
 *
 * @param x from 0 to screen width
 * @param y from 0 to screen height
 */
void moveMouseByOffset(int x, int y);

/**
 * @brief Simulate a single left click.
 *
 * @note This function will sleep for the double click time.
 */
void singleClick();

void doubleClick();

/**
 * @brief Simulate a left click.
 * @note Repeated calls to this function may result in a double click. Use singleClick() if you want to avoid this.
 * @see singleClick()
 */
void leftClick();

void rightClick();

void middleClick();

void scrollUp();

void scrollDown();

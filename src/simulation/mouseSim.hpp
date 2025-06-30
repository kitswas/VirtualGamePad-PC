/**
 * @file mouseSim.hpp
 * @brief Simulates mouse input across platforms.
 *
 * @note Do not include this file directly. Use @link simulate.hpp @endlink instead.
 */
#pragma once

#ifdef WIN32
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
 * @brief Mouse input injector using platform-specific APIs.
 *
 * @details
 * Platform-specific implementations:
 * - Windows: Uses the Windows SendInput API for mouse simulation
 * - Linux: Uses libevdev and uinput for mouse simulation
 * 
 * All methods are static as no instance state is required.
 * Mouse button methods take Qt::MouseButton codes and convert to platform-specific codes.
 */
class MouseInjector
{
  public:
	/**
	 * @brief Move the mouse to the specified coordinates.
	 *
	 * @param x from 0 to screen width
	 * @param y from 0 to screen height
	 */
	static void moveMouseToPosition(int x, int y);

	/**
	 * @brief Move the mouse by the specified offset.
	 *
	 * @param x from 0 to screen width
	 * @param y from 0 to screen height
	 */
	static void moveMouseByOffset(int x, int y);

	/**
	 * @brief Simulate a single left click.
	 *
	 * @note This function will sleep for the double click time.
	 */
	static void singleClick();

	static void doubleClick();

	/**
	 * @brief Simulate a left click.
	 * @note Repeated calls to this function may result in a double click.
	 * Use singleClick() if you want to avoid this.
	 * @see singleClick()
	 */
	static void leftClick();

	static void rightClick();
	static void middleClick();
	static void leftDown();
	static void leftUp();
	static void rightDown();
	static void rightUp();
	static void middleDown();
	static void middleUp();
	static void scrollUp();
	static void scrollDown();

  private:
	static constexpr unsigned int ClickHoldTime = 10; // Time to hold the click in milliseconds
	
#ifdef __linux__
	static std::unique_ptr<libevdev_uinput, void(*)(libevdev_uinput*)> getMouseDevice();
	static std::unique_ptr<libevdev_uinput, void(*)(libevdev_uinput*)> s_mouseDevice;
#endif
};

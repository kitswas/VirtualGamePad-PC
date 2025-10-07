/**
 * @file mouseSim.hpp
 * @brief Simulates mouse input across platforms.
 */
#pragma once

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
 * @brief Mouse input injector using platform-specific APIs.
 *
 * @details
 * Platform-specific implementations:
 * - Windows: Uses the Windows SendInput API for mouse simulation
 * - Linux: Uses libevdev and uinput for mouse simulation
 *
 * Both platforms use instance methods for consistent interface.
 */
class MouseInjector
{
  public:
	MouseInjector();
	~MouseInjector();

	// Delete copy and move operations due to platform-specific restrictions
	MouseInjector(const MouseInjector &) = delete;
	MouseInjector &operator=(const MouseInjector &) = delete;
	MouseInjector(MouseInjector &&) = delete;
	MouseInjector &operator=(MouseInjector &&) = delete;

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
	 * @note Repeated calls to this function may result in a double click.
	 * Use singleClick() if you want to avoid this.
	 * @see singleClick()
	 */
	void leftClick();

	void rightClick();
	void middleClick();
	void leftDown();
	void leftUp();
	void rightDown();
	void rightUp();
	void middleDown();
	void middleUp();
	void scrollUp();
	void scrollDown();

  private:
	static constexpr unsigned int ClickHoldTime = 10; // Time to hold the click in milliseconds

#ifdef __linux__
	std::unique_ptr<libevdev_uinput, void (*)(libevdev_uinput *)> m_mouseDevice;
	void ensureDevice();
#endif
};

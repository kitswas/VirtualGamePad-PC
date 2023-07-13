#pragma once

#include "keyboardSim.hpp"
#include "mouseSim.hpp"

/** This function is used to block input from external devices.
 * It stops keyboard and mouse input events from reaching applications.
 * It is used to prevent the user from accidentally interfering with the program.
 *
 * https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-blockinput
 *
 * @note **Emergency Exit:** <kbd>CTRL</kbd> + <kbd>ALT</kbd> + <kbd>DEL</kbd>
 * @see @link unblockExternalInput()
 */
bool blockExternalInput();

/** This function is used to unblock input from external devices.
 * It allows keyboard and mouse input events to reach applications again.
 *
 * @see @link blockExternalInput()
 */
bool unblockExternalInput();

void clearExistingInput();

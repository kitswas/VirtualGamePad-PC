#include "../keyboardSim.hpp"

#include <unordered_set>
#include <QKeySequence>

/**
 * Extended key lookup table.
 * Reference:
 * https://learn.microsoft.com/en-us/windows/win32/inputdev/about-keyboard-input#extended-key-flag
 */
static const std::unordered_set<WORD> extendedKeys = {
	VK_RMENU, VK_RCONTROL, VK_INSERT, VK_DELETE, VK_HOME,	 VK_END,	VK_PRIOR,	 VK_NEXT,
	VK_RIGHT, VK_UP,	   VK_LEFT,	  VK_DOWN,	 VK_NUMLOCK, VK_CANCEL, VK_SNAPSHOT, VK_DIVIDE};

/**
 * @brief Adds the scan code to the input structure.
 *
 * @details
 * This is necessary because some apps and games use the scan code
 * to determine the actual location of the key on the keyboard.
 * The difference becomes vital when using non-QWERTY keyboard layouts.
 */
inline void KeyboardInjector::addScanCode(INPUT &input, WORD key)
{
	input.ki.wScan = static_cast<WORD>(MapVirtualKeyW(key, MAPVK_VK_TO_VSC));
	input.ki.dwFlags |= KEYEVENTF_SCANCODE;
	if (extendedKeys.contains(key))
	{
		input.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
	}
}

void KeyboardInjector::pressKey(int qtKeyCode)
{
	WORD key = qtKeyToWindowsVK(qtKeyCode);
	INPUT input = {0};
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = key;
	addScanCode(input, key);
	SendInput(1, &input, sizeof(INPUT));
	// Wait
	Sleep(PRESS_INTERVAL);
	// Release the key
	input.ki.dwFlags |= KEYEVENTF_KEYUP;
	SendInput(1, &input, sizeof(INPUT));
}

void KeyboardInjector::pressKeyCombo(std::vector<int> qtKeys)
{
	std::vector<INPUT> inputs(qtKeys.size() * 2);
	for (size_t i = 0; i < qtKeys.size(); i++)
	{
		inputs[i].type = INPUT_KEYBOARD;
		inputs[i].ki.wVk = qtKeyToWindowsVK(qtKeys[i]);
		addScanCode(inputs[i], qtKeys[i]);
	}
	// Wait
	Sleep(PRESS_INTERVAL);
	for (size_t i = 0; i < qtKeys.size(); i++)
	{
		inputs[i + qtKeys.size()].type = INPUT_KEYBOARD;
		inputs[i + qtKeys.size()].ki.wVk = qtKeys[i];
		inputs[i + qtKeys.size()].ki.dwFlags = KEYEVENTF_KEYUP;
		addScanCode(inputs[i + qtKeys.size()], qtKeys[i]);
	}
	SendInput(qtKeys.size() * 2, inputs.data(), sizeof(INPUT));
}

void KeyboardInjector::keyDown(int qtKeyCode)
{
	WORD key = qtKeyToWindowsVK(qtKeyCode);
	INPUT input = {0};
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = key;
	addScanCode(input, key);
	SendInput(1, &input, sizeof(INPUT));
}

void KeyboardInjector::keyUp(int qtKeyCode)
{
	WORD key = qtKeyToWindowsVK(qtKeyCode);
	INPUT input = {0};
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = key;
	input.ki.dwFlags = KEYEVENTF_KEYUP;
	addScanCode(input, key);
	SendInput(1, &input, sizeof(INPUT));
}

void KeyboardInjector::keyComboUp(std::vector<int> qtKeys)
{
	std::vector<INPUT> inputs(qtKeys.size());
	for (size_t i = 0; i < qtKeys.size(); i++)
	{
		inputs[i].type = INPUT_KEYBOARD;
		inputs[i].ki.wVk = qtKeyToWindowsVK(qtKeys[i]);
		inputs[i].ki.dwFlags = KEYEVENTF_KEYUP;
		addScanCode(inputs[i], qtKeys[i]);
	}
	SendInput(qtKeys.size(), inputs.data(), sizeof(INPUT));
}

void KeyboardInjector::keyComboDown(std::vector<int> qtKeys)
{
	std::vector<INPUT> inputs(qtKeys.size());
	for (size_t i = 0; i < qtKeys.size(); i++)
	{
		inputs[i].type = INPUT_KEYBOARD;
		inputs[i].ki.wVk = qtKeyToWindowsVK(qtKeys[i]);
		addScanCode(inputs[i], qtKeys[i]);
	}
	SendInput(qtKeys.size(), inputs.data(), sizeof(INPUT));
}

void KeyboardInjector::typeUnicodeString(const QString& str)
{
	std::wstring wstr = str.toStdWString();
	std::vector<INPUT> inputs(wstr.size() * 2);
	for (size_t i = 0; i < wstr.size(); i++)
	{
		// Press the key
		inputs[2 * i].type = INPUT_KEYBOARD;
		inputs[2 * i].ki.wScan = wstr[i];
		inputs[2 * i].ki.dwFlags = KEYEVENTF_UNICODE;
		// Release the key
		inputs[2 * i + 1].type = INPUT_KEYBOARD;
		inputs[2 * i + 1].ki.wScan = wstr[i];
		inputs[2 * i + 1].ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
	}
	SendInput(wstr.size() * 2, inputs.data(), sizeof(INPUT));
}

WORD KeyboardInjector::qtKeyToWindowsVK(int qtKey)
{
	// Handle mouse buttons
	if (qtKey == Qt::LeftButton) return VK_LBUTTON;
	if (qtKey == Qt::RightButton) return VK_RBUTTON;
	if (qtKey == Qt::MiddleButton) return VK_MBUTTON;
	
	// Map common Qt keys to Windows VK codes
	switch (qtKey) {
		case Qt::Key_Backspace: return VK_BACK;
		case Qt::Key_Tab: return VK_TAB;
		case Qt::Key_Return: 
		case Qt::Key_Enter: return VK_RETURN;
		case Qt::Key_Shift: return VK_SHIFT;
		case Qt::Key_Control: return VK_CONTROL;
		case Qt::Key_Alt: return VK_MENU;
		case Qt::Key_CapsLock: return VK_CAPITAL;
		case Qt::Key_Escape: return VK_ESCAPE;
		case Qt::Key_Space: return VK_SPACE;
		case Qt::Key_PageUp: return VK_PRIOR;
		case Qt::Key_PageDown: return VK_NEXT;
		case Qt::Key_End: return VK_END;
		case Qt::Key_Home: return VK_HOME;
		case Qt::Key_Left: return VK_LEFT;
		case Qt::Key_Up: return VK_UP;
		case Qt::Key_Right: return VK_RIGHT;
		case Qt::Key_Down: return VK_DOWN;
		case Qt::Key_Insert: return VK_INSERT;
		case Qt::Key_Delete: return VK_DELETE;
		case Qt::Key_Menu: return VK_APPS;
		case Qt::Key_Period: return VK_OEM_PERIOD;
		case Qt::Key_Comma: return VK_OEM_COMMA;
		case Qt::Key_Minus: return VK_OEM_MINUS;
		case Qt::Key_Plus: return VK_OEM_PLUS;
		default:
			// For alphanumeric keys, Qt and Windows VK codes are often the same
			if (qtKey >= Qt::Key_A && qtKey <= Qt::Key_Z) {
				return qtKey;
			}
			if (qtKey >= Qt::Key_0 && qtKey <= Qt::Key_9) {
				return qtKey;
			}
			// For function keys
			if (qtKey >= Qt::Key_F1 && qtKey <= Qt::Key_F24) {
				return VK_F1 + (qtKey - Qt::Key_F1);
			}
			// Fallback: assume direct mapping
			return qtKey;
	}
}

#include "keyboardSim.hpp"

#include <unordered_set>

// Extended key lookup table.
// Reference:
// https://learn.microsoft.com/en-us/windows/win32/inputdev/about-keyboard-input#extended-key-flag
static const std::unordered_set<WORD> extendedKeys = {
	VK_RMENU, VK_RCONTROL, VK_INSERT, VK_DELETE, VK_HOME,	 VK_END,	VK_PRIOR,	 VK_NEXT,
	VK_RIGHT, VK_UP,	   VK_LEFT,	  VK_DOWN,	 VK_NUMLOCK, VK_CANCEL, VK_SNAPSHOT, VK_DIVIDE};

void pressKey(WORD key)
{
	INPUT input = {0};
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = key;
	input.ki.wScan = static_cast<WORD>(MapVirtualKeyW(key, MAPVK_VK_TO_VSC));
	input.ki.dwFlags = KEYEVENTF_SCANCODE;
	if (extendedKeys.contains(key))
	{
		input.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
	}
	SendInput(1, &input, sizeof(INPUT));
	// Wait
	Sleep(PRESS_INTERVAL);
	// Release the key
	input.ki.dwFlags |= KEYEVENTF_KEYUP;
	SendInput(1, &input, sizeof(INPUT));
}

void pressKeyCombo(std::vector<WORD> keys)
{
	std::vector<INPUT> inputs(keys.size() * 2);
	for (size_t i = 0; i < keys.size(); i++)
	{
		inputs[i].type = INPUT_KEYBOARD;
		inputs[i].ki.wVk = keys[i];
		inputs[i].ki.wScan = static_cast<WORD>(MapVirtualKeyW(keys[i], MAPVK_VK_TO_VSC));
		inputs[i].ki.dwFlags = KEYEVENTF_SCANCODE;
		if (extendedKeys.contains(keys[i]))
		{
			inputs[i].ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
		}
	}
	// Wait
	Sleep(PRESS_INTERVAL);
	for (size_t i = 0; i < keys.size(); i++)
	{
		inputs[i + keys.size()].type = INPUT_KEYBOARD;
		inputs[i + keys.size()].ki.wVk = keys[i];
		inputs[i + keys.size()].ki.wScan = static_cast<WORD>(MapVirtualKeyW(keys[i], MAPVK_VK_TO_VSC));
		inputs[i + keys.size()].ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE;
		if (extendedKeys.contains(keys[i]))
		{
			inputs[i + keys.size()].ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
		}
	}
	SendInput(keys.size() * 2, inputs.data(), sizeof(INPUT));
}

void keyDown(WORD key)
{
	INPUT input = {0};
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = key;
	input.ki.wScan = static_cast<WORD>(MapVirtualKeyW(key, MAPVK_VK_TO_VSC));
	input.ki.dwFlags = KEYEVENTF_SCANCODE;
	if (extendedKeys.contains(key))
	{
		input.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
	}
	SendInput(1, &input, sizeof(INPUT));
}

void keyUp(WORD key)
{
	INPUT input = {0};
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = key;
	input.ki.wScan = static_cast<WORD>(MapVirtualKeyW(key, MAPVK_VK_TO_VSC));
	input.ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE;
	if (extendedKeys.contains(key))
	{
		input.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
	}
	SendInput(1, &input, sizeof(INPUT));
}

void keyComboUp(std::vector<WORD> keys)
{
	std::vector<INPUT> inputs(keys.size());
	for (size_t i = 0; i < keys.size(); i++)
	{
		inputs[i].type = INPUT_KEYBOARD;
		inputs[i].ki.wVk = keys[i];
		inputs[i].ki.wScan = static_cast<WORD>(MapVirtualKeyW(keys[i], MAPVK_VK_TO_VSC));
		inputs[i].ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE;
		if (extendedKeys.contains(keys[i]))
		{
			inputs[i].ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
		}
	}
	SendInput(keys.size(), inputs.data(), sizeof(INPUT));
}

void keyComboDown(std::vector<WORD> keys)
{
	std::vector<INPUT> inputs(keys.size());
	for (size_t i = 0; i < keys.size(); i++)
	{
		inputs[i].type = INPUT_KEYBOARD;
		inputs[i].ki.wVk = keys[i];
		inputs[i].ki.wScan = static_cast<WORD>(MapVirtualKeyW(keys[i], MAPVK_VK_TO_VSC));
		inputs[i].ki.dwFlags = KEYEVENTF_SCANCODE;
		if (extendedKeys.contains(keys[i]))
		{
			inputs[i].ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
		}
	}
	SendInput(keys.size(), inputs.data(), sizeof(INPUT));
}

void typeUnicodeString(std::wstring str)
{
	std::vector<INPUT> inputs(str.size() * 2);
	for (size_t i = 0; i < str.size(); i++)
	{
		// Press the key
		inputs[2 * i].type = INPUT_KEYBOARD;
		inputs[2 * i].ki.wScan = str[i];
		inputs[2 * i].ki.dwFlags = KEYEVENTF_UNICODE;
		// Release the key
		inputs[2 * i + 1].type = INPUT_KEYBOARD;
		inputs[2 * i + 1].ki.wScan = str[i];
		inputs[2 * i + 1].ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
	}
	SendInput(str.size() * 2, inputs.data(), sizeof(INPUT));
}

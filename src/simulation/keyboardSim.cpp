#include "keyboardSim.hpp"

void pressKey(WORD key)
{
	INPUT input = {0};
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = key;
	SendInput(1, &input, sizeof(INPUT));

	// Release the key
	input.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &input, sizeof(INPUT));
}

void pressKeyCombo(std::vector<WORD> keys)
{
	std::vector<INPUT> inputs(keys.size() * 2);
	for (size_t i = 0; i < keys.size(); i++)
	{
		inputs[i].type = INPUT_KEYBOARD;
		inputs[i].ki.wVk = keys[i];
	}
	for (size_t i = 0; i < keys.size(); i++)
	{
		inputs[i + keys.size()].type = INPUT_KEYBOARD;
		inputs[i + keys.size()].ki.wVk = keys[i];
		inputs[i + keys.size()].ki.dwFlags = KEYEVENTF_KEYUP;
	}
	SendInput(keys.size() * 2, inputs.data(), sizeof(INPUT));
}

void keyUp(WORD key)
{
	INPUT input = {0};
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = key;
	input.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &input, sizeof(INPUT));
}

void keyDown(WORD key)
{
	INPUT input = {0};
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = key;
	SendInput(1, &input, sizeof(INPUT));
}

void keyComboUp(std::vector<WORD> keys)
{
	std::vector<INPUT> inputs(keys.size());
	for (size_t i = 0; i < keys.size(); i++)
	{
		inputs[i].type = INPUT_KEYBOARD;
		inputs[i].ki.wVk = keys[i];
		inputs[i].ki.dwFlags = KEYEVENTF_KEYUP;
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
	}
	SendInput(keys.size(), inputs.data(), sizeof(INPUT));
}

void typeUnicodeString(std::wstring str)
{
	std::vector<INPUT> inputs(str.size() * 2);
	for (size_t i = 0; i < str.size(); i++)
	{
		inputs[i].type = INPUT_KEYBOARD;
		inputs[i].ki.wScan = str[i];
		inputs[i].ki.dwFlags = KEYEVENTF_UNICODE;
	}
	for (size_t i = 0; i < str.size(); i++)
	{
		inputs[i + str.size()].type = INPUT_KEYBOARD;
		inputs[i + str.size()].ki.wScan = str[i];
		inputs[i + str.size()].ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
	}
	SendInput(str.size() * 2, inputs.data(), sizeof(INPUT));
}

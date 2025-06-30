#include "mouseSim.hpp"

void MouseInjector::moveMouseToPosition(int x, int y)
{
	// Get the screen resolution
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Calculate the absolute coordinates
	long absoluteX = (x * 65535L) / screenWidth;
	long absoluteY = (y * 65535L) / screenHeight;

	// Move the mouse
	INPUT input = {0};
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
	input.mi.dx = absoluteX;
	input.mi.dy = absoluteY;
	SendInput(1, &input, sizeof(INPUT));
}

void MouseInjector::moveMouseByOffset(int x, int y)
{
	// Save existing mouse speed
	int mouseSpeed = 0;
	SystemParametersInfo(SPI_GETMOUSESPEED, 0, &mouseSpeed, 0);

	// Configure the mouse speed
	SystemParametersInfo(SPI_SETMOUSESPEED, 0, (void *)1, SPIF_SENDCHANGE);

	// Move the mouse
	INPUT input = {0};
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_MOVE_NOCOALESCE;
	input.mi.dx = x;
	input.mi.dy = y;
	SendInput(1, &input, sizeof(INPUT));

	// Restore the mouse speed
	SystemParametersInfo(SPI_SETMOUSESPEED, 0, (void *)mouseSpeed, SPIF_SENDCHANGE);
}

void MouseInjector::leftClick()
{
	INPUT input = {0};
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	SendInput(1, &input, sizeof(INPUT));

	Sleep(ClickHoldTime);

	ZeroMemory(&input, sizeof(INPUT));
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	SendInput(1, &input, sizeof(INPUT));
}

void MouseInjector::singleClick()
{
	leftClick();
	Sleep(GetDoubleClickTime()); // Sleep for the double click time
}

void MouseInjector::leftDown()
{
	INPUT input = {0};
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	SendInput(1, &input, sizeof(INPUT));
}

void MouseInjector::leftUp()
{
	INPUT input = {0};
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	SendInput(1, &input, sizeof(INPUT));
}

void MouseInjector::doubleClick()
{
	leftClick();
	leftClick();
}

void MouseInjector::rightClick()
{
	INPUT input = {0};
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
	SendInput(1, &input, sizeof(INPUT));

	Sleep(ClickHoldTime);

	ZeroMemory(&input, sizeof(INPUT));
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
	SendInput(1, &input, sizeof(INPUT));
}

void MouseInjector::rightDown()
{
	INPUT input = {0};
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
	SendInput(1, &input, sizeof(INPUT));
}

void MouseInjector::rightUp()
{
	INPUT input = {0};
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
	SendInput(1, &input, sizeof(INPUT));
}

void MouseInjector::middleClick()
{
	INPUT input = {0};
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
	SendInput(1, &input, sizeof(INPUT));

	ZeroMemory(&input, sizeof(INPUT));
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
	SendInput(1, &input, sizeof(INPUT));
}

void MouseInjector::middleDown()
{
	INPUT input = {0};
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
	SendInput(1, &input, sizeof(INPUT));
}

void MouseInjector::middleUp()
{
	INPUT input = {0};
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
	SendInput(1, &input, sizeof(INPUT));
}

void MouseInjector::scrollUp()
{
	INPUT input = {0};
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_WHEEL;
	input.mi.mouseData = WHEEL_DELTA;
	SendInput(1, &input, sizeof(INPUT));
}

void MouseInjector::scrollDown()
{
	INPUT input = {0};
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_WHEEL;
	input.mi.mouseData = -WHEEL_DELTA;
	SendInput(1, &input, sizeof(INPUT));
}

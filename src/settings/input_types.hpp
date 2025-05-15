#pragma once
#include "../../VGP_Data_Exchange/C/GameButtons.h"
#include <qtypes.h>
#include <windows.h>

struct Input
{
	WORD vk{};
	uint is_mouse_key = 0;
};

enum Thumbstick
{
	LeftThumbstickUp,
	LeftThumbstickDown,
	LeftThumbstickLeft,
	LeftThumbstickRight,
	RightThumbstickUp,
	RightThumbstickDown,
	RightThumbstickLeft,
	RightThumbstickRight
};

#pragma once

#include "../../VGP_Data_Exchange/C/GameButtons.h"

#include <minwindef.h>
#include <qtypes.h>

struct ButtonInput
{
	WORD vk{};
	bool is_mouse_button = false; // true if the button is a mouse button
};

/**
 * A thumbstick has two axes (X and Y)
 * Types of input:
 * - The entire thumbstick can be mapped to mouse movement
 * - Or the thumbstick directions can be mapped to keyboard keys/mouse clicks
 * */
struct ThumbstickInput
{
	bool is_mouse_move = false;				   // true if the thumbstick is mapped to mouse movement
	ButtonInput up{}, down{}, left{}, right{}; // up, down, left, right directions of the thumbstick
};

// Thumbstick enum
enum Thumbstick
{
	Thumbstick_Left,
	Thumbstick_Right
};

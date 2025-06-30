#pragma once

#include "../../VGP_Data_Exchange/C/GameButtons.h"
#include <Qt>

// Use Qt's key system for platform independence
typedef int InputKeyCode; // Qt::Key values are int

struct ButtonInput
{
	InputKeyCode vk{}; // Qt::Key value or Qt::MouseButton value
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

// Helper functions for mouse button detection using Qt types
inline bool is_mouse_button(InputKeyCode code) {
    return code == Qt::LeftButton || code == Qt::RightButton || code == Qt::MiddleButton;
}

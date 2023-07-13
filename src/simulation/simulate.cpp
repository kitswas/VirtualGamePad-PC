#include "simulate.hpp"

bool blockExternalInput()
{
	return BlockInput(TRUE);
}

bool unblockExternalInput()
{
	return BlockInput(FALSE);
}

void clearExistingInput()
{
	// Get the keyboard state
	BYTE keyboardState[256];
	GetKeyboardState(keyboardState);

	// Cancel any existing input
	// Iterates through all keys, checking if they are down, and releasing them if they are
	for (int i = 0; i < 256; i++)
	{
		if (keyboardState[i] & 0x80)
		{
			// Release the key
			keyUp(i);
		}
		// ostream_here << "0X" << std::hex << i << std::dec << "(" << i << ")"
		// 		 << "is" << (keyboardState[i] & 0x80 ? "down" : "up");
	}
}

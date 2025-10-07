#pragma once

#ifdef _WIN32
#include <stdbool.h>

// Attach to the parent console if launched from a terminal.
// Returns true if a console is available (either pre-existing or attached).
bool attachToParentConsole();

// Detach from the console if attached.
void detachFromConsole();

#endif // _WIN32

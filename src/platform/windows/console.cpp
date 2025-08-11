#ifdef _WIN32
#include "console.hpp"

#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <windows.h>

bool attachToParentConsole()
{
	// Check if we already have a console
	if (GetConsoleWindow() != nullptr)
	{
		return true;
	}

	// Try to attach to the parent process console (if launched from cmd/terminal)
	if (AttachConsole(ATTACH_PARENT_PROCESS))
	{
		// Redirect stdout and stderr to the console
		FILE *pCout;
		FILE *pCerr;
		FILE *pCin;

		freopen_s(&pCout, "CONOUT$", "w", stdout);
		freopen_s(&pCerr, "CONOUT$", "w", stderr);
		freopen_s(&pCin, "CONIN$", "r", stdin);

		// Make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
		// point to console as well
		std::ios::sync_with_stdio(true);

		// Print a newline to separate from the command that launched us
		std::cout << std::endl;
		return true;
	}
	return false;
}

void detachFromConsole()
{
	FreeConsole();
}

#endif // _WIN32

# VirtualGamePad Server

This is a part of [the VirtualGamePad project](https://kitswas.github.io/VirtualGamePad/).

## History of the server's development

1. The server was orginally writen in C++.
2. Then I rewrote it in Rust with [windows-rs](https://github.com/microsoft/windows-rs) for Rust's memory safety and the newer Gamepad injection Windows support.
3. [Windows.UI.Input.Preview.Injection](https://learn.microsoft.com/en-us/uwp/api/windows.ui.input.preview.injection?view=winrt-22621), as the name says, is in preview and is [buggy](https://github.com/microsoft/microsoft-ui-xaml/issues/8639). _(Read unusable)_  
I, unfortunately, didn't know the source of the problems and decided to rewrite the app according to the Microsoft recommended way for Windows Apps (C# with WinUI 3 and the Windows App SDK).  
This is also when I figured that a GUI is worth the effort.
4. Finally, due to the aforementioned reasons, this was created in C++ with the Win32 API and Qt6 for the UI.

## How it works

1. A socket connection is established between the client and the server.

2. The server uses [the SendInput() function](https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-sendinput) to synthesize keystrokes, mouse motions, and button clicks depending on the input from the client.  
It has the following implications:
    - The virtual gamepad is not limited to games. You can use the virtual gamepad to control any application on the computer.
    - Games that use DirectInput or XInput do not recognize input from the virtual gamepad as gamepad input but as keyboard and mouse input. If required, the controls have to be remapped in the game.

3. The buttons on the virtual gamepad are mapped to the keyboard keys and mouse actions using the maps at src/networking/executor.hpp.  
**Note: The right analog thumbstick is mapped to the mouse, not the arrow keys.**  
User defined maps are not supported, _yet_.

## Build Instructions

### Prerequisites

- [Qt 6.5](https://www.qt.io/download-open-source)
- [CMake](https://cmake.org/download/)

### Cloning and Building

Clone the repository and submodules.

```bash
git clone --recurse-submodules https://github.com/kitswas/VirtualGamePad-PC.git
```

Open the project (open the CMakeLists.txt file in the project root) in Qt Creator and build it.

### Code Formatting

This project uses [clang-format](https://clang.llvm.org/docs/ClangFormat.html) for code formatting.  
It includes a PowerShell script to format the code. Run

```bash
./code-lint.ps1
```

from the project root to format the code.  
**Note: This script requires clang-format to be installed and available in the PATH.**

### Generating Documentation

This project uses [Doxygen](https://www.doxygen.nl/index.html) to generate documentation.  
If Doxygen is available on your system,  
You can generate the documentation by running the following command:

```bash
doxygen Doxyfile
```

This repository has automated workflow via Github Actions.  
Documentation is auto-generated and deployed to Github Pages.  
Disable Actions on your own copy or delete the [workflows](./.github/workflows) folder if you do not want to be billed. This is **not my responsiblity.**

# AGENTS.md

## Project Overview

**VirtualGamePad PC** is a cross-platform C++ server application that enables mobile devices to function as virtual game controllers for PC. It receives gamepad input over TCP and translates it into system-level input events.

- **Language**: C++ with Qt6 (Widgets, Network)
- **Build System**: CMake with Ninja, platform presets for Windows/Linux
- **Communication**: TCP server using custom Colfer binary protocol
- **Version**: Check APP_VERSION in CMakeLists.txt

## Architecture

```text
Mobile Client (TCP) -> Server (Qt) -> Data Parser (Colfer) -> Executor -> Platform API (Windows: SendInput/WinRT, Linux: libevdev/uinput)
```

## Instructions

1. Never edit this file.
2. Build.md contains build instructions.
3. Do not modify anything in third-party-libs and doxygen-awesome-css.
4. Do not modify anything in VGP_Data_Exchange unless explicitly stated.

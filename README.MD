# VirtualGamePad Server

[![GitHub Downloads (all assets, all releases)](https://img.shields.io/github/downloads/kitswas/VirtualGamePad-PC/total)](https://github.com/kitswas/VirtualGamePad-PC/releases/latest)
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=kitswas_VirtualGamePad-PC&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=kitswas_VirtualGamePad-PC)

> [!IMPORTANT]
>
> - For installation, usage, troubleshooting, and FAQs, visit the [main project website](https://kitswas.github.io/VirtualGamePad/).
> - This README/website is focused on technical documentation for the server.

## History of the server's development

1. The server was originally written in C++.
2. Then I rewrote it in Rust with [windows-rs](https://github.com/microsoft/windows-rs) for Rust's memory safety and the newer Gamepad injection Windows support.
3. [Windows.UI.Input.Preview.Injection](https://learn.microsoft.com/en-us/uwp/api/windows.ui.input.preview.injection?view=winrt-22621), as the name says, is in preview and is [buggy](https://github.com/microsoft/microsoft-ui-xaml/issues/8639). _(Read unusable)_  
I, unfortunately, didn't know the source of the problems and decided to rewrite the app according to the Microsoft recommended way for Windows Apps (C# with WinUI 3 and the Windows App SDK).  
This is also when I figured that a GUI is worth the effort.
4. Finally, due to the aforementioned reasons, this was created in C++ with the Win32 API and Qt6 for the UI.
5. Experimental gamepad executor for Windows using `Windows.UI.Input.Preview.Injection` was added.
6. Linux support was introduced due to user requests.

## How it works

> [!NOTE]  
> **Design choice:** [Colfer](https://github.com/pascaldekloe/colfer) over Protocol Buffers.  
> Reason: [Performance](https://github.com/pascaldekloe/colfer/wiki/Benchmark) and simplicity.

1. Client-Server Communication:  
   The mobile client (Android app) connects to the server over TCP. Communication uses a [custom binary protocol](https://github.com/kitswas/VGP_Data_Exchange) for efficient, structured data exchange.

2. Input Parsing and Execution:  
   The server receives gamepad state from the client. These are parsed and mapped to system-level input events via an Executor.

   There are two Executors which you can choose from:
   - Keyboard/Mouse Executor: Maps gamepad buttons/axes to keyboard keys and mouse movements/clicks via user-defined keymap profiles. Use this for games without native gamepad support.
   - Gamepad Executor: Simulates a virtual gamepad device. You can run multiple instances of the server to create multiple virtual gamepads for local multiplayer.

   The Executor uses platform APIs:
   - Windows: Uses [SendInput()](https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-sendinput) for keyboard/mouse, and WinRT APIs for gamepad simulation.
   - Linux: Uses [`uinput`/`libevdev`](https://www.kernel.org/doc/html/v6.0/input/uinput.html) for input injection.

3. Keymap Profiles:  
   Users can define custom keymap profiles for different games or applications. Profiles are managed via the GUI and stored locally.

4. System-Level Input Injection:  
   The server synthesizes input events at the OS level, allowing control of any application. _No external drivers are needed._

5. Security and Privacy:  
   All communication is local (Wi-Fi/LAN). No ads, tracking, or telemetry. Source code is open for review.

## Source Layout

- src - Main source code (networking, simulation, UI, platform-specific code)
- res - Resources (like icons and logos)
- VGP_Data_Exchange - Communication protocol implementation
- third-party-libs - External libraries (e.g., QR code generator)
- docs - Doxygen documentation output
- cmake - CMake modules for platform setup

## Build Instructions

See [the build document](Build.md) for detailed build instructions.

### Generating Documentation

This project uses [Doxygen](https://www.doxygen.nl/index.html) to generate documentation.  
If Doxygen is available on your system,  
You can generate the documentation by running the following command:

```bash
doxygen Doxyfile
```

This repository has automated workflow via Github Actions.  
Documentation is auto-generated and deployed to Github Pages.

---

**For usage, downloads, and troubleshooting, visit the [main project website](https://kitswas.github.io/VirtualGamePad/).**

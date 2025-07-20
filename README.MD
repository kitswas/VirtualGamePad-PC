# VirtualGamePad Server

[![GitHub Downloads (all assets, all releases)](https://img.shields.io/github/downloads/kitswas/VirtualGamePad-PC/total)](https://github.com/kitswas/VirtualGamePad-PC/releases/latest)
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=kitswas_VirtualGamePad-PC&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=kitswas_VirtualGamePad-PC)

This is a part of [the VirtualGamePad project](https://kitswas.github.io/VirtualGamePad/).  
The source is available at <https://github.com/kitswas/VirtualGamePad-PC>.

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
    - Games that use DirectInput or XInput do not recognize input from the virtual gamepad as gamepad input but as keyboard and mouse input.

3. User defined keymaps are supported. You can create keymap profiles for different games or applications.

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

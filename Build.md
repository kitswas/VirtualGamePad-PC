# Build Instructions

## Prerequisites

### Common Requirements

- [Qt 6.8+](https://www.qt.io/download-qt-installer-oss) - Use the Qt Installer in the link. Choose the prebuilt components for your compiler under 'Qt'. Cmake, Qt Creator and Ninja are available under 'Tools'. (`mise run build` provisions Qt 6.11.0 via aqtinstall when no system Qt is found.)
- [CMake](https://cmake.org/download/) (pinned in `mise.toml` if you use mise)
- [Ninja](https://ninja-build.org/) (pinned in `mise.toml` if you use mise)
- [Git](https://git-scm.com/downloads)

### Linux (Ubuntu/Debian)

- Other development packages:

  ```bash
  sudo apt-get update
  sudo apt-get install -y libevdev-dev
  ```

## Quick Start

Optional: with [mise](https://mise.jdx.dev/) installed, `mise run doctor` checks prerequisites and `mise run build` configures and builds Release (using your system Qt if found, or provisioning Qt via `aqtinstall` if missing). `mise tasks ls` lists the rest.

1. **Clone the repository with submodules:**

   ```bash
   git clone --recurse-submodules https://github.com/kitswas/VirtualGamePad-PC.git
   cd VirtualGamePad-PC
   ```

2. **Configure Qt paths (if needed):**
   - With mise, `mise run build` skips this step: it writes `cmake_cache_<platform>.cmake` for the Qt it finds or provisions.
   - Otherwise, copy the appropriate sample cache file:
     - Windows: Copy `cmake_cache_windows.cmake.sample` to `cmake_cache_windows.cmake`
     - Linux: Copy `cmake_cache_linux.cmake.sample` to `cmake_cache_linux.cmake`
   - Edit the cache file to match your Qt installation path

3. **Choose Build Mode (Optional):**

   By default, the project builds in **portable mode** where settings are stored in the application directory (the parent of the directory containing `bin/`).

   For **installable mode** (settings in standard OS locations like `~/.config` or `%LOCALAPPDATA%`), add `-DPORTABLE_BUILD=OFF`:

   Windows:

   ```bash
   cmake --preset windows -DPORTABLE_BUILD=OFF
   ```

   Linux:

   ```bash
   cmake --preset linux -DPORTABLE_BUILD=OFF
   ```

4. **Configure and Build:**

   Windows:

   ```bash
   cmake --preset windows # -C cmake_cache_windows.cmake if needed
   cmake --build build-windows --config Release
   ```

   Linux:

   ```bash
   cmake --preset linux # -C cmake_cache_linux.cmake if needed
   cmake --build build-linux --config Release
   ```

5. **Package for distribution:**

   Windows:

   ```powershell
   mkdir dist
   cmake --install build-windows --prefix "$PWD/dist" --config Release
   ```

   Linux:

   ```bash
   mkdir dist
   cmake --install build-linux --prefix $PWD/dist --config Release
   ```

## Development Builds

For debug builds, replace config `Release` with `Debug` in the build commands.

## IDE Support

### Qt Creator

- Open `CMakeLists.txt` in Qt Creator
- Configure the project
- Build and run from the IDE

### Visual Studio Code

- Install [Qt C++ Extension Pack](https://marketplace.visualstudio.com/items?itemName=TheQtCompany.qt-cpp-pack)
- Run 'Qt: Register Qt installation' after installing the extension (Ctrl+Shift+P)
- Open the project folder in VS Code

## Code Formatting

Format code using clang-format:

```bash
cmake --build build --target format
```

**Note: This requires clang-format to be installed and available in the PATH.**

## Git Hooks

Enable the repository hooks once per clone:

```bash
git config core.hooksPath .githooks
```

They enforce running the formatter before committing:

- `pre-commit`: clang-format check of the staged C/C++ files - blocks the commit
- `post-commit`: auto-formats the just-committed C/C++ files, leaving fixes as unstaged changes to review
- `pre-push`: clang-format check of every tracked C/C++ source - blocks the push
- `post-merge`: full build via `mise build` after merges/pulls (advisory only)

Bypass a hook with `--no-verify`; disable all of them with `git config --unset core.hooksPath`.

## Documentation

Generate documentation with Doxygen:

```bash
doxygen Doxyfile
```

## Troubleshooting

### Windows Issues

The gamepad executor requires administrative privileges and enabling [app sideloading (Developer mode in Windows Settings)](ms-settings:developers).

### Linux Permission Issues

If you encounter `/dev/uinput` permission errors, grant access to the active local user with `uaccess` (same approach as Steam's `60-steam-input.rules`). No group or re-login needed:

```bash
# Create udev rule (keep the 60- prefix: it must sort before 73-seat-late.rules)
printf '%s\n' 'KERNEL=="uinput", SUBSYSTEM=="misc", TAG+="uaccess", OPTIONS+="static_node=uinput"' | sudo tee /etc/udev/rules.d/60-virtualgamepad.rules

# Reload udev and trigger
sudo udevadm control --reload-rules
sudo udevadm trigger /dev/uinput

# Verify your user has an ACL entry
getfacl /dev/uinput
```

`uaccess` only works for local seat sessions. For SSH or headless sessions, use a group instead:

```bash
sudo groupadd uinput
sudo usermod -a -G uinput $USER
printf '%s\n' 'KERNEL=="uinput", SUBSYSTEM=="misc", MODE="0660", GROUP="uinput"' | sudo tee /etc/udev/rules.d/60-virtualgamepad.rules
sudo udevadm control --reload-rules
sudo udevadm trigger /dev/uinput
# Log out and back in
```

If the module is not loaded, run `sudo modprobe uinput`. To load it at boot, add `uinput` to `/etc/modules`.

### Avoiding Common Issues

- Update the CMake cache files with correct Qt installation paths
- For Windows, ensure MinGW or MSVC compiler matches your Qt installation (or use the bundled Qt MinGW)

### Build Artifacts Location

- **Windows and Linux**: `./dist/` contains the full application structure
  (`bin/` has the executable, Qt libraries and `qt.conf`; Qt plugins and
  translations are in the sibling `plugins/` and `translations/` folders)

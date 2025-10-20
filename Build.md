# Build Instructions

## Prerequisites

### Common Requirements

- [Qt 6.8+](https://www.qt.io/download-qt-installer-oss) - Use the Qt Installer in the link. Choose the prebuilt components for your compiler under 'Qt'. Cmake, Qt Creator and Ninja are available under 'Tools'.
- [CMake](https://cmake.org/download/)
- [Ninja](https://ninja-build.org/)
- [Git](https://git-scm.com/downloads)

### Linux (Ubuntu/Debian)

- Other development packages:

  ```bash
  sudo apt-get update
  sudo apt-get install -y libevdev-dev
  ```

## Quick Start

1. **Clone the repository with submodules:**

   ```bash
   git clone --recurse-submodules https://github.com/kitswas/VirtualGamePad-PC.git
   cd VirtualGamePad-PC
   ```

2. **Configure Qt paths (if needed):**
   - Copy the appropriate sample cache file:
     - Windows: Copy `cmake_cache_windows.cmake.sample` to `cmake_cache_windows.cmake`
     - Linux: Copy `cmake_cache_linux.cmake.sample` to `cmake_cache_linux.cmake`
   - Edit the cache file to match your Qt installation path

3. **Configure and Build:**

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

4. **Package for distribution:**

   Windows:

   ```powershell
   mkdir dist
   cmake --install build-windows --prefix dist --config Release
   ```

   Linux:

   ```bash
   mkdir dist
   cmake --install build-linux --prefix $PWD/dist --config Release
   ```

   Windows build artifacts will be in `./dist/bin/` and Linux build artifacts will be in `./dist/`.

   To create an installer with CPack:

   Windows (NSIS installer):

   ```powershell
   cmake --build build-windows --config Release
   cmake --build build-windows --target package
   ```

   Linux (DEB/RPM package):

   ```bash
   cmake --build build-linux --config Release
   cmake --build build-linux --target package
   ```

   The installer/package will be generated in the build directory (e.g., `.exe`, `.deb`, or `.rpm`).

### Signing Windows Installers

You can sign the Windows installer (`.exe`) and export the public certificate and fingerprint using either a PFX file (command line) or environment variables.

#### 1. Command Line PFX File

Pass the installer path, PFX file, and password to CMake:

```powershell
cmake --preset windows -DFILE_TO_SIGN="build-windows/VirtualGamePad-PC--installer.exe" -DPFX_FILE="D:/path/to/kitswas.pfx" -DPFX_PASSWORD="password"
cmake --build build-windows --config Release
cmake --build build-windows --target package
cmake --build build-windows --target sign-installer
```

#### 2. Environment Variable (Base64)

Set the PFX as a base64 string and password in your environment, and provide the installer path:

```powershell
$env:PFX_BASE64 = "<base64-encoded-pfx>"
$env:PFX_PASSWORD = "yourpassword"
cmake --preset windows -DFILE_TO_SIGN="build-windows/VirtualGamePad-PC--installer.exe"
cmake --build build-windows --config Release
cmake --build build-windows --target package
cmake --build build-windows --target sign-installer-env
```

After signing, the following files will be generated in the installer output directory:

- `installer.cer` (public certificate)
- `installer.cer.sha256` (SHA256 fingerprint)
- `installer.cer.info.txt` (certificate info)

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
cmake --build build-windows --target format # or build-linux
```

**Note: This requires clang-format to be installed and available in the PATH.**

## Documentation

Generate documentation with Doxygen:

```bash
doxygen Doxyfile
```

## Troubleshooting

### Windows Issues

The gamepad executor requires administrative privileges and enabling [app sideloading (Developer mode in Windows Settings)](ms-settings:developers).

### Linux Permission Issues

If you encounter `/dev/uinput` permission errors, run:

```bash
# Add user to uinput group
sudo groupadd uinput
sudo usermod -a -G uinput $USER

# Create udev rule
sudo sh -c 'echo KERNEL=="uinput", SUBSYSTEM=="misc", MODE="0660", GROUP="uinput" > /etc/udev/rules.d/99-uinput.rules'

# Reload udev and trigger
sudo udevadm control --reload-rules
sudo udevadm trigger /dev/uinput

# Log out and back in, or restart
```

### Avoiding Common Issues

- Update the CMake cache files with correct Qt installation paths
- For Windows, ensure MinGW or MSVC compiler matches your Qt installation (or use the bundled Qt MinGW)

### Build Artifacts Location

- **Windows**: `./dist/bin/` contains the executable and dependencies
- **Linux**: `./dist/` contains the full application structure

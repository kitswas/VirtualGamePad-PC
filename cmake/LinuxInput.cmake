# LinuxInput.cmake - Find and configure Linux input dependencies
#
# This module finds the required libraries for Linux input injection:
# - libevdev
#
# Variables set by this module:
# - UINPUT_FOUND - True if uinput support is available
# - UINPUT_LIBRARIES - Libraries to link against
# - UINPUT_INCLUDE_DIRS - Include directories
# - EVDEV_FOUND - True if evdev support is available
# - EVDEV_LIBRARIES - evdev libraries to link against
# - EVDEV_INCLUDE_DIRS - evdev include directories

if(NOT UNIX)
    message(FATAL_ERROR "LinuxInput.cmake should only be included on Linux systems")
endif()

# Find pkg-config (required for finding libraries)
find_package(PkgConfig REQUIRED)

# Find libevdev (for input event handling)
pkg_check_modules(LIBEVDEV REQUIRED libevdev)
if(LIBEVDEV_FOUND)
    message(STATUS "Found libevdev: ${LIBEVDEV_VERSION}")
    set(UINPUT_LIBRARIES ${LIBEVDEV_LIBRARIES})
    set(UINPUT_INCLUDE_DIRS ${LIBEVDEV_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "libevdev is required for Linux input simulation. Please install libevdev-dev package.")
endif()

# Check for uinput kernel headers
find_path(UINPUT_HEADER_DIR
    NAMES linux/uinput.h
    PATHS /usr/include /usr/local/include
    DOC "Path to linux/uinput.h header"
)

if(NOT UINPUT_HEADER_DIR)
    message(FATAL_ERROR "Could not find linux/uinput.h. Please install linux-libc-dev or kernel headers package.")
endif()

# Check for input.h header
find_path(INPUT_HEADER_DIR
    NAMES linux/input.h
    PATHS /usr/include /usr/local/include
    DOC "Path to linux/input.h header"
)

if(NOT INPUT_HEADER_DIR)
    message(FATAL_ERROR "Could not find linux/input.h. Please install linux-libc-dev or kernel headers package.")
endif()

# Combine include directories
list(APPEND UINPUT_INCLUDE_DIRS ${UINPUT_HEADER_DIR} ${INPUT_HEADER_DIR})

# Check for /dev/uinput accessibility and provide helpful warnings
if(EXISTS "/dev/uinput")
    execute_process(
        COMMAND test -r /dev/uinput -a -w /dev/uinput
        RESULT_VARIABLE UINPUT_ACCESS_CHECK
        OUTPUT_QUIET
        ERROR_QUIET
    )
    
    if(NOT UINPUT_ACCESS_CHECK EQUAL 0)
        message(STATUS "Found /dev/uinput device")
        message(WARNING 
            "Cannot access /dev/uinput with current permissions. "
            "You may need to:\n"
            "  1. Create the uinput group if it doesn't exist: sudo groupadd uinput\n"
            "  2. Add your user to the 'uinput' group: sudo usermod -a -G uinput $USER\n"
            "  3. Make it persistent with this command:\n"
            "sudo sh -c 'echo KERNEL==\\\"uinput\\\", SUBSYSTEM==\\\"misc\\\", MODE=\\\"0660\\\", GROUP=\\\"uinput\\\" > /etc/udev/rules.d/99-uinput.rules'\n"
            "  4. Restart udev:\n"
            "sudo udevadm control --reload-rules\n"
            "sudo udevadm trigger /dev/uinput\n"
            "  5. Log out and back in\n"
            "  6. Or run the application with sudo (not recommended for security)"
            "  7. Or, for quick and dirty access, run:\nsudo chmod 666 /dev/uinput")
    else()
        message(STATUS "Found /dev/uinput device with proper access")
    endif()
else()
    message(WARNING 
        "/dev/uinput not found. You may need to:\n"
        "  1. Load the uinput kernel module: sudo modprobe uinput\n"
        "  2. Make it persistent: echo 'uinput' | sudo tee -a /etc/modules")
endif()

# Add preprocessor definitions for Linux build
add_definitions(-DLINUX_BUILD)

message(STATUS "Linux input dependencies configured successfully")
message(STATUS "  - libevdev version: ${LIBEVDEV_VERSION}")
message(STATUS "  - Include dirs: ${UINPUT_INCLUDE_DIRS}")
message(STATUS "  - Libraries: ${UINPUT_LIBRARIES}")

# Note: Users may need to add themselves to the 'input' group or run as root
# to access /dev/uinput.

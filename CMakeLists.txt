cmake_minimum_required(VERSION 3.16)

set(APP_VERSION 0.3.0)
add_definitions(-DAPP_VERSION="${APP_VERSION}")

project("Virtual Gamepad PC" VERSION ${APP_VERSION} LANGUAGES CXX C)

# Platform validation
if (NOT WIN32 AND NOT LINUX)
    message(FATAL_ERROR "This project only supports Windows and Linux")
endif()

# Add clang-format target for code formatting
find_program(CLANG_FORMAT "clang-format")
if(CLANG_FORMAT)
    # Get all source files
    file(GLOB_RECURSE ALL_SOURCE_FILES 
        "${CMAKE_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_SOURCE_DIR}/src/*.hpp"
        "${CMAKE_SOURCE_DIR}/src/*.h"
    )
    
    add_custom_target(format
        COMMAND ${CLANG_FORMAT} -style=file -i ${ALL_SOURCE_FILES}
        COMMENT "Formatting source code with clang-format"
        VERBATIM
    )
    
    message(STATUS "clang-format found: ${CLANG_FORMAT}")
    message(STATUS "Format target added. Use 'cmake --build build --target format' to format the code.")
else()
    message(STATUS "clang-format not found. Format target not created.")
endif()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

# Platform-specific setup
if(WIN32)
    # Include CppWinRT setup for Windows
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CppWinRT.cmake)
elseif(LINUX)
    # Include Linux input dependencies
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/LinuxInput.cmake)
endif()

add_subdirectory(VGP_Data_Exchange/C)

set(QR_CODE_GEN_SOURCES
    third-party-libs/QR-Code-generator/cpp/qrcodegen.cpp
    third-party-libs/QR-Code-generator/cpp/qrcodegen.hpp
)

add_library(QR_Code_Generator SHARED ${QR_CODE_GEN_SOURCES})

# Manifest file for the application
# This file is used to request permissions on Windows
SET(APP_MANIFEST_FILE "${CMAKE_SOURCE_DIR}/src/VGamepadPC.exe.manifest")

# Add project files sorted in alphabetical order
set(PROJECT_SOURCES
    res/icons.qrc
    src/main.cpp
    src/networking/executor.cpp
    src/networking/executor.hpp
    src/networking/server.cpp
    src/networking/server.hpp
    src/networking/server.ui
    src/settings/settings.hpp
    src/settings/settings_singleton.cpp
    src/settings/settings_singleton.hpp
    src/settings/keymap_profile.hpp
    src/settings/keymap_profile.cpp
    src/simulation/gamepadSim.hpp
    src/simulation/keyboardSim.hpp
    src/simulation/mouseSim.hpp
    src/ui/about.cpp
    src/ui/about.hpp
    src/ui/about.ui
    src/ui/badge.cpp
    src/ui/badge.hpp
    src/ui/buttoninputbox.cpp
    src/ui/buttoninputbox.hpp
    src/ui/mainmenu.cpp
    src/ui/mainmenu.hpp
    src/ui/mainmenu.ui
    src/ui/mainwindow.cpp
    src/ui/mainwindow.hpp
    src/ui/mainwindow.ui
    src/ui/preferences.cpp
    src/ui/preferences.hpp
    src/ui/preferences.ui
)

# Platform-specific simulation sources
if(WIN32)
    list(APPEND PROJECT_SOURCES
        src/simulation/windows/gamepadSim.cpp
        src/simulation/windows/keyboardSim.cpp
        src/simulation/windows/mouseSim.cpp
    )
elseif(LINUX)
    list(APPEND PROJECT_SOURCES
        src/simulation/linux/gamepadSim.cpp
        src/simulation/linux/keyboardSim.cpp
        src/simulation/linux/mouseSim.cpp
    )
endif()

qt_add_executable(VGamepadPC
    ${PROJECT_SOURCES}
)

target_link_libraries(VGamepadPC PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
    Data_Exchange
    QR_Code_Generator
)

# Platform-specific linking and include directories
if(WIN32)
    target_link_libraries(VGamepadPC PRIVATE
        cppwinrt
        WindowsApp
    )
elseif(LINUX)
    target_link_libraries(VGamepadPC PRIVATE
        ${UINPUT_LIBRARIES}
    )
    target_include_directories(VGamepadPC PRIVATE
        ${UINPUT_INCLUDE_DIRS}
    )
endif()

set_target_properties(VGamepadPC PROPERTIES
    WIN32_EXECUTABLE TRUE
)

set(CMAKE_INSTALL_DIR "${CMAKE_SOURCE_DIR}/dist")

install(TARGETS VGamepadPC Data_Exchange QR_Code_Generator
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Platform-specific installation
if(WIN32)
    # Manifest file for the application
    # This file is used to request permissions on Windows
    SET(APP_MANIFEST_FILE "${CMAKE_SOURCE_DIR}/src/VGamepadPC.exe.manifest")
    
    install(FILES "${APP_MANIFEST_FILE}" 
        DESTINATION "${CMAKE_INSTALL_BINDIR}"
    )

    install(
        CODE "execute_process(COMMAND \"${WINDEPLOYQT_EXECUTABLE}\" \"\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/VGamepadPC.exe\")"
    )
elseif(LINUX)
    # For Linux, we handle packaging via AppImage
    # cmake --build build --target appimage
    # Include AppImage packaging for Linux
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/AppImagePackaging.cmake)
endif()

# Platform-specific post-build steps
if(WIN32)
    # Manifest file for the application
    # This file is used to request permissions on Windows
    SET(APP_MANIFEST_FILE "${CMAKE_SOURCE_DIR}/src/VGamepadPC.exe.manifest")

    # On Windows, we need to copy the DLLs to the output directory using windeployqt
    # This is done by adding a custom command as a post-build step
    # The CMake Cache variable WINDEPLOYQT_EXECUTABLE should contain the path to windeployqt
    add_custom_command(
        TARGET VGamepadPC POST_BUILD
        COMMAND ${WINDEPLOYQT_EXECUTABLE} $<TARGET_FILE:VGamepadPC>
        COMMENT "Running windeployqt to copy Qt dependencies to output directory"
        VERBATIM
    )

    # Configure the executable to use the manifest
    # Just place the manifest in the output directory and Windows will use it
    add_custom_command(
        TARGET VGamepadPC POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${APP_MANIFEST_FILE}
            ${CMAKE_BINARY_DIR}/VGamepadPC.exe.manifest
        COMMENT "Copying manifest file to output directory"
        VERBATIM
    )
endif()

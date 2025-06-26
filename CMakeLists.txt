cmake_minimum_required(VERSION 3.16)

set(APP_VERSION 0.3.0)
add_definitions(-DAPP_VERSION="${APP_VERSION}")

project("Virtual Gamepad PC" VERSION ${APP_VERSION} LANGUAGES CXX C)

if (NOT WIN32)
    message(FATAL_ERROR "This project is only supported on Windows")
endif()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

# Include CppWinRT setup
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CppWinRT.cmake)

add_subdirectory(VGP_Data_Exchange/C)

set(QR_CODE_GEN_SOURCES
    third-party-libs/QR-Code-generator/cpp/qrcodegen.cpp
    third-party-libs/QR-Code-generator/cpp/qrcodegen.hpp
)

add_library(QR_Code_Generator SHARED ${QR_CODE_GEN_SOURCES})

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
    src/simulation/simulate.cpp
    src/simulation/simulate.hpp
    src/simulation/gamepadSim.cpp
    src/simulation/gamepadSim.hpp
    src/simulation/keyboardSim.cpp
    src/simulation/keyboardSim.hpp
    src/simulation/mouseSim.cpp
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

qt_add_executable(VGamepadPC
    ${PROJECT_SOURCES}
)

target_link_libraries(VGamepadPC PRIVATE 
    Qt${QT_VERSION_MAJOR}::Widgets 
    Qt${QT_VERSION_MAJOR}::Network 
    Data_Exchange 
    QR_Code_Generator
    cppwinrt
    WindowsApp
)

set_target_properties(VGamepadPC PROPERTIES
    WIN32_EXECUTABLE TRUE
)

install(TARGETS VGamepadPC Data_Exchange QR_Code_Generator
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
    CODE "execute_process(COMMAND \"${WINDEPLOYQT_EXECUTABLE}\" \"\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/VGamepadPC.exe\")"
)

# On Windows, we need to copy the DLLs to the output directory using windeployqt
# This is done by adding a custom command as a post-build step
# The CMake Cache variable WINDEPLOYQT_EXECUTABLE should contain the path to windeployqt
add_custom_command(
    TARGET VGamepadPC POST_BUILD
    COMMAND ${WINDEPLOYQT_EXECUTABLE} $<TARGET_FILE:VGamepadPC>
)

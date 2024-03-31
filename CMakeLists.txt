cmake_minimum_required(VERSION 3.5)

project("Virtual Gamepad PC" VERSION 0.1 LANGUAGES CXX C)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

add_subdirectory(VGP_Data_Exchange/C)

set(THIRD_PARTY_LIB_SOURCES
    third-party-libs/QR-Code-generator/cpp/qrcodegen.cpp
    third-party-libs/QR-Code-generator/cpp/qrcodegen.hpp
)

add_library(QR_Code_Generator SHARED ${THIRD_PARTY_LIB_SOURCES})

# Add project files sorted in alphabetical order
set(PROJECT_SOURCES
    res/icons.qrc
    src/main.cpp
    src/ui/mainwindow.cpp
    src/ui/mainwindow.hpp
    src/ui/mainwindow.ui
    src/simulation/mouseSim.cpp
    src/simulation/mouseSim.hpp
    src/simulation/keyboardSim.cpp
    src/simulation/keyboardSim.hpp
    src/simulation/simulate.cpp
    src/simulation/simulate.hpp
    src/networking/server.cpp
    src/networking/server.hpp
    src/networking/server.ui
    src/networking/executor.cpp
    src/networking/executor.hpp
    src/ui/preferences.cpp
    src/ui/preferences.hpp
    src/ui/preferences.ui
    src/settings.cpp
    src/settings.hpp
    src/settings_key_variables.cpp
    src/settings_key_variables.hpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(VGamepadPC
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )

# Define target properties for Android with Qt 6 as:
# set_property(TARGET VGamepadPC APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
# ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(VGamepadPC SHARED
            ${PROJECT_SOURCES}
        )

    # Define properties for Android with Qt 5 after find_package() calls as:
    # set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(VGamepadPC
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(VGamepadPC PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network Data_Exchange QR_Code_Generator)

set_target_properties(VGamepadPC PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER io.github.kitswas.VGamepadPC
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS VGamepadPC
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(VGamepadPC)
endif()

# On Windows, we need to copy the DLLs to the output directory using windeployqt
# This is done by adding a custom command as a post-build step
# The CMake Cache variable WINDEPLOYQT_EXECUTABLE should contain the path to windeployqt
if(WIN32)
    add_custom_command(
        TARGET VGamepadPC POST_BUILD
        MAIN_DEPENDENCY VGamepadPC
        COMMAND ${WINDEPLOYQT_EXECUTABLE} $<TARGET_FILE:VGamepadPC>
    )
endif()

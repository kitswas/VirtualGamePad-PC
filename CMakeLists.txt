cmake_minimum_required(VERSION 3.19)

set(APP_VERSION 0.4.0)
set(APP_NAME "Virtual Gamepad PC")
add_definitions(-DAPP_VERSION="${APP_VERSION}")

project("${APP_NAME}" VERSION ${APP_VERSION} LANGUAGES CXX C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
    message(STATUS "Format target added. Use 'cmake --build <build-dir> --target format' to format the code.")
else()
    message(STATUS "clang-format not found. Format target not created.")
endif()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
# Platform-specific setup
if(WIN32)
    # Include CppWinRT setup for Windows
    include(CppWinRT)
elseif(LINUX)
    # Include Linux input dependencies
    include(LinuxInput)
endif()

add_subdirectory(VGP_Data_Exchange/C)

set(QR_CODE_GEN_SOURCES
    third-party-libs/QR-Code-generator/cpp/qrcodegen.cpp
    third-party-libs/QR-Code-generator/cpp/qrcodegen.hpp
)

add_library(QR_Code_Generator SHARED ${QR_CODE_GEN_SOURCES})

# Manifest file for the application
# This file is used to request permissions on Windows
SET(APP_MANIFEST_FILE "${CMAKE_SOURCE_DIR}/res/VGamepadPC.exe.manifest")

# Add project files sorted in alphabetical order
set(PROJECT_SOURCES
    res/icons.qrc
    src/appdir.hpp
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
        src/platform/windows/console.cpp
        src/platform/windows/console.hpp
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
    Qt${QT_VERSION_MAJOR}::Core
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

# Platform-specific resources/tools
if(WIN32)
    # Manifest file for the application
    # This file is used to request permissions on Windows
    SET(APP_MANIFEST_FILE "${CMAKE_SOURCE_DIR}/res/VGamepadPC.exe.manifest")

    # Windows deployment tool
    if(NOT DEFINED WINDEPLOYQT_EXECUTABLE)
        # Find the windeployqt executable in the Qt installation path
        # This is typically located in the bin directory of the Qt installation
        # Adjust the path based on your Qt installation
        find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS ${CMAKE_PREFIX_PATH}/bin)
    endif()
    if(WINDEPLOYQT_EXECUTABLE)
        message(STATUS "Found windeployqt: ${WINDEPLOYQT_EXECUTABLE}")
    endif()
endif()

# Platform-specific installation
if(WIN32)
    install(FILES "${APP_MANIFEST_FILE}" 
        DESTINATION "${CMAKE_INSTALL_BINDIR}"
    )

    install(
        CODE "execute_process(COMMAND \"${WINDEPLOYQT_EXECUTABLE}\" \"\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/VGamepadPC.exe\")"
    )
elseif(LINUX)
    # https://cmake.org/cmake/help/latest/prop_tgt/INSTALL_RPATH.html
    set_target_properties(VGamepadPC PROPERTIES
        INSTALL_RPATH_USE_LINK_PATH TRUE
        INSTALL_RPATH "$ORIGIN/../lib"
        BUILD_WITH_INSTALL_RPATH FALSE
    )
    qt_generate_deploy_app_script(
        TARGET VGamepadPC
        OUTPUT_SCRIPT deploy_script
    )
    message(STATUS "Deploy script generated: ${deploy_script}")
    install(SCRIPT ${deploy_script})
endif()

# Platform-specific post-build steps
if(WIN32)
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
            $<TARGET_FILE_DIR:VGamepadPC>/VGamepadPC.exe.manifest
        COMMENT "Copying manifest file to output directory"
        VERBATIM
    )
endif()


# Installer packaging with CPack
include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_NAME "VirtualGamePad-PC")
set(CPACK_PACKAGE_VERSION "${APP_VERSION}")
set(CPACK_PACKAGE_CONTACT "kitswas")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Cross-platform server to use mobile devices as virtual game controllers for PC.")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENCE.TXT")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.MD")

if(WIN32)
    include(PortableNSIS)
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_NSIS_DISPLAY_NAME "VirtualGamePad-PC")
    set(CPACK_NSIS_CONTACT "kitswas")
    set(CPACK_NSIS_MODIFY_PATH ON)
elseif(UNIX)
    set(CPACK_GENERATOR "DEB;RPM")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "kitswas")
endif()

include(CPack)
message(STATUS "Create installer with: cmake --build <build-dir> --target package")

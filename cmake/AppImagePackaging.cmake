# AppImagePackaging.cmake - Download appimagetool and create AppImage packages
#
# This module adds targets for creating AppImage packages on Linux:
# - appimage_prepare: Creates the AppDir structure with all required files
# - appimage: Creates the actual AppImage file using appimagetool
#
# Usage: include this file in your CMakeLists.txt when targeting Linux

if(NOT UNIX OR LINUX)
    message(FATAL_ERROR "AppImagePackaging.cmake should only be included on Linux systems")
endif()

# Set AppImage variables
set(APPIMAGE_NAME "VGamepadPC")
set(APPDIR "${CMAKE_INSTALL_DIR}/AppDir")
set(APPIMAGE_TOOL_DIR "${CMAKE_BINARY_DIR}/appimage-tool")

# Function to download appimagetool if not present
function(download_appimagetool)
    if(EXISTS "${APPIMAGE_TOOL_DIR}/appimagetool-x86_64.AppImage")
        message(STATUS "appimagetool already downloaded")
        return()
    endif()
    
    message(STATUS "Downloading appimagetool...")
    file(MAKE_DIRECTORY "${APPIMAGE_TOOL_DIR}")
    
    # Download appimagetool using CMake's file(DOWNLOAD)
    file(DOWNLOAD 
        "https://github.com/probonopd/go-appimage/releases/download/continuous/appimagetool-891-x86_64.AppImage"
        "${APPIMAGE_TOOL_DIR}/appimagetool-x86_64.AppImage"
        EXPECTED_HASH SHA256=dda846cec1135fb978da17050a734607453247d39fab8f6adf0c9e860205adda
        SHOW_PROGRESS
        STATUS DOWNLOAD_STATUS
    )
    
    list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
    if(NOT STATUS_CODE EQUAL 0)
        list(GET DOWNLOAD_STATUS 1 ERROR_MESSAGE)
        message(FATAL_ERROR "Failed to download appimagetool: ${ERROR_MESSAGE}")
        return()
    endif()
    
    # Make the downloaded file executable
    execute_process(
        COMMAND chmod +x "${APPIMAGE_TOOL_DIR}/appimagetool-x86_64.AppImage"
        RESULT_VARIABLE CHMOD_RESULT
    )
    
    if(NOT CHMOD_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to make appimagetool executable")
        return()
    endif()
    
    message(STATUS "appimagetool downloaded successfully")
endfunction()

# Call the function to download appimagetool
download_appimagetool()

# Create desktop file for the application
set(DESKTOP_FILE "${CMAKE_BINARY_DIR}/VGamepadPC.desktop")
file(WRITE ${DESKTOP_FILE}
"[Desktop Entry]
Name=Virtual Gamepad PC
Comment=Control your PC with a virtual gamepad from your mobile device
Exec=VGamepadPC
Icon=VGamepadPC
Terminal=false
Type=Application
Categories=Utility;Game;
")

# Create AppRun script
file(WRITE "${CMAKE_BINARY_DIR}/AppRun"
"#!/bin/bash
HERE=$(dirname \"$(readlink -f \"${0}\")\")
export LD_LIBRARY_PATH=${HERE}/usr/lib:${LD_LIBRARY_PATH}
exec \"${HERE}/usr/bin/VGamepadPC\" \"$@\"
")

# Set execute permissions on the AppRun script
file(CHMOD "${CMAKE_BINARY_DIR}/AppRun" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

# Add a custom target to prepare the AppDir structure
add_custom_target(appimage_prepare ALL
    DEPENDS VGamepadPC
    COMMENT "Preparing AppDir structure"
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${APPDIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${APPDIR}/usr/bin
    COMMAND ${CMAKE_COMMAND} -E make_directory ${APPDIR}/usr/lib
    COMMAND ${CMAKE_COMMAND} -E make_directory ${APPDIR}/usr/share/applications
    COMMAND ${CMAKE_COMMAND} -E make_directory ${APPDIR}/usr/share/icons/hicolor/256x256/apps
    
    # Copy the executable
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:VGamepadPC> ${APPDIR}/usr/bin/
    
    # Copy shared libraries built by the project
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Data_Exchange> ${APPDIR}/usr/lib/
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:QR_Code_Generator> ${APPDIR}/usr/lib/
    
    # Copy the desktop file
    COMMAND ${CMAKE_COMMAND} -E copy ${DESKTOP_FILE} ${APPDIR}/usr/share/applications/VGamepadPC.desktop
    
    # Extract icon from Qt resources (if available) or copy from source
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/res/logos/GamepadIcon.png ${APPDIR}/usr/share/icons/hicolor/256x256/apps/VGamepadPC.png
    
    # Copy the AppRun script
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/AppRun ${APPDIR}/AppRun
)

# Get Qt prefix path from Qt installation
get_filename_component(QT_PREFIX_PATH "${Qt6_DIR}/../../.." ABSOLUTE)
message(STATUS "Qt prefix path for AppImage: ${QT_PREFIX_PATH}")

add_custom_target(appimage
    DEPENDS appimage_prepare
    COMMENT "Creating AppImage package"
    COMMAND ${CMAKE_COMMAND} -E env QTDIR=${QT_PREFIX_PATH} ${APPIMAGE_TOOL_DIR}/appimagetool-x86_64.AppImage -s deploy ${APPDIR}/usr/share/applications/VGamepadPC.desktop
    COMMAND ${CMAKE_COMMAND} -E env QTDIR=${QT_PREFIX_PATH} ${APPIMAGE_TOOL_DIR}/appimagetool-x86_64.AppImage ${APPDIR}
)

message(STATUS "AppImage packaging configured. Use 'cmake --build build --target appimage' to create an AppImage")

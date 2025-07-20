# CMake cache file for Windows builds
# This file contains platform-specific CMake variables

# Qt6 configuration for Windows
# Adjust the path based on your Qt installation
set(QT_VERSION "6.8.3")
set(QT_COMPILER "mingw_64") # or "msvc2022_64" based on your compiler
set(CMAKE_PREFIX_PATH "D:/Qt/${QT_VERSION}/${QT_COMPILER}" CACHE STRING "Qt installation path")
message(STATUS "Using Qt6 from: ${CMAKE_PREFIX_PATH}")

# MinGW configuration (to ensure the correct compiler is used)
set(QT_BUNDLED_MINGW "D:/Qt/Tools/mingw1310_64")
set(CMAKE_C_COMPILER "${QT_BUNDLED_MINGW}/bin/gcc.exe" CACHE FILEPATH "C compiler")
set(CMAKE_CXX_COMPILER "${QT_BUNDLED_MINGW}/bin/g++.exe" CACHE FILEPATH "C++ compiler")

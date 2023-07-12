cmake_minimum_required(VERSION 3.16)
project(CPP_project)

set(CMAKE_CXX_STANDARD 20)

# Some compiler flags for GCC and Clang, disable if unwanted
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU") 
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wconversion -Wold-style-cast -Wnull-dereference")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -DDEBUG")
	# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Werror") # Uncomment to treat warnings as errors
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -s -DNDEBUG")
endif()

set(SOURCES src/main.cpp)
add_executable(executable ${SOURCES})

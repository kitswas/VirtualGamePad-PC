# OpenSSFHardening.cmake
#
# CMake module to apply OpenSSF-recommended compiler and linker hardening.
# https://best.openssf.org/Compiler-Hardening-Guides/Compiler-Options-Hardening-Guide-for-C-and-C++
#
# Usage:
#   include(OpenSSFHardening)
#   add_executable(myApp main.cpp) # Or add_library(myLib ...)
#   openssf_harden_target(myApp)

# Prevent multiple inclusion
if(DEFINED OpenSSF_Hardening_INCLUDED)
	return()
endif()
set(OpenSSF_Hardening_INCLUDED TRUE)

# Ensure target_link_options() is available (CMake ≥ 3.13)
if(NOT COMMAND target_link_options)
	message(FATAL_ERROR "OpenSSFHardening.cmake requires CMake ≥ 3.13 for target_link_options()")
endif()


# 1. Feature tests
include(CheckCXXCompilerFlag)
include(CheckLinkerFlag)
check_cxx_compiler_flag("-ftrivial-auto-var-init=zero"    OpenSSF_HARDENING_HAS_TRIVIAL_AUTO_INIT)
check_cxx_compiler_flag("-fno-delete-null-pointer-checks" OpenSSF_HARDENING_HAS_NO_DELETE_NULL)
check_cxx_compiler_flag("-fhardened"                      OpenSSF_HARDENING_HAS_HARDENED)
check_cxx_compiler_flag("-fsanitize=undefined"            OpenSSF_HARDENING_HAS_UB_SAN)
check_linker_flag(CXX "-fsanitize=undefined"              OpenSSF_HARDENING_LINK_HAS_UB_SAN)

# 2. Base hardening flags (C & C++)
set(OpenSSF_Hardening_FLAGS
	-Wall
	-Wextra
	-Wpedantic
	-Wformat=2
	-Wconversion
	-Wimplicit-fallthrough
	-Wnull-dereference
	-Werror=format-security
	-fstack-clash-protection
	-fstack-protector-strong
	-fno-strict-aliasing
	-fno-strict-overflow
)
set(OpenSSF_Hardening_LINK_FLAGS)

if(OpenSSF_HARDENING_HAS_TRIVIAL_AUTO_INIT)
	list(APPEND OpenSSF_Hardening_FLAGS
		-ftrivial-auto-var-init=zero
	)
endif()
if(OpenSSF_HARDENING_HAS_NO_DELETE_NULL)
	list(APPEND OpenSSF_Hardening_FLAGS
		-fno-delete-null-pointer-checks
	)
endif()
if(OpenSSF_HARDENING_HAS_HARDENED)
	list(APPEND OpenSSF_Hardening_FLAGS
		-fhardened
	)
endif()
if(OpenSSF_HARDENING_HAS_UB_SAN AND OpenSSF_HARDENING_LINK_HAS_UB_SAN)
	list(APPEND OpenSSF_Hardening_FLAGS
		-fsanitize=undefined
	)
	list(APPEND OpenSSF_Hardening_LINK_FLAGS
		-fsanitize=undefined
	)
endif()


# 3. Compiler-version‐specific flags
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "13.0")
		list(APPEND OpenSSF_Hardening_FLAGS -fstrict-flex-arrays=3)
	endif()
	if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "15.0")
		list(APPEND OpenSSF_Hardening_FLAGS -fzero-init-padding-bits=all)
	endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "16.0")
		list(APPEND OpenSSF_Hardening_FLAGS -fstrict-flex-arrays=3)
	endif()
endif()


# 4. C-only hardening flags
set(OpenSSF_Hardening_C_FLAGS
	-Werror=implicit
	-Werror=incompatible-pointer-types
	-Werror=int-conversion
)


# 5. Linker hardening flags - Platform specific setup

# Platform-specific linker flags
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
	# macOS-specific linker flags
	# macOS doesn't support GNU ld flags like --as-needed
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "Windows")
	# Linux/Windows linker flags (GNU ld compatible)
	list(APPEND OpenSSF_Hardening_LINK_FLAGS
		-Wl,--as-needed
		-Wl,--no-copy-dt-needed-entries
	)
	
	if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
		list(APPEND OpenSSF_Hardening_LINK_FLAGS
			-Wl,-z,nodlopen
			-Wl,-z,noexecstack
			-Wl,-z,relro
			-Wl,-z,now
		)
	endif()
endif()


# 6. Function: openssf_harden_target
function(openssf_harden_target target)
	# 6.1 Apply compiler hardening flags
	target_compile_options(${target} PRIVATE
		${OpenSSF_Hardening_FLAGS}
		$<$<COMPILE_LANGUAGE:C>:${OpenSSF_Hardening_C_FLAGS}>
	)

	# 6.2 Enable PIE/PIC
	set_target_properties(${target} PROPERTIES
		POSITION_INDEPENDENT_CODE ON
	)

	# 6.3 Apply linker hardening flags
	target_link_options(${target} PRIVATE
		${OpenSSF_Hardening_LINK_FLAGS}
	)

	# 6.4 Release-only defines
	target_compile_definitions(${target} PRIVATE
		$<$<CONFIG:Release>:_FORTIFY_SOURCE=3;_GLIBCXX_ASSERTIONS>
	)
endfunction()

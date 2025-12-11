include(FetchContent)
# Get the nuget package manager executable from https://dist.nuget.org/win-x86-commandline/latest/nuget.exe
# This is used to download the CppWinRT package
set(FETCHCONTENT_BASE_DIR "${CMAKE_BINARY_DIR}/cppwinrt_fetchcontent")
if(NOT EXISTS "${FETCHCONTENT_BASE_DIR}/nuget.exe")
    message(STATUS "Downloading nuget.exe...")
    file(MAKE_DIRECTORY "${FETCHCONTENT_BASE_DIR}")
    
    file(DOWNLOAD "https://dist.nuget.org/win-x86-commandline/latest/nuget.exe"
                 "${FETCHCONTENT_BASE_DIR}/nuget.exe"
                 EXPECTED_HASH SHA256=8ddc8cc04298fa08277efdca35373eb158f0c95f5bb1b15efcab2b62952028f6
                 SHOW_PROGRESS
                 STATUS download_status)
    
    if(NOT download_status EQUAL 0)
        message(FATAL_ERROR "Failed to download nuget.exe: ${download_status}")
    else()
        message(STATUS "nuget.exe downloaded successfully")
    endif()
endif()

# Download the CppWinRT package during configuration time
set(CPPWINRT_VERSION 2.0.250303.1)
set(CPPWINRT_ROOT "${FETCHCONTENT_BASE_DIR}/cppwinrt")

# Only download if not already present
if(NOT EXISTS "${CPPWINRT_ROOT}/Microsoft.Windows.CppWinRT.${CPPWINRT_VERSION}")
    message(STATUS "Downloading CppWinRT package...")
    file(MAKE_DIRECTORY "${CPPWINRT_ROOT}")
    
    execute_process(
        COMMAND "${FETCHCONTENT_BASE_DIR}/nuget.exe" install Microsoft.Windows.CppWinRT -Version ${CPPWINRT_VERSION} -OutputDirectory "${CPPWINRT_ROOT}"
        RESULT_VARIABLE nuget_result
        OUTPUT_VARIABLE nuget_output
        ERROR_VARIABLE nuget_error
    )
    
    if(NOT nuget_result EQUAL 0)
        message(FATAL_ERROR "Failed to download CppWinRT package: ${nuget_error}")
    else()
        message(STATUS "CppWinRT package downloaded successfully")
    endif()
endif()

set(CPPWINRT_PACKAGE_ROOT "${CPPWINRT_ROOT}/Microsoft.Windows.CppWinRT.${CPPWINRT_VERSION}")

# Generate the CppWinRT header files if not already generated
if(NOT EXISTS "${CPPWINRT_PACKAGE_ROOT}/include/winrt")
    message(STATUS "Generating CppWinRT headers...")
    execute_process(
        COMMAND "${CPPWINRT_PACKAGE_ROOT}/bin/cppwinrt.exe" -in local -output "${CPPWINRT_PACKAGE_ROOT}/include"
        RESULT_VARIABLE cppwinrt_result
        OUTPUT_VARIABLE cppwinrt_output
        ERROR_VARIABLE cppwinrt_error
    )
    
    if(NOT cppwinrt_result EQUAL 0)
        message(FATAL_ERROR "Failed to generate CppWinRT headers: ${cppwinrt_error}")
    else()
        message(STATUS "CppWinRT headers generated successfully")
    endif()
endif()

add_library(cppwinrt INTERFACE)
target_include_directories(cppwinrt INTERFACE "${CPPWINRT_PACKAGE_ROOT}/include")
target_link_libraries(cppwinrt INTERFACE "${CPPWINRT_PACKAGE_ROOT}/build/native/lib/win32/cppwinrt_fast_forwarder.lib")

cmake_minimum_required (VERSION 3.3)

project(Luna)

include_directories(./Code)
include_directories(./SDKs)

# Build Options.
option(LIB "Build All Engine Modules as Static Library." OFF)
option(CONTRACT_ASSERTION "Enables contract assertions. This is always enabled in debug build." ON)
option(THREAD_SAFE_ASSERTION "Enables thread safe assertions. This is always enabled in debug build." ON)

# Detect Platforms.
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(PLATFORM_64BIT 1)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(PLATFORM_32BIT 1)
else()
    message(FATAL_ERROR "Unknown Architecture is specified.")
endif()

# Define Global Constants.
set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(BUILD_DIR ${CMAKE_BINARY_DIR})

if(PLATFORM_64BIT)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug64)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/RelWithDebugInfo64)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release64)
elseif(PLATFORM_32BIT)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/RelWithDebugInfo)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release)
else()
    message(FATAL_ERROR "Unknown Architecture is specified.")
endif()

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo" CACHE STRING "" FORCE)

# Define Macros.
if(LIB)
    add_definitions("-DLUNA_BUILD_LIB")
endif()

if(CONTRACT_ASSERTION)
    add_definitions("-DLUNA_ENABLE_CONTRACT_ASSERTION")
endif()

if(THREAD_SAFE_ASSERTION)
    add_definitions("-DLUNA_ENABLE_THREAD_SAFE_ASSERTION")
endif()

# Choose C++ Stds.
if(NOT "cxx_std_11" IN_LIST CMAKE_CXX_COMPILE_FEATURES)
    message(FATAL_ERROR "C++11 Support is required to compile Luna Engine")
endif()
# Since most compilers supports selection of multiple C++ standards, we always choose the latest standard that the compiler supports manually.
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions("-DLUNA_MANUAL_CONFIG_CPP_STANDARD")

# Currently there are known issues in VS2019 that C++20 is not fully supported, so use C++17 until they are tested fixed.
# if("cxx_std_20" IN_LIST CMAKE_CXX_COMPILE_FEATURES)
#    set(CMAKE_CXX_STANDARD 20)
#    add_definitions("-DLUNA_COMPILER_CPP14 -DLUNA_COMPILER_CPP17 -DLUNA_COMPILER_CPP20")
# else

if("cxx_std_17" IN_LIST CMAKE_CXX_COMPILE_FEATURES)
    set(CMAKE_CXX_STANDARD 17)
    add_definitions("-DLUNA_COMPILER_CPP14 -DLUNA_COMPILER_CPP17")
elseif("cxx_std_14" IN_LIST CMAKE_CXX_COMPILE_FEATURES)
    set(CMAKE_CXX_STANDARD 14)
    add_definitions("-DLUNA_COMPILER_CPP14")
else()
    # CPP11 is always enabled, so there is no need to declare it.
    set(CMAKE_CXX_STANDARD 11)
endif()

# Set multiple thread compile for msvc.
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
    add_definitions(-DUNICODE -D_UNICODE)
endif()

add_subdirectory(Code)
add_subdirectory(SDKs)
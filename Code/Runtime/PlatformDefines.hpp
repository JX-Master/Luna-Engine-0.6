// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file PlatformDefines.hpp
* @author JXMaster
* @date 2018/11/12
 */
#pragma once
//------------------------------------------ Config CPU ------------------------------------------
#ifndef LUNA_MANUAL_CONFIG_CPU_ARCHITECTURE
    #if defined(__x86_64__) || defined(_M_X64) || defined(_AMD64_) || defined(_M_AMD64)
        #define LUNA_PLATFORM_X86_64	     1
    #elif defined(__i386) || defined(_M_IX86) || defined(_X86_)
        #define LUNA_PLATFORM_X86		     1
    #elif defined(__aarch64__) || defined(__AARCH64) || defined(_M_ARM64)
        #define LUNA_PLATFORM_ARM64			1
    #elif defined(__arm__) || defined(_M_ARM)
        #define LUNA_PLATFORM_ARM32			1
    #elif defined(__POWERPC64__) || defined(__powerpc64__)
        #define LUNA_PLATFORM_POWERPC64		1
    #elif defined(__POWERPC__) || defined(__powerpc__)
        #define LUNA_PLATFORM_POWERPC32		1
    #else
        #error Unrecognized CPU was used.
    #endif
#endif

#ifndef LUNA_MANUAL_CONFIG_CPU_TRAITS
    #if defined(__AVX__)
        #define LUNA_PLATFORM_AVX   1
    #endif
    #if defined(__AVX2__)
        #define LUNA_PLATFORM_AVX2  1
    #endif

    #if defined(LUNA_PLATFORM_X86)
        #define LUNA_PLATFORM_32BIT				1
        #define LUNA_PLATFORM_LITTLE_ENDIAN		1
        #define LUNA_PLATFORM_SSE	1
        #define LUNA_PLATFORM_SSE2	1
    #endif

    #if defined(LUNA_PLATFORM_X86_64)
        #define LUNA_PLATFORM_64BIT				1
        #define LUNA_PLATFORM_LITTLE_ENDIAN		1
        #define LUNA_PLATFORM_SSE	1
        #define LUNA_PLATFORM_SSE2	1
    #endif

    #if defined(LUNA_PLATFORM_ARM32)
        #define LUNA_PLATFORM_32BIT				1
        #define LUNA_PLATFORM_LITTLE_ENDIAN		1
    #endif

    #if defined(LUNA_PLATFORM_ARM64)
        #define LUNA_PLATFORM_64BIT				1
        #define LUNA_PLATFORM_LITTLE_ENDIAN		1
        #define LUNA_PLATFORM_SSE	1
        #define LUNA_PLATFORM_SSE2	1
    #endif

    #if defined(LUNA_PLATFORM_POWERPC32)
        #define LUNA_PLATFORM_32BIT				1
        #define LUNA_PLATFORM_BIG_ENDIAN		1
    #endif

    #if defined(LUNA_PLATFORM_POWERPC64)
        #define LUNA_PLATFORM_64BIT				1
        #define LUNA_PLATFORM_BIG_ENDIAN		1
    #endif
#endif

#ifndef LUNA_MANUAL_CONFIG_PLATFORM
    #if defined(__ORBIS__)
        #define LUNA_PLATFORM_ORBIS		1
    #elif defined(_DURANGO)
        #define LUNA_PLATFORM_DURANGO	1
    #elif defined(__ANDROID__)
        #define LUNA_PLATFORM_ANDROID	1
    #elif defined(__APPLE__) && __APPLE__
        #include <TargetConditionals.h>
        #if  defined(__IPHONE__) || (defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE) || (defined(TARGET_IPHONE_SIMULATOR) && TARGET_IPHONE_SIMULATOR)
            #define LUNA_PLATFORM_IOS		1
        #elif  defined(__MACH__) || (defined(__MSL__) && (__dest_os == __mac_os_x))
            #define LUNA_PLATFORM_MACOS		1
        #endif
    #elif defined(__linux__) || defined(__linux)
        #define LUNA_PLATFORM_LINUX		1
    #elif defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
        #define LUNA_PLATFORM_WINDOWS	1
    #else
        #error "Unrecognized Platform"
    #endif
#endif

#ifndef LUNA_MANUAL_CONFIG_PLATFORM_TRAITS
    #if defined(LUNA_PLATFORM_WINDOWS)
        #define LUNA_PLATFORM_DESKTOP	1
    #endif
    #if defined(LUNA_PLATFORM_LINUX)
        #define LUNA_PLATFORM_DESKTOP	1
        #define LUNA_PLATFORM_POSIX	    1
    #endif
    #if defined(LUNA_PLATFORM_MACOS)
        #define LUNA_PLATFORM_DESKTOP   1
        #define LUNA_PLATFORM_POSIX     1
        #define LUNA_PLATFORM_APPLE     1
    #endif
    #if defined(LUNA_PLATFORM_ANDROID)
        #define LUNA_PLATFORM_MOBILE	1
    #endif
    #if defined(LUNA_PLATFORM_IOS)
        #define LUNA_PLATFORM_MOBILE	1
        #define LUNA_PLATFORM_APPLE     1
    #endif
    #if defined(LUNA_PLATFORM_DURANGO)
        #define LUNA_PLATFORM_CONSOLE	1
    #endif
    #if defined(LUNA_PLATFORM_ORBIS)
        #define LUNA_PLATFORM_CONSOLE	1
    #endif
#endif

#ifdef LUNA_PLATFORM_WINDOWS
	#include <winsdkver.h>
	#ifndef LUNA_MANUAL_CONFIG_PLATFORM_VERSION
		#define LUNA_PLATFORM_VERSION WINVER
	#endif

	#define LUNA_PLATFORM_VERSION_WIN7	0x0601
	#define LUNA_PLATFORM_VERSION_WIN8	0x0602
	#define LUNA_PLATFORM_VERSION_WIN8_1	0x0603
	#define LUNA_PLATFORM_VERSION_WIN10 	0x0A00
#endif

#ifndef LUNA_MANUAL_CONFIG_COMPILER
    #if defined(__clang__)
        #define LUNA_COMPILER_CLANG  1
    #elif defined(__GNUC__)
        #define LUNA_COMPILER_GCC    1
    #elif defined(_MSC_VER)
        #define LUNA_COMPILER_MSVC   1
    #else
        #error Unrecognized compiler was used.
    #endif
#endif

#ifndef LUNA_MANUAL_CONFIG_COMPILER_TRAITS
    #if defined(LUNA_COMPILER_MSVC)
        #define LUNA_COMPILER_VERSION _MSC_VER
    #elif defined(LUNA_COMPILER_CLANG)
        #define LUNA_COMPILER_VERSION (__clang_major__ * 100 + __clang_minor__)
    #elif defined(LUNA_COMPILER_GCC)
        #define LUNA_COMPILER_VERSION (__GNUC__ * 1000 + __GNUC_MINOR__)
    #endif
#endif

#ifndef LUNA_MANUAL_CONFIG_CPP_STANDARD
    #if (defined(LUNA_COMPILER_CLANG) || defined(LUNA_COMPILER_GCC))
		#if __cplusplus >= 201703L
			#define LUNA_COMPILER_CPP17
		#endif
		#if __cplusplus >= 201402L
			#define  LUNA_COMPILER_CPP14
		#endif
    #elif defined(LUNA_COMPILER_MSVC)
        #if (LUNA_COMPILER_VERSION >= 1920)    // VS 2019
            #define LUNA_COMPILER_CPP17
        #endif
		#if (LUNA_COMPILER_VERSION >= 1910)    // VS 2017
			#define LUNA_COMPILER_CPP14
		#endif
    #else
        #error "Failed to delect C++ standard version."
    #endif
#endif // LUNA_MANUAL_CONFIG_CPP_STANDARD_VERSION

#if !defined(LUNA_COMPILER_CPP14)
	# error "Luna Engine needs CPP14 support to build."
#endif

// Compiler Defines
#if defined(LUNA_COMPILER_MSVC)
    #define LUNA_DLL_EXPORT __declspec(dllexport)
#else
    #define LUNA_DLL_EXPORT __attribute__ ((visibility("default")))
#endif

#ifdef LUNA_COMPILER_MSVC
#define LUNA_FUNCTION __FUNCTION__
#endif

// define LUNA_BUILD_LIB in module's project config or API's source file, not in public domain.
#if defined(LUNA_BUILD_LIB)
	#define LUNA_EXPORT 
#else
	#define LUNA_EXPORT LUNA_DLL_EXPORT
#endif

#ifndef LUNA_MANUAL_CONFIG_DEBUG_LEVEL
    #if defined(_DEBUG)
        #define LUNA_DEBUG_LEVEL 2
    #else
        #define LUNA_DEBUG_LEVEL 1
    #endif
#endif

#define LUNA_DEBUG_LEVEL_DEBUG 2
#define LUNA_DEBUG_LEVEL_PROFILE 1
#define LUNA_DEBUG_LEVEL_RELEASE 0

#if LUNA_DEBUG_LEVEL == LUNA_DEBUG_LEVEL_DEBUG
#define LUNA_DEBUG		1
#define LUNA_PROFILE	1
#define LUNA_RELEASE	1
#endif

#if LUNA_DEBUG_LEVEL == LUNA_DEBUG_LEVEL_PROFILE
#define LUNA_PROFILE	1
#define LUNA_RELEASE	1
#endif

#if LUNA_DEBUG_LEVEL == LUNA_DEBUG_LEVEL_RELEASE
#define LUNA_RELEASE	1
#endif

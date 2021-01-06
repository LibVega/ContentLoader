/*
 * MIT License - Copyright (c) 2020-2021 Sean Moss
 * This file is subject to the terms and conditions of the MIT License, the text of which can be found in the
 * 'LICENSE' file at the root of this repository, or online at <https://opensource.org/licenses/MIT>.
 */

#pragma once

 /* Ensure 64-bit */
#if !defined(_M_X64) && !defined(__x86_64__)
#	error Must be built as 64-bit.
#endif

/* OS Detection */
#if defined(_WIN32)
#	define VEGA_WIN32
#	ifndef NOMINMAX
#		define NOMINMAX
#	endif
#	ifndef WIN32_LEAN_AND_MEAN
#		define WIN32_LEAN_AND_MEAN
#	endif
#elif defined(__APPLE__)
#	include "TargetConditionals.h"
#	if TARGET_OS_OSX==1
#		define VEGA_MACOS
#	else
#		error Unsupported Apple OS platform, please use macOS.
#	endif // TARGET_OS_OSX==1
#elif defined(__linux__)
#	define VEGA_LINUX
#else
#	error Supported OS not detected - please use Windows, OSX, or desktop Linux.
#endif // defined(_WIN32)

/* Compiler Detection */
#if defined(_MSC_VER)
#	define VEGA_MSVC
#elif defined(__clang__)
#	define VEGA_CLANG
#elif defined(__GNUC__)
#	define VEGA_GCC
#else
#	error Unsupported compiler detected - please use MSVC, GNU GCC, or Clang.
#endif // defined(_MSC_VER)

/* API Macros */
#if defined(VEGA_MSVC)
#	define VEGA_API_EXPORT extern "C" __declspec(dllexport)
#else
#	define VEGA_API_EXPORT extern "C" __attribute__((visibility("default")))
#endif

/* Standard Includes */
#include <cstdint>
#include <string>

/* Public API Booleans */
typedef uint32_t VegaBool;
#define VEGA_TRUE (VegaBool(1))
#define VEGA_FALSE (VegaBool(0))

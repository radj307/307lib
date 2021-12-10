/**
 * @file	Windows.hpp
 * @author	radj307
 * @brief	Wraps the <Windows.h> header inside of the "win" namespace.
 *\n
 */
#pragma once
#include <cctype> ///< @brief Include this before Windows.h so the isblank/isspace/ispunct/isalpha/isalnum/etc. functions are in global scope.

#define WINDOWS_HPP

#pragma region WinDefinePre
#ifndef _WINDOWS_HPP_ENABLE_ALL	// If you need the full content of Windows.h, define this macro.
 // Disable features:
#define NOMINMAX				// Remove min & max macros
#define WIN32_LEAN_AND_MEAN		// Remove most significant bloat
#define NOCOLOR					// Disable color macros (these are deprecated anyway, use ANSI sequences)
//#define NOTEXTMETRIC			// Disable text metrics
#define NOMB					// Disable MB_* & MessageBox()
#define NOMSG					// Disable MSG typedef
#define NOOPENFILE				// Disable OpenFile macros

#undef _MSC_EXTENSIONS			// Disable MSC extensions
#ifndef _WINDOWS_
#define _WIN32_WINNT 0x0601		// Define WINNT version
#endif

#endif
#pragma endregion WinDefinePre

 /**
  * @namespace	win
  * @brief		Contains everything from the <Windows.h> header file.
  */
namespace win {
#include <Windows.h> // include Windows.h
#ifndef _WINDOWS_HPP_ENABLE_ALL
	/// @brief Used by some WIN32 API functions to specify the STDIN stream.
	inline constexpr auto _STD_INPUT_HANDLE{ STD_INPUT_HANDLE };
	/// @brief Used by some WIN32 API functions to specify the STDOUT stream.
	inline constexpr auto _STD_OUTPUT_HANDLE{ STD_OUTPUT_HANDLE };
	/// @brief Used by some WIN32 API functions to specify the STDERR stream.
	inline constexpr auto _STD_ERROR_HANDLE{ STD_ERROR_HANDLE };
#endif
}

#pragma region WinDefinePost
#ifndef _WINDOWS_HPP_ENABLE_ALL

#undef STD_INPUT_HANDLE
#undef STD_OUTPUT_HANDLE
#undef STD_ERROR_HANDLE
#undef LoadCursor

#endif
#pragma endregion WinDefinePost
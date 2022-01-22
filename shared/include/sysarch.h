/**
 * @file	sysarch.h
 * @author	radj307
 * @brief	Contains preprocessor definitions useful for determining properties of the host system.
 */
#pragma once

 /// FORWARD DOCUMENTATION FOR DOXYGEN
#pragma region Documentation
/**
 * @def	ARCH
 * @brief	Defines the system architecture being used by the compiler.
 * | Value   	| Description			|
 * |:---------:|:----------------------|
 * | 8		    | 8-bit Architecture	|
 * | 16		| 16-bit Architecture	|
 * | 32		| 32-bit Architecture	|
 * | 64		| 64-bit Architecture	|
 */
#define ARCH
#undef ARCH
/**
 * @def	OS_LINUX
 * @brief	POSIX Operating System. Value is set to ARCH.
 */
#define OS_LINUX
#undef OS_LINUX
/**
 * @def		OS_CYGWIN
 * @brief	Windows Operating System with Cygwin. Value is set to ARCH.
 */
#define OS_CYGWIN
#undef OS_CYGWIN
/**
* @def		OS_WIN
* @brief	Windows Operating System. Value is set to ARCH.
*/
#define OS_WIN
#undef OS_WIN
/**
 * @def		OS_MAC
 * @brief	Mac Operating System. Value is set to ARCH.
 */
#define OS_MAC
#undef OS_MAC
/**
 * @def		OS_NONE
 * @brief	Running without an operating system. Value is set to ARCH if it exists.
 *\n		minver:	C++11
 */
#define OS_NONE
#undef OS_NONE
 /**
  * @def	LANG_CPPCLI
  * @brief	When defined, C++/CLI Language extension is present.
  */
#define LANG_CPPCLI
#undef LANG_CPPCLI
  /**
  * @def	LANG_CPP
  * @brief	Defines the version of C++ being used by the compiler. This is equal to (__cplusplus > [STD_PREV_VERSION_VALUE]). _Ex: `17` == `__cplusplus > 201402L`_
  * | Value		| Description                           | `__cplusplus`  |
  * |:---------:|:--------------------------------------|:---------------|
  * | <NULL>	| Version prior to C++98                | <NULL>         |
  * | `98`		| C++98                                 | v == `199711L` |
  * | `11`		| C++11		                            | v > `199711L`  |
  * | `14`		| C++14		                            | v > `201103L`  |
  * | `17`		| C++17		                            | v > `201402L`  |
  * | `20`		| C++20 _May be partial implementation_ | v > `201703L`  |
  * | `23`		| C++23 _(Or newer)_                    | v > `202002L`  |
 */
#define LANG_CPP
#undef LANG_CPP
 /**
  * @def	LANG_C
  * @brief	Defines the version of C being used by the compiler. This is equal to ({ __STDC_VERSION__ || __STDC__ } > [NONSTD_PREV_VERSION_VALUE]), note that the values set here are implementation defined and may not be accurate.
  * | Value		| Description     | `__STDC_VERSION__` or `__STDC__` |
  * |:---------:|:----------------|----------------------------------|
  * | `1`		| gcc `__STDC__`  | `1`                              |
  * | `90`		| C89 or C90      | <NULL>                           |
  * | `95`		| C95		      | v == `199409L`                   |
  * | `99`		| C99		      | v > `199409L`                    |
  * | `11`		| C11		      | v > `199901L`                    |
  * | `18`		| C18		      | v > `201112L`                    |
  * | `20`		| Higher than C18 | v > `201710L`                    |
  */
#define LANG_C
#undef LANG_C
/**
 * @def		STRICT_POINTER_SAFETY
 * @brief	When defined, the implementation has strict std::pointer_safety.
 *\n		minver:	C++11
 *\n		maxver:	C++23
 */
#define STRICT_POINTER_SAFETY
#undef STRICT_POINTER_SAFETY
/**
 * @def		MULTITHREADING_CAPABLE
 * @brief	When defined, the system is capable of multithreading using C++ threads.
 *\n		minver:	C++11
 */
#define MULTITHREADING_CAPABLE
#undef MULTITHREADING_CAPABLE

#pragma endregion Documentation


#if INTPTR_MAX == INT64_MAX
	#define ARCH 64		///< 64-Bit System Architecture
#elif INTPTR_MAX == INT32_MAX
	#define ARCH 32		///< 32-Bit System Architecture
#elif INTPTR_MAX == INT16_MAX
	#define ARCH 16		///< 16-Bit System Architecture
#elif INTPTR_MAX == INT8_MAX
	#define ARCH 8		///< 8-Bit System Architecture
#endif // ARCH

  // OS
#if defined(__GNUG__) || defined(__GNUC__)
	// Current Operating System & Architecture.
	#define OS_LINUX ARCH
#endif
#if defined(__CYGWIN__)
	// Current Operating System & Architecture.
	#define OS_CYGWIN ARCH
#endif
#if defined(_WIN32)
	// Current Operating System & Architecture.
	#define OS_WIN ARCH
#endif
#if defined(_MAC)
	// Current Operating System & Architecture.
	#define OS_MAC ARCH
#endif
#if __STDC_HOSTED__ == 0
#ifdef ARCH
	// Current Operating System & Architecture.
	#define OS_NONE ARCH
#else
	// Current Operating System & Architecture.
	#define OS_NONE
#endif
#endif

// Language
#ifdef __cplusplus_cli
	// Using C++/CLI
	#define LANG_CPPCLI
#endif

#if defined(__cplusplus) // C++
	  // Check C++ version
	#if __cplusplus > 202002L
		#define LANG_CPP 23			///< C++23
	#elif __cplusplus > 201703L
		#define LANG_CPP 20			///< C++20
	#elif __cplusplus > 201402L
		#define LANG_CPP 17			///< C++17
	#elif __cplusplus > 201102L
		#define LANG_CPP 14			///< C++14
	#elif __cplusplus > 199711L
		#define LANG_CPP 11			///< C++11
	#elif __cplusplus == 199711L
		#define LANG_CPP 98			///< C++98
		#ifdef _MSC_VER
			#pragma message(R"(MSVC detected, but "/Zc:__cplusplus" wasn't set in the compiler arguments, cannot accurately determine C++ version.)")
		#endif
	#else 
		#define LANG_CPP	NULL		///< Unknown
	#endif

	#if __STDCPP_THREADS__
		#define MULTITHREADING_CAPABLE
	#endif

	#if __STDCPP_STRICT_POINTER_SAFETY__
		#define STRICT_POINTER_SAFETY
	#endif

	#if LANG_CPP >= 11
		#ifdef __STDC_VERSION__
			#if __STDC_VERSION__ > 201710L
				#define LANG_C 20			///< newer than C18
			#elif __STDC_VERSION__ > 201112L
				#define LANG_C 18			///< C18
			#elif __STDC_VERSION__ > 199901L
				#define LANG_C 11			///< C11
			#elif __STDC_VERSION__ > 199409L
				#define LANG_C 99			///< C99
			#elif __STDC_VERSION__ == 199409L
				#define LANG_C 95			///< C95
			#else
				#define LANG_C 90			///< C89 or C90
			#endif
		#endif
	#endif
#endif // C++
#ifndef LANG_C // if C wasn't specified with __STDC_VERSION__
	#ifdef __STDC__
		#define LANG_C __STDC__
	#endif
#elif defined(__STDC__) // if C was specified with __STDC_VERSION__ & __STDC__ exists as well, it probably defines the conformance.
	#define LANG_C_COMFORMANCE __STDC__
#endif

#pragma region Compiler
	#if defined(_MSC_VER)
		#define COMPILER_MSVC 		///< @brief Microsoft Visual C/C++ Compiler
	#elif defined(__GNUC__)
		#define COMPILER_GCC 		///< @brief gcc/g++ Compiler
	#elif defined(__clang__)
		#define COMPILER_CLANG 		///< @brief Clang Compiler
	#elif defined(__EMSCRIPTEN__)
		#define COMPILER_WEB		///< @brief Web Compiler
	#elif defined(__MINGW32__) || defined(__MINGW64__)
		#define COMPILER_MINGW		///< @brief MinGW/MinGW-w64
	#endif
#pragma endregion Compiler


/// Feature Macros ///

/** @def */

  
#ifndef CONSTEXPR /** @def CONSTEXPR @brief Macro for C++17 constexpr. */
#if LANG_CPP >= 17
#define CONSTEXPR constexpr
#define INLINE inline
#else
#define CONSTEXPR
#define INLINE
#endif
#endif

#ifndef CONSTEVAL
#if LANG_CPP >= 20
#define CONSTEVAL consteval
#else
#define CONSTEVAL
#endif
#endif

#if defined(COMPILER_MSVC) && defined(OS_WIN) /** @def WINCONSTEXPR @brief Macro for MSVC-constexpr extensions. */
#define WINCONSTEXPR CONSTEXPR
#else
#define WINCONSTEXPR
#endif

#if !defined(STRINGIZE) && !defined(_STRINGIZE_INTERNAL) /** @def STRINGIZE @brief Stringize the given preprocessor macro by enclosing it in quotes. */
#define _STRINGIZE_INTERNAL(x) #x
#define STRINGIZE(x) _STRINGIZE_INTERNAL(x)
#endif

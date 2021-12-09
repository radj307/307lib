/**
 * @file	sysarch.h
 * @author	radj307
 * @brief	Contains preprocessor definitions useful for determining properties of the host system.
 */
#ifndef SYSARCH
#define SYSARCH

 /**
  * @def	ARCH
  * @brief	Defines the system architecture being used by the compiler.
  * | Value	| Description			|
  * |---:---	| ---:---				|
  * | 8		| 8-bit Architecture	|
  * | 16		| 16-bit Architecture	|
  * | 32		| 32-bit Architecture	|
  * | 64		| 64-bit Architecture	|
  */
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
/**
 * @def		OS_LINUX
 * @brief	POSIX Operating System. Value is set to ARCH.
 */
#define OS_LINUX ARCH
#elif defined(__CYGWIN__)
/**
 * @def		OS_CYGWIN
 * @brief	Windows Operating System with Cygwin. Value is set to ARCH.
 */
#define OS_CYGWIN ARCH
#define OS_WIN ARCH
#elif defined(_WIN32)
/**
 * @def		OS_WIN
 * @brief	Windows Operating System. Value is set to ARCH.
 */
#define OS_WIN ARCH
#elif defined(_MAC)
/**
 * @def		OS_MAC
 * @brief	Mac Operating System. Value is set to ARCH.
 */
#define OS_MAC ARCH
#endif // OS

 /**
  * @def	CPPCLI
  * @brief	When defined, C++/CLI Language extension is present.
  */
#define CPPCLI
#undef CPPCLI
  /**
  * @def	CPP
  * @brief	Defines the version of C++ being used by the compiler.
  * | Value		| Description |
  * |-----:-----|-----:-------|
  * | NULL		| Version prior to C++98 |
  * | 98		| C++98, or if using MSVC this may indicate that the "/Zc:__cplusplus" flag wasn't added to the compiler commandline |
  * | 11		| C++11		  |
  * | 14		| C++14		  |
  * | 17		| C++17		  |
  * | 20		| At least partially implemented C++20. |
  * | 23		| C++23 _(Or newer if this file hasn't been updated)_ |
 */
#define CPP
#undef CPP
 /**
  * @def	C
  * @brief	Defines the version of C being used by the compiler.
  * | Value		| Description |
  * |-----:-----|-----:-------|
  * | NULL		| Unknown	  |
  * | 90		| C89 or C90  |
  * | 95		| C95		  |
  * | 99		| C99		  |
  * | 11		| C11		  |
  * | 18		| C18		  |
  * | 20		| Any version of C newer than C18 |
  */
#define C
#undef C

#ifdef __cplusplus_cli
#define CPPCLI
#endif
#if defined(__cplusplus)

  // Check C++ version
#if __cplusplus > 202002L
#define CPP 23			///< C++23
#elif __cplusplus > 201703L
#define CPP 20			///< C++20
#elif __cplusplus > 201402L
#define CPP 17			///< C++17
#elif __cplusplus > 201102L
#define CPP 14			///< C++14
#elif __cplusplus > 199711L
#define CPP 11			///< C++11
#elif __cplusplus == 199711L
#define CPP 98			///< C++98
#ifdef _MSC_VER
#pragma message(R"(MSVC detected, but "/Zc:__cplusplus" wasn't set in the compiler arguments! Cannot determine C++ version.)")
#endif
#else 
#define CPP	NULL		///< Unknown
#endif

#elif defined(__STDC_VERSION__) || defined(__STDC__)

  // Check C version
#if defined(__STDC__) && !defined(__STDC_VERSION__)
#define C 90			///< C89 or C90
#elif __STDC_VERSION__ > 201710L
#define C 20			///< newer than C18
#elif __STDC_VERSION__ > 201112L
#define C 18			///< C18
#elif __STDC_VERSION__ > 199901L
#define C 11			///< C11
#elif __STDC_VERSION__ > 199409L
#define C 99			///< C99
#elif __STDC_VERSION__ == 199409L
#define C 95			///< C95
#else
#define C NULL
#endif

#else
#pragma message(R"(Didn't detect "__cplusplus" or "__STDC_VERSION__" macros!)")
#endif

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

#endif // SYSARCH

#ifndef _CONSTEXPR
#if CPP >= 20
#define _CONSTEXPR constexpr
#else
#define _CONSTEXPR
#endif
#endif
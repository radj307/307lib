#pragma once
/**
 * @file	sysarch.h
 * @author	radj307
 * @brief	Contains preprocessor definitions useful for determining properties of the host system.
 */


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


#pragma region ARCH
#if INTPTR_MAX == INT64_MAX
#	define ARCH 64		///< 64-Bit System Architecture
#elif INTPTR_MAX == INT32_MAX
#	define ARCH 32		///< 32-Bit System Architecture
#elif INTPTR_MAX == INT16_MAX
#	define ARCH 16		///< 16-Bit System Architecture
#elif INTPTR_MAX == INT8_MAX
#	define ARCH 8		///< 8-Bit System Architecture
#endif // ARCH
#pragma endregion ARCH

#pragma region OS
#if defined(__GNUG__) || defined(__GNUC__)
	// Current Operating System & Architecture.
#	define OS_LINUX ARCH
#endif
#if defined(__CYGWIN__)
	// Current Operating System & Architecture.
#	define OS_CYGWIN ARCH
#endif
#if defined(_WIN32)
	// Current Operating System & Architecture.
#	define OS_WIN ARCH
#endif
#if defined(_MAC)
	// Current Operating System & Architecture.
#	define OS_MAC ARCH
#endif
#if __STDC_HOSTED__ == 0
#	ifdef ARCH
	// Current Operating System & Architecture.
#		define OS_NONE ARCH
#	else
	// Current Operating System & Architecture.
#		define OS_NONE
#	endif
#endif
#pragma endregion OS

#pragma region LANG_CPPCLI
#ifdef __cplusplus_cli
	// Using C++/CLI
#define LANG_CPPCLI
#endif // LANG_CPPCLI
#pragma endregion LANG_CPPCLI

#pragma region LANG_CPP
#if defined(__cplusplus) // C++
	  // Check C++ version
#	if __cplusplus > 202002L
#		define LANG_CPP 23			///< C++23
#	elif __cplusplus > 201703L
#		define LANG_CPP 20			///< C++20
#	elif __cplusplus > 201402L
#		define LANG_CPP 17			///< C++17
#	elif __cplusplus > 201102L
#		define LANG_CPP 14			///< C++14
#	elif __cplusplus > 199711L
#		define LANG_CPP 11			///< C++11
#	elif __cplusplus == 199711L
#		define LANG_CPP 98			///< C++98
#		ifdef _MSC_VER
#			pragma message(R"(MSVC detected, but "/Zc:__cplusplus" wasn't set in the compiler arguments, cannot accurately determine C++ version.)")
#		endif
#	else 
#		define LANG_CPP	NULL		///< Unknown
#	endif

#	ifdef LANG_CPP
#		define LANG_CXX LANG_CPP
#	endif

#	if __STDCPP_THREADS__
#		define MULTITHREADING_CAPABLE
#	endif

#	if __STDCPP_STRICT_POINTER_SAFETY__
#		define STRICT_POINTER_SAFETY
#	endif

#	ifdef __STDC_VERSION__
#		if __STDC_VERSION__ > 201710L
#			define LANG_C 20			///< newer than C18
#		elif __STDC_VERSION__ > 201112L
#			define LANG_C 18			///< C18
#		elif __STDC_VERSION__ > 199901L
#			define LANG_C 11			///< C11
#		elif __STDC_VERSION__ > 199409L
#			define LANG_C 99			///< C99
#		elif __STDC_VERSION__ == 199409L
#			define LANG_C 95			///< C95
#		else
#			define LANG_C 90			///< C89 or C90
#		endif
#	endif
#endif // LANG_CPP // LANG_CXX // MULTITHREADING_CAPABLE // STRING_POINTER_SAFETY
#pragma endregion LANG_CPP

#pragma region LANG_C
#ifndef LANG_C // if C wasn't specified with __STDC_VERSION__
#	ifdef __STDC__
#		define LANG_C __STDC__
#	endif
#elif defined(__STDC__) // if C was specified with __STDC_VERSION__ & __STDC__ exists as well, it probably defines the conformance.
#	define LANG_C_COMFORMANCE __STDC__
#endif // LANG_C // LANG_C_COMFORMANCE
#pragma endregion LANG_C

#pragma region Compiler
#if defined(_MSC_VER)
#	define COMPILER_MSVC 		///< @brief Microsoft Visual C/C++ Compiler
#elif defined(__GNUC__)
#	define COMPILER_GCC 		///< @brief gcc/g++ Compiler
#elif defined(__clang__)
#	define COMPILER_CLANG 		///< @brief Clang Compiler
#elif defined(__EMSCRIPTEN__)
#	define COMPILER_WEB		///< @brief Web Compiler
#elif defined(__MINGW32__) || defined(__MINGW64__)
#	define COMPILER_MINGW		///< @brief MinGW/MinGW-w64
#endif
#pragma endregion Compiler


/// Feature Macros ///

#ifndef INLINE
#	if LANG_CXX >= 17
#		define INLINE inline
#	else
#		define INLINE
#	endif
#endif

#ifndef CONSTEXPR
#	if LANG_CXX >= 17
#		define CONSTEXPR constexpr
#	else
#		define CONSTEXPR
#	endif
#endif


#pragma region CONSTEXPR
#ifndef CONSTEXPR /** @def CONSTEXPR @brief Macro for C++17 constexpr. */
#	if LANG_CPP >= 11
// C++17 inline is supported.
#		define INLINE inline
// C++11 constexpr is supported.
#		define CONSTEXPR constexpr
#	else
// C++17 inline is not supported.
#		define INLINE
// C++17 constexpr is not supported.
#		define CONSTEXPR
#	endif
#endif
#pragma endregion CONSTEXPR

#pragma region WINCONSTEXPR
#if defined(COMPILER_MSVC) && defined(OS_WIN) /** @def WINCONSTEXPR @brief Macro for MSVC-constexpr extensions. */
// windows-specific constexpr extensions are supported.
#	define WINCONSTEXPR CONSTEXPR
#else
// windows-specific constexpr extensions are not supported.
#	define WINCONSTEXPR
#endif
#pragma endregion WINCONSTEXPR

#pragma region CONSTEVAL
#ifndef CONSTEVAL
#	if LANG_CPP >= 20
// consteval is supported.
#		define CONSTEVAL consteval
#	else
// consteval is not supported.
#		define CONSTEVAL
#	endif
#endif
#pragma endregion CONSTEVAL

#if !defined(STRINGIZE) && !defined(STRINGIZE_I) /** @def STRINGIZE @brief Stringize the given preprocessor macro by enclosing it in quotes. */
// This is used internally by the STRINGIZE macro, do not use this directly!
#	define STRINGIZE_I(x) #x
// This is used to convert preprocessor macros into strings.
#	define STRINGIZE(x) STRINGIZE_I(x)
#endif

#if !defined(VA_OPT_SUPPORTED) && !defined(VA_OPT_SUPPORTED_I)&& !defined(PP_THIRD_ARG) /** @def VA_OPT_SUPPORTED @brief Portable \_\_VA\_OPT\_\_ support detection method. @author https://stackoverflow.com/a/48045656/8705305 */
#	if __cplusplus <= 201703L && defined(__GNUC__) && !defined(__clang__) && !defined(__EDG__)
// The __VA_OPT__ macro is not supported on this version of gcc.
#		define VA_OPT_SUPPORTED false
#	else
/// @brief	This is used internally by the VA_OPT_SUPPORTED macro, do not use this directly!
#		define PP_THIRD_ARG(a,b,c,...) c
/// @brief	This is used internally by the VA_OPT_SUPPORTED macro, do not use this directly!
#		define VA_OPT_SUPPORTED_I(...) PP_THIRD_ARG(__VA_OPT__(,),true,false,)
/// @brief	This is `true` when __VA_OPT__ is supported.
#		define VA_OPT_SUPPORTED VA_OPT_SUPPORTED_I(?)
#	endif
#endif

#ifndef $SYSARCH_NO_FUNCTIONS
/**
 * @def		$SYSARCH_NO_FUNCTIONS
 * @brief	Disables macro functions from sysarch.h
 *\n		These are used by other parts of 307lib and disabling them may cause severe issues!
 */
#define $SYSARCH_NO_FUNCTIONS
#undef $SYSARCH_NO_FUNCTIONS

 /**
  * @def		$_$unwrap
  * @brief		Unwraps/expands the given macro tokens so that you have the macro's VALUE rather than just the name.
  * @param fst	First argument. (C99/standard Compatibility)
  * @param ...	Any number of variaidic arguments.
  */
#define $_$unwrap(fst, ...) fst ##__VA_ARGS__

#include <string>

  /**
   * @def			$nameof
   * @brief		Gets the unqualified, string representation of a given variable, type, object, or function.
   * @param obj	A fully or partially-qualified name; do not include semicolons, brackets, or parameters.
   * @returns		The stringized, unqualified name of obj.
   */
#define $nameof(obj) \
([](std::string&& str) -> std::string {\
	if (const size_t pos{ str.find_last_of(":.->")}; pos < str.size())\
		return str.substr(pos + 1ull);\
	return str;\
}(STRINGIZE(obj)))

   /**
	* @define			$c
	* @brief			Macro that calls static_cast with the given typename & variable.
	* @param type		Output type of the cast expression.
	* @param var		Input value of the cast expression.
	* @returns		Result of calling static_cast<type>(var)
	*/
#define $c(type, var) static_cast<type>(var)

	/**
	 * @define			$fwd
	 * @brief			Macro that calls std::forward with the given variable.
	 * @param var		The variable to forward.
	 * @returns			Result of calling ::std::forward<decltype(var)>(var)
	 */
#define $fwd(var) ::std::forward<decltype(var)>(var)

	 /**
	  * @def				$make_bitfield_operators
	  * @brief				Defines bitwise operators for the given type. *( `operator|`, `operator&`, `operator^`, `operator|=`, `operator&=`, `operator^=` )*
	  *\n					This was designed for `enum` types, but can be used on any types so long as they can be casted to `int`.
	  * @param enumName	The typename of the type to define operators for.
	  */
#define $make_bitfield_operators(enumName, enumType)                                                                         \
INLINE enumName operator|(enumName const& l, enumName const& r) noexcept { return $c(enumName, ($c(int, l) | $c(int, r))); } \
INLINE enumName operator&(enumName const& l, enumName const& r) noexcept { return $c(enumName, ($c(int, l) & $c(int, r))); } \
INLINE enumName operator^(enumName const& l, enumName const& r) noexcept { return $c(enumName, ($c(int, l) ^ $c(int, r))); } \
INLINE enumName& operator|=(enumName& l, enumName const& r) noexcept { return l = $c(enumName, ($c(int, l) | $c(int, r))); } \
INLINE enumName& operator&=(enumName& l, enumName const& r) noexcept { return l = $c(enumName, ($c(int, l) & $c(int, r))); } \
INLINE enumName& operator^=(enumName& l, enumName const& r) noexcept { return l = $c(enumName, ($c(int, l) ^ $c(int, r))); }

#define $make_comparison_operators(enumName, enumType)																		 \
INLINE CONSTEXPR bool operator==(enumName const& l, enumType const& r) noexcept { return $c(enumType, l) == r;}              \
INLINE CONSTEXPR bool operator!=(enumName const& l, enumType const& r) noexcept { return $c(enumType, l) != r;}

	  /**
	   * @def				$make_typed_bitfield
	   * @brief			Creates an enum of the specified type and with the given members, and defines bitwise operators so it can be used as a bitfield flag.
	   * @param enumName	The typename to give to the enum.
	   * @param enumType	The (integral) type of the enum.
	   * @param ...		Any number of enum values *(in the correct syntax, and with each one 2x the size of the last, or combinations of previous values)* to define within the bitfield enum type.
	   */
#define $make_typed_bitfield(enumName, enumType, ...) enum class $_$unwrap(enumName) : $_$unwrap(enumType) { __VA_ARGS__ }; $make_bitfield_operators(enumName, enumType)

	   /**
		* @def			$make_bitfield
		* @brief			Creates an enum of type int with the given members, and defines bitwise operators so it can be used as a bitfield flag.
		* @details		Internally calls $make_typed_bitfield with type int.
		* @param enumName	The typename to give to the enum.
		* @param enumType	The (integral) type of the enum.
		* @param ...		Any number of enum values *(in the correct syntax, and with each one 2x the size of the last, or combinations of previous values)* to define within the bitfield enum type.
		*/
#define $make_bitfield(enumName, ...) $make_typed_bitfield(enumName, int, __VA_ARGS__)

#endif

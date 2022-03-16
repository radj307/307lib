/**
 * @file strconv.hpp
 * @brief Contains string conversion functions that can be passed as a std::function as they are not overloaded.
 * @author radj307
 */
#pragma once
#include <sysarch.h>
#include <var.hpp>
#include <stringify.hpp>

#include <string>
#include <sstream>
#include <optional>
#include <iomanip>

 /**
  * @namespace str
  * @brief Contains various string manipulation, conversion, and parsing functions
  */
namespace str {

	/// TOLOWER

	/**
	 * @brief		Convert a character stored in an int to lowercase.
	 * @param c		Input character.
	 * @returns		int
	 */
	inline CONSTEXPR int tolower(const int& c) noexcept
	{
		if (c >= static_cast<int>('A') && c <= static_cast<int>('Z'))
			return c + 32;
		return c;
	}

#pragma region ChangeCase_Lower
	/**
	 * @brief		Convert a character to lowercase.
	 * @param c		Char to convert. If the character is not an uppercase letter, it will be returned unmodified.
	 * @returns		char
	 */
	[[nodiscard]] inline CONSTEXPR char tolower(const char& c) noexcept
	{
		if (c >= 'A' && c <= 'Z')
			return c + static_cast<char>(32);
		return c;
	}

	/**
	 * @brief		Convert a convertible-character to lowercase.
	 * @tparam T	Type that is convertible to char.
	 * @param c		Value to convert. If the value does not resolve to an uppercase letter, it will be returned as a character.
	 * @returns		char
	 */
#if LANG_CPP >= 20 // v >= C++20
	template<std::convertible_to<char> T>
	[[nodiscard]] inline static CONSTEXPR char tolower(const T& c) noexcept
	{
		return tolower(static_cast<char>(c));
	}
#elif LANG_CPP >= 11 // C++11 <= v < C++20
	template<typename T>
	[[nodiscard]] static CONSTEXPR std::enable_if_t<std::is_same_v<T, char> || std::is_convertible_v<T, char>, char> tolower(const T& c) noexcept
	{
		return tolower(std::move(static_cast<char>(c)));
	}
#endif

	/**
	 * @brief		Convert a whole string to lowercase.
	 * @param str	String to convert.
	 * @returns		std::string
	 */
	[[nodiscard]] inline WINCONSTEXPR const std::string tolower(std::string str) noexcept
	{
		for (auto& ch : str)
			ch = std::move(tolower(ch));
		return str;
	}

#pragma endregion ChangeCase_Lower
#pragma region ChangeCase_LowerWide

	/**
	 * @brief		Convert a character to lowercase.
	 * @param c		Wide-char to convert. If the character is not an uppercase letter, it will be returned unmodified.
	 * @returns		wchar_t
	 */
	[[nodiscard]] inline CONSTEXPR wchar_t tolower(const wchar_t& c) noexcept
	{
		if (c >= static_cast<wchar_t>('A') && c <= static_cast<wchar_t>('Z'))
			return c + static_cast<wchar_t>(32);
		return c;
	}

	/**
	 * @brief		Convert a convertible-character to lowercase.
	 * @tparam T	Type that is convertible to char.
	 * @param c		Value to convert. If the value does not resolve to an uppercase letter, it will be returned as a character.
	 * @returns		char
	 */
#if LANG_CPP >= 20 // v >= C++20
	template<std::convertible_to<wchar_t> T>
	[[nodiscard]] inline static CONSTEXPR wchar_t tolower(const T& c) noexcept
	{
		return tolower(static_cast<wchar_t>(c));
	}
#elif LANG_CPP >= 11 // C++11 <= v < C++20
	template<typename T>
	[[nodiscard]] static CONSTEXPR std::enable_if_t<std::is_same_v<T, wchar_t> || std::is_convertible_v<T, wchar_t>, wchar_t> tolower(const T& c) noexcept
	{
		return tolower(std::move(static_cast<wchar_t>(c)));
	}
#endif

	/**
	 * @brief		Convert a whole string to lowercase.
	 * @param str	String to convert.
	 * @returns		std::string
	 */
	[[nodiscard]] inline WINCONSTEXPR const std::wstring tolower(std::wstring str) noexcept
	{
		for (auto& wch : str)
			wch = std::move(tolower(wch));
		return str;
	}

#pragma endregion ChangeCase_LowerWide

	/// TOUPPER

	/**
	 * @brief		Convert a character stored in an int to uppercase.
	 * @param c		Input character.
	 * @returns		int
	 */
	inline CONSTEXPR int toupper(const int& c) noexcept
	{
		if (c >= static_cast<int>('a') && c <= static_cast<int>('a'))
			return c - 32;
		return c;
	}

#pragma region ChangeCase_Upper
	/**
	 * @brief Convert a character to uppercase.
	 * @param c	- Char to convert. If the character is not a lowercase letter, it will be returned unmodified.
	 * @returns char
	 */
	[[nodiscard]] inline CONSTEXPR char toupper(const char& c) noexcept
	{
		if (c >= 'a' && c <= 'z')
			return c - static_cast<char>(32);
		return c;
	}

	/**
	 * @brief Convert a convertible-character to uppercase.
	 * @tparam T	- Type that is convertible to char.
	 * @param c		- Value to convert. If the value does not resolve to a lowercase letter, it will be returned as a character.
	 * @returns char
	 */
#if LANG_CPP >= 20 // v >= C++20
	template<typename T> requires std::convertible_to<T, char>
	[[nodiscard]] inline static CONSTEXPR char toupper(const T& c) noexcept
	{
		return toupper(std::move(static_cast<char>(c)));
	}
#elif LANG_CPP >= 11 // C++11 <= v < C++20
	template<typename T>
	[[nodiscard]] static CONSTEXPR std::enable_if_t<std::is_convertible_v<T, char>, char> toupper(const T& c)
	{
		return toupper(std::move(static_cast<char>(c)));
	}
#endif

	/**
	 * @brief Convert a whole string to lowercase.
	 * @param str	- String to convert.
	 * @returns std::string
	 */
	[[nodiscard]] inline WINCONSTEXPR const std::string toupper(std::string str) noexcept
	{
		for (auto& ch : str)
			ch = std::move(toupper(ch));
		return str;
	}

#pragma endregion ChangeCase_Upper
#pragma region ChangeCase_UpperWide

	/**
	 * @brief		Convert a character to lowercase.
	 * @param c		Wide-char to convert. If the character is not an uppercase letter, it will be returned unmodified.
	 * @returns		wchar_t
	 */
	[[nodiscard]] inline CONSTEXPR wchar_t toupper(const wchar_t& c) noexcept
	{
		if (c >= static_cast<wchar_t>('a') && c <= static_cast<wchar_t>('z'))
			return c - static_cast<wchar_t>(32);
		return c;
	}

	/**
	 * @brief		Convert a convertible-character to lowercase.
	 * @tparam T	Type that is convertible to char.
	 * @param c		Value to convert. If the value does not resolve to an uppercase letter, it will be returned as a character.
	 * @returns		char
	 */
#if LANG_CPP >= 20 // v >= C++20
	template<std::convertible_to<wchar_t> T>
	[[nodiscard]] inline static CONSTEXPR wchar_t toupper(const T& c) noexcept
	{
		return tolower(static_cast<wchar_t>(c));
	}
#elif LANG_CPP >= 11 // C++11 <= v < C++20
	template<typename T>
	[[nodiscard]] static CONSTEXPR std::enable_if_t<std::is_same_v<T, wchar_t> || std::is_convertible_v<T, wchar_t>, wchar_t> toupper(const T& c) noexcept
	{
		return tolower(std::move(static_cast<wchar_t>(c)));
	}
#endif

	/**
	 * @brief		Convert a whole string to lowercase.
	 * @param str	String to convert.
	 * @returns		std::string
	 */
	[[nodiscard]] inline WINCONSTEXPR const std::wstring toupper(std::wstring str) noexcept
	{
		for (auto& wch : str)
			wch = std::move(tolower(wch));
		return str;
	}

#pragma endregion ChangeCase_UpperWide

	/**
	 * @namespace	optional
	 * @brief		Contains string to numeric type converters that use optional wrapper
	 *				 objects instead of exceptions to indicate whether an input value is valid or not.
	 */
	namespace optional {

		/**
		 * @brief				Convert a given string to an integer type.
		 * @tparam T:			Templated Integral Type.
		 * @param s:			Input string containing the ASCII representation of a number, in any base.
		 * @param nextAfter:	Optional index pointer set to the next character position after the processed substring.
		 * @param base:			The base representation that the input string is using.
		 *\n					Defaults to base-10 (decimal).
		 * @returns				std::optional<T>
		 */
		template<std::integral T>
		inline std::optional<T> sto(const std::function<T(const char*, char**, int)>& converter, const std::string& s, size_t* nextAfter = nullptr, const size_t& base = 10ull) noexcept
		{
			int& errnoRef{ errno };
			const char* ptr{ s.c_str() };
			char* eptr{ nullptr };
			errnoRef = 0;
			const auto out{ converter(ptr, &eptr, base) };

			if (ptr == eptr || errnoRef == ERANGE)
				return std::nullopt;

			if (nextAfter)
				*nextAfter = static_cast<size_t>(eptr - ptr);

			return static_cast<T>(out);
		}
		/**
		 * @brief				Convert a string representation of an integer in any base to a decimal integer type.
		 * @param s:			Input string containing the ASCII representation of a number, in any base.
		 * @param nextAfter:	Optional index pointer set to the next character position after the processed substring.
		 * @param base:			The base representation that the input string is using.
		 *\n					Defaults to base-10 (decimal).
		 * @returns				std::optional<signed int>
		 */
		inline std::optional<signed int> stoi(const std::string& s, size_t* nextAfter = nullptr, const size_t& base = 10ull) noexcept
		{
			return sto<signed int>(::strtol, s, nextAfter, base);
		}
		/**
		 * @brief				Convert a string representation of an integer in any base to a decimal integer type.
		 * @param s:			Input string containing the ASCII representation of a number, in any base.
		 * @param nextAfter:	Optional index pointer set to the next character position after the processed substring.
		 * @param base:			The base representation that the input string is using.
		 *\n					Defaults to base-10 (decimal).
		 * @returns				std::optional<unsigned int>
		 */
		inline std::optional<unsigned int> stoui(const std::string& s, size_t* nextAfter = nullptr, const size_t& base = 10ull) noexcept
		{
			return sto<unsigned int>(::strtoul, s, nextAfter, base);
		}
		/**
		 * @brief				Convert a string representation of an integer in any base to a decimal integer type.
		 * @param s:			Input string containing the ASCII representation of a number, in any base.
		 * @param nextAfter:	Optional index pointer set to the next character position after the processed substring.
		 * @param base:			The base representation that the input string is using.
		 *\n					Defaults to base-10 (decimal).
		 * @returns				std::optional<long>
		 */
		inline std::optional<long> stol(const std::string& s, size_t* nextAfter = nullptr, const size_t& base = 10ull) noexcept
		{
			return sto<long>(::strtol, s, nextAfter, base);
		}
		/**
		 * @brief				Convert a string representation of an integer in any base to a decimal integer type.
		 * @param s:			Input string containing the ASCII representation of a number, in any base.
		 * @param nextAfter:	Optional index pointer set to the next character position after the processed substring.
		 * @param base:			The base representation that the input string is using.
		 *\n					Defaults to base-10 (decimal).
		 * @returns				std::optional<unsigned long>
		 */
		inline std::optional<unsigned long> stoul(const std::string& s, size_t* nextAfter = nullptr, const size_t& base = 10ull) noexcept
		{
			return sto<unsigned long>(::strtoul, s, nextAfter, base);
		}
		/**
		 * @brief				Convert a string representation of an integer in any base to a decimal integer type.
		 * @param s:			Input string containing the ASCII representation of a number, in any base.
		 * @param nextAfter:	Optional index pointer set to the next character position after the processed substring.
		 * @param base:			The base representation that the input string is using.
		 *\n					Defaults to base-10 (decimal).
		 * @returns				std::optional<long long>
		 */
		inline std::optional<long long> stoll(const std::string& s, size_t* nextAfter = nullptr, const size_t& base = 10ull) noexcept
		{
			return sto<long long>(::strtoll, s, nextAfter, base);
		}
		/**
		 * @brief				Convert a string representation of an integer in any base to a decimal integer type.
		 * @param s:			Input string containing the ASCII representation of a number, in any base.
		 * @param nextAfter:	Optional index pointer set to the next character position after the processed substring.
		 * @param base:			The base representation that the input string is using.
		 *\n					Defaults to base-10 (decimal).
		 * @returns				std::optional<unsigned long long>
		 */
		inline std::optional<unsigned long long> stoull(const std::string& s, size_t* nextAfter = nullptr, const size_t& base = 10ull) noexcept
		{
			return sto<unsigned long long>(::strtoull, s, nextAfter, base);
		}
		/**
		 * @brief				Convert a string representation of a decimal floating-point number to a floating-point number.
		 * @param converter:	A conversion function of the same signature as the strto[f|d|ld]() functions.
		 * @param s:			Input string containing a decimal floating-point number.
		 * @param nextAfter:	Optional index pointer set to the next character position after the processed substring.
		 * @returns				std::optional<T>
		 */
		template<std::floating_point T>
		inline std::optional<T> sto(const std::function<T(const char*, char**)>& converter, const std::string& s, size_t* nextAfter = nullptr)
		{
			int& errnoRef{ errno };
			const char* ptr{ s.c_str() };
			char* eptr{ nullptr };
			errnoRef = 0;
			const auto out{ converter(ptr, &eptr) };

			if (ptr == eptr || errnoRef == ERANGE)
				return std::nullopt;

			if (nextAfter)
				*nextAfter = static_cast<size_t>(eptr - ptr);

			return static_cast<T>(out);
		}
		/**
		 * @brief				Convert a string representation of a floating-point number to a float.
		 * @param s:			Input string containing a decimal floating-point number.
		 * @param nextAfter:	Optional index pointer set to the next character position after the processed substring.
		 * @returns				std::optional<float>
		 */
		inline std::optional<float> stof(const std::string& s, size_t* nextAfter = nullptr)
		{
			return sto<float>(::strtof, s, nextAfter);
		}
		/**
		 * @brief				Convert a string representation of a floating-point number to a double.
		 * @param s:			Input string containing a decimal floating-point number.
		 * @param nextAfter:	Optional index pointer set to the next character position after the processed substring.
		 * @returns				std::optional<double>
		 */
		inline std::optional<double> stod(const std::string& s, size_t* nextAfter = nullptr)
		{
			return sto<double>(::strtod, s, nextAfter);
		}
		/**
		 * @brief				Convert a string representation of a floating-point number to a long double.
		 * @param s:			Input string containing a decimal floating-point number.
		 * @param nextAfter:	Optional index pointer set to the next character position after the processed substring.
		 * @returns				std::optional<long double>
		 */
		inline std::optional<long double> stold(const std::string& s, size_t* nextAfter = nullptr)
		{
			return sto<long double>(::strtold, s, nextAfter);
		}
	}
	/**
	 * @namespace	legacy
	 * @brief		Legacy conversion functions that use simple try-catch blocks to avoid exceptions from invalid inputs to stoX functions.
	 */
	namespace legacy {
		/**
		 * @brief		Convert a string to a non-optional boolean with the following rules:
		 *\n			- Returns true if the input string is (case-insensitive) equal to "true".
		 *\n			- Otherwise, returns false.
		 * @param str	Input String.
		 * @param nextCh	Optional pointer that is set to the index of the next character in the string after the number.
		 * @returns		bool
		 */
		[[nodiscard]] inline WINCONSTEXPR bool stob(std::string str) noexcept
		{
			return str::tolower(str) == "true";
		}

		/**
		 * @function ctos(char)
		 * @brief Converts char to string. Can be passed as std::function. Does not throw exceptions.
		 * @param c		- Char to convert
		 * @returns string
		 */
		template<typename CharType> requires (sizeof(CharType) <= sizeof(wchar_t) && sizeof(CharType) >= 0ull && !std::is_same_v<CharType, bool> && !std::is_same_v<CharType, short>)
			inline CONSTEXPR const std::string ctos(CharType c) noexcept
		{
			return std::string(1ull, std::move(c));
		}

		/**
		 * @brief		Converts string to char. Wrapper for std::stoi that can be passed as std::function. Does not throw exceptions.
		 * @param str	String to convert
		 * @returns		char
		 */
		[[nodiscard]] inline char stoc(const std::string& str) noexcept
		{
			return !str.empty() ? str.at(0ull) : static_cast<char>(0);
		}
		/**
		 * @brief		Converts string to unsigned char. Wrapper for std::stoi that can be passed as std::function. Does not throw exceptions.
		 * @param str	String to convert
		 * @returns		unsigned char
		 */
		[[nodiscard]] inline unsigned char stouc(const std::string& str) noexcept
		{
			return !str.empty() ? static_cast<unsigned char>(str.at(0ull)) : static_cast<unsigned char>(0);
		}
		/**
		 * @brief		Converts string to wchar_t. Wrapper for std::stoi that can be passed as std::function. Does not throw exceptions.
		 * @param str	String to convert
		 * @returns		wchar_t
		 */
		[[nodiscard]] inline wchar_t stowc(const std::wstring& str) noexcept
		{
			return !str.empty() ? str.at(0ull) : static_cast<wchar_t>(0);
		}

		/**
		 * @brief				Convert a boolean to a string.
		 * @param val			Target boolean to convert.
		 * @param first_upper	When true, returns a string where the first letter is uppercase.
		 * @returns				std::string
		 */
		[[nodiscard]] inline std::string bool_to_string(const bool& val, const bool& first_upper = false)
		{
			if (val)
				return first_upper ? "True" : "true";
			return first_upper ? "False" : "false";
		}

		/**
		 * @brief		Convert a string to a boolean.
		 * @param str	String to convert to boolean. Must contain ONLY the words "true" or "false". (case-insensitive)
		 * @returns		std::optional<bool>
		 */
		template<typename ReturnType = std::optional<bool>> requires std::same_as<ReturnType, bool> || std::same_as<ReturnType, std::optional<bool>>
		[[nodiscard]] inline ReturnType string_to_bool(std::string str)
		{
			str = tolower(str);
			if (str == "true")
				return true;
			else if (str == "false")
				return false;
			if constexpr (std::same_as<ReturnType, std::optional<bool>>)
				return std::nullopt;
			else return false;
		}

		// Functions that emulate STL string <-> type functions, except they don't throw exceptions and can be passed as std::function.
#		pragma region change_type
		/**
		 * @brief			Converts string to int. Wrapper for std::stoi that can be passed as std::function. Does not throw exceptions.
		 * @param str		String to convert
		 * @param nextCh	Optional pointer that is set to the index of the next character in the string after the number.
		 * @returns			int
		 */
		[[nodiscard]] inline int stoi(const std::string& str, size_t* nextCh = nullptr) noexcept
		{
			try {
				return std::stoi(str, nextCh);
			} catch (...) { return 0; }
		}
		/**
		 * @brief			Converts string to short. Wrapper for std::stoi that can be passed as std::function. Does not throw exceptions.
		 * @param str		String to convert
		 * @param nextCh	Optional pointer that is set to the index of the next character in the string after the number.
		 * @returns			short
		 */
		[[nodiscard]] inline short stos(const std::string& str, size_t* nextCh = nullptr) noexcept
		{
			try {
				return static_cast<short>(std::stoi(str, nextCh));
			} catch (...) { return static_cast<short>(0); }
		}
		/**
		 * @brief			Converts string to unsigned int. Wrapper for static_cast<unsigned>(std::stoi) that can be passed as std::function. Does not throw exceptions.
		 * @param str		String to convert
		 * @param nextCh	Optional pointer that is set to the index of the next character in the string after the number.
		 * @returns			unsigned int
		 */
		[[nodiscard]] inline unsigned int stoui(const std::string& str, size_t* nextCh = nullptr) noexcept
		{
			try {
				return static_cast<unsigned>(std::stoi(str, nextCh));
			} catch (...) { return 0u; }
		}
		/**
		 * @brief			Converts string to long. Wrapper for std::stol that can be passed as std::function. Does not throw exceptions.
		 * @param str		String to convert
		 * @param nextCh	Optional pointer that is set to the index of the next character in the string after the number.
		 * @returns			long
		 */
		[[nodiscard]] inline long stol(const std::string& str, size_t* nextCh = nullptr) noexcept
		{
			try {
				return std::stol(str, nextCh);
			} catch (...) { return 0l; }
		}
		/**
		 * @brief			Converts string to long long. Wrapper for std::stol that can be passed as std::function. Does not throw exceptions.
		 * @param str		String to convert
		 * @param nextCh	Optional pointer that is set to the index of the next character in the string after the number.
		 * @returns			long long
		 */
		[[nodiscard]] inline long long stoll(const std::string& str, size_t* nextCh = nullptr) noexcept
		{
			try {
				return static_cast<long long>(std::stoll(str, nextCh));
			} catch (...) { return 0ll; }
		}
		/**
		 * @brief			Converts string to unsigned long. Wrapper for std::stoul that can be passed as std::function. Does not throw exceptions.
		 * @param str		String to convert
		 * @param nextCh	Optional pointer that is set to the index of the next character in the string after the number.
		 * @returns			unsigned long
		 */
		[[nodiscard]] inline unsigned long stoul(const std::string& str, size_t* nextCh = nullptr) noexcept
		{
			try {
				return std::stoul(str, nextCh);
			} catch (...) { return 0ul; }
		}
		/**
		 * @brief			Converts string to unsigned long. Wrapper for std::stoul that can be passed as std::function. Does not throw exceptions.
		 * @param str		String to convert
		 * @param nextCh	Optional pointer that is set to the index of the next character in the string after the number.
		 * @returns			unsigned long
		 */
		[[nodiscard]] inline unsigned long long stoull(const std::string& str, size_t* nextCh = nullptr) noexcept
		{
			try {
				return std::stoull(str, nextCh);
			} catch (...) { return 0ul; }
		}
		/**
		 * @brief			Converts string to unsigned short. Wrapper for static_cast<unsigned short>(std::stoi) that can be passed as std::function. Does not throw exceptions.
		 * @param str		String to convert
		 * @param nextCh	Optional pointer that is set to the index of the next character in the string after the number.
		 * @returns			unsigned int
		 */
		[[nodiscard]] inline unsigned short stous(const std::string& str, size_t* nextCh = nullptr) noexcept
		{
			try {
				return static_cast<unsigned short>(std::stoi(str, nextCh));
			} catch (...) { return static_cast<unsigned short>(0); }
		}
		/**
		 * @brief			Converts string to float. Wrapper for std::stof that can be passed as std::function. Does not throw exceptions.
		 * @param str		String to convert
		 * @param nextCh	Optional pointer that is set to the index of the next character in the string after the number.
		 * @returns			float
		 */
		[[nodiscard]] inline float stof(const std::string& str, size_t* nextCh = nullptr) noexcept
		{
			try {
				return std::stof(str, nextCh);
			} catch (...) { return 0.0F; }
		}
		/**
		 * @brief			Converts string to double. Wrapper for std::stod that can be passed as std::function. Does not throw exceptions.
		 * @param str		String to convert
		 * @param nextCh	Optional pointer that is set to the index of the next character in the string after the number.
		 * @returns			double
		 */
		[[nodiscard]] inline double stod(const std::string& str, size_t* nextCh = nullptr) noexcept
		{
			try {
				return std::stod(str, nextCh);
			} catch (...) { return 0.0; }
		}
		/**
		 * @brief			Converts string to double. Wrapper for std::stod that can be passed as std::function. Does not throw exceptions.
		 * @param str		String to convert
		 * @param nextCh	Optional pointer that is set to the index of the next character in the string after the number.
		 * @returns			double
		 */
		[[nodiscard]] inline long double stold(const std::string& str, size_t* nextCh = nullptr) noexcept
		{
			try {
				return std::stold(str, nextCh);
			} catch (...) { return 0.0L; }
		}
#		pragma endregion change_type
	}

	using legacy::string_to_bool;
	using legacy::bool_to_string;
	using legacy::stob;
	using legacy::stoc;
	using legacy::ctos;

	/**
	 * @brief			Converts string to int. Wrapper for std::stoi that can be passed as std::function. Does not throw exceptions.
	 * @param str		String to convert
	 * @param nextCh	Optional pointer that is set to the index of the next character in the string after the number.
	 * @returns			int
	 */
	[[nodiscard]] inline int stoi(const std::string& str) noexcept
	{
		return str::legacy::stoi(str, nullptr);
	}
	/**
	 * @brief			Converts string to short. Wrapper for std::stoi that can be passed as std::function. Does not throw exceptions.
	 * @param str		String to convert
	 * @param nextCh	Optional pointer that is set to the index of the next character in the string after the number.
	 * @returns			short
	 */
	[[nodiscard]] inline short stos(const std::string& str) noexcept
	{
		return str::legacy::stoi(str, nullptr);
	}
	/**
	 * @brief			Converts string to unsigned int. Wrapper for static_cast<unsigned>(std::stoi) that can be passed as std::function. Does not throw exceptions.
	 * @param str		String to convert
	 * @param nextCh	Optional pointer that is set to the index of the next character in the string after the number.
	 * @returns			unsigned int
	 */
	[[nodiscard]] inline unsigned int stoui(const std::string& str) noexcept
	{
		return str::legacy::stoi(str, nullptr);
	}
	/**
	 * @brief			Converts string to long. Wrapper for std::stol that can be passed as std::function. Does not throw exceptions.
	 * @param str		String to convert
	 * @param nextCh	Optional pointer that is set to the index of the next character in the string after the number.
	 * @returns			long
	 */
	[[nodiscard]] inline long stol(const std::string& str) noexcept
	{
		return str::legacy::stol(str, nullptr);
	}
	/**
	 * @brief			Converts string to long long. Wrapper for std::stol that can be passed as std::function. Does not throw exceptions.
	 * @param str		String to convert
	 * @param nextCh	Optional pointer that is set to the index of the next character in the string after the number.
	 * @returns			long long
	 */
	[[nodiscard]] inline long long stoll(const std::string& str) noexcept
	{
		return str::legacy::stoll(str, nullptr);
	}
	/**
	 * @brief			Converts string to unsigned long. Wrapper for std::stoul that can be passed as std::function. Does not throw exceptions.
	 * @param str		String to convert
	 * @param nextCh	Optional pointer that is set to the index of the next character in the string after the number.
	 * @returns			unsigned long
	 */
	[[nodiscard]] inline unsigned long stoul(const std::string& str) noexcept
	{
		return str::legacy::stoul(str, nullptr);
	}
	/**
	 * @brief			Converts string to unsigned long. Wrapper for std::stoul that can be passed as std::function. Does not throw exceptions.
	 * @param str		String to convert
	 * @param nextCh	Optional pointer that is set to the index of the next character in the string after the number.
	 * @returns			unsigned long
	 */
	[[nodiscard]] inline unsigned long long stoull(const std::string& str) noexcept
	{
		return str::legacy::stoull(str, nullptr);
	}
	/**
	 * @brief			Converts string to unsigned short. Wrapper for static_cast<unsigned short>(std::stoi) that can be passed as std::function. Does not throw exceptions.
	 * @param str		String to convert
	 * @param nextCh	Optional pointer that is set to the index of the next character in the string after the number.
	 * @returns			unsigned int
	 */
	[[nodiscard]] inline unsigned short stous(const std::string& str) noexcept
	{
		return str::legacy::stoi(str, nullptr);
	}
	/**
	 * @brief			Converts string to float. Wrapper for std::stof that can be passed as std::function. Does not throw exceptions.
	 * @param str		String to convert
	 * @param nextCh	Optional pointer that is set to the index of the next character in the string after the number.
	 * @returns			float
	 */
	[[nodiscard]] inline float stof(const std::string& str) noexcept
	{
		return str::legacy::stof(str, nullptr);
	}
	/**
	 * @brief			Converts string to double. Wrapper for std::stod that can be passed as std::function. Does not throw exceptions.
	 * @param str		String to convert
	 * @param nextCh	Optional pointer that is set to the index of the next character in the string after the number.
	 * @returns			double
	 */
	[[nodiscard]] inline double stod(const std::string& str) noexcept
	{
		return str::legacy::stod(str, nullptr);
	}
	/**
	 * @brief			Converts string to double. Wrapper for std::stod that can be passed as std::function. Does not throw exceptions.
	 * @param str		String to convert
	 * @param nextCh	Optional pointer that is set to the index of the next character in the string after the number.
	 * @returns			double
	 */
	[[nodiscard]] inline long double stold(const std::string& str) noexcept
	{
		return str::legacy::stold(str, nullptr);
	}

	/**
	 * @brief					Convert from any valid streamable type to a string.
	 *\n						This function is a wrapper around stringify(), with a focus on (numeric) => (string) conversions.
	 * @tparam T:				A streamable input type.
	 * @param value:			The input value.
	 * @param fmt:				Format flags to apply to the stream.
	 * @param ...additionalFmt:	Additional stream formatting functions to apply after the given fmtflags.
	 * @returns					const std::string
	 */
	template<var::Streamable<std::stringstream> T, var::Streamable<std::stringstream>... Ts>
	[[nodiscard]] inline CONSTEXPR const std::string to_string(const T& value, const std::ios_base::fmtflags& fmt, Ts&&... additionalFmt)
	{
		std::stringstream ss;
		ss.setf(fmt);
		(ss << ... << additionalFmt);
		ss << value;
		return ss.str();
	}

	/**
	 * @brief			Convert numeric types to string.
	 * @param value:	The input value.
	 * @param fmt:		Format flags to apply to the stream.
	 * @returns			const std::string
	 */
	template<var::numeric T>
	[[nodiscard]] inline CONSTEXPR const std::string to_string(const T& value, const std::ios_base::fmtflags& fmt = std::ios_base::fixed)
	{
		return to_string<T>(value, fmt);
	}

	/**
	 * @brief					Convert a floating-point number to a string, with variable precision and automatic truncation.
	 *\n						This is a legacy support function.
	 * @param value:			The input value.
	 * @param precision:		The number of digits to set the floating-point precision field to.
	 * @param force_decimal:	When true, the returned string will end with ".0" if there are no non-zero digits after the decimal point.
	 * @returns					std::string
	*/
	template<std::floating_point T>
	[[nodiscard]] inline CONSTEXPR const std::string to_string(const T& value, const std::streamsize& precision = 8, const bool& force_decimal = false)
	{
		std::string s{ to_string<T>(value, std::ios_base::fixed, std::setprecision(precision)) };
		if (const auto& pos{ s.find_last_of(".") }; pos < s.size()) {
			if (const auto& lastNonZero{ s.find_last_not_of("123456789", pos) }; lastNonZero < s.size())
				s = s.substr(0ull, lastNonZero + 1ull);
			else s = s.substr(0ull, pos);
		}
		return s;
	}

	/**
	 * @brief			Convert a boolean to a string.
	 * @param value:	The input value.
	 * @param fmt:		Format flags to apply to the stream.
	 * @returns			const std::string
	 */
	template<std::same_as<bool> T>
	[[nodiscard]] inline CONSTEXPR const std::string to_string(const T& value, const std::ios_base::fmtflags& fmt = std::ios_base::boolalpha)
	{
		return to_string<T>(value, fmt);
	}
}
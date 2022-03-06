/**
 * @file strconv.hpp
 * @brief Contains string conversion functions that can be passed as a std::function as they are not overloaded.
 * @author radj307
 */
#pragma once
#include <sysarch.h>
#include <string>
#include <sstream>
#include <optional>
#include <iomanip>

 /**
  * @namespace str
  * @brief Contains various string manipulation, conversion, and parsing functions
  */
namespace str {
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
	 * @brief				Convert a floating-point to a string in standard notation, with a specified amount of precision.
	 * @tparam T			Floating Point Type
	 * @param val			Target double to convert.
	 * @param precision		Number of digits after the decimal point to include.
	 * @param force_decimal	When true, always includes the decimal point even when there are no significant digits following it. (Ex: true:"1.0" | false:"1")
	 * @returns				std::string
	 */
	template<class T> requires std::floating_point<T>
	[[nodiscard]] inline CONSTEXPR const std::string to_string(const T& val, const std::streamsize& precision = 8, const bool force_decimal = false)
	{
		std::stringstream tmp;
		tmp << std::setprecision(precision) << std::fixed << val;
		auto str{ tmp.str() };
		size_t last{ str.size() };
		for (auto it{ str.rbegin() }; it != str.rend(); ++it) {
			if (*it == '0')
				--last;
			else break;
		}
		if (str.find('.') == last - 1ull) { // if the last pos is directly after the decimal pos
			if (force_decimal) // if force decimal is true, increment last by one to prevent trailing decimal point
				++last;
			else // else remove the decimal as well
				--last;
		}
		return str.substr(0ull, last);
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
#pragma region change_type
	/**
	 * @brief		Convert a string to a non-optional boolean with the following rules:
	 *\n			- Returns true if the input string is (case-insensitive) equal to "true".
	 *\n			- Otherwise, returns false.
	 * @param str	Input String.
	 * @returns		bool
	 */
	[[nodiscard]] inline WINCONSTEXPR bool stob(const std::string& str)
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
	[[nodiscard]] inline char stoc(const std::string& str) noexcept { try { return static_cast<char>(std::stoi(str)); } catch (...) { return static_cast<short>(0); } }
	/**
	 * @brief		Converts string to unsigned char. Wrapper for std::stoi that can be passed as std::function. Does not throw exceptions.
	 * @param str	String to convert
	 * @returns		unsigned char
	 */
	[[nodiscard]] inline unsigned char stouc(const std::string& str) noexcept { try { return static_cast<unsigned char>(std::stoi(str)); } catch (...) { return static_cast<short>(0); } }
	/**
	 * @brief		Converts string to wchar_t. Wrapper for std::stoi that can be passed as std::function. Does not throw exceptions.
	 * @param str	String to convert
	 * @returns		wchar_t
	 */
	[[nodiscard]] inline wchar_t stowc(const std::string& str) noexcept { try { return static_cast<wchar_t>(std::stoi(str)); } catch (...) { return static_cast<short>(0); } }
	/**
	 * @brief		Converts string to int. Wrapper for std::stoi that can be passed as std::function. Does not throw exceptions.
	 * @param str	String to convert
	 * @returns		int
	 */
	[[nodiscard]] inline int stoi(const std::string& str) noexcept { try { return std::stoi(str); } catch (...) { return 0; } }
	/**
	 * @brief		Converts string to short. Wrapper for std::stoi that can be passed as std::function. Does not throw exceptions.
	 * @param str	String to convert
	 * @returns		short
	 */
	[[nodiscard]] inline short stos(const std::string& str) noexcept { try { return static_cast<short>(std::stoi(str)); } catch (...) { return static_cast<short>(0); } }
	/**
	 * @brief		Converts string to unsigned int. Wrapper for static_cast<unsigned>(std::stoi) that can be passed as std::function. Does not throw exceptions.
	 * @param str	String to convert
	 * @returns		unsigned int
	 */
	[[nodiscard]] inline unsigned int stoui(const std::string& str) noexcept { try { return static_cast<unsigned>(std::stoi(str)); } catch (...) { return 0u; } }
	/**
	 * @brief		Converts string to long. Wrapper for std::stol that can be passed as std::function. Does not throw exceptions.
	 * @param str	String to convert
	 * @returns		long
	 */
	[[nodiscard]] inline long stol(const std::string& str) noexcept { try { return std::stol(str); } catch (...) { return 0l; } }
	/**
	 * @brief		Converts string to long long. Wrapper for std::stol that can be passed as std::function. Does not throw exceptions.
	 * @param str	String to convert
	 * @returns		long long
	 */
	[[nodiscard]] inline long long stoll(const std::string& str) noexcept { try { return static_cast<long long>(std::stoll(str)); } catch (...) { return 0ll; } }
	/**
	 * @brief		Converts string to unsigned long. Wrapper for std::stoul that can be passed as std::function. Does not throw exceptions.
	 * @param str	String to convert
	 * @returns		unsigned long
	 */
	[[nodiscard]] inline unsigned long stoul(const std::string& str) noexcept { try { return std::stoul(str); } catch (...) { return 0ul; } }
	/**
	 * @brief		Converts string to unsigned long. Wrapper for std::stoul that can be passed as std::function. Does not throw exceptions.
	 * @param str	String to convert
	 * @returns		unsigned long
	 */
	[[nodiscard]] inline unsigned long long stoull(const std::string& str) noexcept { try { return std::stoull(str); } catch (...) { return 0ul; } }
	/**
	 * @brief		Converts string to unsigned short. Wrapper for static_cast<unsigned short>(std::stoi) that can be passed as std::function. Does not throw exceptions.
	 * @param str	String to convert
	 * @returns		unsigned int
	 */
	[[nodiscard]] inline unsigned short stous(const std::string& str) noexcept { try { return static_cast<unsigned short>(std::stoi(str)); } catch (...) { return static_cast<unsigned short>(0); } }
	/**
	 * @brief		Converts string to float. Wrapper for std::stof that can be passed as std::function. Does not throw exceptions.
	 * @param str	String to convert
	 * @returns		float
	 */
	[[nodiscard]] inline float stof(const std::string& str) noexcept { try { return std::stof(str); } catch (...) { return 0.0f; } }
	/**
	 * @brief		Converts string to double. Wrapper for std::stod that can be passed as std::function. Does not throw exceptions.
	 * @param str	String to convert
	 * @returns		double
	 */
	[[nodiscard]] inline double stod(const std::string& str) noexcept { try { return std::stod(str); } catch (...) { return 0.0; } }
	/**
	 * @brief		Converts string to double. Wrapper for std::stod that can be passed as std::function. Does not throw exceptions.
	 * @param str	String to convert
	 * @returns		double
	 */
	[[nodiscard]] inline long double stold(const std::string& str) noexcept { try { return std::stold(str); } catch (...) { return static_cast<long double>(0.0); } }
#pragma endregion change_type
}
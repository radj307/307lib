#pragma once
#include <sysarch.h>

#include <concepts>
#include <string>
#include <charconv>
#include <optional>

namespace str {
#pragma region tonumber
	/**
	 * @brief			Converts a string to an integral number.
	 *\n				This method uses std::from_chars, so it is as fast as possible for converting strings to numbers.
	 * @param s			Input string.
	 * @param base		The number base to interpret the input string in.
	 * @returns			The number represented by the string s if successful; otherwise std::nullopt.
	 */
	template<std::integral T>
	T tonumber(const std::string s, const int base = 10) noexcept
	{
		T v{};
		if ($c(int, std::from_chars(s.data(), s.data() + s.size(), v, base).ec) == 0)
			return v;
		return{};
	}
	/**
	 * @brief			Converts a string to a floating-point number.
	 *\n				This method uses std::from_chars, so it is as fast as possible for converting strings to numbers.
	 * @param s			Input string.
	 * @param fmt		Format flags that determine what notations are allowed in the input string.
	 * @returns			The number represented by the string s if successful; otherwise std::nullopt.
	 */
	template<std::floating_point T>
	T tonumber(const std::string s, const std::chars_format fmt = std::chars_format::general) noexcept
	{
		T v{};
		if ($c(int, std::from_chars(s.data(), s.data() + s.size(), v, fmt).ec) == 0)
			return v;
		return{};
	}
#pragma endregion tonumber

#pragma region stringify
	/**
	 * @brief Creates a temporary stringstream, inserts all of the given arguments, then returns the result of the stringstream's str() function.
	 * @tparam ...Ts	- Variadic Templated Arguments.
	 * @param ...args	- Arguments to insert into the stream, in order. Nearly anything can be included here, so long as it has an operator<< stream insertion operator.
	 * @returns std::string
	 */
	template<typename TChar = char, typename TCharTraits = std::char_traits<TChar>, typename TAlloc = std::allocator<TChar>, var::streamable<std::basic_stringstream<TChar, TCharTraits, TAlloc>>... Ts>
	[[nodiscard]] constexpr static std::basic_string<TChar, TCharTraits, TAlloc> stringify(Ts&&... args)
	{
		if constexpr (var::none<Ts...>)
			return{};
		std::basic_stringstream<TChar, TCharTraits, TAlloc> buffer;	// init stringstream
		(buffer << ... << std::forward<Ts>(args));	// insert variadic arguments in order
		return std::move(buffer.str());		// return as string
	}
#pragma endregion stringify

#pragma region trim
	/**
	 * @brief			Remove preceding characters from a given string.
	 * @param str		Input string.
	 * @param chars		Any number of characters to remove.
	 * @returns			String without preceding or trailing whitespace.
	 */
	inline std::string trim_preceding(std::string str, const std::string& chars)
	{
		if (const auto& beg{ str.find_first_not_of(chars) }; beg != std::string::npos)
			return str.substr(beg);
		return str;
	}
	/**
	 * @brief			Remove preceding characters from a given string.
	 * @param str		Input string.
	 * @param chars		Any number of characters to remove.
	 * @returns			String without preceding or trailing whitespace.
	 */
	template<var::same_or_convertible<char>... Ts>
	std::string trim_preceding(std::string const& str, Ts&&... chars)
	{
		return trim_preceding(str, std::string{ std::forward<Ts>(chars)... });
	}

	/**
	 * @brief			Remove preceding characters from a given string_view.
	 * @param str		Input string_view.
	 * @param chars		Any number of characters to remove.
	 * @returns			String without preceding or trailing whitespace.
	 */
	inline std::string_view trim_preceding(std::string_view str, const std::string& chars)
	{
		if (const auto& beg{ str.find_first_not_of(chars) }; beg != std::string::npos)
			return str.substr(beg);
		return str;
	}
	/**
	 * @brief			Remove preceding characters from a given string_view.
	 * @param str		Input string_view.
	 * @param chars		Any number of characters to remove.
	 * @returns			String without preceding or trailing whitespace.
	 */
	template<var::same_or_convertible<char>... Ts>
	std::string_view trim_preceding(std::string_view const& str, Ts&&... chars)
	{
		return trim_preceding(str, std::string{ std::forward<Ts>(chars)... });
	}

	/**
	 * @brief			Remove trailing characters from a given string.
	 * @param str		Input string.
	 * @param chars		Any number of characters to remove.
	 * @returns			String without preceding or trailing whitespace.
	 */
	inline std::string trim_trailing(std::string str, const std::string& chars)
	{
		if (const auto& end{ str.find_last_not_of(chars) }; end != std::string::npos)
			return str.substr(0ull, end + 1ull);
		return str;
	}
	/**
	 * @brief			Remove trailing characters from a given string.
	 * @param str		Input string.
	 * @param chars		Any number of characters to remove.
	 * @returns			String without preceding or trailing whitespace.
	 */
	template<var::same_or_convertible<char>... Ts>
	std::string trim_trailing(std::string const& str, Ts&&... chars)
	{
		return trim_trailing(str, std::string{ std::forward<Ts>(chars)... });
	}

	/**
	 * @brief			Remove trailing characters from a given string.
	 * @param str		Input string.
	 * @param chars		Any number of characters to remove.
	 * @returns			String without preceding or trailing whitespace.
	 */
	inline std::string_view trim_trailing(std::string_view str, const std::string& chars)
	{
		if (const auto& end{ str.find_last_not_of(chars) }; end != std::string::npos)
			return str.substr(0ull, end + 1ull);
		return str;
	}
	/**
	 * @brief			Remove trailing characters from a given string.
	 * @param str		Input string.
	 * @param chars		Any number of characters to remove.
	 * @returns			String without preceding or trailing whitespace.
	 */
	template<var::same_or_convertible<char>... Ts>
	std::string_view trim_trailing(std::string_view const& str, Ts&&... chars)
	{
		return trim_trailing(str, std::string{ std::forward<Ts>(chars)... });
	}

	/**
	 * @brief			Remove preceding & trailing characters from a given string.
	 * @param str		Input string.
	 * @param chars		Any number of characters to remove.
	 * @returns			String without preceding or trailing whitespace.
	 */
	inline std::string trim(std::string const& str, const std::string& chars = " \t\r\n\v")
	{
		return trim_trailing(trim_preceding(str, chars), chars);
	}

	/**
	 * @brief			Remove preceding & trailing characters from a given string.
	 * @param str		Input string.
	 * @param chars		Any number of characters to remove.
	 * @returns			String without preceding or trailing whitespace.
	 */
	template<var::same_or_convertible<char>... Ts>
	inline std::string trim(std::string const& str, Ts&&... chars)
	{
		return trim_trailing(trim_preceding(str, std::forward<Ts>(chars)...), std::forward<Ts>(chars)...);
	}

	/**
	 * @brief			Remove preceding & trailing characters from a given string.
	 * @param str		Input string.
	 * @param chars		Any number of characters to remove.
	 * @returns			String without preceding or trailing whitespace.
	 */
	inline std::string_view trim(std::string_view const& str, const std::string& chars = " \t\r\n\v")
	{
		return trim_trailing(trim_preceding(str, chars), chars);
	}

	/**
	 * @brief			Remove preceding & trailing characters from a given string.
	 * @param str		Input string.
	 * @param chars		Any number of characters to remove.
	 * @returns			String without preceding or trailing whitespace.
	 */
	template<var::same_or_convertible<char>... Ts>
	inline std::string_view trim(std::string_view const& str, Ts&&... chars)
	{
		return trim_trailing(trim_preceding(str, std::forward<Ts>(chars)...), std::forward<Ts>(chars)...);
	}
#pragma endregion trim

#pragma region tolower
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
#pragma endregion tolower

#pragma region toupper
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
#pragma endregion toupper

	/**
	 * @brief		Converts a string to bool.
	 * @param s		Case-insensitive input string.
	 *				Recognized values for true include "true", "1", "on", & "yes";
	 *				Recognized values for false include "false", "0", "off", & "no".
	 * @returns		true when s was one of the recognized values for true; false when s was one of the recognized values for false; otherwise std::nullopt if s was invalid.
	 */
	inline CONSTEXPR std::optional<bool> tobool(std::string const& s) noexcept
	{
		const auto& lower{ tolower(s) };
		if (lower == "true" || lower == "1" || lower == "on" || lower == "yes")
			return true;
		else if (lower == "false" || lower == "0" || lower == "off" || lower == "no")
			return false;
		else return std::nullopt;
	}
	/**
	 * @brief		Converts a bool to string.
	 * @param b		Input bool.
	 * @returns		The string representation of the given boolean; "true" or "false".
	 */
	inline CONSTEXPR std::string frombool(const bool b) noexcept
	{
		if (b) return "true"; else return "false";
	}
}

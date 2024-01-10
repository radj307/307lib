#pragma once
#include <sysarch.h>
#include <var.hpp>
#include <make_exception.hpp>

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
	 * @param base		The number base to interpret the input string in. The minimum is 2 and the maximum is 36.
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

#pragma region fromnumber
	/**
	 * @brief					Converts the specified integral value to a string.
	 * @tparam BUFFER_SIZE	  -	The size of the buffer to use for holding the result. Defaults to 64.
	 * @tparam TChar		  -	basic_string char type.
	 * @tparam TCharTraits	  -	basic_string char_traits type.
	 * @tparam TAlloc		  -	basic_string allocator type.
	 * @tparam T			  -	The type of integral value to convert.
	 * @param n				  -	The integral value to convert.
	 * @param base			  -	The number base to convert to. Defaults to 10 (decimal).
	 * @returns					A string representing the specified number.
	 */
	template<size_t BUFFER_SIZE = 64, std::integral T, typename TChar = char, typename TCharTraits = std::char_traits<TChar>, typename TAlloc = std::allocator<TChar>>
	[[nodiscard]] std::basic_string<TChar, TCharTraits, TAlloc> fromnumber(T const n, unsigned base = 10) noexcept(false)
	{
		if (base < 2 || base > 36)
			throw make_exception("Base ", base, " is out of range [2, 36]!");

		TChar buf[BUFFER_SIZE]{};
		TChar* p{ buf };

		std::to_chars_result r{ std::to_chars(p, p + sizeof(buf), n, base) };
		if (r.ec != std::errc())
			throw make_exception("Failed to convert integral ", n, " to string in base ", base, " (errc: ", static_cast<int>(r.ec), ")!");

		return std::basic_string<TChar, TCharTraits, TAlloc>(buf);
	}
	/**
	 * @brief					Converts the specified floating-point value to a string.
	 * @tparam BUFFER_SIZE	  -	The size of the buffer to use for holding the result. Defaults to 64.
	 * @tparam TChar		  -	basic_string char type.
	 * @tparam TCharTraits	  -	basic_string char_traits type.
	 * @tparam TAlloc		  -	basic_string allocator type.
	 * @tparam T			  -	The type of floating-point to convert.
	 * @param n				  -	The floating-point value to convert.
	 * @param fmt			  -	The format to use when representing the value.
	 * @param precision		  -	The precision to use when representing the value.
	 * @returns					A string representing the specified number.
	 */
	template<size_t BUFFER_SIZE = 64, std::floating_point T, typename TChar = char, typename TCharTraits = std::char_traits<TChar>, typename TAlloc = std::allocator<TChar>>
	[[nodiscard]] std::basic_string<TChar, TCharTraits, TAlloc> fromnumber(T const n, std::chars_format const fmt = std::chars_format::general, unsigned const precision = 8) noexcept(false)
	{
		TChar buf[BUFFER_SIZE]{};
		TChar* p{ buf };

		std::to_chars_result r{ std::to_chars(p, p + sizeof(buf), n, fmt, precision) };
		if (r.ec != std::errc())
			throw make_exception("Failed to convert floating-point ", n, " to string with fmt ", static_cast<int>(fmt), " and precision ", precision, " (errc: ", static_cast<int>(r.ec), ")!");

		return std::basic_string<TChar, TCharTraits, TAlloc>(buf);
	}
#pragma endregion fromnumber

#pragma region stringify
	/**
	 * @brief					Serializes the specified arguments by inserting them into a
	 *							 temporary std::stringstream and returning the result.
	 * @tparam TChar		  -	basic_string char type.
	 * @tparam TCharTraits	  -	basic_string char_traits type.
	 * @tparam TAlloc		  -	basic_string allocator type.
	 * @param ...args		  -	Any number of elements that can be inserted into a std::ostream using operator<<.
	 * @returns					The resulting string.
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

#pragma region stringify_if
	/**
	 * @brief					When the specified condition resolves to true, the specified arguments
	 *							 are serialized by inserting them into a temporary std::stringstream.
	 * @tparam TChar		  -	basic_string char type.
	 * @tparam TCharTraits	  -	basic_string char_traits type.
	 * @tparam TAlloc		  -	basic_string allocator type.
	 * @param condition		  -	A condition function.
	 * @param ...args		  -	Any number of elements that can be inserted into a std::ostream using operator<<.
	 * @returns					The resulting string.
	 */
	template<typename TChar = char, typename TCharTraits = std::char_traits<TChar>, typename TAlloc = std::allocator<TChar>, class ConditionFunc, var::streamable<std::basic_stringstream<TChar, TCharTraits, TAlloc>>... Ts>
	[[nodiscard]] constexpr static std::basic_string<TChar, TCharTraits, TAlloc> stringify_if(ConditionFunc const& condition, Ts&&... args)
	{
		if constexpr (var::none<Ts...>)
			return{};
		if (!condition())
			return{};
		std::basic_stringstream<TChar, TCharTraits, TAlloc> buffer;	// init stringstream
		(buffer << ... << std::forward<Ts>(args));	// insert variadic arguments in order
		return std::move(buffer.str());		// return as string
	}
#pragma endregion stringify_if

#pragma region stringify_join
	/**
	 * @brief					Joins the elements in the specified iterator range by
	 *							 inserting each one into a temporary std::stringstream.
	 * @tparam Iter			  -	The iterator type.
	 * @tparam TChar		  -	basic_string char type.
	 * @tparam TCharTraits	  -	basic_string char_traits type.
	 * @tparam TAlloc		  -	basic_string allocator type.
	 * @param begin			  -	The begin iterator for the range of elements.
	 * @param end			  -	The (exclusive) end iterator for the range of elements.
	 * @param join...		  -	Any number of stream operators or objects that implement an
	 *							 operator<< for ostreams to insert between each element.
	 * @returns					The resulting string.
	 */
	template<typename TChar = char, typename TCharTraits = std::char_traits<TChar>, typename TAlloc = std::allocator<TChar>, std::forward_iterator Iter, var::streamable<std::basic_stringstream<TChar, TCharTraits, TAlloc>>... Ts>
	[[nodiscard]] constexpr static std::basic_string<TChar, TCharTraits, TAlloc> stringify_join(Iter const& begin, Iter const& end, Ts&&... join)
	{
		// short-circuit if the container is empty
		if (std::distance(begin, end) == 0)
			return{};

		// initialize the buffer
		std::basic_stringstream<TChar, TCharTraits, TAlloc> buffer;

		// insert the selected items
		buffer << *begin;
		if constexpr (var::none<Ts...>) {
			// insert the selected items
			for (auto it{ begin + 1 }; it != end; ++it) {
				buffer << *it;
			}
		}
		else {
			// insert the selected items (with join inputs)
			for (auto it{ begin + 1 }; it != end; ++it) {
				(buffer << ... << std::forward<Ts>(join)) << *it;
			}
		}

		return std::move(buffer.str());
	}
	/**
	 * @brief					Joins the selected elements in the specified iterator range by
	 *							 inserting each one into a temporary std::stringstream.
	 * @tparam Iter			  -	The iterator type.
	 * @tparam Selector		  -	Selector function type.
	 * @tparam TChar		  -	basic_string char type.
	 * @tparam TCharTraits	  -	basic_string char_traits type.
	 * @tparam TAlloc		  -	basic_string allocator type.
	 * @param begin			  -	The begin iterator for the range of elements.
	 * @param end			  -	The (exclusive) end iterator for the range of elements.
	 * @param selector		  -	A function that selects what to insert for each element.
	 * @param join...		  -	Any number of stream operators or objects that implement an
	 *							 operator<< for ostreams to insert between each element.
	 * @returns					The resulting string.
	 */
	template<typename TChar = char, typename TCharTraits = std::char_traits<TChar>, typename TAlloc = std::allocator<TChar>, std::forward_iterator Iter, class Selector, var::streamable<std::basic_stringstream<TChar, TCharTraits, TAlloc>>... Ts>
	[[nodiscard]] constexpr static std::basic_string<TChar, TCharTraits, TAlloc> stringify_sjoin(Iter const& begin, Iter const& end, Selector const& selector, Ts&&... join)
	{
		// short-circuit if the container is empty
		if (std::distance(begin, end) == 0)
			return{};

		// initialize the buffer
		std::basic_stringstream<TChar, TCharTraits, TAlloc> buffer;

		if constexpr (var::none<Ts...>) {
			// insert the selected items
			for (auto it{ begin }; it != end; ++it) {
				buffer << selector(*it);
			}
		}
		else {
			// insert the selected items (with join inputs)
			for (auto it{ begin }; it != end; ++it) {
				(buffer << ... << std::forward<Ts>(join)) << selector(*it);
			}
		}

		return std::move(buffer.str());
	}
#pragma endregion stringify_join

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
	[[nodiscard]] inline STRCONSTEXPR std::string tolower(std::string str) noexcept
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
	[[nodiscard]] inline STRCONSTEXPR std::wstring tolower(std::wstring str) noexcept
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
	[[nodiscard]] inline STRCONSTEXPR std::string toupper(std::string str) noexcept
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
	[[nodiscard]] inline STRCONSTEXPR std::wstring toupper(std::wstring str) noexcept
	{
		for (auto& wch : str)
			wch = std::move(tolower(wch));
		return str;
	}

#pragma endregion ChangeCase_UpperWide
#pragma endregion toupper

#pragma region to/from bool
	/**
	 * @brief		Converts a string to bool.
	 * @param s		Case-insensitive input string.
	 *				Recognized values for true include "true", "1", "on", & "yes";
	 *				Recognized values for false include "false", "0", "off", & "no".
	 * @returns		true when s was one of the recognized values for true; false when s was one of the recognized values for false; otherwise std::nullopt if s was invalid.
	 */
	inline STRCONSTEXPR std::optional<bool> tobool(std::string const& s) noexcept
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
	inline STRCONSTEXPR std::string frombool(const bool b) noexcept
	{
		if (b) return "true"; else return "false";
	}
#pragma endregion to/from bool

#pragma region encoding
	template<var::any_same<char8_t, char16_t, char32_t> T>
	constexpr static std::string utfchar_to_string(T const& character)
	{
		if constexpr (sizeof(T) == 1)
			return{ 1, character };
		else {
			unsigned char* p{ &character };

			char arr[sizeof(T)]{};
			for (unsigned i{ 0 }, i_max{ sizeof(T) }; i < i_max; ++i) {
				arr[i] = *(p++);
			}
			return{ arr };
		}
	}
#pragma endregion encoding
}

/**
 * @file	strcompare.hpp
 * @author	radj307
 * @brief	Contains the string comparison functions from strlib.
 *\n		Comparison functions are any string functions that return a boolean 'query' about a given string.
 */
#pragma once
#include <charcompare.hpp>
#include <var.hpp>

#include <concepts>

namespace str {
#	pragma region Comparison

	/**
	 * @brief				Check if the given string starts with the given characters.
	 * @param str			Input String.
	 * @param ...prefix		Any number of types that can be represented with a string to check for.
	 * @returns				bool
	 */
	template<var::streamable<std::ostream>... Ts>
	inline WINCONSTEXPR bool startsWith(const std::string& str, Ts&&... prefix)
	{
		if constexpr (sizeof...(Ts) == 0ull)
			return false;
		else if (str.empty())
			return false;

		const std::string& comp{ stringify(std::forward<Ts>(prefix)...) };

		if (comp.size() > str.size())
			return false;

		size_t matches{ 0ull };
		for (auto itstr{ str.begin() }, itstrEnd{ str.end() }, itcomp{ comp.begin() }, itcompEnd{ comp.end() }; itstr < itstrEnd && itcomp < itcompEnd; ++itstr, ++itcomp) {
			if (*itstr == *itcomp)
				++matches;
			else return false;
		}
		return matches == comp.size();
	}

	/**
	 * @brief				Check if the given string starts with any of the given character sequences.
	 * @param str			Input String.
	 * @param ...suffixes	Any number of independent suffixes to check. Each variable is checked independently of the others.
	 * @returns				bool
	 */
	template<var::streamable<std::ostream>... Ts>
	inline WINCONSTEXPR bool startsWithAny(const std::string& str, Ts&&... prefixes)
	{
		return var::variadic_or(startsWith(str, prefixes)...);
	}

	/**
	 * @brief				Check if the given string ends with the given characters.
	 * @param str			Input String.
	 * @param ...suffix		Any number of types that can be represented with a string to check for.
	 *\n					The resolved strings are checked in reverse sequential order.
	 * @returns				bool
	 */
	template<var::streamable<std::ostream>... Ts>
	inline WINCONSTEXPR bool endsWith(const std::string& str, Ts&&... suffix)
	{
		if constexpr (sizeof...(Ts) == 0ull)
			return false;
		else if (str.empty())
			return false;

		const std::string& comp{ stringify(std::forward<Ts>(suffix)...) };

		if (comp.size() > str.size())
			return false;

		size_t matches{ 0ull };
		for (auto itstr{ str.rbegin() }, itstrEnd{ str.rend() }, itcomp{ comp.rbegin() }, itcompEnd{ comp.rend() }; itstr < itstrEnd && itcomp < itcompEnd; ++itstr, ++itcomp) {
			if (*itstr == *itcomp)
				++matches;
			else return false;
		}
		return matches == comp.size();
	}

	/**
	 * @brief				Check if the given string ends with any of the given character sequences.
	 * @param str			Input String.
	 * @param ...suffixes	Any number of independent suffixes to check. Each variable is checked independently of the others.
	 * @returns				bool
	 */
	template<var::streamable<std::ostream>... Ts>
	inline WINCONSTEXPR bool endsWithAny(const std::string& str, Ts&&... suffixes)
	{
		return var::variadic_or(endsWith(str, suffixes)...);
	}

	/**
	 * @brief				Check if the given string is equal to at least one other string.
	 * @param str			Input string to compare.
	 * @param ...compare	Comparison Strings.
	 * @returns				bool
	 */
	template<bool IGNORE_CASE = false, var::all_same_or_convertible<std::string>... Ts>
	inline WINCONSTEXPR bool equalsAny(const std::string& str, Ts&&... compare)
	{
		if constexpr (IGNORE_CASE) {
			const auto& lc{ str::tolower(str) };
			return var::variadic_or(lc == str::tolower(compare)...);
		}
		return var::variadic_or(str == compare...);
	}
	/**
	 * @brief				Check if the given string is equal to at least one other string.
	 * @param str			Input string to compare.
	 * @param ...compare	Comparison Strings.
	 * @returns				bool
	 */
	template<bool IGNORE_CASE = false, var::all_same_or_convertible<std::wstring, char>... Ts>
	inline WINCONSTEXPR bool equalsAny(const std::wstring& str, Ts&&... compare)
	{
		if constexpr (IGNORE_CASE) {
			const auto& lc{ str::tolower(str) };
			return var::variadic_or(lc == str::tolower(compare)...);
		}
		return var::variadic_or(str == compare...);
	}

	/**
	 * @brief				Check if a given string matches any of a list of strings.
	 * @tparam MatchCase	When true, matches are case sensitive, otherwise they are case insensitive.
	 * @tparam StrT			String Type.
	 * @tparam Ts...		Variadic number of types that are the same as StrT.
	 * @param str			Input String
	 * @param matches		At least one string to compare to str.
	 * @returns bool
	 */
	template<bool MatchCase = true, var::valid_string StrT, var::same_or_convertible<StrT>... Ts> requires var::at_least_one<Ts...>
	inline WINCONSTEXPR bool matches_any(const StrT& str, const Ts&... matches)
	{
		if constexpr (!MatchCase)
			return var::variadic_or((str::tolower(str) == str::tolower(matches))...);
		return var::variadic_or((str == matches)...);
	}

#	pragma endregion Comparison

#	pragma region Predicates

	/**
	 * @brief		Calls the given predicate function if the input character is within the specified range.
	 * @details		This is intended as a wrapper around standard functions like `isalpha` that only accept characters in the range (-1 <= c <= 255).
	 * @param func	The function to call, if the restrictions are met.
	 * @param in	The input character.
	 * @param min	The minimum value that can be passed to the given function.
	 * @param max	The maximum value that can be passed to the given function.
	 * @returns		bool
	 */
	template<std::predicate<char> Pred>
	constexpr bool call_if_valid(const Pred& func, const char& in, const char& min = -1, const char& max = 255)
	{
		return (in >= min && in <= max) && func(in) != 0;
	}

	/**
	 * @brief					Checks if the given string represents a valid number. **Do not pass whitespace characters to this function.**
	 * @param str				Input string.
	 * @param base				The number base to use when checking if digits are valid.
	 * @param allowDecimal		When true, one decimal point is allowed somewhere in the string.
	 * @param allowNegatived	When true, one dash is allowed at the beginning of the string.
	 * @returns					true when the given string represents a valid number; otherwise false.
	 */
	[[nodiscard]] WINCONSTEXPR inline bool isnumber(const std::string& str, const uint8_t base, const bool allowDecimal, const bool allowNegative)
	{
		bool fst{ true }, found_decimal{ false };
		for (const auto& ch : str) {
			switch (ch) {
			case '.':
				if (!allowDecimal)
					return false;
				else if (found_decimal)
					return false;
				else
					found_decimal = true;
				break;
			case '-':
				if (!allowNegative)
					return false;
				else if (!fst)
					return false;
				break;
			default:
				if (!isdigit_base(ch, base))
					return false;
				break;
			}

			if (fst) fst = false;
		}
		return true;
	}
	/**
	 * @brief		Checks if the given string represents a valid integral. This function automatically disallows the dash prefix (negative) character for unsigned integral types.
	 * @param str	Input string.
	 * @param base	The number base to use when checking if a character is a valid digit.
	 * @returns		true when str specifies a valid integral number.
	 */
	template<std::integral T>
	[[nodiscard]] WINCONSTEXPR inline bool isnumber(const std::string& str, const uint8_t base = 10)
	{
		if constexpr (std::unsigned_integral<T>)
			return isnumber(str, base, false, false);
		else
			return isnumber(str, base, false, true);
	}
	/**
	 * @brief		Checks if the given string represents a valid floating-point. Allows a maximum of 1 decimal and 1 dash, but only as a prefix.
	 * @param str	Input string.
	 * @param base	The number base to use when checking if a character is a valid digit.
	 * @returns		true when str specifies a valid floating-point number.
	 */
	template<std::floating_point T>
	[[nodiscard]] WINCONSTEXPR inline bool isnumber(const std::string& str, const uint8_t base = 10)
	{
		return isnumber(str, base, true, true);
	}
	/**
	 * @brief		Checks if the given string represents a valid floating-point. Allows a maximum of 1 decimal and 1 dash, but only as a prefix.
	 * @param str	Input string.
	 * @param base	The number base to use when checking if a character is a valid digit.
	 * @returns		true when str specifies a valid floating-point number.
	 */
	[[nodiscard]] WINCONSTEXPR inline bool isnumber(const std::string& str, const uint8_t base = 10)
	{
		return isnumber<float>(str, base);
	}

#	pragma endregion Predicates

}

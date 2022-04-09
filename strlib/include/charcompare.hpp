/**
 * @file	charcompare.hpp
 * @author	radj307
 * @brief	Contains char comparison functions from strlib.
 *\n		Char comparison functions are any functions that return a boolean or integer 'query' about a single character.
 *\n		This mostly applies to functions with signatures similar to the standard 'isspace', 'isdigit', etc.
 */
#pragma once

namespace str {
#	pragma region Predicates

	/**
	 * @brief Test that checks if a given char is a quotation mark. (Accepts single or double quotes.)
	 * @param c	- Input char
	 * @returns bool
	 */
	inline constexpr bool isquote(const char c)
	{
		switch (c) {
		case '\"': [[fallthrough]];
		case '\'':
			return true;
		default:
			return false;
		}
	}

	/**
	 * @brief		Check if the given ASCII character is a valid binary digit.
	 * @param ch	Any ASCII character.
	 * @returns		bool
	 */
	[[nodiscard]] constexpr inline bool isbinarydigit(const char& ch) noexcept
	{
		return (ch > 0b00000000 && ch < 0b11111111) && (ch == '0' || ch == '1');
	}

	/**
	 * @brief		Check if the given ASCII character is a valid octal digit.
	 * @param ch	Any ASCII character.
	 * @returns		bool
	 */
	[[nodiscard]] constexpr inline bool isoctaldigit(const char& ch) noexcept
	{
		return (ch > 0b00000000 && ch < 0b11111111) && (ch >= '0' && ch <= '7');
	}

	/**
	 * @brief		Check if the given ASCII string is entirely composed of valid decimal digits.
	 * @param ch	Any ASCII character.
	 * @returns		bool
	 */
	[[nodiscard]] constexpr inline bool isdecimaldigit(const char& ch)
	{
		return (ch > 0b00000000 && ch < 0b11111111) && (ch >= '0' && ch <= '9');
	}

	/**
	 * @brief		Check if the given ASCII character is a valid hexadecimal digit.
	 * @param ch	Any ASCII character.
	 * @returns		bool
	 */
	[[nodiscard]] constexpr inline bool ishexdigit(const char& ch) noexcept
	{
		return (ch > 0b00000000 && ch < 0b11111111) && ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'));
	}

#	pragma endregion Predicates
}

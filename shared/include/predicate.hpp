#pragma once
/**
 * @file	predicate.hpp
 * @author	radj307
 * @brief	Contains safe & bool-typed versions of standard predicate functions, as well as some new ones.
 */

namespace shared {
	template<std::predicate<int> Func>
	[[nodiscard]] inline bool is_(int const& c, Func const& predicate) noexcept
	{
		return (c >= -1 && c <= 255) && predicate(c);
	}
	/**
	 * @brief	Check if the given character is any alphabetic character.
	 * @param c	The character to check.
	 * @returns true when the given character is alphabetic; otherwise false.
	 */
	[[nodiscard]] inline bool isalpha(int const& c) noexcept
	{
		return is_(c, ::isalpha);
	}
	/**
	 * @brief	Check if the given character is any whitespace character.
	 *\n		(' ', '\t', '\v', '\r', '\n')
	 * @param c	The character to check.
	 * @returns true when the given character is whitespace; otherwise false.
	 */
	[[nodiscard]] inline bool isspace(int const& c) noexcept
	{
		return is_(c, ::isspace);
	}
	/**
	 * @brief	Check if the given character is any digit character.
	 *\n		('0', '1', '2', '3', '4', '5', '6', '7', '8', '9')
	 * @param c	The character to check.
	 * @returns true when the given character is a digit; otherwise false.
	 */
	[[nodiscard]] inline bool isdigit(int const& c) noexcept
	{
		return is_(c, ::isdigit);
	}
	/**
	 * @brief	Check if the given character is any alphanumeric character.
	 * @param c	The character to check.
	 * @returns true when the given character is alphanumeric; otherwise false.
	 */
	[[nodiscard]] inline bool isalnum(int const& c) noexcept
	{
		return is_(c, ::isalnum);
	}
}

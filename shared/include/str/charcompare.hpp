#pragma once
/**
 * @file	charcompare.hpp
 * @author	radj307
 * @brief	Contains char comparison functions from strlib.
 *\n		Char comparison functions are any functions that return a boolean or integer 'query' about a single character.
 *\n		This mostly applies to functions with signatures similar to the standard 'isspace', 'isdigit', etc.
 */

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

	/// @brief	[Base2] Binary system.
	inline constexpr const auto BINARY{ 2 };
	/// @brief	[Base3] Ternary system.
	inline constexpr const auto TERNARY{ 3 };
	/// @brief	[Base4] Quarternary system.
	inline constexpr const auto QUARTERNARY{ 4 };
	/// @brief	[Base8] Octal system.
	inline constexpr const auto OCTAL{ 8 };
	/// @brief	[Base10] Decimal system.
	inline constexpr const auto DECIMAL{ 10 };
	/// @brief	[Base12] Duodecimal system.
	inline constexpr const auto DUODECIMAL{ 12 };
	/// @brief	[Base16] Hexadecimal system.
	inline constexpr const auto HEXADECIMAL{ 16 };
	/// @brief	[Base32] Base32 system.
	inline constexpr const auto BASE32{ 32 };

	/// @brief			The minimum accepted numeric base for the isdigit_base function. (binary)
	inline constexpr const auto MIN_BASE{ BINARY };
	/// @brief			The maximum accepted numeric base for the isdigit_base function. (base32)
	inline constexpr const auto MAX_BASE{ BASE32 };
	/**
	 * @brief			All chars used to represent the MAX_BASE (base32).
	 * @details			This is used by the isdigit_base function.
	 */
	inline constexpr const char ALL_NUMERIC_CHARS[MAX_BASE]{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V' };

	/**
	 * @brief			Checks if the given character is a valid digit in the specified arbitrary number base, from MIN_BASE (2) to MAX_BASE (32).
	 * @param ch		Input character. *(This is converted to uppercase within this function, so you don't need to do that beforehand)*
	 * @param base		The number base, and consequently the valid character range. **(min: 2 *(binary)*, default: 10 *(decimal)*, max: 32)**
	 * @returns			true when ch is a valid digit in the specified base; otherwise false. If the specified base is out-of-bounds, immediately returns false.
	 */
	[[nodiscard]] WINCONSTEXPR inline bool isdigit_base(char ch, const uint8_t base = 10) noexcept
	{
		if (base < MIN_BASE || base > MAX_BASE)
			return false;
		else if (ch >= 'a' && ch <= 'z')
			ch = toupper(ch);
		for (uint8_t i{ 0 }; i < base; ++i)
			if (ch == ALL_NUMERIC_CHARS[i])
				return true;
		return false;
	}
	/**
	 * @brief			Checks if the given character is a valid digit in the specified number base.
	 * @tparam BASE		The number base to test for. **(min: 2 *(binary)*, max: 32)**
	 * @param ch		Input character.
	 * @returns			true when ch is a valid digit in the specified base.
	 */
	template<uint8_t BASE> requires (BASE >= MIN_BASE && BASE <= MAX_BASE)
		[[nodiscard]] WINCONSTEXPR inline bool isdigit_base(const char& ch) noexcept { return isdigit_base(ch, BASE); }

	/// @returns	true when ch is a valid binary (base2) digit [01]; otherwise false.
	[[nodiscard]] WINCONSTEXPR inline bool isbinarydigit(const char& ch) noexcept
	{
		return isdigit_base(ch, BINARY);
	}
	/// @returns	true when ch is a valid ternary (base3) digit [012]; otherwise false.
	[[nodiscard]] WINCONSTEXPR inline bool isternarydigit(const char& ch) noexcept
	{
		return isdigit_base(ch, TERNARY);
	}
	/// @returns	true when ch is a valid quaternary (base4) digit [0123]; otherwise false.
	[[nodiscard]] WINCONSTEXPR inline bool isquarternarydigit(const char& ch) noexcept
	{
		return isdigit_base(ch, QUARTERNARY);
	}
	/// @returns	true when ch is a valid octal (base8) digit [01234567]; otherwise false.
	[[nodiscard]] WINCONSTEXPR inline bool isoctaldigit(const char& ch) noexcept
	{
		return isdigit_base(ch, OCTAL);
	}
	/// @returns	true when ch is a valid decimal (base10) digit [0123456789]; otherwise false.
	[[nodiscard]] WINCONSTEXPR inline bool isdecimaldigit(const char& ch)
	{
		return isdigit_base(ch, DECIMAL);
	}
	/// @returns	true when ch is a valid duodecimal (base12) digit [0123456789AB]; otherwise false.
	[[nodiscard]] WINCONSTEXPR inline bool isduodecimaldigit(const char& ch)
	{
		return isdigit_base(ch, DUODECIMAL);
	}
	/// @returns	true when ch is a valid hexadecimal (base16) digit [0123456789ABCDEF]; otherwise false.
	[[nodiscard]] WINCONSTEXPR inline bool ishexdigit(const char& ch) noexcept
	{
		return isdigit_base(ch, HEXADECIMAL);
	}
	/// @returns	true when ch is a valid base32 digit [0123456789ABCDEFGHIJKLMNOPQRSTUV]; otherwise false.
	[[nodiscard]] WINCONSTEXPR inline bool isbase32digit(const char& ch) noexcept
	{
		return isdigit_base(ch, BASE32);
	}

	/**
	 * @namespace	stdpred
	 * @brief		Contains improved overloads of predicate functions from the standard library, such as 'isspace', 'isdigit', and 'isalpha', among others
	 */
	namespace stdpred {
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

#	pragma endregion Predicates
}

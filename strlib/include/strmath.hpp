/**
 * @file	strmath.hpp
 * @author	radj307
 * @brief	Contains functions for performing mathematics conversions & calculations on strings.
 *\n		Primarily focuses on converting between number bases like binary, octal, decimal, & hexadecimal.
 */
#pragma once
#include <make_exception.hpp>
#include <var.hpp>
#include <str.hpp>

#include <concepts>

namespace str {
#	pragma region Predicates

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
	 * @brief		Check if the given ASCII string is entirely composed of valid binary digits.
	 * @param str	Any ASCII string.
	 * @returns		bool
	 */
	[[nodiscard]] WINCONSTEXPR inline bool isbinary(const std::string& str) noexcept
	{
		for (auto it{ str.begin() + (str::startsWith(str, "0b") ? 2 : 0) }, endit{ str.end() }; it != endit; ++it)
			if (!isbinarydigit(*it))
				return false;
		return true;
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
	 * @brief		Check if the given ASCII string is entirely composed of valid octal digits.
	 * @param str	Any ASCII string.
	 * @return		bool
	 */
	[[nodiscard]] WINCONSTEXPR inline bool isoctal(const std::string& str) noexcept
	{
		for (auto it{ str.begin() + (str::startsWith(str, '\\') ? 1 : 0) }, endit{ str.end() }; it != endit; ++it)
			if (!isoctaldigit(*it))
				return false;
		return true;
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
	 * @brief		Check if the given ASCII string is entirely composed of valid decimal digits.
	 * @param str	Any ASCII string.
	 * @return		bool
	 */
	[[nodiscard]] WINCONSTEXPR inline bool isdecimal(const std::string& str) noexcept
	{
		for (const auto& ch : str)
			if (!isdecimaldigit(ch))
				return false;
		return true;
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

	/**
	 * @brief		Check if the given ASCII string is entirely composed of valid hexadecimal digits.
	 * @param str	Any ASCII string.
	 * @returns		bool
	 */
	[[nodiscard]] WINCONSTEXPR inline bool ishex(const std::string& str) noexcept
	{
		for (auto it{ str.begin() + (str::startsWith(str, "0x") ? 2 : 0) }, endit{ str.end() }; it != endit; ++it)
			if (!ishexdigit(*it))
				return false;
		return true;
	}

#	pragma endregion Predicates

	/**
	 * @brief			Get the length of the given integral number in digits.
	 * @param integral	Input number.
	 * @returns			size_t
	 */
	template<std::integral T>
	[[nodiscard]] constexpr inline static size_t getLength(T integral) noexcept
	{
		size_t count{ 1ull };
		for (const auto& ten{ static_cast<T>(10) }, two{ static_cast<T>(2) }; integral > ten; ++count)
			integral /= two;
		return count;
	}

	// @brief	Alias for type (long long).
	using decimal = long long;

	/**
	 * @brief		Convert an alphanumeric representation of a number to its decimal equivalent.
	 * @param ch	Any alphanumeric number representation from 0(0) to Z(36).
	 * @returns		decimal
	 */
	constexpr decimal fromAlphaNumeric(const char& ch) noexcept(false)
	{
		if (ch >= '0' && ch <= '9')
			return static_cast<decimal>(ch - '0');

		const char& c = str::toupper(ch);
		if (c >= 'A' && c <= 'F')
			return static_cast<decimal>(c - 'A') + 10ll;

		throw make_exception("str::fromAlphaNumeric() failed:  \'", c, "\' ", (c != ch ? ("(\'"s + ch + "\') ") : ""), "isn't a valid alphanumeric digit character in the range ( 0 - Z(36) )");
	}
	/**
	 * @brief			Convert any number between 0(0) to 36(Z) to its alphanumeric representation.
	 * @param n			Input number between 0 - 36.
	 * @param upper		When true, the returned character is uppercase instead of lowercase.
	 * @returns			char
	 */
	template<std::integral T>
	constexpr char toAlphaNumeric(const T& n, const bool& upper = true) noexcept(false)
	{
		if (n < static_cast<T>(0) || n > static_cast<T>('Z' - 'A' + 10))
			throw make_exception("str::toAlphaNumeric() failed:  \'", n, "\' cannot be converted to an alphanumeric value because it isn't within the range ( 0 - 36(Z) )");

		if (n < 10)
			return '0' + n;
		else return (upper ? 'A' : 'a') + (n - 10);
	}

	/**
	 * @brief				Convert a number from an arbitrary base to base 10 (decimal).
	 * @param number		Input number as a string.
	 * @param fromBase		The base of the input number.
	 * @returns				decimal
	 */
	WINCONSTEXPR decimal toBase10(std::string number, const int& fromBase) noexcept(false)
	{
		if (number.empty())
			return 0;
		else if (fromBase < 2 || fromBase > 36)
			throw make_exception("str::toBase10() failed:  \'", fromBase, "\' isn't a valid alphanumeric base in the range: (2 - 36)!");
		else if (fromBase == 10)
			return str::stoll(number);

		if (str::startsWith(number, "0b") || str::startsWith(number, "0x"))
			number = number.substr(2ull);
		else if (str::startsWith(number, '\\'))
			number = number.substr(1ull);

		bool negative{ number.at(0) == '-' };

		decimal value{ 0ll }, dValue{ 1ll };

		for (auto it{ number.rbegin() }; it != number.rend(); ++it) {
			value += fromAlphaNumeric(*it) * dValue;
			dValue *= fromBase;
		}

		if (negative)
			value = -value;
		return value;
	}

	/**
	 * @brief				Convert a number from base 10 (decimal) to an arbitrary base.
	 * @param number		Input decimal number.
	 * @param toBase		The target number base.
	 * @returns				std::string
	 */
	WINCONSTEXPR std::string fromBase10(const decimal& number, const int& toBase) noexcept(false)
	{
		if (toBase < 2 || toBase > 36)
			throw make_exception("str::fromBase10() failed:  \'", toBase, "\' isn't a valid alphanumeric base in the range: (2 - 36)!");
		else if (toBase == 10)
			return std::to_string(number);

		// short circuit if zero
		if (number == 0)
			return "0";

		bool negative{ number < 0 };
		// copy the positive value of number to a mutable value
		decimal n{ negative ? -number : number };

		std::string value;
		value.reserve(getLength(number));

		do {
			value += toAlphaNumeric(n % toBase);
			n /= toBase;
		} while (n > 0);

		if (negative)
			value += '-';

		return str::reverse(value);
	}
	/**
	 * @brief				Convert a number from base 10 (decimal) to an arbitrary base.
	 * @param number		Input decimal number.
	 * @param toBase		The target number base.
	 * @returns				std::string
	 */
	WINCONSTEXPR std::string fromBase10(const std::string& number, const int& toBase) noexcept(false)
	{
		return fromBase10(str::stoll(number), toBase);
	}

	struct FloatingBase {
	protected:
		int base;

	public:
		std::string number;

		template<std::integral T>
		FloatingBase(const T& number) : base{ 10 }, number{ std::to_string(number) } {}

		FloatingBase(const std::string& number, const int& base) : base{ base }, number{ number } {}

		FloatingBase(const int& base) : base{ base }, number{ "0" } {}

		/**
		 * @brief		Get the current base.
		 * @returns		int
		 */
		int getBase() const { return base; }

		/**
		 * @brief		Set the numeric base to a new value, and convert the number.
		 * @param base	The base to convert the current number to.
		 * @returns		int
		 *\n			The previous base.
		 */
		int setBase(const int& base) { const int copy{ base }; convertBase(base); return copy; }

		[[nodiscard]] std::string convertedToBase(const int& tBase) const
		{
			if (base == tBase)
				return number;
			if (base == 10) // convert from decimal
				return fromBase10(str::stoll(number), tBase);
			else if (tBase == 10) // convert to decimal
				return std::to_string(toBase10(number, base));
			else // convert current base to decimal, then convert to the target base
				return fromBase10(toBase10(number, base), tBase);
			return{ "0" };
		}

		void convertBase(const int& tBase)
		{
			if (base == tBase)
				return;
			number = convertedToBase(tBase);
			base = tBase;
		}

		friend std::ostream& operator<<(std::ostream& os, const FloatingBase& fb)
		{
			return os << fb.number;
		}
	};



#	pragma region Converters

	/**
	 * @brief			Convert a binary number to its decimal equivalent.
	 * @param binary	Binary number contained within a string. This must NOT include any additional characters or prefixes.
	 * @returns			long long
	 */
	inline long long binaryToDecimal(const std::string& binary) noexcept(false)
	{
		long long v{ 0ll };
		for (const auto& ch : binary) {
			v <<= 1ll;
			if (ch == '1')
				v |= 0b1;
			else if (ch != '0')
				throw make_exception("Invalid binary number: \'", ch, '\'');
		}
		return v;
	}

	/**
	 * @brief			Convert an integral number to its binary equivalent.
	 * @param decimal	Decimal number.
	 * @returns			std::string
	 */
	inline std::string decimalToBinary(long long const& decimal) noexcept
	{
		long long n{ static_cast<long long>(decimal) };
		std::string v;
		for (; n > 0ll; n /= 2ll)
			v += n % 2ll;
		return v;
	}



	/**
	 * @brief			Converts a given hexadecimal character to its decimal equivalent.
	 * @param ch		Input character. 1-9, A=10, B=11, C=12, D=13, E=14, F=15. Any character outside of this range will throw an exception.
	 * @returns			long long
	 */
	inline long long hexToDecimal(const char ch) noexcept(false)
	{
		if (isdigit(ch))
			return static_cast<long long>(ch - '0');
		else if ((ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'))
			return (static_cast<long long>(str::toupper(ch) - 'A') + 10);
		throw make_exception("getHexValue()\tFailed to convert \'", ch, "\' to hexadecimal!");
	}

	/**
	 * @brief			Convert from hexadecimal to decimal.
	 * @param hex		Input hexadecimal value.
	 * @returns			value
	 */
	inline long long hexToDecimal(std::string const& hex) noexcept(false)
	{
		const auto from_base{ 16 };
		if (!hex.empty()) {
			long long power{ 1 }, result{ 0 };
			for (auto c{ hex.rbegin() }; c != hex.rend(); ++c) {
				if (*c == 'x')
					break; // avoid checking "0x" prefix
				const auto v{ hexToDecimal(*c) };
				if (v >= from_base)
					throw make_exception("Hexadecimal value \'", *c, "\' converted to invalid hex number \"", v, "\"!\n", indent(10), "Please report this error to the developer!");
				result += v * power;
				power *= from_base;
			}
			return result;
		}
		else throw make_exception("Received an empty hexadecimal number, conversion failed!");
	}

	/**
	 * @brief			Convert from decimal to hexadecimal.
	 * @param dec		Input decimal value.
	 * @param ...fmt	(Optional) Additional stream formatting objects to include before printing the value.
	 *\n				 Ex: std::uppercase, std::nouppercase, etc.
	 * @returns			std::string
	 */
	template<var::Streamable<std::stringstream>... Ts>
	inline std::string decimalToHex(long long const& dec, Ts&&... fmt) noexcept
	{
		return str::stringify(
			std::hex,
			std::forward<Ts>(fmt)...,
			dec
		);
	}

	/**
	 * @brief			Convert from decimal to hexadecimal.
	 * @param dec		Input decimal value as a string.
	 * @param ...fmt	(Optional) Additional stream formatting objects to include before printing the value.
	 *\n				 Ex: std::uppercase, std::nouppercase, etc.
	 * @returns			std::string
	 */
	template<var::Streamable<std::stringstream>... Ts>
	inline std::string decimalToHex(std::string const& dec, Ts&&... fmt) noexcept
	{
		return str::stringify(
			std::hex,
			std::forward<Ts>(fmt)...,
			str::stoll(dec)
		);
	}

#	pragma endregion Converters
}
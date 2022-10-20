/**
 * @file		strconv.hpp
 * @author		radj307
 * @brief		Contains char & string conversion functions with signatures similar to the standard `atoi` & `stoi` style functions, with additional type support.
 *\n			This includes the `tolower` & `toupper` functions that have stricter validation & have overloads that accept whole strings rather than single characters.
 *\n			Multiple 'generations' of these functions are available, but all have a common theme:
 *				- Remove the possibility of exceptions being thrown during conversion operations.
 *				- Optimized for being passed as (pseudo)-lambdas to other functions, especially in an environment where exceptions are unwanted or unnecessary.
 * @attention	Make sure you carefully evaluate whether or not to use these functions outside of strlib, as in many cases the exception-avoidance measures taken by all of these functions may not be desirable.
 *\n			This header is specifically designed to handle many potential errors behind-the-scenes without any indications, and may react unexpectedly when used as a drop-in replacement for standard library functions, despite their ability to do that.
 */
#pragma once
#include <sysarch.h>
#include <var.hpp>
#include <stringify.hpp>
#include <strcore.hpp>

#include <concepts>
#include <string>
#include <sstream>
#include <optional>
#include <iomanip>

 /**
  * @namespace str
  * @brief Contains various string manipulation, conversion, and parsing functions
  */
namespace str {
	// TOLOWER

#pragma region StringNumericTypeConversions
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
		[[nodiscard]] inline CONSTEXPR const std::string bool_to_string(const bool& val, const bool& first_upper = false)
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
		[[nodiscard]] CONSTEXPR ReturnType string_to_bool(std::string str)
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
		[[nodiscard]] inline int stoi(const std::string& str, size_t* nextCh = nullptr, int base = 10) noexcept
		{
			try {
				return std::stoi(str, nextCh, base);
			} catch (...) { return 0; }
		}
		/**
		 * @brief			Converts string to short. Wrapper for std::stoi that can be passed as std::function. Does not throw exceptions.
		 * @param str		String to convert
		 * @param nextCh	Optional pointer that is set to the index of the next character in the string after the number.
		 * @returns			short
		 */
		[[nodiscard]] inline short stos(const std::string& str, size_t* nextCh = nullptr, int base = 10) noexcept
		{
			try {
				return static_cast<short>(std::stoi(str, nextCh, base));
			} catch (...) { return static_cast<short>(0); }
		}
		/**
		 * @brief			Converts string to unsigned int. Wrapper for static_cast<unsigned>(std::stoi) that can be passed as std::function. Does not throw exceptions.
		 * @param str		String to convert
		 * @param nextCh	Optional pointer that is set to the index of the next character in the string after the number.
		 * @returns			unsigned int
		 */
		[[nodiscard]] inline unsigned int stoui(const std::string& str, size_t* nextCh = nullptr, int base = 10) noexcept
		{
			try {
				return static_cast<unsigned>(std::stoi(str, nextCh, base));
			} catch (...) { return 0u; }
		}
		/**
		 * @brief			Converts string to long. Wrapper for std::stol that can be passed as std::function. Does not throw exceptions.
		 * @param str		String to convert
		 * @param nextCh	Optional pointer that is set to the index of the next character in the string after the number.
		 * @returns			long
		 */
		[[nodiscard]] inline long stol(const std::string& str, size_t* nextCh = nullptr, int base = 10) noexcept
		{
			try {
				return std::stol(str, nextCh, base);
			} catch (...) { return 0l; }
		}
		/**
		 * @brief			Converts string to long long. Wrapper for std::stol that can be passed as std::function. Does not throw exceptions.
		 * @param str		String to convert
		 * @param nextCh	Optional pointer that is set to the index of the next character in the string after the number.
		 * @returns			long long
		 */
		[[nodiscard]] inline long long stoll(const std::string& str, size_t* nextCh = nullptr, int base = 10) noexcept
		{
			try {
				return static_cast<long long>(std::stoll(str, nextCh, base));
			} catch (...) { return 0ll; }
		}
		/**
		 * @brief			Converts string to unsigned long. Wrapper for std::stoul that can be passed as std::function. Does not throw exceptions.
		 * @param str		String to convert
		 * @param nextCh	Optional pointer that is set to the index of the next character in the string after the number.
		 * @returns			unsigned long
		 */
		[[nodiscard]] inline unsigned long stoul(const std::string& str, size_t* nextCh = nullptr, int base = 10) noexcept
		{
			try {
				return std::stoul(str, nextCh, base);
			} catch (...) { return 0ul; }
		}
		/**
		 * @brief			Converts string to unsigned long. Wrapper for std::stoul that can be passed as std::function. Does not throw exceptions.
		 * @param str		String to convert
		 * @param nextCh	Optional pointer that is set to the index of the next character in the string after the number.
		 * @returns			unsigned long
		 */
		[[nodiscard]] inline unsigned long long stoull(const std::string& str, size_t* nextCh = nullptr, int base = 10) noexcept
		{
			try {
				return std::stoull(str, nextCh, base);
			} catch (...) { return 0ul; }
		}
		/**
		 * @brief			Converts string to unsigned short. Wrapper for static_cast<unsigned short>(std::stoi) that can be passed as std::function. Does not throw exceptions.
		 * @param str		String to convert
		 * @param nextCh	Optional pointer that is set to the index of the next character in the string after the number.
		 * @returns			unsigned int
		 */
		[[nodiscard]] inline unsigned short stous(const std::string& str, size_t* nextCh = nullptr, int base = 10) noexcept
		{
			try {
				return static_cast<unsigned short>(std::stoi(str, nextCh, base));
			} catch (...) { return static_cast<unsigned short>(0); }
		}
		/**
		 * @brief			Converts string to float. Wrapper for std::stof that can be passed as std::function. Does not throw exceptions.
		 * @param str		String to convert
		 * @param nextCh	Optional pointer that is set to the index of the next character in the string after the number.
		 * @returns			float
		 */
		[[nodiscard]] inline float stof(const std::string& str, size_t* nextCh = nullptr, int base = 10) noexcept
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
	 * @brief					Convert a floating-point number to a string, with variable precision and automatic truncation.
	 *\n						This is a legacy support function.
	 * @param value:			The input value.
	 * @param precision:		The number of digits to set the floating-point precision field to.
	 * @param force_decimal:	When true, the returned string will end with ".0" if there are no non-zero digits after the decimal point.
	 * @returns					std::string
	*/
	template<std::floating_point T>
	[[nodiscard]] inline CONSTEXPR const std::string to_string(const T& value, const std::streamsize& precision = 8, const bool force_decimal = false)
	{
		std::string s{ str::stringify(std::fixed, std::setprecision(precision), value) };

		const size_t decimalPos{ s.rfind('.') };

		if (decimalPos < s.size()) {
			const size_t lastNonZeroPos{ s.find_last_not_of("0") };

			if (lastNonZeroPos >= s.size() - 1)
				return s;
			else if (lastNonZeroPos > decimalPos) {
				s.erase(s.begin() + (lastNonZeroPos + 1ull), s.end());
			}
			else {
				s.erase(s.begin() + (decimalPos + 2ull), s.end());
			}
		}

		return s;
	}
#pragma endregion StringNumericTypeConversions
}

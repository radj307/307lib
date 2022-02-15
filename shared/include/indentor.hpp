/**
 * @file	indentor.hpp
 * @author	radj307
 * @brief	Contains the indentor object definition.
 *\n		The indentor can be used to quickly and easily insert variable indentation into an output stream, or retrieve it as a string.
 */
#pragma once
#include <sysarch.h>

#include <ostream>
#include <concepts>
#include <iomanip>

 /**
  * @struct		indentor
  * @brief		Handles calculating variable indentation for strings or output streams.
  * @tparam T	The integral type to use for the size values.
  */
template<std::integral T = std::streamsize, typename CharT = char>
struct indentor {
	/// @brief	The size type used.
	using size_type = T;
	/// @brief	The character width to use.
	using char_type = CharT;
	/// @brief	The string type associated with the given char_type.
	using string_type = std::basic_string<char_type, std::char_traits<char_type>, std::allocator<char_type>>;
protected:
	/// @brief	The calculated size of the indentation, in characters.
	const size_type sz;
public:
	/**
	 * @brief		Constructor
	 * @param size	The maximum size of the indent, in characters. If this is smaller than the value of "used", size will not go below 0.
	 * @param used	The number of characters that are already printed and should be accounted for before printing the indentation.
	 */
	CONSTEXPR indentor(size_type const& size, size_type const& used = static_cast<size_type>(0)) : sz{ (size < used) ? static_cast<size_type>(0) : (size - used) } {}

	/**
	 * @brief		Retrieve the size in characters of the indentation.
	 * @returns		size_type
	 */
	CONSTEXPR size_type size() const { return sz; }
	/**
	 * @brief		Retrieve the indentation as a string of given characters, or spaces by default.
	 * @param c		The character to use for the indentation string.
	 * @returns		string_type
	 */
	WINCONSTEXPR string_type toString(const char& c = ' ') const { return string_type(c, sz); }
	/**
	 * @brief		Retrieve the indentation as a string.
	 * @returns		string_type
	 */
	WINCONSTEXPR operator string_type() const { return toString(); }
	/**
	 * @brief		Retrieve the size of the indentation in characters.
	 * @returns		size_type
	 */
	CONSTEXPR operator size_type() const { return sz; }

	/**
	 * @brief		Output stream insertion operator.
	 *\n			This function should not be directly called, instead use the syntax:
	 *\n				<< indentor_object;
	 * @param os	Output stream instance.
	 * @param ind	Indentor instance.
	 * @returns		std::ostream&
	 */
	friend std::ostream& operator<<(std::ostream& os, const indentor<T, CharT>& ind)
	{
		if (ind.sz == static_cast<T>(0))
			return os;
		return os << std::setw(static_cast<std::streamsize>(ind.sz - static_cast<T>(1))) << ' ';
	}
};
/// @brief	Variable on-demand indentation for output streams. 
using indent = indentor<std::streamsize>;

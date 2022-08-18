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

namespace shared {
	template<typename TChar, typename TCharTraits = std::char_traits<TChar>, typename TAlloc = std::allocator<TChar>>
	struct basic_indentor {
		using string_t = std::basic_string<TChar, TCharTraits, TAlloc>;

	protected:
		size_t _size;
		TChar _fill_char;

	public:
		/**
		 * @brief		Constructor
		 * @param size	The maximum size of the indent, in characters. If this is smaller than the value of "used", size will not go below 0.
		 * @param used	The number of characters that are already printed and should be accounted for before printing the indentation.
		 */
		CONSTEXPR basic_indentor(const size_t size, const size_t used = 0ull) : _size{ (size <= used) ? 0ull : (size - used) }, _fill_char{ ' ' } {}
		/**
		 * @brief		Constructor
		 * @param size	The maximum size of the indent, in characters. If this is smaller than the value of "used", size will not go below 0.
		 * @param used	The number of characters that are already printed and should be accounted for before printing the indentation.
		 * @param fill	The character to use for indentation. (Default: ' ')
		 */
		CONSTEXPR basic_indentor(const size_t size, const size_t used, const char fill) : _size{ (size <= used) ? 0ull : (size - used) }, _fill_char{ fill } {}

		/**
		 * @brief		Retrieve the size in characters of the indentation.
		 * @returns		The size of the indentation space.
		 */
		CONSTEXPR size_t size() const
		{
			return this->_size;
		}

		/**
		 * @brief		Retrieve the indentation as a string of given characters, or spaces by default.
		 * @returns		string_t
		 */
		CONSTEXPR string_t toString() const
		{
			return string_t(this->_size, this->_fill_char);
		}

		/**
		 * @brief		Retrieve the indentation as a string.
		 * @returns		string_t
		 */
		CONSTEXPR operator string_t() const { return toString(); }
		/**
		 * @brief		Retrieve the size of the indentation in characters.
		 * @returns		size_t
		 */
		CONSTEXPR operator size_t() const { return _size; }

		/**
		 * @brief		Output stream insertion operator.
		 *\n			This function should not be directly called, instead use the syntax:
		 *\n				<< indentor_object;
		 * @param os	Output stream instance.
		 * @param ind	Indentor instance.
		 * @returns		std::ostream&
		 */
		friend std::ostream& operator<<(std::ostream& os, const basic_indentor<TChar, TCharTraits, TAlloc>& ind)
		{
			if (ind._size == 0ull)
				return os;
			return os << std::setfill(ind._fill_char) << std::setw(static_cast<std::streamsize>(ind._size)) << ind._fill_char;
		}
	};
	using indentor = basic_indentor<char, std::char_traits<char>, std::allocator<char>>;
	using windentor = basic_indentor<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t>>;
	/// @brief	Variable on-demand indentation for output streams. 
	using indent = indentor;
}

#ifndef INDENTOR_HPP_NOGLOBAL
using shared::indentor;
using shared::windentor;
using shared::indent;
#endif

/**
 * @file	indentor.hpp
 * @author	radj307
 * @brief	Contains the indentor object definition.
 *\n		The indentor can be used to quickly and easily insert variable indentation into an output stream, or retrieve it as a string.
 */
#pragma once
#include <sysarch.h>

#include <ostream>
#include <iomanip>

namespace shared {
	template<typename TChar, typename TCharTraits = std::char_traits<TChar>, typename TAlloc = std::allocator<TChar>>
	struct basic_indentor {
		using string_t = std::basic_string<TChar, TCharTraits, TAlloc>;

	protected:
		size_t width;
		TChar fill_char;

	public:
		/**
		 * @brief		Constructor
		 * @param size	The maximum size of the indent, in characters. If this is smaller than the value of "used", size will not go below 0.
		 */
		CONSTEXPR basic_indentor(const size_t size) : width{ size }, fill_char{ static_cast<TChar>(' ') } {}
		/**
		 * @brief		Constructor
		 * @param size	The maximum size of the indent, in characters. If this is smaller than the value of "used", size will not go below 0.
		 * @param used	The number of characters that are already printed and should be accounted for before printing the indentation.
		 */
		CONSTEXPR basic_indentor(const size_t size, const size_t used) : width{ (size <= used) ? 0ull : (size - used) }, fill_char{ static_cast<TChar>(' ') } {}
		/**
		 * @brief		Constructor
		 * @param size	The maximum size of the indent, in characters. If this is smaller than the value of "used", size will not go below 0.
		 * @param used	The number of characters that are already printed and should be accounted for before printing the indentation.
		 * @param fill	The character to use for indentation. (Default: ' ')
		 */
		CONSTEXPR basic_indentor(const size_t size, const size_t used, const TChar fill) : width{ (size <= used) ? 0ull : (size - used) }, fill_char{ fill } {}

		/**
		 * @brief		Retrieve the size in characters of the indentation.
		 * @returns		The size of the indentation space.
		 */
		virtual CONSTEXPR size_t size() const
		{
			return this->width;
		}

		/**
		 * @brief		Retrieve the indentation as a string of given characters, or spaces by default.
		 * @returns		string_t
		 */
		virtual CONSTEXPR string_t toString() const
		{
			return string_t(this->width, this->fill_char);
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
		CONSTEXPR operator size_t() const { return size; }

		/**
		 * @brief		Output stream insertion operator.
		 * @param os	Output stream instance.
		 * @param ind	Indentor instance.
		 * @returns		The output stream instance.
		 */
		friend std::basic_ostream<TChar, TCharTraits>& operator<<(std::basic_ostream<TChar, TCharTraits>& os, const basic_indentor<TChar, TCharTraits, TAlloc>& ind)
		{
			if (ind.width == 0ull)
				return os;

			// set fill char
			const auto prevFill{ os.fill() };
			const bool nonDefaultFill{ ind.fill_char != prevFill };
			if (nonDefaultFill) os << std::setfill(ind.fill_char);

			// setw, then print a single additional fill char so that setw is actually triggered
			os << std::setw(static_cast<std::streamsize>(ind.width)) << ind.fill_char;

			// reset fill char
			if (nonDefaultFill) os << std::setfill(prevFill);
			return os;
		}
	};

	using indentor = basic_indentor<char, std::char_traits<char>, std::allocator<char>>;
	using windentor = basic_indentor<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t>>;
	/// @brief	Variable on-demand indentation for output streams using std::setw.
	using indent = indentor;
}

#ifndef INDENTOR_NOGLOBAL
using shared::indent;
#endif

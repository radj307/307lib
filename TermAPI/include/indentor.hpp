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

namespace term {
	/**
	 * @struct		indentor
	 * @brief		Handles calculating variable indentation for strings or output streams.
	 * @tparam T	The integral type to use for the size values.
	 */
	template<std::integral T = std::streamsize>
	struct indentor {
		/// @brief	The size type used.
		using size_type = T;
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

		CONSTEXPR size_type size() const { return sz; }
		WINCONSTEXPR std::string toString(const char& c = ' ') const { return std::string(c, sz); }

		WINCONSTEXPR operator std::string() const { return toString(); }

		/**
		 * @brief	Stream insertion operator
		 * @returns	std::ostream&
		 */
		friend std::ostream& operator<<(std::ostream& os, const indentor<T>& ind)
		{
			if (ind.sz == static_cast<T>(0))
				return os;
			return os << std::setw(static_cast<std::streamsize>(ind.sz - static_cast<T>(1))) << ' ';
		}
	};
	/// @brief	Variable on-demand indentation for output streams.
	using indent = indentor<std::streamsize>;
}
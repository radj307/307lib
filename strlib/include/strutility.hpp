#pragma once
#include <ostream>
#include <string>
#include <sstream>
#include <concepts>

namespace str {
	/**
	 * @namespace op
	 * @brief Contains templated operator overloads compatible with std types so that they don't pollute the namespace.
	 */
	namespace op {
		/**
		 * @function operator<<(ostream&, cont_type<T, allocator<T>>&)
		 * @brief Templated stream insertion operator<< overload for STL containers such as std::vector & std::list.
		 * @tparam ContType	- Type of container
		 * @tparam T			- Type of element stored in the container
		 * @param os			- Output stream ref
		 * @param cont			- Container ref
		 * @returns ostream&
		 */
		template<template<class, class> class ContType, class T>
		static std::ostream& operator<<(std::ostream& os, const ContType<T, std::allocator<T>>& cont) noexcept
		{
			try {
				for (const auto& i : cont)
					os << i << ' ';
			} catch (...) {}
			return os;
		}

	}

	/**
	 * @brief Simply checks the given position value against std::string::npos
	 * @tparam T		- Position value type.
	 * @param pos		- Position to check.
	 * @returns bool
	 *\n		true	- The given position is NOT equal to std::string::npos
	 *\n		false	- The given position is equal to std::string::npos, and is invalid.
	 */
	template<class T> [[nodiscard]] constexpr bool pos_valid(const T& pos) noexcept
	{
		return static_cast<size_t>(pos) != std::string::npos;
	}


	/**
	 * @brief Retrieve the length of the longest string in a given STL container.
	 * @tparam RT		- Return Type. Must be an integral value.
	 * @param cont		- Container to check.
	 * @param offset	- Add this number to the returned value. Including this allows template auto-deduction.
	 * @returns RT
	 */
	template<std::integral RT, template<class, class> class Cont>
	[[nodiscard]] RT get_longest_string(const Cont<std::string, std::allocator<std::string>>& cont, RT offset = static_cast<RT>(0))
	{
		size_t longest{ 0llu };
		for (auto& it : cont)
			if (const auto sz{ it.size() }; sz > longest)
				longest = sz;
		return offset + static_cast<RT>(longest);
	}

	/**
	 * @brief			Count the number of delimiters that occur within a given stringstream.
	 * @param ss:		Input Stringstream
	 * @param delim:	The delimiter to count.
	 * @returns			size_t
	 */
	inline static size_t count(std::stringstream& ss, char delim)
	{
		size_t count{ 0ull };
		for (std::string sbuf; std::getline(ss, sbuf, delim); ++count) {}
		return count;
	}
}
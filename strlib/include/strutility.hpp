/**
 * @file	strutility.hpp
 * @author	radj307
 * @brief	Contains various utility functions & operators from strlib.
 *\n		This also contains 'global' scope additions from strlib, such as `usiang namespace std::string_literals;`, which can be disabled by defining `DISABLE_STR_LITERALS`.
 */
#pragma once
#include <strconv.hpp>

#include <ostream>
#include <string>
#include <sstream>
#include <concepts>

/// Define DISABLE_STR_LITERALS to disable adding std::string_literals to the global namespace.
#ifndef DISABLE_STR_LITERALS
/**
 * @def		DISABLE_STR_LITERALS
 * @brief	Disables the "using namespace std::string_literals;" from being declared in the global namespace.
 */
#define DISABLE_STR_LITERALS // for doxygen to see the definition
#undef DISABLE_STR_LITERALS
using namespace std::string_literals;
#endif

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

	/**
	 * @brief				Variadic variant of the getline function.
	 * @tparam Ch			Char Type
	 * @tparam Tr			Char Traits
	 * @tparam A			Allocator Type
	 * @tparam DelimT...	Any number of types that are the same or convertible to Ch
	 * @param is			Input stream ref
	 * @param str			Output string ref
	 * @param delimiters	Any number of characters to use as delimiters.
	 * @returns				basic_istream<Ch, Tr>&
	 */
	template<class Ch, class Tr, class A, var::same_or_convertible<Ch>... DelimT>
	std::basic_istream<Ch, Tr>& getline(std::basic_istream<Ch, Tr>& is, std::basic_string<Ch, Tr, A>& str, DelimT const&... delims)
	{
		std::string::size_type nread{ 0 }; // number of characters read
		if (std::istream::sentry(is, true)) {
			std::streambuf* sbuf{ is.rdbuf() };
			str.clear();
			while (nread < str.max_size()) {
				auto c1{ sbuf->sbumpc() };
				if (Tr::eq_int_type(c1, Tr::eof())) {
					is.setstate(std::istream::eofbit);
					break;
				}
				++nread;
				if (const Ch ch{ Tr::to_char_type(c1) }; !var::variadic_or(ch == delims...))
					str.push_back(ch);
				else break;
			}
		}
		if (nread == 0 || nread >= str.max_size())
			is.setstate(std::istream::failbit);
		return is;
	}
}

/**
 * @file	Sequence.hpp
 * @author	radj307
 * @brief	Contains the ANSI::Sequence object.
 */
#pragma once
#include <sysarch.h>
#include <var.hpp>

#include <ostream>
#include <string>
#include <sstream>
#include <concepts>

namespace ANSI {
	template<var::valid_char TChar, typename TCharTraits = std::char_traits<TChar>, typename TAlloc = std::allocator<TChar>>
	using basic_sequence = std::basic_string<TChar, TCharTraits, TAlloc>;

	using sequence = basic_sequence<char>;
	using wsequence = basic_sequence<wchar_t>;

	// char
	template<var::valid_char TChar = char, typename TCharTraits = std::char_traits<TChar>, typename TAlloc = std::allocator<TChar>, var::streamable<std::basic_stringstream<TChar, TCharTraits, TAlloc>>... Ts>
	CONSTEXPR basic_sequence<TChar, TCharTraits, TAlloc> make_sequence(Ts&&... segments) noexcept
	{
		std::basic_stringstream<TChar, TCharTraits, TAlloc> buffer;
		(buffer << ... << std::forward<Ts>(segments));
		return buffer.str();
	}
}

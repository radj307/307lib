#pragma once
#include <sysarch.h>
#include <var.hpp>

#include <concepts>
#include <string>
#include <istream>
#include <ostream>

namespace shared {
	/**
	 * @struct	basic_vstring
	 * @brief	'virtual' string object that has a constructor overload that accepts a single character.
	 */
	template<var::valid_char TChar, std::derived_from<std::char_traits<TChar>> TCharTraits = std::char_traits<TChar>, std::derived_from<std::allocator<TChar>> TAlloc = std::allocator<TChar>>
	struct basic_vstring : public std::basic_string<TChar, TCharTraits, TAlloc> {
		using base = std::basic_string<TChar, TCharTraits, TAlloc>;
		using base::base;
		CONSTEXPR basic_vstring(const TChar& c) : base(1ull, c) {}
		CONSTEXPR basic_vstring(const std::basic_string<TChar, TCharTraits, TAlloc>& s) : base(s) {}

		CONSTEXPR bool is_single_char() const { return this->size() == 1ull; }
		CONSTEXPR char get_single_char() const { return this->front(); }

		explicit CONSTEXPR operator TChar() const noexcept(false)
		{
			if (this->empty()) throw make_custom_exception<std::out_of_range>("");
		}
		CONSTEXPR operator std::basic_string<TChar, TCharTraits, TAlloc>() const noexcept { return *this; }
	#ifdef _FILESYSTEM_
		CONSTEXPR operator std::filesystem::path() const noexcept { return std::filesystem::path{ this->operator std::basic_string<TChar, TCharTraits, TAlloc>() }; }
	#endif
	};
	/// @brief	Narrow-char width string type.
	using vstring = basic_vstring<char, std::char_traits<char>, std::allocator<char>>;
	using wvstring = basic_vstring<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t>>;
}

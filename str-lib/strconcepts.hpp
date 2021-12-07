/**
 * @file	strconcepts.hpp
 * @author	radj307
  * @brief	Extends the str namespace with string/stringstream-related helper concepts.
  *\n		__Requires C++20__
  *\n		- _valid_string_
  *\n			- Accepts any STL std::basic_string-derived types present in the <xstring> library.
  *\n		- _valid_string_view_
  *\n			- Accepts any STL std::basic_string_view-derived types present in the <xstring> library.
  *\n		- _valid_stringbuf_
  *\n			- Accepts any STL std::basic_stringbuf-derived types present in the <iosfwd> library.
  *\n		- _valid_stringstream_
  *\n			- Accepts any STL std::basic_stringstream-derived types present in the <iosfwd> library.
  *\n		- _valid_streambuf_
  *\n			- Accepts any STL std::basic_streambuf-derived types present in the <iosfwd> library.
  *\n		- _valid_streampos_
  *\n			- Accepts any STL std::fpos/std::streampos-derived types present in the <iosfwd> library.
 */
#pragma once
#include <string>		// For STL std::string, std::wstring, std::u8string, std::u16string, & std::u32string types, as well as all related std::basic_string_view-derived types.
#include <iosfwd>		// For STL std::stringbuf, std::stringstream, std::streambuf, & std::streampos types.
#include <concepts>		// For C++20 helper concepts & the <type_traits> library.
 /**
  * @namespace	concepts
  * @brief		Extends the str namespace with C++20 concepts.
  *\n			- _valid_char_
  *\n				- Accepts any fundamental char types.
  *\n			- _valid_string_
  *\n				- Accepts any STL std::basic_string-derived types present in the <xstring> library.
  *\n			- _valid_string_view_
  *\n				- Accepts any STL std::basic_string_view-derived types present in the <xstring> library.
  *\n			- _valid_stringbuf_
  *\n				- Accepts any STL std::basic_stringbuf-derived types present in the <iosfwd> library.
  *\n			- _valid_stringstream_
  *\n				- Accepts any STL std::basic_stringstream-derived types present in the <iosfwd> library.
  *\n			- _valid_streambuf_
  *\n				- Accepts any STL std::basic_streambuf-derived types present in the <iosfwd> library.
  *\n			- _valid_streampos_
  *\n				- Accepts any STL std::fpos/std::streampos-derived types present in the <iosfwd> library.
  */
namespace str::concepts {
	/**
	 * @concept		valid_char
	 * @brief		Allows all standard char types as of C++20, including signed/unsigned char, wchar_t, UTF-8, UTF-16, & UTF-32 types.
	 * @tparam T	Input Type.
	 */
	template<class T> concept valid_char =
		std::same_as<T, signed char>
		|| std::same_as<T, unsigned char>
		|| std::same_as<T, char>
		|| std::same_as<T, wchar_t>
		|| std::same_as<T, char8_t>
		|| std::same_as<T, char16_t>
		|| std::same_as<T, char32_t>;
	/**
	 * @concept		valid_string
	 * @brief		Allows standard string types, including narrow/wide, UTF-8, UTF-16, & UTF-32 types.
	 * @tparam T	Input Type.
	 */
	template<class T> concept valid_string =
		std::same_as<T, std::string>
		|| std::same_as<T, std::wstring>
		|| std::same_as<T, std::u8string>
		|| std::same_as<T, std::u16string>
		|| std::same_as<T, std::u32string>;
	/**
	 * @concept		valid_string_view
	 * @brief		Allows standard string_view types, including narrow/wide, UTF-8, UTF-16, & UTF-32 types.
	 * @tparam T	Input Type.
	 */
	template<class T> concept valid_string_view =
		std::same_as<T, std::string_view>
		|| std::same_as<T, std::wstring_view>
		|| std::same_as<T, std::u8string_view>
		|| std::same_as<T, std::u16string_view>
		|| std::same_as<T, std::u32string_view>;
	/**
	 * @concept		valid_stringbuf
	 * @brief		Allows standard stringbuf types, including narrow/wide types.
	 * @tparam T	Input Type.
	 */
	template<class T> concept valid_stringbuf = std::same_as<T, std::stringbuf> || std::same_as<T, std::wstringbuf>;
	/**
	 * @concept		valid_stringstream
	 * @brief		Allows standard stringstream types, including i/o, narrow/wide types.
	 * @tparam T	Input Type.
	 */
	template<class T> concept valid_stringstream =
		std::same_as<T, std::stringstream>
		|| std::same_as<T, std::istringstream>
		|| std::same_as<T, std::ostringstream>
		|| std::same_as<T, std::wstringstream>
		|| std::same_as<T, std::wistringstream>
		|| std::same_as<T, std::wostringstream>;
	/**
	 * @concept		valid_streambuf
	 * @brief		Allows standard streambuf types, including narrow/wide types.
	 * @tparam T	Input Type.
	 */
	template<class T> concept valid_streambuf = std::same_as<T, std::streambuf> || std::same_as<T, std::wstreambuf>;
	/**
	 * @concept		valid_streampos
	 * @brief		Allows standard streampos types, including narrow/wide, UTF-8, UTF-16, &UTF-32 types.
	 * @tparam T	Input Type.
	 */
	template<class T> concept valid_streampos =
		std::same_as<T, std::streampos>
		|| std::same_as<T, std::wstreampos>
		|| std::same_as<T, std::u8streampos>
		|| std::same_as<T, std::u16streampos>
		|| std::same_as<T, std::u32streampos>;
}
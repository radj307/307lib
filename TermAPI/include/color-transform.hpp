/**
 * @file	color-transform.hpp
 * @author	radj307
 * @brief	Contains functions for transforming color values between different formats.
 */
#pragma once
#include <sysarch.h>
#include <make_exception.hpp>
#include <var.hpp>
#include <math.hpp>

#include <cmath>
#include <utility>
#include <sstream>
#include <iomanip>

namespace color {
	using ColorT = short;
	template<::std::integral T = ColorT>
	using RGB = ::std::tuple<T, T, T>;

	/**
	 * @brief		Convert an RGB value to a SGR color value.
	 *\n			https://github.com/chadj2/bash-ui/blob/master/COLORS.md#xterm-colorspaces
	 * @tparam T	Any integral type.
	 * @param r		Red value. (Range: 0 - 5)
	 * @param g		Green value. (Range: 0 - 5)
	 * @param b		Blue value. (Range: 0 - 5)
	 * @returns		T
	 */
	template<::std::integral T = ColorT>
	inline CONSTEXPR const T rgb_to_sgr(const T& r, const T& g, const T& b)
	{
		return { r * static_cast<T>(36) + g * static_cast<T>(6) + (b + static_cast<T>(16)) };
	}

	/**
	 * @brief		Convert an RGB value to a SGR color value.
	 * @tparam T	Any floating-point type.
	 * @param r		Red value. (Range: 0.0 - 1.0)
	 * @param g		Green value. (Range: 0.0 - 1.0)
	 * @param b		Blue value. (Range: 0.0 - 1.0)
	 * @returns		ColorT
	 */
	template<::std::floating_point T>
	inline CONSTEXPR const ColorT rgb_to_sgr(const T& r, const T& g, const T& b) noexcept
	{
		T max{ var::largest(r, g, b) };
		const auto translatef{ [&max](const auto& fp) -> ColorT {
			return static_cast<ColorT>(::std::round(fp / max * static_cast<T>(5.0)));
		} };
		return rgb_to_sgr<ColorT>(translatef(r), translatef(g), translatef(b));
	}

	/**
	 * @brief				Convert an RGB value to an SGR value.
	 * @tparam T			Integral Type
	 * @param rgb_color		Input RGB color value.
	 * @returns				T
	 */
	template<::std::integral T = ColorT>
	inline CONSTEXPR const T rgb_to_sgr(const RGB<T>& rgb_color) noexcept
	{
		return rgb_to_sgr<T>(::std::get<0>(rgb_color), ::std::get<1>(rgb_color), ::std::get<2>(rgb_color));
	}

	/**
	 * @brief				Convert an SGR color value into an RGB color value.
	 * @tparam T			Any integral type.
	 * @param sgr_color		An SGR color value.
	 * @returns				::std::tuple<T, T, T>
	 */
	template<::std::integral T = ColorT>
	inline CONSTEXPR const RGB<T> sgr_to_rgb(const T& sgr_color)
	{
		T tmp{ sgr_color - static_cast<T>(16) };

		T blue{ tmp % static_cast<T>(6) };
		tmp /= static_cast<T>(6);
		T green{ tmp % static_cast<T>(6) };
		tmp /= static_cast<T>(6);

		return{ tmp, green, blue };
	}

#pragma region Hexadecimal_Transformations

	/**
	 * @brief		Check if the given character is a valid hexadecimal value. (Range: 0 - 9 | a - f | A - F)
	 * @param ch	Input Character.
	 * @returns		bool
	 */
	inline static CONSTEXPR bool ishexnum(const char& ch)
	{
		switch (ch) {
		case '0': [[fallthrough]];
		case '1': [[fallthrough]];
		case '2': [[fallthrough]];
		case '3': [[fallthrough]];
		case '4': [[fallthrough]];
		case '5': [[fallthrough]];
		case '6': [[fallthrough]];
		case '7': [[fallthrough]];
		case '8': [[fallthrough]];
		case '9': [[fallthrough]];
		case 'a': [[fallthrough]];
		case 'A': [[fallthrough]];
		case 'b': [[fallthrough]];
		case 'B': [[fallthrough]];
		case 'c': [[fallthrough]];
		case 'C': [[fallthrough]];
		case 'd': [[fallthrough]];
		case 'D': [[fallthrough]];
		case 'e': [[fallthrough]];
		case 'E': [[fallthrough]];
		case 'f': [[fallthrough]];
		case 'F':
			return true;
		default:
			return false;
		}
	}

	/**
	 * @brief				Convert a single hexadecimal character to an integral value.
	 * @tparam ReturnType	Type of integral to return.
	 * @param ch			A single valid hexadecimal character. (Range: 0 - F)
	 * @returns				ReturnType
	 */
	template<::std::integral ReturnType>
	inline static CONSTEXPR ReturnType hex_to_int(const char& ch)
	{
		if (isdigit(ch))
			return static_cast<ReturnType>(ch - '0');
		else if (ch >= 'a' && ch <= 'f')
			return static_cast<ReturnType>(ch - 'a') + static_cast<ReturnType>(10);
		else if (ch >= 'A' && ch <= 'F')
			return static_cast<ReturnType>(ch - 'A') + static_cast<ReturnType>(10);
		return static_cast<ReturnType>(0);
	}

	/**
	 * @brief				Convert a single hexadecimal value to an integral.
	 * @tparam ReturnType	Type of integral to return.
	 * @param hex_str		Hexadecimal value as a string.
	 *\n					Note that this CANNOT contain prefix characters like '#' or "0x"
	 * @returns				ReturnType
	 */
	template<::std::integral ReturnType>
	inline static CONSTEXPR ReturnType hex_to_int(const std::string& hex_str) noexcept
	{
		if (hex_str.empty())
			return static_cast<ReturnType>(0);
		const auto base{ static_cast<ReturnType>(16) };
		ReturnType power{ 1 }, result{ 0 };
		for (auto ch{ hex_str.rbegin() }; ch != hex_str.rend(); ++ch) {
			if (const auto v{ hex_to_int<ReturnType>(*ch) }; v < base) {
				result += v * power;
				power = power * base;
			}
		}
		return result;
	}

	/**
	 * @brief		Convert a hexadecimal value to an RGB color value.
	 * @tparam T	Integral Type.
	 * @param hex	A hexadecimal value as a string.
	 *\n			Note that this CANNOT contain prefix characters like '#' or "0x"
	 * @returns		RGB<T>
	 */
	template<::std::integral T = ColorT>
	inline static CONSTEXPR RGB<T> hex_to_rgb(const ::std::string& hex) noexcept
	{
		return{
			math::normalize(hex_to_int<T>(hex.substr(0ull, 2ull)), { 0, 255 }, {0, 5}),
			math::normalize(hex_to_int<T>(hex.substr(2ull, 2ull)), { 0, 255 }, {0, 5}),
			math::normalize(hex_to_int<T>(hex.substr(4ull, 2ull)), { 0, 255 }, {0, 5})
		};
	}

	/**
	 * @brief		Convert a hexadecimal string to an SGR color value.
	 * @tparam T	Integral Type.
	 * @param hex	A hexadecimal value as a string.
	 *\n			Note that this CANNOT contain prefix characters like '#' or "0x"
	 * @returns		T
	 *				| Value | Description   |
	 *				| ----- | ------------- |
	 *				| 0     | Invalid Input |
	 *				| 0-255 | Output Value  |
	 */
	template<::std::integral T = ColorT>
	inline static CONSTEXPR T hex_to_sgr(const ::std::string& hex) noexcept
	{
		if (!hex.empty() && ::std::all_of(hex.begin(), hex.end(), ishexnum))
			return rgb_to_sgr(hex_to_rgb(hex));
		return 0;
	}

	/**
	 * @brief			Convert an integer value to hexadecimal.
	 * @tparam T		Integral Type.
	 * @param v			Input Value.
	 * @param uppercase	When true, the returned hexadecimal string will use uppercase letters rather than lowercase.
	 * @returns			::std::string
	 */
	template<::std::integral T = ColorT>
	inline static ::std::string int_to_hex(const T& v, const bool& uppercase = true) noexcept
	{
		::std::ostringstream buffer;
		buffer << ::std::hex << ::std::setfill('0') << (uppercase ? ::std::uppercase : ::std::nouppercase) << v;
		return buffer.str();
	}

	/**
	 * @brief			Convert an RGB color value to hexadecimal.
	 * @tparam T		Integral Type.
	 * @param rgb		Input RGB Color Value.
	 * @param uppercase	When true, the returned hexadecimal string will use uppercase letters rather than lowercase.
	 * @returns			::std::string
	 */
	template<::std::integral T = ColorT>
	inline static ::std::string rgb_to_hex(const RGB<T>& rgb, const bool& uppercase = true) noexcept
	{
		using namespace ::std::string_literals;
		return{ "0x"s + int_to_hex<T>(::std::get<0>(rgb)) + int_to_hex<T>(::std::get<1>(rgb)) + int_to_hex<T>(::std::get<2>(rgb)) };
	}

	/**
	 * @brief			Convert an SGR color value to hexadecimal.
	 * @tparam T		Integral Type.
	 * @param sgr		Input SGR color value.
	 * @param uppercase	When true, the returned hexadecimal string will use uppercase letters rather than lowercase.
	 * @returns			::std::string
	 */
	template<::std::integral T = ColorT>
	inline static ::std::string sgr_to_hex(const T& sgr, const bool& uppercase = true) noexcept
	{
		return rgb_to_hex<T>(sgr_to_rgb<T>(sgr), uppercase);
	}

#pragma endregion Hexadecimal_Transformations

#pragma region ConversionParser

	template<std::integral T = ColorT>
	inline T parse_hex_to_sgr(std::string hex)
	{
		if (!hex.empty())
			return static_cast<T>(0);
		if (hex.at(0ull) == '#')
			hex = hex.substr(1ull);
		return hex_to_sgr<T>(hex);
	}

#pragma endregion ConversionParser
}
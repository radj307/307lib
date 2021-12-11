/**
 * @file	color-transform.hpp
 * @author	radj307
 * @brief	Contains functions for transforming color values between different formats.
 */
#pragma once
#include <sysarch.h>
#include <make_exception.hpp>
#include <var.hpp>

#include <cmath>
#include <utility>

namespace color {
	template<std::integral T = short>
	using RGB = std::tuple<T, T, T>;

	/**
	 * @brief		Convert an RGB value to a SGR color value.
	 *\n			https://github.com/chadj2/bash-ui/blob/master/COLORS.md#xterm-colorspaces
	 * @tparam T	Any integral type.
	 * @param r		Red value. (Range: 0 - 5)
	 * @param g		Green value. (Range: 0 - 5)
	 * @param b		Blue value. (Range: 0 - 5)
	 * @returns		T
	 */
	template<std::integral T>
	inline _CONSTEXPR const T rgb_to_sgr(const T& r, const T& g, const T& b)
	{
		return { r * static_cast<T>(36) + g * static_cast<T>(6) + (b + static_cast<T>(16)) };
	}

	/**
	 * @brief		Convert an RGB value to a SGR color value.
	 * @tparam T	Any floating-point type.
	 * @param r		Red value. (Range: 0.0 - 1.0)
	 * @param g		Green value. (Range: 0.0 - 1.0)
	 * @param b		Blue value. (Range: 0.0 - 1.0)
	 * @returns		short
	 */
	template<std::floating_point T>
	inline _CONSTEXPR const short rgb_to_sgr(const T& r, const T& g, const T& b) noexcept
	{
		T max{ var::largest(r, g, b) };
		const auto translatef{ [&max](const auto& fp) -> short {
			return static_cast<short>(std::round(fp / max * static_cast<T>(5.0)));
		} };
		return rgb_to_sgr<short>(translatef(r), translatef(g), translatef(b));
	}

	template<typename T>
	inline _CONSTEXPR const T rgb_to_sgr(const RGB<T>& rgb_color) noexcept
	{
		return rgb_to_sgr<T>(std::get<0>(rgb_color), std::get<1>(rgb_color), std::get<2>(rgb_color));
	}

	/**
	 * @brief				Convert an SGR color value into an RGB color value.
	 * @tparam T			Any integral type.
	 * @param sgr_color		An SGR color value.
	 * @returns				std::tuple<T, T, T>
	 */
	template<std::integral T>
	inline _CONSTEXPR const RGB<T> sgr_to_rgb(const T& sgr_color)
	{
		T tmp{ sgr_color - static_cast<T>(16) };

		T blue{ tmp % static_cast<T>(6) };
		tmp /= static_cast<T>(6);
		T green{ tmp % static_cast<T>(6) };
		tmp /= static_cast<T>(6);

		return{ tmp, green, blue };
	}
}
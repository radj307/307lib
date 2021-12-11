/**
 * @file	color-transform.hpp
 * @author	radj307
 * @brief	Contains functions for transforming color values between different formats.
 */
#include <sysarch.h>
#include <make_exception.hpp>
#include <str.hpp>
#include <var.hpp>

#include <cmath>
#include <utility>

namespace color {
	/**
	 * @brief		Convert an RGB value to a SGR color value.
	 *\n			https://github.com/chadj2/bash-ui/blob/master/COLORS.md#xterm-colorspaces
	 * @tparam T	Any integral type.
	 * @param r		Red value. (Range: 0 - 5)
	 * @param g		Green value. (Range: 0 - 5)
	 * @param b		Blue value. (Range: 0 - 5)
	 * @returns		T
	 */
	template<typename T>
	inline _CONSTEXPR const std::enable_if_t<std::is_integral_v<T>, T> rgb_to_sgr(const T& r, const T& g, const T& b)
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
	template<typename T>
	inline _CONSTEXPR const std::enable_if_t<std::is_floating_point_v<T>, short> rgb_to_sgr(const T& r, const T& g, const T& b) noexcept
	{
		T max{ var::largest(r, g, b) };
		const auto translatef{ [&max](const auto& fp) -> short {
			return static_cast<short>(std::round(fp / max * static_cast<T>(5.0)));
		} };
		return rgb_to_sgr<short>(translatef(r), translatef(g), translatef(b));
	}

	/**
	 * @brief
	 * @tparam T			Any integral type.
	 * @param sgr_color
	 * @return
	 */
	template<typename T>
	inline _CONSTEXPR const std::enable_if_t<std::is_integral_v<T>, std::tuple<T, T, T>> sgr_to_rgb(const T& sgr_color)
	{
		T tmp{ sgr_color - static_cast<T>(16) };

		T blue{ tmp % static_cast<T>(6) };
		tmp /= static_cast<T>(6);
		T green{ tmp % static_cast<T>(6) };
		tmp /= static_cast<T>(6);
		//T red{ tmp };

		return{ tmp, green, blue };
	}
}
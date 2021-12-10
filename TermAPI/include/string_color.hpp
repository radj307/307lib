/**
 * @file	string_color.hpp
 * @author	radj307
 * @brief	Contains functions used to convert between color values and strings.
 */
#include <sysarch.h>
#include <str.hpp>
#include <color-values.h>
#include <make_exception.hpp>

namespace color {
	inline std::string to_string(const short& color)
	{
		if (color < 0 || color > 255)
			throw make_exception("Color value out of range (0-255): ", color);
	}

	inline _CONSTEXPR const short to_color(std::string str)
	{
		str = str::tolower(str);
	}
}
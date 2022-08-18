#pragma once
/**
 * @file	env.hpp
 * @author	radj307
 * @brief	Contains the env namespace's _getvar_ function, used to retrieve the value of a given environment variable.
 */
#include <sysarch.h>

#include <string>
#include <optional>
#include <cstdlib> // for C environment functions

namespace env {
	/**
	 * @brief		Retrieve the value of a given environment variable as a std::string.
	 * @param name	Environment variable name.
	 * @returns		std::string
	 */
	std::optional<std::string> getvar(const std::string_view&) noexcept;

	inline std::string get_home() noexcept
	{
		#ifdef OS_WIN
		return getvar("USERPROFILE").value_or(getvar("HOME").value_or(""));
		#else
		return getvar("HOME").value_or("");
		#endif
	}
}

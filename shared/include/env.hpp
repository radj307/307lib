/**
 * @file	env.hpp
 * @author	radj307
 * @brief	Contains the env namespace's _getvar_ function, used to retrieve the value of a given environment variable.
 */
#pragma once
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
	INLINE std::optional<std::string> getvar(const std::string_view& name) noexcept
	{
		#pragma warning (disable: 4996) // disable deprecation warning
		const auto var{ std::getenv(name.data()) };
		return(var != nullptr ? var : static_cast<std::optional<std::string>>(std::nullopt));
		#pragma warning (default: 4996)
	}
}
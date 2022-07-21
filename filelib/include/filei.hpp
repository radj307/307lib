/**
 * @file	filei.hpp
 * @author	radj307
 * @brief	Contains file input functions.
 */
#pragma once
#include "openmode.h"

#include <sstream>
#include <filesystem>

#ifdef read
#undef read
#endif

namespace file {
	/**
	 * @brief		Read the contents of a file to a stringstream.
	 * @param path	The location of the target file.
	 * @param mode	The mode to use when opening the file.
	 * @returns		std::stringstream&&
	 */
	std::stringstream read(const std::filesystem::path&, const openmode& = openmode::in) noexcept;
	/**
	 * @brief		Read the contents of a file to a stringstream.
	 * @param path	The location of the target file.
	 * @param mode	The mode to use when opening the file.
	 * @returns		std::stringstream&&
	 */
	std::stringstream read(const std::filesystem::path&, const std::ios_base::openmode&) noexcept;
}

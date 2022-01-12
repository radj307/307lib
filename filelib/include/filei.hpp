/**
 * @file	filei.hpp
 * @author	radj307
 * @brief	Contains file input functions.
 */
#pragma once
#include <sstream>
#include <filesystem>

#ifdef read
#undef read
#endif

namespace file {
	/**
	 * @brief		Read the contents of a file to a stringstream.
	 * @param path	Target Filepath
	 * @returns		std::stringstream&&
	 */
	std::stringstream read(const std::filesystem::path&) noexcept;
}

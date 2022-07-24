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
	template<class TChar = char, class TCharTraits = std::char_traits<TChar>, class TAlloc = std::allocator<TChar>, template<class, class, class> class TStream = std::basic_stringstream>
	TStream<TChar, TCharTraits, TAlloc> read(const std::filesystem::path& path, const openmode& mode = openmode::in) noexcept
	{
		TStream<TChar, TCharTraits, TAlloc> buffer;
		if (std::basic_ifstream<TChar, TCharTraits> ifs(path, static_cast<std::ios_base::openmode>(mode)); ifs.is_open())
			buffer << ifs.rdbuf();
		return std::move(buffer);
	}

	/**
	 * @brief		Read the contents of a file to a stringstream.
	 * @param path	The location of the target file.
	 * @param mode	The mode to use when opening the file.
	 * @returns		std::stringstream&&
	 */
	template<typename TChar = char, typename TCharTraits = std::char_traits<TChar>, typename TAlloc = std::allocator<TChar>, template<class, class, class> class TStream = std::basic_stringstream>
	TStream<TChar, TCharTraits, TAlloc> read(const std::filesystem::path& path, const std::ios_base::openmode& mode) noexcept
	{
		TStream<TChar, TCharTraits, TAlloc> buffer;
		if (std::basic_ifstream<TChar, TCharTraits> ifs(path, static_cast<std::ios_base::openmode>(mode)); ifs.is_open())
			buffer << ifs.rdbuf();
		return std::move(buffer);
	}
}

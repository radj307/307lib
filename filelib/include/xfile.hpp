#pragma once
/**
 * @file	xfile.hpp
 * @author	radj307
 * @brief	Fluid file I/O methods; updated alternative to fileio.hpp
 */
#include <var.hpp>
#include <make_exception.hpp>

#include <fstream>
#include <filesystem>

namespace file {
	template<var::valid_char TChar = char, std::derived_from<std::char_traits<TChar>> TCharTraits = std::char_traits<TChar>>
	CONSTEXPR std::basic_ifstream<TChar, TCharTraits> xread(std::filesystem::path const& path)
	{
		if (std::basic_ifstream<TChar, TCharTraits> ifs{ path }; ifs.is_open())
			return std::move(ifs);
		throw make_exception("Cannot open file '", path.generic_string(), "' for reading!");
	}
	template<var::valid_char TChar = char, std::derived_from<std::char_traits<TChar>> TCharTraits = std::char_traits<TChar>>
	CONSTEXPR std::basic_ofstream<TChar, TCharTraits> xwrite(std::filesystem::path const& path)
	{
		if (std::basic_ofstream<TChar, TCharTraits> ofs{ path }; ofs.is_open())
			return std::move(ofs);
		throw make_exception("Cannot open file '", path.generic_string(), "' for writing!");
	}
}

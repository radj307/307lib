#pragma once
#include <sysarch.h>
#include <fileio.hpp>
#include <fileutil.hpp>
#include <str.hpp>
#include <make_exception.hpp>
#include <parser/INIParser.hpp>
#if LANG_CPP >= 17
#include <filesystem>
#endif

namespace file::ini {
	/**
	 * @class INI
	 * @brief Extends the (token::parse) INIContainer object with file I/O functionality.
	 */
	struct INI : token::parse::INIContainer {
		/**
		 * @brief Move-Constructor.
		 * @param map	- rvalue reference of a pre-constructed INIContainer::Map.
		 */
		INI(INIContainer::Map&& map) : INIContainer(std::move(map)) {}
	#if LANG_CPP >= 17
		INI(const std::filesystem::path& path) : INI(std::move(token::parse::INIParser(path.generic_string(), file::read(path)).operator token::parse::INIContainer::Map())) {}
	#else
		INI(const std::string& filename) : INI(std::move(token::parse::INIParser(filename, file::read(filename)).operator token::parse::INIContainer::Map())) {}
	#endif
		INI() = default;

	#if LANG_CPP >= 17
	#define IN_TYPE std::filesystem::path
	#else
	#define IN_TYPE std::string
	#endif

		/**
		 * @brief Read a specified file and merge its contents with the local INI map.
		 * @param filename				- Name and path to target INI file.
		 * @param overwrite_existing	- When true, any existing variables will have their values overwritten by the ones in the specified file.
		 */
		void read(const IN_TYPE& filename, const bool overwrite_existing = true)
		{
			merge_container(std::move(token::parse::INIParser(filename, file::read(filename))), overwrite_existing);
		}
		template<str::ConvertibleStringT... VT> requires (sizeof...(VT) > 1) void read(const VT&... filenames)
		{
			for (auto& file : var::variadic_accumulate<std::string>(std::string{ filenames }...))
				read(file);
		}
		bool write(const IN_TYPE& filename, const bool append = false) const
		{
			return file::write(filename, str::streamify(*this).rdbuf(), append);
		}
	};
}
namespace file { using ini::INI; }
#pragma once
#include <sysarch.h>
#include <str.hpp>
#include <var.hpp>

#if CPP >= 17
#include <filesystem>
#else
#include <fstream>
#endif

#include <sstream>
#include <algorithm>


#ifndef _CONSTEXPR
#if CPP >= 17
#define _CONSTEXPR constexpr
#else
#define _CONSTEXPR
#endif
#endif

namespace file {
	/**
	 * @brief		Counts the number of characters that appear in a given file stream.
	 * @param ifs	Input Stream
	 * @returns		std::streamoff
	 */
	inline _CONSTEXPR std::streamoff count(auto&& is, const char& character) noexcept(false)
	{
		const auto count{ std::count(std::istreambuf_iterator<char>(std::forward<decltype(is)>(is)), std::istreambuf_iterator<char>(), character) };
		is.seekg(std::ios::beg);
		return count;
	}
	/**
	 * @brief		Retrieve the number of newline characters in a given file.
	 * @param ifs	Input Stream
	 * @returns		std::streamoff
	 */
	inline _CONSTEXPR std::streamoff getLineCount(auto&& is) noexcept(false)
	{
		return count(std::forward<decltype(is)>(is), '\n');
	}

	inline _CONSTEXPR bool isPathSeparator(const char& ch)
	{
		return ch == '/' || ch == '\\';
	}
}

#if CPP >= 17
#include <filesystem>

namespace file {
	inline bool hasTrailingSeparator(const std::filesystem::path& path)
	{
		return !path.empty() && isPathSeparator(path.generic_string().back());
	}

	using Directory = std::vector<std::filesystem::directory_entry>;

	/**
	 * @brief	Retrieve the current working directory.
	 * @returns std::filesystem::path
	 */
	template<std::same_as<std::filesystem::path> RT>
	static const RT getWorkingDir()
	{
		return std::filesystem::current_path();
	}
	/**
	 * @brief	Retrieve the current working directory.
	 * @returns std::string
	 */
	static const std::string getWorkingDir()
	{
		return getWorkingDir<std::filesystem::path>().generic_string();
	}

	/**
	 * @brief Returns a vector of std::filesystem::directory_entry types, representing the contents of the given directory path.
	 * @param path	- Target directory
	 * @returns Directory
	 */
	inline Directory getDirectory(const std::string& path)
	{
		Directory vec;
		for (auto const& it : std::filesystem::directory_iterator{ path })
			vec.emplace_back(it);
		return vec;
	}

	/**
	 * @brief Retrieve all files from the given directory path with a given extension.
	 * @param path		- Target Directory
	 * @param extension	- Target Extension, must have '.' prefix!
	 * @returns Directory
	 */
	inline Directory getAllFilesWithExtension(std::string path, const std::string& extension)
	{
		auto directory_iterator{ std::filesystem::directory_iterator{ std::move(path) } };

		Directory vec;
		vec.reserve(std::count_if(begin(directory_iterator), end(directory_iterator), [](auto&& entry) { return entry.is_regular_file(); }));

		for (auto& it : directory_iterator) {
			if (!it.is_regular_file())
				continue;
			if (const auto path{ it.path() }; path.has_extension() && path.extension() == extension)
				vec.emplace_back(it);
		}

		vec.shrink_to_fit();
		return vec;
	}

	/**
	 * @function exists(string&)
	 * @brief Checks if a file exists & can be opened at the specified location.
	 * @param filename  - The name/location of the target file.
	 * @return true		- The target file exists.
	 * @return false	- The target file does not exist.
	 */
	 //static bool exists(const std::filesystem::path& filename) noexcept
	 //{
	 //	return std::filesystem::exists(filename);
	 //}

	 /**
	  * @function exists(T&&...)
	  * @brief Check if an arbitrary number of files ( >1 ) exists on the local disk.
	  * @tparam T		- Variadic String Template.
	  * @param filelist	- Variadic Strings.
	  * @return true		- At least one of the target file(s) exist.
	  * @return false	- None of the target file(s) exist.
	  */
	  //template<var::more_than<1>... T> static bool exists(T&& ...filelist)
	  //{
	  //	static_assert(sizeof...(filelist) > 0);
	  //	auto ret{ false };
	  //	(([&ret](const std::string& filename) {
	  //		const auto doesExist{ exists(filename) };
	  //		if (doesExist)
	  //			ret = true;
	  //		return doesExist;
	  //		}(filelist)), ...);
	  //	return ret;
	  //}

	inline bool exists(const std::string& filepath) noexcept(false)
	{
	#if CPP >= 17
		return std::filesystem::exists(filepath);
	#else
		return std::ofstream(filepath).is_open();
	#endif
	}

#if CPP >= 17
	inline bool exists(const std::filesystem::path& path) noexcept(false)
	{
		return std::filesystem::exists(path);
	}
#if CPP >= 20
	template<typename... VT> requires var::more_than<1ull, VT...>
	inline bool exists(const VT&... paths) noexcept(false)
	{

	}
#endif
#endif

	/**
	 * @brief Replace the extension of a given filename.
	 * @param filename		- Input filename
	 * @param new_extension	- New Extension
	 * @returns std::string
	 */
	inline std::filesystem::path replace_extension(std::filesystem::path filename, const std::filesystem::path& new_extension)
	{
		return filename.replace_extension(new_extension);
	}

	/**
	 * @brief Convert a filesystem path to a std::string. All this does is call the path::generic_string() function for you.
	 * @param path	- Input std::filesystem::path.
	 * @returns std::string
	 */
	inline std::string to_string(const std::filesystem::path& path)
	{
		return path.generic_string();
	}
}
#endif
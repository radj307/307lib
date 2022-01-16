#pragma once
#include <sysarch.h>
#include <str.hpp>
#include <var.hpp>

#if LANG_CPP >= 17
#include <filesystem>
#else
#include <fstream>
#endif

#include <sstream>
#include <algorithm>

namespace file {
	/**
	 * @brief		Counts the number of characters that appear in a given file stream.
	 * @param ifs	Input Stream
	 * @returns		std::streamoff
	 */
	inline CONSTEXPR std::streamoff count(auto&& is, const char& character) noexcept(false)
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
	inline CONSTEXPR std::streamoff getLineCount(std::istream& is) noexcept(false)
	{
		return count(std::forward<decltype(is)>(is), '\n');
	}

	/**
	 * @brief		Checks if the given character is a slash '/' or backslash '\\'.
	 * @param ch	Input character
	 * @returns		bool
	 */
	inline CONSTEXPR bool isPathSeparator(const char& ch) { return ch == '/' || ch == '\\'; }

	/**
	 * @brief		Checks if the given path has a trailing separator character. ('/' | '\\')
	 * @param path	Input path.
	 * @returns		bool
	 */
	inline WINCONSTEXPR bool hasTrailingSeparator(const std::string_view& path) { return !path.empty() && isPathSeparator(path.back()); }

	/**
	 * @brief			Check if the given filepath exists on the system. The given path must resolve to a file, not directory.
	 * @param filepath	The filepath to check
	 * @returns			bool
	 */
	#if LANG_CPP >= 17
	inline bool exists(const std::filesystem::path& filepath) { return filepath.has_filename() && std::filesystem::exists(filepath); }
	#else
	inline bool exists(const std::string_view& filepath) { return std::ofstream(filepath.data()).is_open(); }
	#endif

	/**
	 * @brief				Check if any of the given filepaths exist in the filesystem.
	 * @tparam ...Paths		Types that must be accepted by the exists() function.
	 * @param ...filepaths	Any number of filepaths to check.
	 * @returns				bool
	 */
	template<typename... Paths> static bool any_exist(Paths&&... filepaths) { return var::variadic_or(exists(std::forward<Paths>(filepaths))...); }
	/**
	 * @brief				Check if all of the given filepaths exist in the filesystem.
	 * @tparam ...Paths		Types that must be accepted by the exists() function.
	 * @param ...filepaths	Any number of filepaths to check.
	 * @returns				bool
	 */
	template<typename... Paths> static bool all_exist(Paths&&... filepaths) { return var::variadic_and(exists(std::forward<Paths>(filepaths))...); }
}

#if LANG_CPP >= 17
namespace file {
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
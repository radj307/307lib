/**
 * @file	envpath.hpp
 * @author	radj307
 * @brief	Contains the PATH-variable-related functions of the env namespace.
 *\n		__Features__
 *\n		- env::getvar(<var_name>)
 *\n			Wrapper for the C getenv() function.
 *\n		- env::get_path()
 *\n			Retrieves the PATH environment variable as a vector.
 *\n		- env::PATH
 *\n			Object that can be used to resolve relative paths or filenames to absolute filepaths.
 *\n
 *\n		__Macros__
 *\n		- ENV_DEFAULT_RETURN_TYPE
 *\n			Can be used to override the default return type for most env functions/methods.
 *\n			Must be either std::string or std::filesystem::path.
 */
#pragma once
#include <sysarch.h>
#include <env.hpp>
#include <str.hpp>
#include <fileutil.hpp>
#include <var.hpp>

#include <optional>
#include <filesystem>
#include <concepts>

 /**
  * @def		ENV_DEFAULT_RETURN_TYPE
  * @brief	You can override this preprocessor macro to specify either std::string or std::filesystem::path as the default return type for env.hpp functions.
  */
#ifndef ENV_DEFAULT_RETURN_TYPE
#define ENV_DEFAULT_RETURN_TYPE std::string
#endif

namespace env {
	/**
	 * @brief	Retrieve the value of the PATH environment variable as a vector of filesystem paths.
	 * @returns	std::vector<std::filesystem::path>
	 */
	template<std::same_as<std::vector<std::filesystem::path>> RT = std::vector<ENV_DEFAULT_RETURN_TYPE>>
	inline RT get_path() noexcept(false)
	{
		if (const auto v{ getvar("path") }; v.has_value()) {
			std::vector<std::filesystem::path> path;
			path.reserve(std::count(v.value().begin(), v.value().end(), ';'));
			std::stringstream ss{ v.value() };
			for (std::string ln{}; std::getline(ss, ln, ';'); ss.flush())
				path.emplace_back(ln);
			path.shrink_to_fit();
			return path;
		}
		throw make_exception("Failed to retrieve PATH variable!");
	}
	/**
	 * @brief	Retrieve the value of the PATH environment variable as a vector of strings.
	 * @returns	std::vector<std::string>
	 */
	template<std::same_as<std::vector<std::string>> RT = std::vector<ENV_DEFAULT_RETURN_TYPE>>
	inline RT get_path() noexcept(false)
	{
		if (const auto v{ getvar("path") }; v.has_value()) {
			std::vector<std::string> path;
			path.reserve(std::count(v.value().begin(), v.value().end(), ';'));
			std::stringstream ss{ v.value() };
			for (std::string ln{}; std::getline(ss, ln, ';'); ss.flush())
				path.emplace_back(ln);
			path.shrink_to_fit();
			return path;
		}
		throw make_exception("Failed to retrieve PATH variable!");
	}

	#ifndef USE_DEPRECATED_PATH_ALGORITHM
	/**
	 * @struct	PATH
	 * @brief	Object used to interact with the PATH environment variable.
	 *\n		Also contains useful filesystem path resolution methods using various methods.
	 */
	class PATH {
		/// @brief	Contains all of the filepaths stored in the PATH environment variable.
		std::vector<std::filesystem::path> path_var;
		/// @brief	Contains the path to the current program, if argv[0] was passed to the constructor.
		std::filesystem::path program_path;

	protected:
		/**
		 * @brief				Checks if the given path exists with one of the given filename extensions.
		 *\n					If the target path exists with one of the given extensions, that path is returned.
		 *\n					If the given target path exists without an extension, or if the target path
		 *\n					wasn't found with any extension, it will be returned unmodified.
		 * @param path			Target absolute filepath. If a relative filepath is given, it is returned unmodified.
		 * @param extensions	List of filename extensions, if extensions are given, each path location is checked for the existence of the target path with any of the extensions appended to it.
		 * @returns				std::filesystem::path
		 */
		std::filesystem::path findWithExtension(std::filesystem::path path, const std::vector<std::string>& extensions) const
		{
			if (path.empty() || file::exists(path) || path.is_relative())
				return path;
			for (auto& ext : extensions)
				if (!ext.empty())
					if (const std::filesystem::path target{ path / ext }; file::exists(path / ext))
						return target;
			return path;
		}
		/**
		 * @brief				Searches each location in the PATH environment variable for the given target filename/path.
		 * @param filename		Target filename/path to find.
		 * @param extensions	Optional list of filename extensions, if extensions are given, each path location is checked for the existence of the target path with any of the extensions appended to it.
		 * @returns				std::filesystem::path
		 */
		std::filesystem::path findInPath(const std::filesystem::path& filename, const std::vector<std::string>& extensions) const
		{
			if (!file::exists(filename))
				for (auto& path : path_var)
					if (const auto p{ findWithExtension(path / filename, extensions) }; file::exists(p))
						return p; // found path with extension
			return filename;
		}
	public:
		/**
		 * @brief		Constructor that accepts argv[0] to use in filepath resolution operations.
		 * @param arg0	The value of argv[0]. (char* & std::string are implicitly convertible to std::filesystem::path)
		 */
		PATH(std::filesystem::path arg0) noexcept(false) : path_var{ get_path<std::vector<std::filesystem::path>>() }, program_path{ arg0.remove_filename() } {}
		/**
		 * @brief	Default Constructor.
		 */
		PATH() noexcept(false) : path_var{ get_path<std::vector<std::filesystem::path>>() } {}

		///* OPERATORS & FORWARDED FUNCTIONS *///
		auto begin() const { return path_var.begin(); }
		auto end() const { return path_var.end(); }
		auto rbegin() const { return path_var.rbegin(); }
		auto rend() const { return path_var.rend(); }
		auto empty() const { return path_var.empty(); }
		auto size() const { return path_var.size(); }
		auto at(auto&& pos) const { return path_var.at(std::forward<decltype(pos)>(pos)); }

		///* FUNCTIONS *///
		/**
		 * @brief		Check if the PATH variable contains a given filepath.
		 * @param path	The path to check for.
		 * @returns		bool
		 */
		auto contains(const std::filesystem::path& path) const { return std::any_of(path_var.begin(), path_var.end(), [&path](auto&& elem) { return elem == path; }); }
		/**
		 * @brief		Check if the PATH variable contains a given filepath. This function uses perfect-forwarding & is the equivalent of calling contains().
		 * @param path	The path to check for.
		 * @returns		bool
		 */
		auto exists(auto&& path) const { return contains(std::forward<decltype(path)>(path)); }


		/**
		 * @brief				Resolve a given filesystem path by checking, in order;
		 *\n					1. the current working directory,
		 *\n					2. the program location (if given in the constructor),
		 *\n					3. the PATH environment variable.
		 * @param path			The target filename/path to resolve. If the path can't be found, this is returned unmodified.
		 * @param extensions	Optional list of filename extensions, if extensions are given, resolution will check if the given path exists with any of the given extensions.
		 * @returns				std::filesystem::path
		 */
		std::filesystem::path resolve(std::filesystem::path path, const std::vector<std::string>& extensions = {}, const bool& checkProgramPath = true) const
		{
			// if the path is absolute or the file already exists, return unmodified
			if (path.is_absolute() || file::exists(path))
				return path;
			// else path is relative or not a path; check working directory first
			if (const auto wdfp{ findWithExtension(file::getWorkingDir<std::filesystem::path>() / path, extensions) }; file::exists(wdfp))
				return wdfp;
			// if argv[0] was supplied & is an absolute path, check that directory.
			if (checkProgramPath && !program_path.empty() && program_path.is_absolute()) {
				if (auto p{ program_path / path }; file::exists(p))
					return p;
				else if (p = findWithExtension(p, extensions); file::exists(p))
					return p;
			}
			// target not located in working dir or the program directory; fallback to check PATH environment variable
			return findInPath(path, extensions);
		}

		/**
		 * @brief				Resolve a given filesystem path by checking, in order;
		 *\n					1. the current working directory,
		 *\n					2. the program location (if given in the constructor),
		 *\n					3. the PATH environment variable.
		 *\n					Calling this function is the equivalent of splitting the result of resolve() by its filename.
		 *\n					If no filename is included as part of the target path, the second return value is empty.
		 * @param path			The target path to resolve. If the path can't be found, this is returned unmodified.
		 * @param extensions	Optional list of filename extensions, if extensions are given, resolution will check if the given path exists with any of the given extensions.
		 * @returns				std::pair<std::filesystem::path, std::filesystem::path>
		 */
		std::pair<std::filesystem::path, std::filesystem::path> resolve_split(std::filesystem::path path, const std::vector<std::string>& extensions = {}, const bool& checkProgramPath = true) const
		{
			auto p{ resolve(path, extensions, checkProgramPath) };
			if (p.has_filename()) {
				const auto filename{ p.filename() };
				return{ p.remove_filename(), filename };
			}
			return{ p, {} };
		}

		/**
		 * @brief		Get the path to the currently running program's directory.
		 * @param arg0	Command used to call this program. If this is left blank and no arg0 was passed in the constructor, an exception is thrown.
		 * @throws		std::exception
		 * @returns		std::filesystem::path
		 */
		std::filesystem::path get_program_dir(const std::optional<std::filesystem::path>& arg0 = std::nullopt) const noexcept(false)
		{
			if (arg0.has_value() || !program_path.empty())
				return resolve_split(arg0.value_or(program_path)).first;
			throw make_exception("env::PATH::get_current_location() failed:  No arg0 was given!");
		}
	};
	#else
	class PATH {
		using ContainerType = std::vector<std::string>;
		const ContainerType path;

		static inline ContainerType get_path()
		{
			if (const auto v{ getvar("path") }; v.has_value()) {
				ContainerType path;
				path.reserve(std::count(v.value().begin(), v.value().end(), ';'));
				std::stringstream ss{ v.value() };
				for (std::string ln{}; std::getline(ss, ln, ';'); ss.flush())
					path.emplace_back(ln);
				path.shrink_to_fit();
				return path;
			}
			throw make_exception("Failed to retrieve PATH variable!");
		}

		static inline std::string normalize_path(std::string path, const char& desiredPathDelim = '/')
		{
			const char inv_delim{ desiredPathDelim == '/' ? '\\' : '/' };
			for (auto& ch : path) {
				if (ch == inv_delim)
					ch = desiredPathDelim;
				else if (ch == '\"' || ch == '\'')
					break;
			}
			return path;
		}

	public:
		constexpr PATH() : path{ std::move(get_path()) } {}


		/**
		 * @brief				Parse the user's PATH environment variable to find the location of argv[0]
		 * @param arg			argv[0] from main() or another path to resolve.
		 * @param extensions	A list of possible file extensions to append to arg before giving up on a directory.
		 * @returns				std::pair<std::string, std::string>
		 *\n		first		The path to the directory where this program is located, including a trailing slash.
		 *\n		second		The name used to call this program.
		 */
		template<var::any_same<std::string, std::filesystem::path> RT = std::string>
		std::pair<RT, RT> resolve_split(std::string name, const std::vector<std::string>& extensions = { ".exe", ".bat", ".so" }) const noexcept
		{
			const auto [filepath, filename] { str::rsplit(normalize_path(name), '/') };
			if (!path.empty() && !str::pos_valid(filepath.find('.')))
				return{ filepath, filename }; // return absolute path
			// iterate through PATH env var
			for (auto& it : path) {
				std::string target(it + '/');
				if (file::exists(target + name))
					return { target, name };
				for (auto& ext : extensions)
					if (file::exists(std::string{ target + name }.append(ext)))
						return { target, { name + ext } };
			}
			return { {}, static_cast<RT>(name) }; // return not found
		}

		/**
		 * @brief				Parse the user's PATH environment variable to find the location of argv[0]
		 * @param arg			argv[0] from main() or another path to resolve.
		 * @param extensions	A list of possible file extensions to append to arg before giving up on a directory.
		 * @returns				std::string
		 */
		std::string resolve(const std::string& arg, const std::vector<std::string>& extensions = { ".exe", ".bat", ".so" }) const noexcept
		{
			const auto& [filepath, filename] { resolve_split(arg, extensions) };
			if (!filepath.empty() && !filename.empty())
				return filepath + '/' + filename;
			return filename;
		}
	};
	#endif

}
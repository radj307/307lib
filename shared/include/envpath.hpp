/**
 * @file	envpath.hpp
 * @author	radj307
 * @brief	Helpers for the system's PATH environment variable, including the env::PATH class.
 */
#pragma once
// 307lib::shared
#include <sysarch.h>
#include <env.hpp>		//< for env::getvar

// STL
#include <vector>		//< for std::vector
#include <filesystem>	//< for std::filesystem::path
#include <sstream>		//< for std::stringstream
#include <algorithm>	//< for std::any_of

namespace env {
	/// @brief	The name of the PATH environment variable on the host machine
	INLINE CONSTEXPR char const* const PATH_VAR_NAME{
	#ifdef OS_WIN
		"path"
	#else
		"PATH"
	#endif
	};

	/**
	 * @brief		Gets the filepaths in the PATH environment variable.
	 * @detail		You can use getvar() to get the PATH environment variable as a std::string.
	 * @returns		A vector containing a filesystem::path for each entry in the PATH variable.
	 */
	inline std::vector<std::filesystem::path> get_path()
	{
		std::vector<std::filesystem::path> paths;

		// retrieve the PATH variable
		if (std::string result{ getvar(PATH_VAR_NAME).value_or("") };
			!result.empty()) {
			// reserve enough space for all of the paths
			paths.reserve(std::count(result.begin(), result.end(), ';') + 1);

			// enumerate the result string and split by semicolons
			for (auto it{ result.begin() }, it_end{ result.end() }, prev_begin{ it };
				 ; //< enter the loop even when at the end
				 ++it) {
				if (it == it_end) {
					// this is the last path, push it and break from the loop
					paths.emplace_back(std::filesystem::path{ std::string_view(prev_begin, it) });
					break;
				}
				else if (*it == ';') {
					// reached a delimiter
					paths.emplace_back(std::filesystem::path{ std::string_view(prev_begin, it) });
					prev_begin = it + 1;
				}
			}

			// remove any unused space if present
			paths.shrink_to_fit();
		}

		return paths;
	}

	/// @brief	Helper class for interacting with the PATH environment variable.
	class PATH {
	public:
		using const_iterator = typename std::vector<std::filesystem::path>::const_iterator;
		using reverse_iterator = typename std::reverse_iterator<const_iterator>;

	private:
		std::vector<std::filesystem::path> paths;

	public:
		/// @brief	Creates a new instance containing paths retrieved from the system environment's PATH variable.
		PATH() : paths{ get_path() } {}

	#pragma region std::vector Methods

		const_iterator begin() const noexcept { return paths.begin(); }
		const_iterator end() const noexcept { return paths.end(); }
		reverse_iterator rbegin() const noexcept { return paths.rbegin(); }
		reverse_iterator rend() const noexcept { return paths.rend(); }
		const_iterator cbegin() const noexcept { return paths.cbegin(); }
		const_iterator cend() const noexcept { return paths.cend(); }
		reverse_iterator crbegin() const noexcept { return paths.crbegin(); }
		reverse_iterator crend() const noexcept { return paths.crend(); }

		WINCONSTEXPR size_t size() const noexcept { return paths.size(); }
		std::filesystem::path at(size_t const index) const noexcept { return paths.at(index); }

	#pragma endregion std::vector Methods

		/**
		 * @brief			Checks if the specified path exists in the list of paths.
		 * @param path	  -	A filesystem path to compare against each path.
		 * @returns			true when the path was found; otherwise, false.
		 */
		bool contains(std::filesystem::path const& path) const
		{
			return std::any_of(paths.begin(), paths.end(), [&path](auto&& pathv) { return path == pathv; });
		}

		/**
		 * @brief					Finds the first path in the list with the specified subpath.
		 * @param relativePath    -	A filename or relative filesystem path.
		 * @returns					Iterator pointing to the first path in the list where the specified
		 *							 relativePath exists when successful; otherwise, the end iterator.
		 */
		const_iterator find(std::filesystem::path const& relativePath) const
		{
			if (!relativePath.is_relative())
				return paths.end();

			const const_iterator it_end{ paths.end() };

			// enumerate the paths & check if the specified subpath exists in it
			for (const_iterator it{ paths.begin() }; it != it_end; ++it) {
				if (std::filesystem::exists(*it / relativePath)) {
					return it;
				}
			}

			return it_end;
		}
		/**
		 * @brief					Finds the first path containing a file with any of the specified extensions.
		 * @param filename		  -	A filename, or relative path that contains a filename, to search for.
		 * @param fileExtensions  -	Any number of file extensions to search for, in priority order.
		 * @returns					Iterator pointing to the first path in the list where the specified
		 *							 filename exists with any one of the specified fileExtensions when
		 *							 successful; otherwise, the end iterator.
		 */
		const_iterator find(std::filesystem::path const& filename, std::vector<std::string> const& fileExtensions) const
		{
			if (!filename.has_filename()) //< don't check if the file exists yet; files may have multiple extensions and we won't find the intended one
				return paths.end();

			const const_iterator it_end{ paths.end() };

			// enumerate the paths & check if the specified subpath exists in it
			for (const_iterator it{ paths.begin() }; it != it_end; ++it) {
				for (const auto& extension : fileExtensions) {
					if (std::filesystem::exists(*it / filename / extension)) {
						return it;
					}
				}
			}

			return it_end;
		}

		/**
		 * @brief					Attempts to resolve the specified relative path to an existing absolute path.
		 * @param relativePath	  -	A relative path to resolve. If it is already absolute, it is returned.
		 * @returns					The resolved path when successful; otherwise, the unmodified relativePath.
		 */
		std::filesystem::path resolve(std::filesystem::path const& relativePath) const
		{
			// if the path is absolute, return it unmodified
			if (relativePath.is_absolute())
				return relativePath;

			// otherwise, check if it exists in the working directory
			else if (const auto pathRelativeToWorkingDir{ std::filesystem::current_path() / relativePath };
					 std::filesystem::exists(pathRelativeToWorkingDir))
				return pathRelativeToWorkingDir;

			// otherwise, search the path var for the specified path
			else if (const auto it{ find(relativePath) };
					 it != end())
				return *it / relativePath;

			// otherwise, return the path unmodified
			return relativePath;
		}
		/**
		 * @brief					Attempts to resolve the specified relative path to an existing absolute
		 *							 path, and splits it by its parent_path and filename components.
		 * @param relativePath	  -	A relative path to resolve and split. If it is already absolute, it is
		 *							 split and returned without attempting to resolve it further.
		 * @returns					A pair containing the parent directory path and filename, respectively.
		 */
		std::pair<std::filesystem::path, std::filesystem::path> resolve_split(std::filesystem::path const& relativePath) const
		{
			const auto result{ resolve(relativePath) };

			// both parent_path and filename return an empty path when the path doesn't include them:
			return std::make_pair(result.parent_path(), result.filename());
		}
	};
}

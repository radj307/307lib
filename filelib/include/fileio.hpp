#pragma once
#include <sysarch.h>
#include <var.hpp>

#include <fstream>
#include <utility>
#include <string>
#include <sstream>
#include <filesystem>

#ifdef read
#undef read
#endif
#ifdef write
#undef write
#endif

#include <iostream>

namespace file {
	/**
	 * @brief		Read the contents of a file to a stringstream.
	 * @param path	Target Filepath
	 * @returns		std::stringstream&&
	 */
	std::stringstream read(const std::filesystem::path&) noexcept;

	/**
	 * @brief			Write the contents of a stringstream buffer to a file.
	 * @param path		Target filepath
	 * @param buffer	Stringstream rvalue reference.
	 * @param append	When true, the buffer is appended to the end of the file rather than overwriting the previous contents.
	 * @returns			bool
	 *\n				true	Successfully wrote all data to file without error.
	 *\n				false	Failed to write all data to file because of an error.
	 */
	bool write(const std::filesystem::path&, std::stringstream&&, const bool& = false);

	/**
	 * @brief			Write any number of objects to a file.
	 * @tparam APPEND	When true, appends the given types to the file instead of overwriting the file's previous contents.
	 * @tparam T...		Variadic Types
	 * @param path		Target Filepath
	 * @param data...	Any number of objects to write to the file.
	 *\n				The object must have a std::ostream::operator<< compatible overload.
	 * @returns			bool
	 *\n				true	Successfully wrote all data to file without error.
	 *\n				false	Failed to write all data to file because of an error.
	 */
	template<::var::Streamable... T> inline bool write(const std::filesystem::path& path, T&&... data)
	{
		std::stringstream buffer;
		(buffer << ... << std::move(data));
		return write(path, std::move(buffer), false);
	}
	/**
	 * @brief			Append any number of objects to the end of a file.
	 * @tparam T...		Variadic Types
	 * @param path		Target Filepath
	 * @param data...	Any number of objects to write to the file.
	 *\n				The object must have a std::ostream::operator<< compatible overload.
	 * @returns			bool
	 *\n				true	Successfully wrote all data to file without error.
	 *\n				false	Failed to write all data to file because of an error.
	 */
	template<::var::Streamable... T> inline bool append(const std::filesystem::path& path, T&&... data)
	{
		std::stringstream buffer;
		(buffer << ... << std::move(data));
		return write(path, std::move(buffer), true);
	}
}

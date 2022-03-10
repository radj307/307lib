/**
 * @file	fileo.hpp
 * @author	radj307
 * @brief	Contains file output functions.
 */
#pragma once
#include "openmode.h"

#include <var.hpp>

#include <utility>
#include <string>
#include <sstream>
#include <filesystem>

namespace file {
	/**
	 * @brief			Write the contents of a stringstream buffer to a file.
	 * @param path		Target filepath
	 * @param buffer	Stringstream rvalue reference.
	 * @param mode		Open mode flags that control how the stream operates.
	 * @returns			bool
	 *\n				true	Successfully wrote all data to file without error.
	 *\n				false	Failed to write all data to file because of an error.
	 */
	bool write_to(const std::filesystem::path&, std::stringstream&&, openmode const& = openmode::out | openmode::trunc);
	/**
	 * @brief			Write the contents of a stringstream buffer to a file.
	 * @param path		Target filepath
	 * @param buffer	Stringstream rvalue reference.
	 * @param append	When true, the buffer is appended to the end of the file rather than overwriting the previous contents.
	 * @returns			bool
	 *\n				true	Successfully wrote all data to file without error.
	 *\n				false	Failed to write all data to file because of an error.
	 */
	bool write_to(const std::filesystem::path& path, std::stringstream&& buffer, const bool& append);
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
	template<var::Streamable... Ts> inline bool write(const std::filesystem::path& path, Ts&&... data)
	{
		#pragma warning (disable:26800)// "Use of a moved-from object: "buffer" (lifetime.1)."
		std::stringstream buffer;
		(buffer << ... << std::move(data));
		return write_to(path, std::move(buffer), openmode::out | openmode::trunc);
		#pragma warning (default:26800)// "Use of a moved-from object: "buffer" (lifetime.1)."
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
	template<var::Streamable... Ts> inline bool append(const std::filesystem::path& path, Ts&&... data)
	{
		#pragma warning (disable:26800)// "Use of a moved-from object: "buffer" (lifetime.1)."
		std::stringstream buffer;
		(buffer << ... << std::move(data));
		return write_to(path, std::move(buffer), openmode::out | openmode::app);
		#pragma warning (default:26800)// "Use of a moved-from object: "buffer" (lifetime.1)."
	}
}

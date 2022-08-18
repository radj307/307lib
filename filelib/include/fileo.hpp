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
	template<typename TChar, typename TCharTraits = std::char_traits<TChar>, typename TAlloc = std::allocator<TChar>>
	bool write_to(const std::filesystem::path& path, std::basic_stringstream<TChar, TCharTraits, TAlloc>&& buffer, openmode const& mode = openmode::out | openmode::trunc)
	{
		if (std::basic_ofstream<TChar, TCharTraits> ofs(path, static_cast<std::ios_base::openmode>(mode)); ofs.is_open())
			return static_cast<bool>(ofs << std::move(buffer.rdbuf()));
		return false;
	}
	/**
	 * @brief			Write the contents of a stringstream buffer to a file.
	 * @param path		Target filepath
	 * @param buffer	Stringstream rvalue reference.
	 * @param append	When true, the buffer is appended to the end of the file rather than overwriting the previous contents.
	 * @returns			bool
	 *\n				true	Successfully wrote all data to file without error.
	 *\n				false	Failed to write all data to file because of an error.
	 */
	template<typename TChar, typename TCharTraits = std::char_traits<TChar>, typename TAlloc = std::allocator<TChar>>
	bool write_to(const std::filesystem::path& path, std::basic_stringstream<TChar, TCharTraits, TAlloc>&& buffer, const bool& append)
	{
		if (std::basic_ofstream<TChar, TCharTraits> ofs(path, static_cast<std::ios_base::openmode>(append ? openmode::app : openmode::trunc)); ofs.is_open())
			return static_cast<bool>(ofs << std::move(buffer.rdbuf()));
		return false;
	}
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
#	pragma warning (disable:26800)// "Use of a moved-from object: "buffer" (lifetime.1)."
	template<typename TChar = char, typename TCharTraits = std::char_traits<TChar>, typename TAlloc = std::allocator<TChar>, var::streamable<std::basic_stringstream<TChar, TCharTraits, TAlloc>>... Ts>
	inline bool write(const std::filesystem::path& path, Ts&&... data)
	{
		std::basic_stringstream<TChar, TCharTraits, TAlloc> buffer;
		(buffer << ... << std::forward<Ts>(data));
		return write_to(path, std::move(buffer), openmode::out | openmode::trunc);
	}
#	pragma warning (default:26800)// "Use of a moved-from object: "buffer" (lifetime.1)."

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
	template<typename TChar = char, typename TCharTraits = std::char_traits<TChar>, typename TAlloc = std::allocator<TChar>, var::streamable<std::basic_ofstream<TChar, TCharTraits>>... Ts>
	inline bool append(const std::filesystem::path& path, Ts&&... data)
	{
		#pragma warning (disable:26800)// "Use of a moved-from object: "buffer" (lifetime.1)."
		std::basic_stringstream<TChar, TCharTraits, TAlloc> buffer;
		(buffer << ... << std::move(data));
		return write_to(path, std::move(buffer), openmode::out | openmode::app);
		#pragma warning (default:26800)// "Use of a moved-from object: "buffer" (lifetime.1)."
	}
}

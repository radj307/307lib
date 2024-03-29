#pragma once
/**
 * @file	stringify.hpp
 * @author	radj307
 * @brief	Contains the various stringify variant functions.
 */
#include <sysarch.h>
#include <var.hpp>
#include <strutility.hpp>
#include <strcore.hpp>

#include <vector>
#include <utility>
#include <optional>

namespace str {
#	pragma warning (disable: 26800) // disable moved-from-object warning

	/**
	 * @brief			Creates a stringstream, inserts all of the given arguments, then move-returns the resulting stringstream.
	 * @tparam ...Ts	Variadic Templated Arguments.
	 * @param ...args	Arguments to insert into the stream, in order.
	 * @returns		std::stringstream
	 */
	template<var::streamable... Ts>
	[[nodiscard]] static std::stringstream streamify(Ts&&... args)
	{
		std::stringstream buffer;
		(buffer << ... << std::forward<Ts>(args));
		return std::move(buffer);
	}

	/**
	 * @brief Creates a temporary stringstream and inserts all of the given arguments in sequential order, then uses the given delimiter to split the resulting string into a vector of strings.
	 * @tparam DelimType	- Input Delimiter Type
	 * @tparam ...VT		- Variadic Templated Arguments.
	 * @param delimiter		- Delimiter to use when splitting the result.
	 * @param ...args		- Arguments to insert into the stream in sequential order.
	 * @returns std::vector<std::string>
	 */
	template<var::streamable... Ts>
	[[nodiscard]] constexpr static const std::vector<std::string> stringify_split(const char& delimiter, Ts&&... args)
	{
		std::stringstream buffer;
		(buffer << ... << std::forward<Ts>(args)) << delimiter;
		std::vector<std::string> vec;
		vec.reserve(count(buffer, delimiter));
		for (std::string sub{}; std::getline(buffer, sub, delimiter); vec.emplace_back(sub)) {}
		vec.shrink_to_fit();
		return std::move(vec);
	}

	/**
	 * @brief Creates a temporary stringstream and inserts all of the given arguments in sequential order, then uses the given delimiter to split the resulting string into a vector of strings.
	 * @tparam DelimType	- Input Delimiter Type
	 * @tparam ...VT		- Variadic Templated Arguments.
	 * @param delimiter		- Delimiter to use when splitting the result.
	 * @param ...args		- Arguments to insert into the stream in sequential order.
	 * @returns std::vector<std::string>
	 */
	template<var::streamable T>
	[[nodiscard]] constexpr static const std::vector<std::string> stringify_split(const char& delimiter, const std::vector<T>& args)
	{
		std::stringstream buffer;
		size_t count{ 0ull };
		for (auto it{ args.begin() }; it != args.end(); ++it, ++count) {
			buffer << *it;
			if (std::distance(it, args.end()) > 1)
				buffer << ' ';
		}
		std::vector<std::string> vec;
		vec.reserve(count);
		for (std::string sub{}; std::getline(buffer, sub, delimiter); vec.emplace_back(sub)) {}
		vec.shrink_to_fit();
		return std::move(vec);
	}

#	pragma warning (default: 26800) // re-enable moved-from-object warning
}
/**
 * @file	Sequence.hpp
 * @author	radj307
 * @brief	Contains the ANSI::Sequence object.
 */
#pragma once
#include <sysarch.h>
#include <var.hpp>

#include <ostream>
#include <string>
#include <sstream>
#include <concepts>

namespace ANSI {
	using Sequence = std::string;
	using wSequence = std::wstring;

	// char
	template<std::same_as<Sequence> Type = Sequence, var::Streamable... Ts>
	const Type make_sequence(const Ts&... segments) noexcept
	{
		if constexpr (sizeof...(segments) > 0ull)
			if (std::stringstream buffer; (buffer << ... << segments))
				return buffer.str();
		return{};
	}
	// wide char
	template<std::same_as<wSequence> Type, var::wStreamable... Ts>
	const Type make_sequence(const Ts&... segments) noexcept
	{
		if constexpr (sizeof...(segments) > 0ull)
			if (std::wstringstream buffer; (buffer << ... << segments))
				return buffer.str();
		return{};
	}
}
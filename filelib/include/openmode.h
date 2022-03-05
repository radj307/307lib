/**
 * @file	openmode.h
 * @author	radj307
 * @brief	Contains the openmode enumerator, which is simply a convenience wrapper for std::ios_base::openmode types.
 */
#pragma once
#include <fstream>

namespace file {
	/**
	 * @enum	openmode
	 * @brief	Convenience wrapper for std::ios_base::openmode types.
	 */
	enum class openmode : int {
		none = 0,
		app = std::ios_base::app,
		binary = std::ios_base::binary,
		in = std::ios_base::in,
		out = std::ios_base::out,
		trunc = std::ios_base::trunc,
		ate = std::ios_base::ate,
	};

	[[nodiscard]] inline openmode operator|(openmode const& left, openmode const& right)
	{
		return static_cast<openmode>(static_cast<int>(left) | static_cast<int>(right));
	}

	[[nodiscard]] inline openmode operator^(openmode const& left, openmode const& right)
	{
		return static_cast<openmode>(static_cast<int>(left) ^ static_cast<int>(right));
	}

	[[nodiscard]] inline openmode operator&(openmode const& left, openmode const& right)
	{
		return static_cast<openmode>(static_cast<int>(left) & static_cast<int>(right));
	}

	[[nodiscard]] inline openmode& operator|=(openmode& mode, openmode const& o)
	{
		return mode = mode | o;
	}

	[[nodiscard]] inline openmode& operator^=(openmode& mode, openmode const& o)
	{
		return mode = mode ^ o;
	}

	[[nodiscard]] inline openmode& operator&=(openmode& mode, openmode const& o)
	{
		return mode = mode & o;
	}

	[[nodiscard]] inline bool operator==(openmode const& mode, openmode const& o)
	{
		return static_cast<int>(mode) == static_cast<int>(o);
	}

	[[nodiscard]] inline bool operator==(openmode const& mode, int const& o)
	{
		return static_cast<int>(mode) == o;
	}

	[[nodiscard]] inline bool operator==(int const& num, openmode const& mode)
	{
		return num == static_cast<int>(mode);
	}

	[[nodiscard]] inline bool operator!=(openmode const& mode, auto&& o)
	{
		return !operator==(mode, std::forward<decltype(o)>(o));
	}
}

#ifndef OPENMODE_H_DISABLE_GLOBAL
using file::operator|;
using file::operator^;
using file::operator&;
using file::operator|=;
using file::operator^=;
using file::operator&=;
using file::operator==;
using file::operator!=;
#endif

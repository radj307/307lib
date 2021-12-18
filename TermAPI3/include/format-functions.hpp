/**
 * @file	format-functions.hpp
 * @author	radj307
 * @brief	Contains Sequence functions for making bold, underlined, and inverted text, as well as resetting them. Also contains the reset() function.
 */
#pragma once
#include <ANSIDefs.h>
#include <Sequence.hpp>
#include <ostream>
namespace color {
	using namespace ANSI;
	/**
	 * @brief		Sets all future characters printed to an output stream as bold using ANSI escape sequences.
	 * @returns		Sequence
	 */
	inline constexpr Sequence bold()
	{
		return Sequence{ make_sequence(ESC, CSI, '1', END) };
	}

	/**
	 * @brief		Removes only bold formatting for all future characters printed to an output stream using ANSI escape sequences.
	 * @returns		Sequence
	 */
	inline constexpr Sequence reset_bold()
	{
		return Sequence{ make_sequence(ESC, CSI, "22", END) };
	}

	/**
	 * @brief		Sets all future characters printed to an output stream as underlined using ANSI escape sequences.
	 * @returns		Sequence
	 */
	inline constexpr Sequence underline()
	{
		return Sequence{ make_sequence(ESC, CSI, '4', END) };
	}

	/**
	 * @brief		Removes only underline formatting for all future characters printed to an output stream using ANSI escape sequences.
	 * @returns		Sequence
	 */
	inline constexpr Sequence reset_underline()
	{
		return Sequence{ make_sequence(ESC, CSI, "24", END) };
	}

	/**
	 * @brief		Sets all future characters printed to an output stream as inverted using ANSI escape sequences.
	 * @returns		Sequence
	 */
	inline constexpr Sequence invert()
	{
		return Sequence{ make_sequence(ESC, CSI, '7', END) };
	}

	/**
	 * @brief		Removes only invert formatting for all future characters printed to an output stream using ANSI escape sequences.
	 * @returns		Sequence
	 */
	inline constexpr Sequence reset_invert()
	{
		return Sequence{ make_sequence(ESC, CSI, "27", END) };
	}

	/**
	 * @brief		Simple output stream placeholder, does not insert anything into an output stream. For use with ternary expressions.
	 * @param os	Target output stream
	 * @returns		std::ostream&
	 */
	inline std::ostream& placeholder(std::ostream& os) { return os; }

	/**
	 * @brief		Reset terminal colors to their defaults.
	 * @returns		Sequence
	 */
	inline static const Sequence reset{ make_sequence(ESC, CSI, "38;5;7", END, ESC, CSI, "48;5;0", END) };

	/**
	 * @brief		Reset terminal colors to their defaults.
	 * @returns		Sequence
	 */
	inline constexpr Sequence reset_all()
	{
		return Sequence{ make_sequence(ESC, CSI, '0', END) };
	}
}
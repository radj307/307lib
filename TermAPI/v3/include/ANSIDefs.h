/**
 * @file ANSIDefs.h
 * @author radj307
 * @brief Contains all of the macros for ANSI escape sequences
 */
#pragma once
#include <type_traits>
#include <str.hpp>

namespace ANSI {
	/// @brief Escape Character. Required at the beginning of all escape sequences.
	inline constexpr const auto ESC{ '\x1b' };
	/// @brief Control Sequence Introducer. Only valid when preceeded by ESC.
	inline constexpr const auto CSI{ '[' };
	/// @brief Operating System Command. Only valid when preceeded by ESC.
	inline constexpr const auto OSC{ ']' };

	/// @brief Defines a color sequence as applying to the foreground (text) layer.
	inline constexpr const auto FORE{ "38;5" };
	/// @brief Defines a color sequence as applying to the background layer.
	inline constexpr const auto BACK{ "48;5" };
	/// @brief Color sequence end character.
	inline constexpr const auto END{ "m" };

	/// @brief Character used by some toggleable escape sequences to define that it should be enabled.
	inline constexpr const auto ENABLE{ 'h' };
	/// @brief Character used by some toggleable escape sequences to define that it should be disabled.
	inline constexpr const auto DISABLE{ 'l' };

	/// @brief Midsequence used by the cursor visibility sequence.
	inline constexpr const auto CURSOR_VISIBLE{ "?25" };
	/// @brief Midsequence used by the cursor blinking sequence.
	inline constexpr const auto CURSOR_BLINK{ "?12" };

	/// @brief Used by the character set sequence.
	inline constexpr const auto CHARACTER_SET{ '(' };
	/// @brief Used by some escape sequences to terminate a string.
	inline constexpr const auto STRING_TERMINATOR{ '\0' };

	template<typename... VT> inline constexpr auto make_sequence(const VT&... ch)
	{
		return std::move(str::stringify(ch...));
	}
}

/// @def	ENABLE_PREPROC	@brief Enables legacy preprocessor definitions for ANSI escape sequence components.
#ifdef ENABLE_PREPROC
/// @def	SEQ_ESC			@brief Escape character.
#define SEQ_ESC "\033"
/// @def	SEQ_BRACKET		@brief When combined with the ESC character, defines a CSI sequence.
#define SEQ_BRACKET "\133"
/// @def	SEQ_FORE		@brief Foreground set color sequence.
#define SEQ_FORE "38;5"
///	@def	SEQ_BACK		@brief Background set color sequence.
#define SEQ_BACK "48;5"
/// @def	SEQ_END			@brief End escape sequence.
#define SEQ_END "m"
/// @def	SEQ_ENABLE		@brief Enable escape sequence.
#define SEQ_ENABLE "h"
/// @def	SEQ_DISABLE		@brief Disable escape sequence.
#define SEQ_DISABLE "l"
/// @def	INSEQ_ESC		@brief Single-character variant of SEQ_ESC
#define INSEQ_ESC '\033'
/// @def	INSEQ_BRACKET	@brief Single-character variant of SEQ_BRACKET
#define INSEQ_BRACKET '\133'
#endif
/**
 * @file	LineCharacter.hpp
 * @author	radj307
 * @brief	TermAPI Extension that adds the LineCharacter object, which wraps most of the DEC Line Drawing characters with friendlier names.
 */
#pragma once
#include <ostream>	// For std::ostream
namespace sys::term {
	/**
	 * @struct	LineCharacter
	 * @brief	Contains pre-made definitions for all DEC line drawing characters.
	 */
	struct LineCharacter {
	private:
		const unsigned char _ch;
		constexpr LineCharacter(const unsigned char& character) : _ch{ character } {}
	public:
		constexpr operator const unsigned char() const { return _ch; }
		static const LineCharacter
			/// @brief	┘	Bottom-Right Corner Line
			CORNER_BOTTOM_RIGHT,
			/// @brief	┐	Top-Right Corner Line
			CORNER_TOP_RIGHT,
			/// @brief	┌	Top-Left Corner Line
			CORNER_TOP_LEFT,
			/// @brief	└	Bottom-Left Corner Line
			CORNER_BOTTOM_LEFT,
			/// @brief	┼	4-Way Cross Junction Line
			JUNCTION_4_WAY,
			/// @brief	┤	3-Way Junction Line facing left. (Missing line is facing right, so the junction is on the right side of the line drawing.)
			JUNCTION_3_WAY_RIGHT,
			/// @brief	├	3-Way Junction Line facing right (Missing line is facing left, so the junction is on the left side of the line drawing.)
			JUNCTION_3_WAY_LEFT,
			/// @brief	┬ 	3-Way Junction Line facing down. (Missing line is on the top, so the junction is on the top side of the line drawing.)
			JUNCTION_3_WAY_TOP,
			/// @brief	┴	3-Way Junction Line facing up. (Missing line is facing down, so the junction is on the bottom side of the line drawing.)
			JUNCTION_3_WAY_BOTTOM,
			/// @brief	│	Vertical Line
			LINE_VERTICAL,
			/// @brief	─	Horizontal Line
			LINE_HORIZONTAL;

		friend std::ostream& operator<<(std::ostream& os, const LineCharacter& line)
		{
			return os << line._ch;
		}
	};
	/// ┘ Bottom-Right Corner Line
	constexpr const LineCharacter LineCharacter::CORNER_BOTTOM_RIGHT{ '\x6a' };
	/// ┐ Top-Right Corner Line
	constexpr const LineCharacter LineCharacter::CORNER_TOP_RIGHT{ '\x6b' };
	/// ┌ Top-Left Corner Line
	constexpr const LineCharacter LineCharacter::CORNER_TOP_LEFT{ '\x6c' };
	/// └ Bottom-Left Corner Line
	constexpr const LineCharacter LineCharacter::CORNER_BOTTOM_LEFT{ '\x6d' };
	/// ┼ 4-Way Cross Junction Line
	constexpr const LineCharacter LineCharacter::JUNCTION_4_WAY{ '\x6e' };
	/// ─ Horizontal Line
	constexpr const LineCharacter LineCharacter::LINE_HORIZONTAL{ '\x71' };
	/// ├ 3-Way Junction Line facing right (Missing line is facing left, so the junction is on the left side of the line drawing.)
	constexpr const LineCharacter LineCharacter::JUNCTION_3_WAY_LEFT{ '\x74' };
	/// ┤ 3-Way Junction Line facing left. (Missing line is facing right, so the junction is on the right side of the line drawing.)
	constexpr const LineCharacter LineCharacter::JUNCTION_3_WAY_RIGHT{ '\x75' };
	/// ┴ 3-Way Junction Line facing up. (Missing line is facing down, so the junction is on the bottom side of the line drawing.)
	constexpr const LineCharacter LineCharacter::JUNCTION_3_WAY_BOTTOM{ '\x76' };
	/// ┬ 3-Way Junction Line facing down. (Missing line is on the top, so the junction is on the top side of the line drawing.)
	constexpr const LineCharacter LineCharacter::JUNCTION_3_WAY_TOP{ '\x77' };
	/// │ Vertical Line
	constexpr const LineCharacter LineCharacter::LINE_VERTICAL{ '\x78' };
}
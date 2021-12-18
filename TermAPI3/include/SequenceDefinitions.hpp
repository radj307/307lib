/**
 * @file SequenceDefinitions.hpp
 * @author radj307
 * @brief	Contains ANSI escape sequence functors derived from Sequence.
 *\n		Covers most/all of the virtual sequences documented by microsoft here:
 *\n		https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences
 */
#pragma once
#include <Sequence.hpp>
#include <CursorOrigin.h>
#include <TermAPIQuery.hpp>

#define SEQUENCE_DEFINITIONS

namespace sys::term {
	using namespace ANSI;
#pragma region Cursor
	/**
	 * @brief	Move the cursor up in the screen buffer.
	 * @param n	Number of characters to move by.
	 * @returns Sequence
	 */
	[[nodiscard]] inline Sequence CursorUp(const unsigned& n = 1u)
	{
		return Sequence(make_sequence(ESC, CSI, n, 'A'));
	}
	/**
	 * @brief	Move the cursor down in the screen buffer.
	 * @param n	Number of characters to move by.
	 * @returns Sequence
	 */
	[[nodiscard]] inline Sequence CursorDown(const unsigned& n = 1u)
	{
		return Sequence(make_sequence(ESC, CSI, n, 'B'));
	}
	/**
	 * @brief	Move the cursor forwards on the current line.
	 * @param n	Number of characters to move by.
	 * @returns Sequence
	 */
	[[nodiscard]] inline Sequence CursorForward(const unsigned& n = 1u)
	{
		return Sequence(make_sequence(ESC, CSI, n, 'C'));
	}
	/**
	 * @brief	Move the cursor backwards on the current line.
	 * @param n	Number of characters to move by.
	 * @returns Sequence
	 */
	[[nodiscard]] inline Sequence CursorBackward(const unsigned& n = 1u)
	{
		return Sequence(make_sequence(ESC, CSI, n, 'D'));
	}
	/**
	 * @brief	Move the cursor to the beginning of one of the next lines.
	 * @param n	Number of lines to move the cursor.
	 * @returns Sequence
	 */
	[[nodiscard]] inline Sequence CursorNextLine(const unsigned& n = 1u)
	{
		return Sequence(make_sequence(ESC, CSI, n, 'E'));
	}
	/**
	 * @brief	Move the cursor to the beginning of a previous line.
	 * @param n	Number of lines to move the cursor.
	 * @returns Sequence
	 */
	[[nodiscard]] inline Sequence CursorPrevLine(const unsigned& n = 1u)
	{
		return Sequence(make_sequence(ESC, CSI, n, 'F'));
	}
	/**
	 * @brief			Set the cursor's horizontal position to a specific column.
	 * @param column	The character number to move the cursor to. The cursor will stay on the current line.
	 * @returns			Sequence
	 */
	[[nodiscard]] inline Sequence CursorHorizontalAbs(const unsigned& column)
	{
		return Sequence(make_sequence(ESC, CSI, !!_internal::CURSOR_MIN_AXIS + column, 'G'));
	}
	/**
	 * @brief			Set the cursor's vertical position to a specific row/line.
	 * @param row		The line number to move the cursor to. The cursor will stay in the current column.
	 * @returns			Sequence
	 */
	[[nodiscard]] inline Sequence CursorVerticalAbs(const unsigned& row)
	{
		return Sequence(make_sequence(ESC, CSI, !!_internal::CURSOR_MIN_AXIS + row, 'd'));
	}
	/**
	 * @brief Set the cursor's position to a given column and row.
	 * @param x_column	- Horizontal position on the target line.
	 * @param y_row		- Vertical position / the target line.
	 * @returns Sequence
	 */
	[[nodiscard]] inline Sequence setCursorPosition(const unsigned& x_column, const unsigned& y_row)
	{
		return Sequence(make_sequence(ESC, CSI, !!_internal::CURSOR_MIN_AXIS + y_row, ';', !!_internal::CURSOR_MIN_AXIS + x_column, 'H'));
	}
	/**
	 * @brief Set the cursor's position to a given column and row.
	 * @param pos	- Pair where the first element is the horizontal position, and the second is the vertical position.
	 * @returns Sequence
	 */
	[[nodiscard]] inline Sequence setCursorPosition(const std::pair<unsigned, unsigned>& pos) { return setCursorPosition(pos.first, pos.second); }
	/**
	 * @brief Save the cursor position. Can be recalled later with LoadCursor.
	 * @returns Sequence
	 */
	[[nodiscard]] inline Sequence SaveCursor()
	{
		return Sequence(make_sequence(ESC, '7'));
	}
	/**
	 * @brief Set the cursor's position to the last saved position with SaveCursor.
	 * @returns Sequence
	 */
	[[nodiscard]] inline Sequence LoadCursor()
	{
		return Sequence(make_sequence(ESC, '8'));
	}
	/**
	 * @brief			Set whether the cursor is visible or not.
	 * @param visible	When true, the cursor is visible.
	 * @returns			Sequence
	 */
	[[nodiscard]] inline Sequence setCursorVisible(const bool& visible)
	{
		return Sequence(make_sequence(ESC, CSI, CURSOR_VISIBLE, (visible ? ENABLE : DISABLE)));
	}
	/// @brief Hides the cursor.
	[[nodiscard]] inline Sequence HideCursor() { return setCursorVisible(false); }
	/// @brief Shows the cursor.
	[[nodiscard]] inline Sequence ShowCursor() { return setCursorVisible(true); }
	/**
	 * @brief			Set whether the cursor is blinking or not.
	 * @param blinking	When true, the cursor is blinking.
	 * @returns			Sequence
	 */
	[[nodiscard]] inline Sequence setCursorBlink(const bool& blinking)
	{
		return Sequence(make_sequence(ESC, CSI, CURSOR_BLINK, (blinking ? ENABLE : DISABLE)));
	}
	/// @brief Enables the cursor blink effect.
	[[nodiscard]] inline Sequence EnableCursorBlink() { return setCursorBlink(true); }
	/// @brief Disables the cursor blink effect.
	[[nodiscard]] inline Sequence DisableCursorBlink() { return setCursorBlink(false); }

	struct Cursor {
		static auto getPos() { return getCursorPosition(); }
		static auto setPos(const std::pair<unsigned, unsigned>& pos) { setCursorPosition(pos)(); }
		static auto setPos(const unsigned& x_column, const unsigned& y_row) { setCursorPosition(x_column, y_row)(); }
		static auto up(const unsigned& n = 1u) { CursorUp(n)(); }
		static auto down(const unsigned& n = 1u) { CursorDown(n)(); }
		static auto forward(const unsigned& n = 1u) { CursorForward(n)(); }
		static auto backward(const unsigned& n = 1u) { CursorBackward(n)(); }
		static auto save() { SaveCursor()(); }
		static auto load() { LoadCursor()(); }
		static auto hide() { HideCursor()(); }
		static auto show() { ShowCursor()(); }
		static auto start_blinking() { EnableCursorBlink()(); }
		static auto stop_blinking() { DisableCursorBlink()(); }
	};
#pragma endregion Cursor

#pragma region Viewport
	/**
	 * @brief			Scroll the viewport up or down by a given number of lines.
	 * @param upNotDown	When true, scrolls up. When false, scrolls down.
	 * @param n			Number of lines to scroll.
	 * @returns			Sequence
	 */
	[[nodiscard]] inline Sequence ScrollBuffer(const bool& upNotDown, const unsigned& n)
	{
		return Sequence(make_sequence(ESC, CSI, n, (upNotDown ? 'S' : 'T')));
	}
	/// @brief Scroll the viewport up by inserting lines from the bottom.
	[[nodiscard]] inline Sequence ScrollUp(const unsigned& n) { return ScrollBuffer(true, n); }
	/// @brief Scroll the viewport down by inserting lines from the top.
	[[nodiscard]] inline Sequence ScrollDown(const unsigned& n) { return ScrollBuffer(false, n); }
#pragma endregion Viewport

#pragma region TextModification
	/**
	 * @brief	Inserts space characters at the current cursor position, shifting any existing text to the right.
	 * @param n	Number of space characters to insert.
	 * @returns Sequence
	 */
	[[nodiscard]] inline Sequence InsertChar(const unsigned& n = 1u)
	{
		return Sequence(make_sequence(ESC, CSI, n, '@'));
	}
	/**
	 * @brief	Deletes characters at the current cursor position, shifting any existing text from the right towards the cursor.
	 * @param n Number of characters to delete.
	 * @returns	Sequence
	 */
	[[nodiscard]] inline Sequence DeleteChar(const unsigned& n = 1u)
	{
		return Sequence(make_sequence(ESC, CSI, n, 'P'));
	}
	/**
	 * @brief	Erases a given number of characters starting at the current cursor position by overwriting them with a space character. Any existing text is not shifted.
	 * @param n Number of characters to erase.
	 * @returns Sequence
	 */
	[[nodiscard]] inline Sequence EraseChar(const unsigned& n = 1u)
	{
		return Sequence(make_sequence(ESC, CSI, n, 'X'));
	}
	/**
	 * @brief	Inserts empty lines above the current cursor position, shifting the cursor down.
	 * @param n Number of lines to insert.
	 * @returns Sequence
	 */
	[[nodiscard]] inline Sequence InsertLine(const unsigned& n = 1u)
	{
		return Sequence(make_sequence(ESC, CSI, n, 'L'));
	}
	/**
	 * @brief	Deletes lines from the screen buffer, starting with the row the cursor is on, shifting any lines below the cursor upwards.
	 * @param n Number of lines to delete.
	 * @returns Sequence
	 */
	[[nodiscard]] inline Sequence DeleteLine(const unsigned& n = 1u)
	{
		return Sequence(make_sequence(ESC, CSI, n, 'M'));
	}
	/**
	 * @enum EraseScope
	 * @brief Modes available to the TextModification EraseIn... functions.
	 */
	enum class EraseScope : unsigned char {
		/// @brief Starts at the cursor position (inclusive), continues until the end of the line/display.
		CURSOR_TO_END = 0u,
		/// @brief Starts at the beginning of the line/display, continues until the cursor position (inclusive).
		BEGIN_TO_CURSOR = 1u,
		/// @brief All text on the line / in the display.
		ALL_TEXT = 2u,
	};
	template<typename T>
	concept EraseInType = std::same_as<T, EraseScope> || std::integral<T>;
	/**
	 * @brief		Replaces all text in the current viewport specified by the given mode with space characters.
	 * @param mode	Operation Mode.
	 *				- _0_ / _CURSOR_TO_END_		Erases all text from the current cursor position until the end of the viewport.
	 *				- _1_ / _BEGIN_TO_CURSOR_	Erases all text from the beginning of the viewport until & including the current cursor position.
	 *				- _2_ / _ALL_TEXT_			Erases all text in the entire viewport.
	 * @returns		Sequence
	 */
	template<EraseInType T>
	[[nodiscard]] inline Sequence EraseInDisplay(const T& erase_scope) noexcept(false)
	{
		if constexpr (!std::same_as<T, EraseScope>)
			if (erase_scope < 0 || erase_scope > 2)
				throw std::exception(str::stringify("EraseInDisplay()\tInvalid erase_scope specifier: \'", erase_scope, "\'! Valid Modes: [0/CURSOR_TO_END|1/BEGIN_TO_CURSOR|2/ALL_TEXT]").c_str());
		return Sequence(make_sequence(ESC, CSI, erase_scope, 'J'));
	}
	/**
	 * @brief		Replaces all text within the current line as specified by the given mode with space characters.
	 * @param mode	Operation Mode.
	 *				- _0_ / _CURSOR_TO_END_		Erases all text from the current cursor position (inclusive) to EOL.
	 *				- _1_ / _BEGIN_TO_CURSOR_	Erases all text from the SOL to the current cursor position (inclusive).
	 *				- _2_ / _ALL_TEXT_			Erases all text on the current line.
	 * @returns		Sequence
	 */
	template<EraseInType T>
	[[nodiscard]] inline Sequence EraseInLine(const T& erase_scope) noexcept(false)
	{
		if constexpr (!std::same_as<T, EraseScope>)
			if (erase_scope < 0 || erase_scope > 2)
				throw std::exception(str::stringify("EraseInLine()\tInvalid mode specifier: \'", erase_scope, "\'! Valid Modes: [0/CURSOR_TO_END|1/BEGIN_TO_CURSOR|2/ALL_TEXT]").c_str());
		return Sequence(make_sequence(ESC, CSI, erase_scope, 'K'));
	}
#pragma endregion TextModification

#pragma region TextFormatting
	/**
	 * @brief		Set the format of the screen and text as specified by the given mode.
	 * @param mode	All possible modes: https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences#text-formatting
	 * @returns		Sequence
	 */
	template<std::integral T>
	[[nodiscard]] inline Sequence SetGraphicsRendition(const T& mode)
	{
		return Sequence(make_sequence(ESC, CSI, mode, 'm'));
	}
	/**
	 * @brief		Set the format of the screen and text as specified by the given mode.
	 * @param mode	All possible modes: https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences#text-formatting
	 * @returns		Sequence
	 */
	template<var::same_or_convertible<std::string> T>
	[[nodiscard]] inline Sequence SetGraphicsRendition(const T& subsequence)
	{
		return Sequence(make_sequence(ESC, CSI, subsequence, 'm'));
	}
#pragma endregion TextFormatting

#pragma region ModeChanges
	/**
	 * @enum	KeyMode
	 * @brief	Mode selector for the setKeyMode function.
	 */
	enum class KeyMode : unsigned char {
		/// @brief	The default mode for either key set. ( KEYPAD: Numeric, CURSOR_KEYS: Normal )
		DEFAULT,
		///	@brief	"Application mode".
		APPLICATION,
	};
	/**
	 * @enum	KeyModeTarget
	 * @brief	Target selector for the setKeyMode function.
	 */
	enum class KeyModeTarget : unsigned char {
		/// @brief	Tenkey numberpad keys.
		KEYPAD,
		/// @brief	Arrow Keys, Home, and End keys.
		CURSOR_KEYS,
	};
	/**
	 * @brief			Set the operation mode for the numberpad/cursor keys.
	 * @param target	Select the target keys to change the operation mode of.
	 * @param mode		Select the mode to apply.
	 * @returns			Sequence
	 */
	inline Sequence setKeyMode(const KeyModeTarget& target, const KeyMode& mode)
	{
		switch (target) {
		case KeyModeTarget::KEYPAD:
			return Sequence(make_sequence(ESC, (mode == KeyMode::APPLICATION ? '=' : '>')));
		case KeyModeTarget::CURSOR_KEYS:
			return Sequence(make_sequence(ESC, CSI, "?1", (mode == KeyMode::APPLICATION ? ENABLE : DISABLE)));
		}
	}
	/// @brief	Enable application mode for the specified keys.
	inline Sequence EnableApplicationMode(const KeyModeTarget& target)
	{
		return setKeyMode(target, KeyMode::APPLICATION);
	}
	/// @brief	Disable application mode for the specified keys.
	inline Sequence DisableApplicationMode(const KeyModeTarget& target)
	{
		return setKeyMode(target, KeyMode::DEFAULT);
	}

#pragma endregion ModeChanges

#pragma region Tabs
	/**
	 * @brief	Sets a tab stop at the current cursor column, causing any applicable tab characters to align to the current column.
	 * @returns Sequence
	 */
	[[nodiscard]] inline Sequence SetTabStop()
	{
		return Sequence(make_sequence(ESC, 'H'));
	}
	/**
	 * @brief	Move the cursor to the next column with a tab stop.
	 *\n		If there are no more tab stops, move to the last column in the row.
	 *\n		If the cursor is in the last column, move to the first column of the next row.
	 * @param n	Number of tab stops to advance the cursor by.
	 * @returns Sequence
	 */
	[[nodiscard]] inline Sequence CursorTabForward(const unsigned& n = 1u)
	{
		return Sequence(make_sequence(ESC, CSI, n, 'I'));
	}
	/**
	 * @brief	Move the cursor to the previous column with a tab stop.
	 *\n		If there are no more tab stops, moves the cursor to the first column.
	 *\n		If the cursor is in the first column, doesn’t move the cursor.
	 * @param n
	 * @returns Sequence
	 */
	[[nodiscard]] inline Sequence CursorTabBackward(const unsigned& n = 1u)
	{
		return Sequence(make_sequence(ESC, CSI, n, 'Z'));
	}
	/**
	 * @brief		Removes tab stops from the current column, if there is one.
	 * @returns		Sequence
	 */
	[[nodiscard]] inline Sequence ClearTabStop()
	{
		return Sequence(make_sequence(ESC, CSI, "0g"));
	}
	/**
	 * @brief	Clear all currently set tab stops.
	 * @returns Sequence
	 */
	[[nodiscard]] inline Sequence ClearAllTabStops()
	{
		return Sequence(make_sequence(ESC, CSI, "3g"));
	}
#pragma endregion Tabs

#pragma region DesignateCharacterSet
	/**
	 * @struct	CharacterSet
	 * @brief	Used to determine which character set to apply in the setCharacterSet() function.
	 */
	enum class CharacterSet : unsigned char {
		/// @brief U.S. ASCII mode
		ASCII = 'B',
		/// @brief VT100 DEC Line Drawing mode (https://vt100.net/docs/vt220-rm/table2-4.html)
		DEC_LINE_DRAWING = '0',
	};
	/**
	 * @brief		Change the active character set mapping to ASCII or DEC Line Drawing Mode.
	 *\n			MS Docs:		https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences#designate-character-set
	 *\n			DEC Characters: https://vt100.net/docs/vt220-rm/table2-4.html
	 * @param chset	Character set to enable.
	 * @returns		Sequence
	 */
	[[nodiscard]] inline Sequence setCharacterSet(const CharacterSet& chset = CharacterSet::ASCII) noexcept(false)
	{
		if (const auto chset_ch{ static_cast<char>(chset) }; chset_ch != static_cast<char>(CharacterSet::ASCII) && chset_ch != static_cast<char>(CharacterSet::DEC_LINE_DRAWING))
			throw std::exception(str::stringify("setCharacterSet()\tReceived invalid chset value: \'", chset_ch, "\'").c_str());
		return Sequence(make_sequence(ESC, CHARACTER_SET, static_cast<unsigned char>(chset)));
	}
	/**
	 * @brief	Set the character set to DEC Line Drawing.
	 * @returns Sequence
	 */
	[[nodiscard]] inline Sequence setLineDrawingMode()
	{
		return setCharacterSet(CharacterSet::DEC_LINE_DRAWING);
	}
	/**
	 * @brief	Set the character set to ASCII.
	 * @returns Sequence
	 */
	[[nodiscard]] inline Sequence unsetLineDrawingMode()
	{
		return setCharacterSet(CharacterSet::ASCII);
	}

#pragma endregion DesignateCharacterSet

#pragma region WindowTitle
	/**
	 * @brief		Set the console window title to a given string.
	 * @param title	A string shorter than 254 characters. If the string is longer, it will be truncated.
	 * @returns		Sequence
	 */
	[[nodiscard]] inline Sequence setWindowTitle(std::string title)
	{
		if (title.size() >= 255ull)
			title = title.substr(0ull, 254ull);
		return Sequence(make_sequence(ESC, OSC, "0;", title.c_str(), STRING_TERMINATOR));
	}
#pragma endregion WindowTitle

#pragma region SoftReset
	/**
	 * @brief	Sets the following terminal properties:
	 *			| Property				| Default Value	Applied |
	 *			|----------:------------|-----------:-----------|
	 *			| Cursor Visibility		| Visible				|
	 *			| Keypad Mode			| Numeric Mode			|
	 *			| Cursor Keys Mode		| Normal Mode			|
	 *			| Top Margin			| 1						|
	 *			| Bottom Margin			| Screen Buffer Height	|
	 *			| Character Set			| US ASCII				|
	 *			| Graphics Rendition	| Off					|
	 *			| Saved Cursor Pos		| Origin Position		|
	 * @returns	Sequence
	 */
	[[nodiscard]] inline Sequence SoftReset()
	{
		return Sequence(make_sequence(ESC, CSI, "!p"));
	}
#pragma endregion SoftReset

#pragma region AlternateScreenBuffer
	/**
	 * @brief				Enable or disable the alternate screen buffer.
	 * @param main_buffer	When true, the alternate screen buffer is enabled. When false, the screen buffer is set back to main.
	 * @returns				Sequence
	 */
	[[nodiscard]] inline Sequence setScreenBuffer(const bool& enable_alternate)
	{
		return Sequence(make_sequence(ESC, CSI, "?1049", (enable_alternate ? ENABLE : DISABLE)));
	}
	/// @brief Enable the alternate screen buffer.
	[[nodiscard]] inline Sequence setAlternateScreenBuffer() { return std::move(setScreenBuffer(true)); }
	/// @brief Disable the alternate screen buffer.
	[[nodiscard]] inline Sequence setMainScreenBuffer() { return setScreenBuffer(false); }
#pragma endregion AlternateScreenBuffer

#pragma region Wrappers_ostream
	/* Cursor */
	[[nodiscard]] inline std::ostream& HideCursor(std::ostream& os) { return os << HideCursor(); }
	[[nodiscard]] inline std::ostream& ShowCursor(std::ostream& os) { return os << ShowCursor(); }

	/* Viewport */

	/* Tabs */
	/// @brief	Sets a tab stop at the current cursor column, causing any applicable tab characters to align to the current column.
	[[nodiscard]] inline std::ostream& SetTabStop(std::ostream& os) { return os << SetTabStop(); }
	/// @brief	Move the cursor to the next column with a tab stop.\n If there are no more tab stops, move to the last column in the row.\n If the cursor is in the last column, move to the first column of the next row.
	[[nodiscard]] inline std::ostream& CursorTabForward(std::ostream& os) { return os << CursorTabForward(); }
	/// @brief	Move the cursor to the previous column with a tab stop.\n If there are no more tab stops, moves the cursor to the first column.\n If the cursor is in the first column, doesn’t move the cursor.
	[[nodiscard]] inline std::ostream& CursorTabBackward(std::ostream& os) { return os << CursorTabBackward(); }
	/// @brief	Removes tab stops from the current column, if there is one.
	[[nodiscard]] inline std::ostream& ClearTabStop(std::ostream& os) { return os << ClearTabStop(); }
	/// @brief	Clear all currently set tab stops.
	[[nodiscard]] inline std::ostream& ClearAllTabStops(std::ostream& os) { return os << ClearAllTabStops(); }

	/* DesignateCharacterSet */
	/// @brief	Set the character set to DEC Line Drawing.
	[[nodiscard]] inline std::ostream& setLineDrawingMode(std::ostream& os) { return os << setLineDrawingMode(); }
	/// @brief	Set the character set to ASCII.
	[[nodiscard]] inline std::ostream& unsetLineDrawingMode(std::ostream& os) { return os << unsetLineDrawingMode(); }

	/* AlternateScreenBuffer */
	/// @brief	Enable the alternate screen buffer.
	[[nodiscard]] inline std::ostream& setAlternateScreenBuffer(std::ostream& os) { return os << setAlternateScreenBuffer(); }
	/// @brief	Disable the alternate screen buffer.
	[[nodiscard]] inline std::ostream& setMainScreenBuffer(std::ostream& os) { return os << setMainScreenBuffer(); }

	/* Soft Reset */
	/// @brief	Resets the following properties: cursor visibility, keypad mode, cursor keys mode, top margin, bottom margin, character-set, graphics rendition, and saved cursor position.
	[[nodiscard]] inline std::ostream& SoftReset(std::ostream& os) { return os << SoftReset(); }
#pragma endregion Wrappers_ostream
}
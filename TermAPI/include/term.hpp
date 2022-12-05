/**
 * @file	term.hpp
 * @author	radj307
 * @brief	Contains a wide variety of cross-platform ANSI escape sequences for every occasion.
 *
 *\n		Covers most of the 'virtual sequences' documented by microsoft here:
 *\n		https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences
 *\n		All of the sequences listed at the above link are usable on both Windows & UNIX-based operating systems.
 */
#pragma once
#include <sysarch.h>
#include <Segments.h>
#include <Sequence.hpp>
#include <CursorOrigin.h>
#include <indentor.hpp>
#include <Message.hpp>
#include <make_exception.hpp>
#include <hasPendingDataSTDIN.h>

#include <iostream>
#include <chrono>
#include <thread>
#include <string>

#ifdef OS_WIN
#include <conio.h> //< For Windows-specific `_kbhit()` & `_getch()` functions.
#else
#include <termios.h>
#include <stdio.h>
#endif

 /**
  * @namespace	term
  * @brief		Contains pre-made ANSI sequences & helper functions for making sequences.
  */
namespace term {
	using namespace std::chrono_literals;
	using namespace ::ANSI;

	/**
	 * @namespace	Query
	 * @brief		Contains query functions used internally by some term functions.
	 */
	namespace query {
		/**
		 * @brief			Receive a query response from STDIN, and return it as a string.
		 * @returns			std::string
		 */
		inline std::string getResponse() noexcept
		{
			std::string s;
			if (hasPendingDataSTDIN())
				while (std::cin)
					s += static_cast<char>(std::cin.get());
			std::cin.clear();
			return s;
		}
	}

#ifndef OS_WIN
	static struct termios old, current;

	// Changes the terminal settings; disables buffered I/O, and optionally enables echo when a key is pressed.
	void initTermios(bool echo = false)
	{
		tcgetattr(0, &old);
		current = old;
		current.c_lflag &= ~ICANON; //< disable buffered I/O
		if (echo)
			current.c_lflag |= ECHO;
		else
			current.c_lflag &= ~ECHO;
		tcsetattr(0, TCSANOW, &current);
	}
	// Resets the terminal settings to their default, pre-`initTermios` state.
	void resetTermios()
	{
		tcsetattr(0, TCSANOW, &old);
	}
#endif

	/**
	 * @brief			Check if a key was pressed.
	 *\n				On windows, this uses `_kbhit`, while on linux, it uses `hasPendingDataSTDIN` (which uses `select()`).
	 * @returns	bool:	True when a key was pressed and there is data waiting in STDIN.
	 */
	inline bool kbhit()
	{
	#ifdef OS_WIN
		return _kbhit() != 0;
	#else
		initTermios();  //< disable buffered I/O
		const auto hasData{ hasPendingDataSTDIN() };
		resetTermios(); //< reset
		return hasData;
	#endif
	}

	/**
	 * @brief			Get a single keypress from STDIN, without blocking.
	 *\n				On windows, this uses _getch(), while on linux, it uses getchar() or getchar_unlocked().
	 * @returns	int:	The key code of the pressed key.
	 *\n				Note that function & arrow keys will insert 2 characters into the STDIN
	 *					 buffer, and will require calling getch() twice to retrieve both characters.
	 */
	inline int getch()
	{
	#ifdef OS_WIN
		return _getch();
	#else
		char c;
		initTermios(); //< disable buffered I/O
	#ifdef MULTITHREADING_CAPABLE
		c = getchar(); //< reset
	#else
		c = getchar_unlocked(); //< this is faster but isn't thread-safe
	#endif
		resetTermios();
		return c;
	#endif
	}

	/// @brief	Prints the escape sequence for DECXCPR(Report Cursor Position).Not thread - safe when multiple threads are printing / reading from STDOUT / STDIN.
	inline static const sequence ReportCursorPosition{ make_sequence(CSI, "6n") };

	/**
	 * @brief		Retrieve the current position of the cursor, measured in characters of the screen buffer.
	 * @tparam RT	Return Type.
	 * @returns		std::pair<RT, RT>
	 */
	template<std::integral RT = size_t>
	inline std::pair<RT, RT> getCursorPosition() noexcept(false)
	{
		const auto stoull{ [](auto&& str) { try { return std::stoull(std::forward<decltype(str)>(str)); } catch (...) { return 0ull; } } };
		std::cout << ReportCursorPosition;
		std::string row, col;
		bool select_col{ false }; ///< @brief when true, appends digits to column; else, appends to row.
		for (auto& c : query::getResponse()) {
			using namespace ANSI;
			switch (c) {
			case '0': [[fallthrough]];
			case '1': [[fallthrough]];
			case '2': [[fallthrough]];
			case '3': [[fallthrough]];
			case '4': [[fallthrough]];
			case '5': [[fallthrough]];
			case '6': [[fallthrough]];
			case '7': [[fallthrough]];
			case '8': [[fallthrough]];
			case '9':
				if (!select_col)
					row += c;
				else
					col += c;
				break;
			case ';': // delim
				select_col = true;
				break;
			case '\x1b': [[fallthrough]];// ESC
			case '\x5b': // CSI
				break;
			case 'R': // sequence end
				if (!row.empty() && !col.empty()) // divider delim has been reached
					return{ static_cast<RT>(!!_internal::CURSOR_MIN_AXIS + stoull(row)), static_cast<RT>(!!_internal::CURSOR_MIN_AXIS + stoull(col)) };
				// else:
				[[fallthrough]];
			default:
				throw make_exception("getCursorPosition()\tReceived unexpected character: \'", c, "\'!");
			}
		}
		throw make_exception("getCursorPosition()\tDidn't receive expected escape sequence! No ending character found!");
	}

	/// @brief	Report device attributes to STDIN.
	inline static const sequence ReportDeviceAttributes{ make_sequence(CSI, "0c") };

	/**
	 * @brief	Get device attributes by calling ReportDeviceAttributes & retrieving the response from STDIN.
	 * @returns	std::string
	*/
	inline std::string getDeviceAttributes() noexcept
	{
		std::cout << ReportDeviceAttributes;
		return query::getResponse();
	}

	/**
	 * @brief	Move the cursor up in the screen buffer.
	 * @param n	Number of characters to move by.
	 * @returns sequence
	 */
	template<std::integral T = unsigned>
	[[nodiscard]] inline sequence CursorUp(const T& n = 1u)
	{
		return make_sequence(CSI, n, 'A');
	}
	/**
	 * @brief	Move the cursor down in the screen buffer.
	 * @param n	Number of characters to move by.
	 * @returns sequence
	 */
	template<std::integral T = unsigned>
	[[nodiscard]] inline sequence CursorDown(const T& n = 1u)
	{
		return make_sequence(CSI, n, 'B');
	}
	/**
	 * @brief	Move the cursor forwards on the current line.
	 * @param n	Number of characters to move by.
	 * @returns sequence
	 */
	template<std::integral T = unsigned>
	[[nodiscard]] inline sequence CursorForward(const T& n = 1u)
	{
		return make_sequence(CSI, n, 'C');
	}
	/**
	 * @brief	Move the cursor backwards on the current line.
	 * @param n	Number of characters to move by.
	 * @returns sequence
	 */
	template<std::integral T = unsigned>
	[[nodiscard]] inline sequence CursorBackward(const T& n = 1u)
	{
		return make_sequence(CSI, n, 'D');
	}
	/**
	 * @brief	Move the cursor to the beginning of one of the next lines.
	 * @param n	Number of lines to move the cursor.
	 * @returns sequence
	 */
	template<std::integral T = unsigned>
	[[nodiscard]] inline sequence CursorNextLine(const T& n = 1u)
	{
		return make_sequence(CSI, n, 'E');
	}
	/**
	 * @brief	Move the cursor to the beginning of a previous line.
	 * @param n	Number of lines to move the cursor.
	 * @returns sequence
	 */
	template<std::integral T = unsigned>
	[[nodiscard]] inline sequence CursorPrevLine(const T& n = 1u)
	{
		return make_sequence(CSI, n, 'F');
	}
	/**
	 * @brief			Set the cursor's horizontal position to a specific column.
	 * @param column	The character number to move the cursor to. The cursor will stay on the current line.
	 * @returns			sequence
	 */
	template<std::integral T>
	[[nodiscard]] inline sequence CursorHorizontalAbs(const T& column)
	{
		return make_sequence(CSI, !!_internal::CURSOR_MIN_AXIS + column, 'G');
	}
	/**
	 * @brief			Set the cursor's vertical position to a specific row/line.
	 * @param row		The line number to move the cursor to. The cursor will stay in the current column.
	 * @returns			sequence
	 */
	template<std::integral T>
	[[nodiscard]] inline sequence CursorVerticalAbs(const T& row)
	{
		return make_sequence(CSI, !!_internal::CURSOR_MIN_AXIS + row, 'd');
	}
	/**
	 * @brief Set the cursor's position to a given column and row.
	 * @param x_column	- Horizontal position on the target line.
	 * @param y_row		- Vertical position / the target line.
	 * @returns sequence
	 */
	template<std::integral Tx, std::integral Ty>
	[[nodiscard]] inline sequence setCursorPosition(const Tx& x_column, const Ty& y_row)
	{
		return make_sequence(CSI, !!_internal::CURSOR_MIN_AXIS + y_row, ';', !!_internal::CURSOR_MIN_AXIS + x_column, 'H');
	}
	/**
	 * @brief Set the cursor's position to a given column and row.
	 * @param pos	- Pair where the first element is the horizontal position, and the second is the vertical position.
	 * @returns sequence
	 */
	template<std::integral Tx, std::integral Ty>
	[[nodiscard]] inline sequence setCursorPosition(const std::pair<Tx, Ty>& pos)
	{
		return setCursorPosition<Tx, Ty>(pos.first, pos.second);
	}

	/// @brief	Save the cursor position. Can be recalled later with LoadCursor.
	inline static const sequence SaveCursor{ make_sequence(ESC, '7') };
	/// @brief	Set the cursor's position to the last saved position with SaveCursor.
	inline static const sequence LoadCursor{ make_sequence(ESC, '8') };

	/// @brief	Enable (Show) the cursor.
	inline static const sequence EnableCursor{ make_sequence(CSI, "?25", ENABLE) };
	/// @brief	Disable (Hide) the cursor.
	inline static const sequence DisableCursor{ make_sequence(CSI, "?25", DISABLE) };

	/**
	 * @brief			Set whether the cursor is visible or not.
	 * @param visible	When true, the cursor is visible.
	 * @returns			sequence
	 */
	[[nodiscard]] inline sequence CursorVisible(const bool& visible)
	{
		return visible ? EnableCursor : DisableCursor;
	}

	/// @brief	Enable cursor blinking
	inline static const sequence EnableCursorBlink{ make_sequence(CSI, "?12", ENABLE) };
	/// @brief	Disable cursor blinking
	inline static const sequence DisableCursorBlink{ make_sequence(CSI, "?12", DISABLE) };

	/**
	 * @brief			Set whether the cursor is blinking or not.
	 * @param blinking	When true, the cursor is blinking.
	 * @returns			sequence
	 */
	[[nodiscard]] inline sequence CursorBlink(const bool& blinking)
	{
		return blinking ? EnableCursorBlink : DisableCursorBlink;
	}
	/// @brief Enables the cursor blink effect.
	/// @brief Disables the cursor blink effect.

	struct Cursor {
		static auto getPos() { return getCursorPosition(); }
		static auto setPos(const std::pair<unsigned, unsigned>& pos) { std::cout << setCursorPosition(pos); }
		static auto setPos(const unsigned& x_column, const unsigned& y_row) { std::cout << setCursorPosition(x_column, y_row); }
		static auto up(const unsigned& n = 1u) { std::cout << CursorUp(n); }
		static auto down(const unsigned& n = 1u) { std::cout << CursorDown(n); }
		static auto forward(const unsigned& n = 1u) { std::cout << CursorForward(n); }
		static auto backward(const unsigned& n = 1u) { std::cout << CursorBackward(n); }
		static auto save() { std::cout << SaveCursor; }
		static auto load() { std::cout << LoadCursor; }
		static auto hide() { std::cout << DisableCursor; }
		static auto show() { std::cout << EnableCursor; }
		static auto start_blinking() { std::cout << EnableCursorBlink; }
		static auto stop_blinking() { std::cout << DisableCursorBlink; }
	};

	/**
	 * @brief		Scroll the viewport up or down by a given number of lines.
	 * @param dir	A boolean value that selects which direction to scroll in.
	 *				| Value | Direction |
	 *				| ----- | --------- |
	 *				| false | Down      |
	 *				| true	| Up        |
	 * @param n		Number of lines to scroll.
	 * @returns		sequence
	 */
	[[nodiscard]] inline sequence Scroll(const bool& dir, const unsigned& n)
	{
		return make_sequence(CSI, n, (dir ? 'S' : 'T'));
	}
	/// @brief Scroll the viewport up by inserting lines from the bottom.
	[[nodiscard]] inline sequence ScrollUp(const unsigned& n = 1u) { return Scroll(true, n); }
	/// @brief Scroll the viewport down by inserting lines from the top.
	[[nodiscard]] inline sequence ScrollDown(const unsigned& n = 1u) { return Scroll(false, n); }

	/**
	 * @brief	Inserts space characters at the current cursor position, shifting any existing text to the right.
	 * @param n	Number of space characters to insert.
	 * @returns sequence
	 */
	[[nodiscard]] inline sequence InsertChar(const unsigned& n = 1u)
	{
		return make_sequence(CSI, n, '@');
	}
	/**
	 * @brief	Deletes characters at the current cursor position, shifting any existing text from the right towards the cursor.
	 * @param n Number of characters to delete.
	 * @returns	sequence
	 */
	[[nodiscard]] inline sequence DeleteChar(const unsigned& n = 1u)
	{
		return make_sequence(CSI, n, 'P');
	}
	/**
	 * @brief	Erases a given number of characters starting at the current cursor position by overwriting them with a space character. Any existing text is not shifted.
	 * @param n Number of characters to erase.
	 * @returns sequence
	 */
	[[nodiscard]] inline sequence EraseChar(const unsigned& n = 1u)
	{
		return make_sequence(CSI, n, 'X');
	}
	/**
	 * @brief	Inserts empty lines above the current cursor position, shifting the cursor down.
	 * @param n Number of lines to insert.
	 * @returns sequence
	 */
	[[nodiscard]] inline sequence InsertLine(const unsigned& n = 1u)
	{
		return make_sequence(CSI, n, 'L');
	}
	/**
	 * @brief	Deletes lines from the screen buffer, starting with the row the cursor is on, shifting any lines below the cursor upwards.
	 * @param n Number of lines to delete.
	 * @returns sequence
	 */
	[[nodiscard]] inline sequence DeleteLine(const unsigned& n = 1u)
	{
		return make_sequence(CSI, n, 'M');
	}

	/// @brief	Erases all characters in the current viewport. (similar to `clear`|`cls`)
	inline static const sequence clear{ make_sequence(CSI, 2, 'J') };

	/**
	 * @brief		Replaces text within the viewport with spaces. The area of text cleared is defined by the given operation mode.
	 * @param mode	Operation Mode Value. (Range: 0 - 2)
	 *\n			__SOVP__ = Start Of ViewPort
	 *\n			__EOVP__ = End Of ViewPort
	 *				| Value | Begin  | End    | Description                                                                |
	 *				| ----- | ------ | ------ | -------------------------------------------------------------------------- |
	 *				| `0`   | Cursor | EOVP   | Erases all text from the cursor to the end of the viewport.                |
	 *				| `1`   | SOVP   | Cursor | Erases all text from the start of the viewport to the cursor.              |
	 *				| `2`   | SOVP   | EOVP   | Erases all text from the start of the viewport to the end of the viewport. |
	 * @returns		sequence
	 */
	template<typename T>
	[[nodiscard]] inline sequence EraseInDisplay(const T& erase_scope)
	{
		return make_sequence(CSI, erase_scope, 'J');
	}


	/**
	 * @brief		Replaces all text within the current line as specified by the given mode with space characters.
	 * @param mode	Operation Mode Value. (Range: 0 - 2)
	 *\n			__SOL__ = Start Of Line
	 *\n			__EOL__ = End Of Line
	 *				| Value | Begin  | End    |
	 *				| ----- | ------ | ------ |
	 *				| `0`   | Cursor | EOL    |
	 *				| `1`   | SOL    | Cursor |
	 *				| `2`   | SOL    | EOL    |
	 * @returns		sequence
	 */
	template<typename T>
	[[nodiscard]] inline sequence EraseInLine(const T& erase_scope) noexcept(false)
	{
		return make_sequence(CSI, erase_scope, 'K');
	}

	template<var::streamable... Ts>
	[[nodiscard]] inline static sequence SGR(const Ts&... modes)
	{
	#		ifdef OS_WIN
		return make_sequence((CSI, modes, 'm')...); // don't allow chaining
	#		else
		return make_sequence(CSI, (modes, ';')..., 'm'); // allow chaining
	#		endif
	}

	template<var::streamable... Ts>
	[[nodiscard]] inline static sequence SelectGraphicsRendition(const Ts&... modes)
	{
		return SGR(modes...);
	}

	/// @brief	Enables Application Mode for the number pad keys.
	inline static const sequence EnableKeypadApplicationMode{ make_sequence(ESC, '=') };
	/// @brief	Disables Application Mode for the number pad keys.
	inline static const sequence DisableKeypadApplicationMode{ make_sequence(ESC, '>') };
	/// @brief	Enables Cursor Keys mode for the home, end, & arrow keys.
	inline static const sequence EnableCursorKeysMode{ make_sequence(CSI, "?1", ENABLE) };
	/// @brief	Disables Cursor Keys mode for the home, end, & arrow keys.
	inline static const sequence DisableCursorKeysMode{ make_sequence(CSI, "?1", DISABLE) };

	/// @brief	Sets a tab stop at the current cursor column, causing any applicable tab characters to align to the current column.
	inline static const sequence SetTabStop{ make_sequence(ESC, 'H') };

	/**
	 * @brief	Move the cursor to the next column with a tab stop.
	 *\n		If there are no more tab stops, move to the last column in the row.
	 *\n		If the cursor is in the last column, move to the first column of the next row.
	 * @param n	Number of tab stops to jump forward by.
	 * @returns sequence
	 */
	[[nodiscard]] inline static sequence NextTabStop(const unsigned& n = 1u)
	{
		return make_sequence(CSI, n, 'I');
	}

	/**
	 * @brief	Move the cursor to the previous column with a tab stop.
	 *\n		If there are no more tab stops, moves the cursor to the first column.
	 *\n		If the cursor is in the first column, doesn�t move the cursor.
	 * @param n	Number of tab stops to jump backwards by.
	 * @returns sequence
	 */
	[[nodiscard]] inline static sequence PrevTabStop(const unsigned& n = 1u)
	{
		return make_sequence(CSI, n, 'Z');
	}

	/// @brief	Unsets any tab stops in the current column from the SetTabStop command.
	inline static const sequence UnsetTabStop{ make_sequence(CSI, "0g") };

	/// @brief	Clear all currently set tab stops.
	inline static const sequence ClearTabStops{ make_sequence(CSI, "3g") };

	/// @brief	Sets a tab stop at every 8th column. (Default)
	inline static const sequence ResetTabStops{ make_sequence(CSI, "?5W") };

	/// @brief	Enable DEC Line Drawing mode.
	inline static const sequence EnableLineDrawing{ make_sequence(OSC, '0') };

	/// @brief	Disable DEC Line Drawing mode.
	inline static const sequence DisableLineDrawing{ make_sequence(OSC, 'B') };

	/**
	 * @brief				Generate an escape sequence that resizes the console screen buffer and/or window.
	 * @param xColumnCount:	Number of horizontal columns.
	 * @param yLineCount:	Number of vertical lines.
	 * @returns				sequence
	 */
	template<std::integral T> requires var::streamable<T, std::stringstream>
	[[nodiscard]] inline static sequence setScreenBufferSize(const T& columnCount, const T& lineCount)
	{
		return make_sequence(CSI, "8;", lineCount, ';', columnCount, 't');
	}

	/**
	 * @brief				Generate an escape sequence that resizes the console screen buffer and/or window.
	 * @param xColumnCount:	Number of horizontal columns.
	 * @param yLineCount:	Number of vertical lines.
	 * @returns				sequence
	 */
	template<std::integral T> requires var::streamable<T, std::stringstream>
	[[nodiscard]] inline static sequence setScreenBufferSize(const std::pair<T, T> colRowPair)
	{
		return make_sequence(CSI, "8;", colRowPair.second, ';', colRowPair.first, 't');
	}

	/**
	 * @brief	Get the size of the screen buffer, in characters.
	 * @returns	std::pair<size_t, size_t>
	 *			 1.	Number of columns. (Horizontal/Width/x)
	 *			 2.	Number of rows. (Vertical/Height/y)
	 */
	[[nodiscard]] std::pair<size_t, size_t> getScreenBufferSize();

#ifdef OS_WIN
	/**
	 * @brief		Set the console window title to a given string.
	 * @param title	A string shorter than 254 characters. If the string is longer, it will be truncated.
	 * @returns		sequence
	 */
	[[nodiscard]] inline sequence WindowTitle(std::string title)
	{
		if (title.size() >= 255ull)
			title = title.substr(0ull, 254ull);
		return make_sequence(OSC, "0;", title.c_str(), NULL_TERM);
	}
#endif

	/**
	 * @brief	Resets the following terminal properties:
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
	 */
	inline static const sequence Reset{ make_sequence(CSI, "!p") };
	/// @brief	Resets foreground (text) colors.
	inline static const sequence ResetTextColor{ make_sequence(CSI, SGR_DEFAULT_FORE, END) };
	/// @brief	Resets background colors.
	inline static const sequence ResetBackColor{ make_sequence(CSI, SGR_DEFAULT_BACK, END) };
	/// @brief	Resets foreground (text) color & background color to their defaults.
	inline static const sequence ResetColor{ make_sequence(ResetTextColor, ResetBackColor) };

#pragma region AlternateScreenBuffer
	/// @brief	Enables the alternate screen buffer.
	inline static const sequence EnableAltScreenBuffer{ make_sequence(CSI, "?1049", ENABLE) };
	/// @brief	Disables the alternate screen buffer.
	inline static const sequence DisableAltScreenBuffer{ make_sequence(CSI, "?1049", DISABLE) };
#pragma endregion AlternateScreenBuffer
}

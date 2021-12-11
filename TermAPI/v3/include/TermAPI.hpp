/**
 * @file TermAPI.hpp
 * @author radj307
 * @brief This header-only library contains cross-platform ANSI escape sequence helper functions designed to be used with iostreams
 */
#pragma once
#ifdef TERMAPI_ENABLE_OLD_FUNCTIONS
#define ENABLE_PREPROC
#endif

#include <sysarch.h>	///< @brief Architecture / C++ version detection

 // Color headers
#include <ANSIDefs.h>
#include <color-values.h>
#include <Layer.hpp>
#include <FormatFlag.hpp>
#include <format-functions.hpp>
#include <setcolor.hpp>
#include <setcolor-functions.hpp>

#if CPP < 20
#warning TermAPI.hpp Requires at least C++20
#else

#ifdef OS_WIN
#include <TermAPIWin.hpp> // Windows-Specific functionality, like enabling/disabling ANSI virtual sequence handling.
#endif

 /// STL
#include <iostream>
#include <iomanip>
#include <sstream>

#ifndef TERMAPI_ENABLE_OLD_FUNCTIONS

#include <SequenceDefinitions.hpp>
#include <Message.hpp>
#else

namespace sys::term {
#pragma region CursorPositioning
	/// @brief Moves the cursor up by _n_ character positions.
	inline void cursorUp(unsigned n = 1u)
	{
		printf(SEQ_ESC SEQ_BRACKET "%uA", n);
	}
	/// @brief Moves the cursor down by _n_ character positions.
	inline void cursorDown(unsigned n = 1u)
	{
		printf(SEQ_ESC SEQ_BRACKET "%uB", n);
	}
	/// @brief Moves the cursor forward (right) by _n_ character positions.
	inline void cursorForward(unsigned n = 1u)
	{
		printf(SEQ_ESC SEQ_BRACKET "%uC", n);
	}
	/// @brief Moves the cursor backward (left) by _n_ character positions.
	inline void cursorBackward(unsigned n = 1u)
	{
		printf(SEQ_ESC SEQ_BRACKET "%uD", n);
	}
	/// @brief Moves the cursor up by _n_ lines.
	inline void cursorPreviousLine(unsigned n = 1u)
	{
		printf(SEQ_ESC SEQ_BRACKET "%uF", n);
	}
	/// @brief Moves the cursor down by _n_ lines.
	inline void cursorNextLine(unsigned n = 1u)
	{
		printf(SEQ_ESC SEQ_BRACKET "%uE", n);
	}
	/// @brief Sets the (absolute, non-relative) cursor position to _n_ (character positions) on the current line. Starts from the left.
	inline void cursorHorizontalAbsolute(unsigned n)
	{
		printf(SEQ_ESC SEQ_BRACKET "%uG", n);
	}
	/// @brief Sets the (absolute, non-relative) cursor position to _n_ (character positions) in the current column. Starts from the top.
	inline void cursorVerticalAbsolute(unsigned n)
	{
		printf(SEQ_ESC SEQ_BRACKET "%ud", n);
	}
	inline void cursorSavePos()
	{
		printf(SEQ_ESC "7");
	}
	inline std::ostream& SaveCursorPos(std::ostream& os)
	{
		cursorSavePos();
		return os;
	}
	inline void cursorLoadPos()
	{
		printf(SEQ_ESC "8");
	}
	inline std::ostream& LoadCursorPos(std::ostream& os)
	{
		cursorLoadPos();
		return os;
	}
	/**
	 * @brief Sets the cursor's position to a given x/y coordinate, in relation to the origin point top left corner (1,1)
	 * @param x			- Target horizontal-axis position, measured in characters of the screen buffer. Starts at 1.
	 * @param y			- Target vertical-axis position, measured in characters of the screen buffer. Starts at 1.
	 */
	inline void cursorPosition(unsigned x, unsigned y)
	{
		printf(SEQ_ESC SEQ_BRACKET "%u;%uH", !!_internal::CURSOR_POS_MIN_ZERO + y, !!_internal::CURSOR_POS_MIN_ZERO + x);
	}
#pragma endregion CursorPositioning

#pragma region Viewport
	inline void scrollUp(unsigned n = 1u)
	{
		printf(SEQ_ESC SEQ_BRACKET "%uS", n);
	}
	inline void scrollDown(unsigned n = 1u)
	{
		printf(SEQ_ESC SEQ_BRACKET "%uT", n);
	}
#pragma endregion Viewport

#pragma region ScreenBuffer
	/**
	 * @brief This swaps to an alternative screen buffer. It is recommended to use this for applications that require user input, as it prevents overwriting the user's current session in the screen buffer and eliminates the need for clearing the screen.
	 */
	inline void screenBufferAlternate()
	{
		printf(SEQ_ESC SEQ_BRACKET "?1049h");
	}
	inline std::ostream& screenBufferAlternate(std::ostream& os)
	{
		screenBufferAlternate();
		return os;
	}
	/**
	 * @brief Sets the screen buffer back to main, it is (required) to call this before the program exits if screenBufferAlternate() was called.
	 */
	inline void screenBufferMain()
	{
		printf(SEQ_ESC SEQ_BRACKET "?1049l");
	}
	inline std::ostream& screenBufferMain(std::ostream& os)
	{
		screenBufferMain();
		return os;
	}
#pragma endregion ScreenBuffer

#pragma region TextModification
	inline void insertChar(unsigned n = 1u)
	{
		printf(SEQ_ESC SEQ_BRACKET "%u@", n);
	}
	// delete characters to the right (fore) of the cursor.
	inline void deleteNext(unsigned n = 1u)
	{
		printf(SEQ_ESC SEQ_BRACKET "%uP", n);
	}
	// delete characters to the left (back) of the cursor.
	inline void deleteLast(unsigned n = 1u)
	{
		printf(SEQ_ESC SEQ_BRACKET "%uX", n);
	}

	inline void insertLine(unsigned n = 1u)
	{
		printf(SEQ_ESC SEQ_BRACKET "%uL", n);
	}
	inline void deleteLine(unsigned n = 1u)
	{
		printf(SEQ_ESC SEQ_BRACKET "%uM", n);
	}

#pragma region TextModification_EraseFunctions
	/**
	 * @struct DTarget
	 * @brief Used to specify the operation mode for the eraseInDisplay / eraseInLine functions.
	 */
	struct DTarget {
	private:
		const unsigned char _index; ///< @brief Internal use only. Differentiates between target modes.
	protected:
		/**
		 * @brief Default Constructor.
		 * @param index	- Index value
		 */
		constexpr DTarget(unsigned char index) : _index{ std::move(index) } {}
	public:
		constexpr operator const unsigned char() const { return _index; }
		constexpr bool operator==(const unsigned char& o) const { return _index == o; }
		constexpr bool operator==(const DTarget& o) const { return operator==(o._index); }
		constexpr bool operator!=(auto&& o) const { return !operator==(std::forward<decltype(o)>(o)); }

		static const DTarget
			CURSOR_TO_EOL,
			SOL_TO_CURSOR,
			WHOLE_BUFFER;
	};
	inline constexpr const DTarget
		/// @brief Specifies from the cursor pos (inclusive) to the end of the line.
		DTarget::CURSOR_TO_EOL{ 0u },
		/// @brief Specifies from the start of the line to the cursor pos (inclusive).
		DTarget::SOL_TO_CURSOR{ 1u },
		/// @brief Specifies the entire screen buffer.
		DTarget::WHOLE_BUFFER{ 2u };

	/**
	 * @brief Allows clearing a portion of the whole display, as specified by the given target mode.
	 * @param target	- Specifies the range to replace with space characters.
	 *\n				  0 / CURSOR_TO_EOL	- From the current cursor position (inclusive) to the end of the screen buffer.
	 *\n				  1 / SOL_TO_CURSOR	- From the start of the screen buffer to the current cursor position (inclusive).
	 *\n				  2 / WHOLE_BUFFER	- The entire screen buffer.
	 */
	inline void eraseInDisplay(DTarget target = DTarget::CURSOR_TO_EOL)
	{
		printf(SEQ_ESC SEQ_BRACKET "%cJ", target.operator const unsigned char());
	}
	/// @brief Clears the whole screen buffer by replacing everything with space characters.
	inline std::ostream& clear(std::ostream& os)
	{
		eraseInDisplay(DTarget::WHOLE_BUFFER);
		return os;
	}
	/**
	 * @brief Replace the current line with space characters, depending on the received target.
	 * @param target	- Specifies the range to replace with space characters.
	 *\n				  0 / CURSOR_TO_EOL	- From the current cursor position (inclusive) to the end of the current line.
	 *\n				  1 / SOL_TO_CURSOR	- From the start of the current line to the current cursor position (inclusive).
	 *\n				  2 / WHOLE_BUFFER	- The entire current line.
	 */
	inline void eraseInLine(DTarget target = DTarget::CURSOR_TO_EOL)
	{
		printf(SEQ_ESC SEQ_BRACKET "%cK", target.operator const unsigned char());
	}
	/// @brief Clears the current line by replacing it with space characters.
	inline std::ostream& clear_line(std::ostream& os)
	{
		eraseInLine(DTarget::WHOLE_BUFFER);
		return os;
	}
#pragma endregion TextModification_EraseFunctions
#pragma endregion TextModification

#pragma region TextFormatting
	/**
	 * @brief Set the text formatting used on all future writes to the screen buffer. Similar/equivalent to calling functions in ColorLib.hpp
	 * @param mode	- Available modes can be found at https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences#text-formatting
	 */
	inline void setGraphicsRendition(unsigned char mode)
	{
		printf(SEQ_ESC SEQ_BRACKET "%cm", mode);
	}
#pragma endregion TextFormatting

	/**
	 * @function cursorVisible(const bool)
	 * @brief Changes the cursor's visibility in the terminal
	 * @param state	- When true, the cursor is visible, else the cursor is not visible.
	 */
	inline void cursorVisible(const bool state) noexcept
	{
		if (state)
			printf(SEQ_ESC SEQ_BRACKET "?25" SEQ_ENABLE);
		else
			printf(SEQ_ESC SEQ_BRACKET "?25" SEQ_DISABLE);
	}

	/**
	 * @brief Start or stop the cursor blinking effect.
	 * @param state	- When true, enables cursor blink, else disables cursor blink.
	 */
	inline void cursorBlink(const bool state) noexcept
	{
		if (state)
			printf(SEQ_ESC SEQ_BRACKET "?12" SEQ_ENABLE);
		else
			printf(SEQ_ESC SEQ_BRACKET "?12" SEQ_DISABLE);
	}

	/**
	 * @struct Cursor
	 * @brief Exposes methods for changing the visibility, position, & blinking of the cursor.
	 */
	struct Cursor {
		using CoordT = std::pair<unsigned short, unsigned short>;
	private:
		std::optional<CoordT> _pos;
		std::optional<bool> _visible, _blink;
	public:
		Cursor(std::optional<bool> visible = std::nullopt, std::optional<bool> blink = std::nullopt) : _pos{ std::nullopt }, _visible{ std::move(visible) }, _blink{ std::move(blink) } {}
		Cursor(CoordT pos, std::optional<bool> visible = std::nullopt, std::optional<bool> blink = std::nullopt) : _pos{ std::move(pos) }, _visible{ std::move(visible) }, _blink{ std::move(blink) } {}

		[[nodiscard]] constexpr std::optional<bool>& visible() { return _visible; }
		[[nodiscard]] constexpr std::optional<bool>& blink() { return _blink; }

		static void visible(const bool& is_visible)
		{
			cursorVisible(is_visible);
		}
		static void blinking(const bool& is_blinking)
		{
			cursorBlink(is_blinking);
		}

		template<typename RT = unsigned short> requires std::integral<RT>
		[[nodiscard]] static std::pair<RT, RT> getPos()
		{
			return getCursorPosition<RT>();
		}

		template<typename T> requires std::integral<T>
		static void setPos(T horizontal_column, T vertical_row)
		{
			cursorPosition(horizontal_column, vertical_row);
		}

		friend std::ostream& operator<<(std::ostream& os, const Cursor& obj)
		{
			if (obj._visible.has_value()) { // set visibility
				if (obj._visible.value())
					printf(SEQ_ESC SEQ_BRACKET "?25" SEQ_ENABLE);
				else
					printf(SEQ_ESC SEQ_BRACKET "?25" SEQ_DISABLE);
			}
			if (obj._blink.has_value()) { // set blinking
				if (obj._blink.value())
					printf(SEQ_ESC SEQ_BRACKET "?12" SEQ_ENABLE);
				else
					printf(SEQ_ESC SEQ_BRACKET "?12" SEQ_DISABLE);
			}
			if (obj._pos.has_value()) { // set position
				const auto& [x, y] {obj._pos.value()};
				cursorPosition(x, y);
			}
			return os;
		}
	};

	struct CursorVisible : public Cursor {
		CursorVisible(const bool isVisible) : Cursor(isVisible) {}
	};
	struct CursorBlink : public Cursor {
		CursorBlink(const bool isBlinking) : Cursor(std::nullopt, isBlinking) {}
	};

	/**
	 * @function colorSet(const short, const bool = true)
	 * @brief Change the color of characters printed to STDOUT using ColorLib.hpp.
	 * @param color			- The color value to set.
	 * @param foreground	- When true, changes the foreground color, else changes the background color.
	 */
	inline void colorSet(short color, color::Layer foreground = color::Layer::FOREGROUND)
	{
		std::cout << color::setcolor(std::move(color), std::move(foreground));
	}

	/**
	 * @function colorReset()
	 * @brief Reset the color and formatting of characters printed to STDOUT.
	 */
	inline void colorReset()
	{
		std::cout << color::reset;
	}
	struct TermMessage {
	protected:
		const std::string _message;
		const short _color;
		const std::streamsize _indent;

	public:
		constexpr TermMessage(const std::string& message, const short& color, const std::streamsize& indent = 8ull) : _message{ message }, _color{ color }, _indent{ indent } {}

		friend std::ostream& operator<<(std::ostream& os, const TermMessage& obj)
		{
			return os << color::setcolor(obj._color) << obj._message << color::reset << str::VIndent(obj._indent + 1ull, obj._message.size());
		}
	};

	inline Sequence error()
	{
		return Sequence(make_sequence(color::makeColorSequence(color::intense_red, Layer::FOREGROUND), "[ERROR]", color::reset));
	}

	/// @brief Prints "[ERROR]" in intense_red
	inline std::ostream& error(std::ostream& os)
	{
		os << TermMessage("[ERROR]", color::intense_red);
		return os;
	}
	/// @brief Prints "[WARN]" in orange
	inline std::ostream& warn(std::ostream& os)
	{
		os << TermMessage("[WARN]", color::orange);
		return os;
	}
	/// @brief Prints "[MSG]" in intense_green
	inline std::ostream& msg(std::ostream& os)
	{
		os << TermMessage("[MSG]", color::intense_green);
		return os;
	}
	/// @brief Prints "[LOG]" in light_gray
	inline std::ostream& log(std::ostream& os)
	{
		os << TermMessage("[LOG]", color::light_gray);
		return os;
	}
	/// @brief 
	inline std::ostream& debug(std::ostream& os)
	{
		os << TermMessage("[DEBUG]", color::intense_magenta);
		return os;
	}
}
#endif
#endif

namespace sys::term {

	inline std::istream& EnableANSI(std::istream& is)
	{
	#ifdef OS_WIN
		ConsoleModeSetter.VirtualSequences(TargetHandle::STDIN, true);
	#endif
		return is;
	}
	inline std::istream& DisableANSI(std::istream& is)
	{
	#ifdef OS_WIN
		ConsoleModeSetter.VirtualSequences(TargetHandle::STDIN, false);
	#endif
		return is;
	}
	inline std::ostream& EnableANSI(std::ostream& os)
	{
	#ifdef OS_WIN
		ConsoleModeSetter.VirtualSequences(
			TargetHandle::STDOUT,
			true
		);
	#endif
		return os;
	}
	inline std::ostream& DisableANSI(std::ostream& os)
	{
	#ifdef OS_WIN
		ConsoleModeSetter.VirtualSequences(
			TargetHandle::STDOUT,
			false
		);
	#endif
		return os;
	}
	inline bool EnableANSI(const TargetHandle& target = TargetHandle::STDIN | TargetHandle::STDOUT)
	{
	#ifdef OS_WIN
		return ConsoleModeSetter.VirtualSequences(
			TargetHandle::STDIN | TargetHandle::STDOUT,
			true
		);
	#else
		return true;
	#endif
	}
	inline bool DisableANSI(const TargetHandle& target = TargetHandle::STDIN | TargetHandle::STDOUT)
	{
	#ifdef OS_WIN
		return ConsoleModeSetter.VirtualSequences(
			TargetHandle::STDIN | TargetHandle::STDOUT,
			false
		);
	#else
		return true;
	#endif
	}
}
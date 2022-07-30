/**
 * @file	FormatFlag.hpp
 * @author	radj307
 * @brief	Contains the FormatFlag struct, a pseudo-enum used to pass formatting information to the setcolor functor.
 */
#pragma once
#include <sysarch.h>
#include <Segments.h>
#include <Sequence.hpp>
#include <var.hpp>

#include <utility>
namespace color {
	using uchar = unsigned char;
	/**
	 * @enum	FormatFlag
	 * @brief	Defines types of formatting that can be controlled using ANSI escape sequences.
	 *\n		Both PascalCase & UPPERCASE enum names are included for backwards compatibility.
	 */
	enum class FormatFlag : uchar {
		/// @brief	No format flags; nothing will be printed.
		None = 0,
		/// @brief	[LEGACY] No format flags; nothing will be printed.
		NONE = None,
		/// @brief	Resets other format flags when combined with them using bitor (|).
		Reset = 1,
		/// @brief	[LEGACY] Resets other format flags when combined with them using bitor (|).
		RESET = Reset,
		/// @brief	Bold Format; makes text appear bold or brighter depending on your terminal emulator.
		Bold = 2,
		/// @brief	[LEGACY] Bold Format; makes text appear bold or brighter depending on your terminal emulator.
		BOLD = Bold,
		/// @brief	Underline Format; makes text & some whitespace appear to be underlined.
		Underline = 4,
		/// @brief	[LEGACY] Underline Format; makes text & some whitespace appear to be underlined.
		UNDERLINE = Underline,
		/// @brief	Invert Format; swaps the foreground & background colors.
		Invert = 8,
		/// @brief	[LEGACY] Invert Format; swaps the foreground & background colors.
		INVERT = Invert,


		/// @brief	
		NoBold = Reset | Bold,
		NO_BOLD = NoBold,
		NoUnderline = Reset | Underline,
		NO_UNDERLINE = NoUnderline,
		NoInvert = Reset | Invert,
		NO_INVERT = NoInvert,
	};
	using FORMAT_FLAG = FormatFlag;

	inline FormatFlag operator&(FormatFlag const& l, FormatFlag const& r) { return static_cast<FormatFlag>((static_cast<uchar>(l) & static_cast<uchar>(r))); }
	inline FormatFlag operator|(FormatFlag const& l, FormatFlag const& r) { return static_cast<FormatFlag>((static_cast<uchar>(l) | static_cast<uchar>(r))); }
	inline FormatFlag operator^(FormatFlag const& l, FormatFlag const& r) { return static_cast<FormatFlag>((static_cast<uchar>(l) ^ static_cast<uchar>(r))); }
	
	inline FormatFlag& operator&=(FormatFlag& l, FormatFlag const& r) { return l = static_cast<FormatFlag>((static_cast<uchar>(l) & static_cast<uchar>(r))); }
	inline FormatFlag& operator|=(FormatFlag& l, FormatFlag const& r) { return l = static_cast<FormatFlag>((static_cast<uchar>(l) | static_cast<uchar>(r))); }
	inline FormatFlag& operator^=(FormatFlag& l, FormatFlag const& r) { return l = static_cast<FormatFlag>((static_cast<uchar>(l) ^ static_cast<uchar>(r))); }

	template<std::integral T> inline bool operator==(FormatFlag const& l, T const& r) { return static_cast<T>(l) == static_cast<T>(r); }
	template<std::integral T> inline bool operator!=(FormatFlag const& l, T const& r) { return static_cast<T>(l) != static_cast<T>(r); }

	template<var::valid_char TChar, typename TCharTraits = std::char_traits<TChar>, typename TAlloc = std::allocator<TChar>>
	std::basic_ostream<TChar, TCharTraits>& operator<<(std::basic_ostream<TChar, TCharTraits>& os, const FormatFlag& format)
	{
		if (format != FormatFlag::None) {
			if ((format & FormatFlag::Reset) != 0) {
				// UNSET
				if ((format & FormatFlag::Bold) != 0)
					os << ANSI::make_sequence<TChar, TCharTraits, TAlloc>(ANSI::CSI, ANSI::SGR_NO_BOLD, ANSI::END);
				if ((format & FormatFlag::Underline) != 0)
					os << ANSI::make_sequence<TChar, TCharTraits, TAlloc>(ANSI::CSI, ANSI::SGR_NO_UNDERLINE, ANSI::END);
				if ((format & FormatFlag::Invert) != 0)
					os << ANSI::make_sequence<TChar, TCharTraits, TAlloc>(ANSI::CSI, ANSI::SGR_POSITIVE, ANSI::END);
			}
			else {
				// SET
				if ((format & FormatFlag::Bold) != 0)
					os << ANSI::make_sequence<TChar, TCharTraits, TAlloc>(ANSI::CSI, ANSI::SGR_BOLD, ANSI::END);
				if ((format & FormatFlag::Underline) != 0)
					os << ANSI::make_sequence<TChar, TCharTraits, TAlloc>(ANSI::CSI, ANSI::SGR_UNDERLINE, ANSI::END);
				if ((format & FormatFlag::Invert) != 0)
					os << ANSI::make_sequence<TChar, TCharTraits, TAlloc>(ANSI::CSI, ANSI::SGR_NEGATIVE, ANSI::END);
			}
		}
		return os;
	}
	// SGR formatting sequences
	inline static const ANSI::sequence
		bold{ ANSI::make_sequence(FormatFlag::Bold) },
		no_bold{ ANSI::make_sequence(FormatFlag::NoBold) },
		underline{ ANSI::make_sequence(FormatFlag::Underline) },
		no_underline{ ANSI::make_sequence(FormatFlag::NoUnderline) },
		invert{ ANSI::make_sequence(FormatFlag::Invert) },
		no_invert{ ANSI::make_sequence(FormatFlag::NoInvert) },
		reset_f{ ANSI::make_sequence(ANSI::CSI, ANSI::SGR_DEFAULT_FORE, ANSI::END) },
		reset_b{ ANSI::make_sequence(ANSI::CSI, ANSI::SGR_DEFAULT_BACK, ANSI::END) },
		reset_fmt{ ANSI::make_sequence(no_bold, no_underline, no_invert) },
		reset{ ANSI::make_sequence(reset_f, reset_b) },
		reset_all{ ANSI::make_sequence(ANSI::CSI, ANSI::SGR_RESET, ANSI::END) };

	inline static const ANSI::wsequence
		wbold{ ANSI::make_sequence<wchar_t>(FormatFlag::Bold) },
		wno_bold{ ANSI::make_sequence<wchar_t>(FormatFlag::NoBold) },
		wunderline{ ANSI::make_sequence<wchar_t>(FormatFlag::Underline) },
		wno_underline{ ANSI::make_sequence<wchar_t>(FormatFlag::NoUnderline) },
		winvert{ ANSI::make_sequence<wchar_t>(FormatFlag::Invert) },
		wno_invert{ ANSI::make_sequence<wchar_t>(FormatFlag::NoInvert) },
		wreset_f{ ANSI::make_sequence<wchar_t>(ANSI::CSI, ANSI::SGR_DEFAULT_FORE, ANSI::END) },
		wreset_b{ ANSI::make_sequence<wchar_t>(ANSI::CSI, ANSI::SGR_DEFAULT_BACK, ANSI::END) },
		wreset_fmt{ ANSI::make_sequence<wchar_t>(wno_bold, wno_underline, wno_invert) },
		wreset{ ANSI::make_sequence<wchar_t>(wreset_f, wreset_b, wreset_fmt) },
		wreset_all{ ANSI::make_sequence<wchar_t>(ANSI::CSI, ANSI::SGR_RESET, ANSI::END) };
}
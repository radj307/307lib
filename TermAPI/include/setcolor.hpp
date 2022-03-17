#pragma once
#include <sysarch.h>
#include <color-format.hpp>
#include <color-values.h>
#include <Sequence.hpp>
#include <Segments.h>
#include <color-transform.hpp>
#ifdef OS_WIN
#define SETCOLOR_NO_RGB
#endif

namespace color {
	enum class Layer : bool {
		F = false,
		B = true,
	};
}

namespace term {
	using color::Layer;
	using color::format;
	using color::FormatFlag;

	/**
	 * @brief		Acts as a wrapper and controller for SGR & RGB color codes, as well as certain SGR format codes.
	 *\n			Uses the ostream operator<< to insert escape sequences into streams.
	 * @tparam SeqT	The type of string to use for escape sequences.
	 */
	template<var::any_same<ANSI::Sequence, ANSI::wSequence> SeqT = ANSI::Sequence>
	struct setcolor_seq {
	protected:
		SeqT _seq;
		format::MutableFlag _fmt;
		/**
		 * @brief		Build a color escape sequence from an SGR (color) code.
		 *\n			If using Windows or if SETCOLOR_NO_RGB is defined, this function is automatically always used instead.
		 * @param lyr	Target Layer. (Foreground/Background)
		 * @param SGR	An SGR color code value.
		 * @returns		SeqT
		 */
		virtual SeqT makeColorSequence(const Layer& lyr, const short& SGR) const
		{
			using namespace ANSI;
			return make_sequence<SeqT>(CSI, 3 + !!static_cast<bool>(lyr), "8;5;", SGR, END);
		}
		/**
		 * @brief		Build an RGB color escape sequence, or if using Windows / SETCOLOR_NO_RGB is defined, it is first converted to SGR sequences.
		 * @param lyr	Target Layer. (Foreground/Background)
		 * @param r		Red color axis
		 * @param g		Green color axis
		 * @param b		Blue color axis
		 * @returns		SeqT
		 */
		virtual SeqT makeColorSequence(const Layer& lyr, const short& r, const short& g, const short& b) const
		{
			using namespace ANSI;
			#ifdef SETCOLOR_NO_RGB
			return make_sequence<SeqT>(CSI, 3 + !!static_cast<bool>(lyr), "8;5;", color::rgb_to_sgr(r, g, b), END);
			#else
			return make_sequence<SeqT>(CSI, 3 + !!static_cast<bool>(lyr), "8;2;", r, ';', g, ';', b, END);
			#endif
		}
		/**
		 * @brief			Build a color escape sequence using a given RGB tuple. This function calls the overloaded RGB function.
		 * @param lyr		Target Layer. (Foreground/Background)
		 * @param rgb_color	An RGB color code as a tuple. Values can range from 0 to 255.
		 * @returns			SeqT
		 */
		virtual SeqT makeColorSequence(const Layer& lyr, const std::tuple<short, short, short>& rgb_color) const
		{
			return makeColorSequence(lyr, std::get<0>(rgb_color), std::get<1>(rgb_color), std::get<2>(rgb_color));
		}

	public:
		setcolor_seq(const SeqT& seq, const FormatFlag& format = format::NONE) : _seq{ seq }, _fmt{ format } {}
		setcolor_seq(const short& sgr_color, const FormatFlag& format = format::NONE, const Layer& layer = Layer::F) : _seq{ makeColorSequence(layer, sgr_color) }, _fmt{ format } {}
		setcolor_seq(const short& r, const short& g, const short& b, const FormatFlag& format = format::NONE, const Layer& layer = Layer::F) : _seq{ makeColorSequence(layer, r, g, b) }, _fmt{ format } {}
		setcolor_seq(const std::tuple<short, short, short>& rgb_color, const FormatFlag& format = format::NONE, const Layer& layer = Layer::F) : _seq{ makeColorSequence(layer, rgb_color) }, _fmt{ format } {}

		setcolor_seq(const short& sgr_color, const Layer& layer, const FormatFlag& format = format::NONE) : _seq{ makeColorSequence(layer, sgr_color) }, _fmt{ format } {}
		setcolor_seq(const short& r, const short& g, const short& b, const Layer& layer, const FormatFlag& format = format::NONE) : _seq{ makeColorSequence(layer, r, g, b) }, _fmt{ format } {}
		setcolor_seq(const std::tuple<short, short, short>& rgb_color, const Layer& layer, const FormatFlag& format = format::NONE) : _seq{ makeColorSequence(layer, rgb_color) }, _fmt{ format } {}

		/**
		 * @brief			Retrieve the sequence associated with this setcolor instance, and optionally include format sequences.
		 * @param use_fmt	When true, the returned sequence includes SGR format sequences, if one is set in this instance's format flags.
		 * @returns			SeqT
		 */
		SeqT as_sequence(const bool& use_fmt = false) const
		{
			if (use_fmt) {
				if constexpr (std::same_as<SeqT, ANSI::Sequence>)
					return ANSI::make_sequence<ANSI::Sequence>(_seq, _fmt.Sequence());
				else if constexpr (std::same_as<SeqT, ANSI::wSequence>)
					return ANSI::make_sequence<ANSI::wSequence>(_seq, _fmt.wSequence());
			}
			return _seq;
		}
		/**
		 * @brief	Returns the result of calling as_sequence(true)
		 * @returns	SeqT
		 */
		SeqT as_sequence_with_format() const { return as_sequence(true); }



		/**
		 * @brief	Calls as_sequence(true)
		 * @returns	SeqT
		 */
		operator SeqT() const { return as_sequence(true); }
		/**
		 * @brief	Returns true if the sequence is not empty.
		 * @returns	bool
		 */
		operator bool() const { return !_seq.empty() && _fmt == format::NONE; }

		/**
		 * @brief	Equality comparison operator between two setcolor instances.
		 * @param o	Another setcolor instance. Both the sequence & format flags are checked.
		 * @returns	bool
		 */
		bool operator==(const setcolor_seq<SeqT>& o) const { return _seq == o._seq && _fmt == o._fmt; }
		/**
		 * @brief	Inequality comparison operator between two setcolor instances. This forwards the argument to operator==, and inverts the result.
		 * @param o	Perfectly-forwarded type that has a valid equality comparison overload.
		 * @returns	bool
		 */
		bool operator!=(auto&& o) const { return !operator==(std::forward<decltype(o)>(o)); }

		/**
		 * @brief	Combine the escape sequences of two setcolor instances, as well as their format flags.
		 * @param o	Another setcolor instance.
		 * @returns	setcolor_seq<SeqT>
		 */
		setcolor_seq<SeqT> operator+(const setcolor_seq<SeqT>& o) const
		{
			return setcolor_seq<SeqT>{ _seq + o._seq, static_cast<unsigned char>(_fmt | o._fmt) };
		}
		/**
		 * @brief	Append another setcolor instance's sequence onto this one.
		 * @param o	Another setcolor instance.
		 * @returns	setcolor_seq<SeqT>&
		 */
		setcolor_seq<SeqT>& operator+=(const setcolor_seq<SeqT>& o) const
		{
			_seq += o._seq;
			_fmt |= o._fmt;
			return *this;
		}
		/**
		 * @brief		Insert this escape sequence into
		 * @param os
		 * @param color
		 * @returns		std::ostream&
		 */
		friend std::ostream& operator<<(std::ostream& os, const setcolor_seq<SeqT>& color)
		{
			return os << color.as_sequence(true);
		}

		/// Declare static constant colors for the basic 8-bit color palette.
		static const setcolor_seq<SeqT> red, green, blue, yellow, magenta, cyan, black, white;
		/// Declare static constant colors for the basic color utility sequences
		static const setcolor_seq<SeqT> reset, reset_f, reset_b, reset_fmt, placeholder;
		/// Declare static constant colors for the basic 16-bit color palette
		static const setcolor_seq<SeqT> intense_red, intense_green, intense_blue, intense_yellow, intense_magenta, intense_cyan;
	};

	/// @brief	Sets the foreground or background color to the specified color. It can also set formatting flags like bold, underline, & invert.
	using setcolor = setcolor_seq<ANSI::Sequence>;
	/// @brief	A setcolor instance that does nothing, for use with ternary expressions.
	static const setcolor setcolor_placeholder{ ANSI::Sequence{}, format::NONE };

	/// Define static constant colors for the basic 8-bit color palette.
	template<> inline const setcolor setcolor::red{ color::red };
	template<> inline const setcolor setcolor::green{ color::green };
	template<> inline const setcolor setcolor::blue{ color::blue };
	template<> inline const setcolor setcolor::yellow{ color::yellow };
	template<> inline const setcolor setcolor::magenta{ color::magenta };
	template<> inline const setcolor setcolor::cyan{ color::cyan };
	template<> inline const setcolor setcolor::black{ color::black };
	template<> inline const setcolor setcolor::white{ color::white };

	template<> inline const setcolor setcolor::reset{ color::reset };
	template<> inline const setcolor setcolor::reset_f{ color::reset_f };
	template<> inline const setcolor setcolor::reset_b{ color::reset_b };
	template<> inline const setcolor setcolor::reset_fmt{ color::reset_fmt };
	template<> inline const setcolor setcolor::placeholder{ ANSI::Sequence() };

	template<> inline const setcolor setcolor::intense_red{ color::intense_red };
	template<> inline const setcolor setcolor::intense_green{ color::intense_green };
	template<> inline const setcolor setcolor::intense_blue{ color::intense_blue };
	template<> inline const setcolor setcolor::intense_yellow{ color::intense_yellow };
	template<> inline const setcolor setcolor::intense_magenta{ color::intense_magenta };
	template<> inline const setcolor setcolor::intense_cyan{ color::intense_cyan };
	//setcolor::white{ color::white }, setcolor::reset{ color::reset }, setcolor::reset_f{ color::reset_f }, setcolor::reset_b{ color::reset_b };

	/// @brief	Sets the foreground or background color to the specified color, for use with wchar_t types. It can also set formatting flags like bold, underline, & invert.
	using wsetcolor = setcolor_seq<ANSI::wSequence>;
	/// @brief	A setcolor instance that does nothing, for use with ternary expressions.
	static const wsetcolor wsetcolor_placeholder{ ANSI::wSequence{}, format::NONE };

	/// Define static constant colors for the basic 8-bit color palette.
	template<> inline const wsetcolor wsetcolor::red{ color::red };
	template<> inline const wsetcolor wsetcolor::green{ color::green };
	template<> inline const wsetcolor wsetcolor::blue{ color::blue };
	template<> inline const wsetcolor wsetcolor::yellow{ color::yellow };
	template<> inline const wsetcolor wsetcolor::magenta{ color::magenta };
	template<> inline const wsetcolor wsetcolor::cyan{ color::cyan };
	template<> inline const wsetcolor wsetcolor::black{ color::black };
	template<> inline const wsetcolor wsetcolor::white{ color::white };

	template<> inline const wsetcolor wsetcolor::reset{ color::wreset };
	template<> inline const wsetcolor wsetcolor::reset_f{ color::wreset_f };
	template<> inline const wsetcolor wsetcolor::reset_b{ color::wreset_b };
	template<> inline const wsetcolor wsetcolor::reset_fmt{ color::wreset_fmt };
	template<> inline const wsetcolor wsetcolor::placeholder{ ANSI::wSequence() };

	template<> inline const wsetcolor wsetcolor::intense_red{ color::intense_red };
	template<> inline const wsetcolor wsetcolor::intense_green{ color::intense_green };
	template<> inline const wsetcolor wsetcolor::intense_blue{ color::intense_blue };
	template<> inline const wsetcolor wsetcolor::intense_yellow{ color::intense_yellow };
	template<> inline const wsetcolor wsetcolor::intense_magenta{ color::intense_magenta };
	template<> inline const wsetcolor wsetcolor::intense_cyan{ color::intense_cyan };
}

namespace color {
	using term::setcolor_seq;
	using term::setcolor;
	using term::wsetcolor;
}
#pragma once
#include <sysarch.h>
#include <color-format.hpp>
#include <color-values.h>
#include <Sequence.hpp>
#include <Segments.h>
#ifdef OS_WIN
#define SETCOLOR_NO_RGB
#endif
#ifdef SETCOLOR_NO_RGB
#include <color-transform.hpp>
#endif

namespace color {
	enum class Layer : bool {
		F = false,
		B = true,
	};

	/**
	 * @brief		Acts as a wrapper and controller for SGR & RGB color codes, as well as certain SGR format codes.
	 *\n			Uses the ostream operator<< to insert escape sequences into streams.
	 * @tparam SeqT	The type of string to use for escape sequences.
	 */
	template<typename SeqT = ANSI::Sequence> requires std::same_as<SeqT, ANSI::Sequence> || std::same_as<SeqT, ANSI::wSequence>
	struct setcolor_seq {
	protected:
		SeqT _seq;
		ColorFormat _fmt;
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
			return make_sequence<SeqT>(CSI, 3 + !!static_cast<bool>(lyr), "8;5;", rgb_to_sgr(r, g, b), END);
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
		setcolor_seq(const SeqT& seq, const FormatFlag& format = NONE) : _seq{ seq }, _fmt{ format } {}
		setcolor_seq(const short& sgr_color, const FormatFlag& format = NONE, const Layer& layer = Layer::F) : _seq{ makeColorSequence(layer, sgr_color) }, _fmt{ format } {}
		setcolor_seq(const short& r, const short& g, const short& b, const FormatFlag& format = NONE, const Layer& layer = Layer::F) : _seq{ makeColorSequence(layer, r, g, b) }, _fmt{ format } {}
		setcolor_seq(const std::tuple<short, short, short>& rgb_color, const FormatFlag& format = NONE, const Layer& layer = Layer::F) : _seq{ makeColorSequence(layer, rgb_color) }, _fmt{ format } {}

		setcolor_seq(const short& sgr_color, const Layer& layer, const FormatFlag& format = NONE) : _seq{ makeColorSequence(layer, sgr_color) }, _fmt{ format } {}
		setcolor_seq(const short& r, const short& g, const short& b, const Layer& layer, const FormatFlag& format = NONE) : _seq{ makeColorSequence(layer, r, g, b) }, _fmt{ format } {}
		setcolor_seq(const std::tuple<short, short, short>& rgb_color, const Layer& layer, const FormatFlag& format = NONE) : _seq{ makeColorSequence(layer, rgb_color) }, _fmt{ format } {}

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
		operator bool() const { return !_seq.empty() && _fmt == FormatFlag::NONE; }

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
	};

	/// @brief	Sets the foreground or background color to the specified color. It can also set formatting flags like bold, underline, & invert.
	using setcolor = setcolor_seq<ANSI::Sequence>;
	/// @brief	A setcolor instance that does nothing, for use with ternary expressions.
	static const setcolor setcolor_placeholder{ ANSI::Sequence{}, NONE };

	/// Define static constant colors for the basic 8-bit color palette.
	inline const setcolor setcolor::red{ color::red }, setcolor::green{ color::green }, setcolor::blue{ color::blue }, setcolor::yellow{ color::yellow }, setcolor::magenta{ color::magenta }, setcolor::cyan{ color::cyan }, setcolor::black{ color::black }, setcolor::white{ color::white };


	/// @brief	Sets the foreground or background color to the specified color, for use with wchar_t types. It can also set formatting flags like bold, underline, & invert.
	using wsetcolor = setcolor_seq<ANSI::wSequence>;
	/// @brief	A setcolor instance that does nothing, for use with ternary expressions.
	static const wsetcolor wsetcolor_placeholder{ ANSI::wSequence{}, NONE };

	/// Define static constant colors for the basic 8-bit color palette.
	inline const wsetcolor wsetcolor::red{ color::red }, wsetcolor::green{ color::green }, wsetcolor::blue{ color::blue }, wsetcolor::yellow{ color::yellow }, wsetcolor::magenta{ color::magenta }, wsetcolor::cyan{ color::cyan }, wsetcolor::black{ color::black }, wsetcolor::white{ color::white };
}
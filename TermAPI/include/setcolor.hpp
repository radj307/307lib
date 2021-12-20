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

	template<typename SeqT = ANSI::Sequence> requires std::same_as<SeqT, ANSI::Sequence> || std::same_as<SeqT, ANSI::wSequence>
	struct setcolor_seq {
	protected:
		SeqT _seq;
		ColorFormat _fmt;

		virtual SeqT makeColorSequence(const Layer& lyr, const short& SGR) const
		{
			using namespace ANSI;
			return make_sequence<SeqT>(CSI, 3 + !!static_cast<bool>(lyr), "8;5;", SGR, END);
		}

		virtual SeqT makeColorSequence(const Layer& lyr, const short& r, const short& g, const short& b) const
		{
			using namespace ANSI;
			#ifdef SETCOLOR_NO_RGB
			return make_sequence<SeqT>(CSI, 3 + !!static_cast<bool>(lyr), "8;5;", rgb_to_sgr(r, g, b), END);
			#else
			return make_sequence<SeqT>(CSI, 3 + !!static_cast<bool>(lyr), "8;2;", r, ';', g, ';', b, END);
			#endif
		}
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

		SeqT as_sequence(const bool& include_format_sequences = false) const
		{
			if (include_format_sequences) {
				if constexpr (std::same_as<SeqT, ANSI::Sequence>)
					return ANSI::make_sequence<ANSI::Sequence>(_seq, _fmt.Sequence());
				else if constexpr (std::same_as<SeqT, ANSI::wSequence>)
					return ANSI::make_sequence<ANSI::wSequence>(_seq, _fmt.wSequence());
			}
			return _seq;
		}
		SeqT as_sequence_with_format() const { return as_sequence(true); }

		operator std::string() const { return as_sequence(true); }
		operator bool() const { return !_seq.empty(); }

		bool operator==(const setcolor_seq<SeqT>& o) const { return _seq == o._seq && _fmt == o._fmt; }
		bool operator!=(auto&& o) const { return !operator==(std::forward<decltype(o)>(o)); }

		setcolor_seq<SeqT> operator+(const setcolor_seq<SeqT>& o) const
		{
			return setcolor_seq<SeqT>{ _seq + o._seq, static_cast<unsigned char>(_fmt | o._fmt) };
		}
		setcolor_seq<SeqT>& operator+=(const setcolor_seq<SeqT>& o) const
		{
			_seq += o._seq;
			_fmt |= o._fmt;
			return *this;
		}

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
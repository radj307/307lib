#pragma once
#include <sysarch.h>
#include <color-format.hpp>
#include <color-values.h>
#include <Sequence.hpp>
#include <Segments.h>
#include <color-transform.hpp>

/**
 * @def		SETCOLOR_NO_RGB
 * @brief	Disables the direct-RGB ANSI escape sequence for operating systems that don't support it, such as Windows.
 *\n		Enabled by default on Windows, disabled by default on all other platforms.
 */
#define SETCOLOR_NO_RGB
#undef SETCOLOR_NO_RGB

#ifdef OS_WIN
#define SETCOLOR_NO_RGB

#include <EnableANSI.hpp>
#endif

namespace color {
	enum class Layer {
		B = 0,
		F = 1,
		Back = B,
		Fore = F,
		Background = Back,
		Foreground = Fore,
	};
	template<var::valid_char TChar, typename TCharTraits = std::char_traits<TChar>>
	inline std::basic_ostream<TChar, TCharTraits>& operator<<(std::basic_ostream<TChar, TCharTraits>& os, const Layer& l)
	{
		return os << (l == Layer::F ? 38 : 48);
	}

	/**
	 * @brief		Acts as a wrapper and controller for SGR & RGB color codes, as well as certain SGR format codes.
	 *\n			Uses the ostream operator<< to insert escape sequences into streams.
	 * @tparam seq_t	The type of string to use for escape sequences.
	 */
	template<var::valid_char TChar = char, typename TCharTraits = std::char_traits<TChar>, typename TAlloc = std::allocator<TChar>>
	struct setcolor_seq {
		using seq_t = std::basic_string<TChar, TCharTraits, TAlloc>;
	protected:
		seq_t _seq;
		/**
		 * @brief		Build a color escape sequence from an SGR (color) code.
		 *\n			If using Windows or if SETCOLOR_NO_RGB is defined, this function is automatically always used instead.
		 * @param lyr	Target Layer. (Foreground/Background)
		 * @param SGR	An SGR color code value.
		 * @returns		seq_t
		 */
		virtual CONSTEXPR seq_t makeColorSequence(const Layer& lyr, const short& SGR) const
		{
			return ANSI::make_sequence<TChar, TCharTraits>(ANSI::CSI, lyr, ";5;", SGR, ANSI::END);
		}
		/**
		 * @brief		Build an RGB color escape sequence, or if using Windows / SETCOLOR_NO_RGB is defined, it is first converted to SGR sequences.
		 * @param lyr	Target Layer. (Foreground/Background)
		 * @param r		Red color axis
		 * @param g		Green color axis
		 * @param b		Blue color axis
		 * @returns		seq_t
		 */
		virtual CONSTEXPR seq_t makeColorSequence(const Layer& lyr, const short& r, const short& g, const short& b) const
		{
			using namespace ANSI;
		#			ifdef SETCOLOR_NO_RGB
			return make_sequence<TChar, TCharTraits>(CSI, lyr, ";5;", color::rgb_to_sgr(r, g, b), END);
		#			else
			return make_sequence<TChar, TCharTraits>(CSI, lyr, ";2;", r, ';', g, ';', b, END);
		#			endif
		}
		/**
		 * @brief			Build a color escape sequence using a given RGB tuple. This function calls the overloaded RGB function.
		 * @param lyr		Target Layer. (Foreground/Background)
		 * @param rgb_color	An RGB color code as a tuple. Values can range from 0 to 255.
		 * @returns			seq_t
		 */
		virtual CONSTEXPR seq_t makeColorSequence(const Layer& lyr, const std::tuple<short, short, short>& rgb_color) const
		{
			return makeColorSequence(lyr, std::get<0>(rgb_color), std::get<1>(rgb_color), std::get<2>(rgb_color));
		}

	public:
		CONSTEXPR setcolor_seq(const seq_t& sequence = {}) : _seq{ sequence } {}
		CONSTEXPR setcolor_seq(const short& sgr_color, const Layer& layer = Layer::F) : _seq{ makeColorSequence(layer, sgr_color) } {}
		CONSTEXPR setcolor_seq(const short& r, const short& g, const short& b, const Layer& layer = Layer::F) : _seq{ makeColorSequence(layer, r, g, b) } {}
		CONSTEXPR setcolor_seq(const std::tuple<short, short, short>& rgb_color, const Layer& layer = Layer::F) : _seq{ makeColorSequence(layer, rgb_color) } {}

		//template<var::streamable<std::basic_stringstream<TChar, TCharTraits, TAlloc>>... Ts>
		//WINCONSTEXPR setcolor_seq(Ts&&... sequence_components) : _seq{ ANSI::make_sequence<TChar, TCharTraits, TAlloc>(std::forward<Ts>(sequence_components)...) } {}

		/**
		 * @brief			Retrieve the sequence associated with this setcolor instance, and optionally include format sequences.
		 * @returns			seq_t
		 */
		CONSTEXPR seq_t as_sequence() const
		{
			return _seq;
		}

		/**
		 * @brief	Calls as_sequence(true)
		 * @returns	seq_t
		 */
		CONSTEXPR operator seq_t() const { return as_sequence(); }

		/**
		 * @brief	Equality comparison operator between two setcolor instances.
		 * @param o	Another setcolor instance. Both the sequence & format flags are checked.
		 * @returns	bool
		 */
		CONSTEXPR bool operator==(const setcolor_seq<TChar, TCharTraits, TAlloc>& o) const { return _seq == o._seq; }
		/**
		 * @brief	Inequality comparison operator between two setcolor instances. This forwards the argument to operator==, and inverts the result.
		 * @param o	Perfectly-forwarded type that has a valid equality comparison overload.
		 * @returns	bool
		 */
		CONSTEXPR bool operator!=(auto&& o) const { return !operator==(std::forward<decltype(o)>(o)); }

		/**
		 * @brief	Combine the escape sequences of two setcolor instances, as well as their format flags.
		 * @param o	Another setcolor instance.
		 * @returns	setcolor_seq<TChar, TCharTraits, TAlloc>
		 */
		CONSTEXPR setcolor_seq<TChar, TCharTraits, TAlloc> operator+(const setcolor_seq<TChar, TCharTraits, TAlloc>& o) const
		{
			return setcolor_seq<TChar, TCharTraits, TAlloc>{ _seq + o._seq };
		}
		/**
		 * @brief	Append another setcolor instance's sequence onto this one.
		 * @param o	Another setcolor instance.
		 * @returns	setcolor_seq<TChar, TCharTraits, TAlloc>&
		 */
		CONSTEXPR setcolor_seq<TChar, TCharTraits, TAlloc>& operator+=(const setcolor_seq<TChar, TCharTraits, TAlloc>& o)
		{
			_seq += o._seq;
			return *this;
		}
		/**
		 * @brief	Returns a new setcolor_seq instance where another sequence is appended onto this instance's sequence.
		 * @param o	Another sequence instance.
		 * @returns	setcolor_seq<TChar, TCharTraits, TAlloc>&
		 */
		CONSTEXPR setcolor_seq<TChar, TCharTraits, TAlloc> operator+(const ANSI::basic_sequence<TChar, TCharTraits, TAlloc>& o) const
		{
			return setcolor_seq<TChar, TCharTraits, TAlloc>{ _seq + o };
		}
		/**
		 * @brief	Append another sequence onto this one.
		 * @param o	Another sequence instance.
		 * @returns	setcolor_seq<TChar, TCharTraits, TAlloc>&
		 */
		CONSTEXPR setcolor_seq<TChar, TCharTraits, TAlloc>& operator+=(const ANSI::basic_sequence<TChar, TCharTraits, TAlloc>& o)
		{
			_seq += o;
			return *this;
		}
		/**
		 * @brief		Returns a new setcolor_seq instance with the given variable appended to this instance's sequence.
		 * @param o		Any object with a valid operator<<
		 * @returns		setcolor_seq<TChar, TCharTraits, TAlloc>
		 */
		template<var::streamable<std::basic_stringstream<TChar, TCharTraits, TAlloc>> T>
		CONSTEXPR setcolor_seq<TChar, TCharTraits, TAlloc> operator+(const T& o) const
		{
			std::basic_stringstream<TChar, TCharTraits, TAlloc> ss;
			ss << o;
			return setcolor_seq<TChar, TCharTraits, TAlloc>{ _seq + ss.str() };
		}
		/**
		 * @brief		Append any type with a valid operator<< to the sequence.
		 * @param o		Any object with a valid operator<<
		 * @returns		setcolor_seq<TChar, TCharTraits, TAlloc>&
		 */
		template<var::streamable<std::basic_stringstream<TChar, TCharTraits, TAlloc>> T>
		CONSTEXPR setcolor_seq<TChar, TCharTraits, TAlloc>& operator+=(const T& o)
		{
			std::basic_stringstream<TChar, TCharTraits, TAlloc> ss;
			ss << o;
			_seq += ss.str();
			return *this;
		}
		/**
		 * @brief		Insert this escape sequence into an output stream.
		 *\n			When compiled for Windows, this method will automatically enable ANSI sequences in the virtual terminal for STDOUT & STDERR. To disable this behaviour, define 'SETCOLOR_NO_AUTOINIT' before including this header.
		 * @returns		std::basic_ostream<TChar, TCharTraits>&
		 */
		friend std::basic_ostream<TChar, TCharTraits>& operator<<(std::basic_ostream<TChar, TCharTraits>& os, const setcolor_seq<TChar, TCharTraits, TAlloc>& color)
		{
		#if defined(OS_WIN) && !defined(SETCOLOR_NO_AUTOINIT)
			return os << term::EnableANSI << color.as_sequence();
		#else
			return os << color.as_sequence();
		#endif
		}

		/**
		 * @brief		Extract characters from an input stream into the sequence.
		 *\n			When compiled for Windows, this method will automatically enable ANSI sequences in the virtual terminal for STDIN. To disable this behaviour, define 'SETCOLOR_NO_AUTOINIT' before including this header.
		 * @param is 
		 * @param s 
		 * @returns		std::basic_istream<TChar, TCharTraits>&
		 */
		friend std::basic_istream<TChar, TCharTraits>& operator>>(std::basic_istream<TChar, TCharTraits>& is, setcolor_seq<TChar, TCharTraits, TAlloc>& s)
		{
		#if defined(OS_WIN) && !defined(SETCOLOR_NO_AUTOINIT)
			return is >> term::EnableANSI >> s._seq;
		#else
			return is >> s._seq;
		#endif
		}

		/// Declare static constant colors for the basic 8-bit color palette.
		static const setcolor_seq<TChar, TCharTraits, TAlloc> red, green, blue, yellow, magenta, cyan, black, white;
		/// Declare static constant colors for the basic color utility sequences
		static const setcolor_seq<TChar, TCharTraits, TAlloc> reset, reset_f, reset_b, reset_fmt, placeholder;
		/// Declare static constant colors for the basic 16-bit color palette
		static const setcolor_seq<TChar, TCharTraits, TAlloc> intense_red, intense_green, intense_blue, intense_yellow, intense_magenta, intense_cyan;
	};

	/// @brief	Sets the foreground or background color to the specified color. It can also set formatting flags like bold, underline, & invert.
	using setcolor = setcolor_seq<char>;
	/// @brief	A setcolor instance that does nothing, for use with ternary expressions.
	static const setcolor setcolor_placeholder{};

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
	template<> inline const setcolor setcolor::placeholder{};

	template<> inline const setcolor setcolor::intense_red{ color::intense_red };
	template<> inline const setcolor setcolor::intense_green{ color::intense_green };
	template<> inline const setcolor setcolor::intense_blue{ color::intense_blue };
	template<> inline const setcolor setcolor::intense_yellow{ color::intense_yellow };
	template<> inline const setcolor setcolor::intense_magenta{ color::intense_magenta };
	template<> inline const setcolor setcolor::intense_cyan{ color::intense_cyan };
	//setcolor::white{ color::white }, setcolor::reset{ color::reset }, setcolor::reset_f{ color::reset_f }, setcolor::reset_b{ color::reset_b };

	/// @brief	Sets the foreground or background color to the specified color, for use with wchar_t types. It can also set formatting flags like bold, underline, & invert.
	using wsetcolor = setcolor_seq<wchar_t>;
	/// @brief	A setcolor instance that does nothing, for use with ternary expressions.
	static const wsetcolor wsetcolor_placeholder{};

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
	template<> inline const wsetcolor wsetcolor::placeholder{};

	template<> inline const wsetcolor wsetcolor::intense_red{ color::intense_red };
	template<> inline const wsetcolor wsetcolor::intense_green{ color::intense_green };
	template<> inline const wsetcolor wsetcolor::intense_blue{ color::intense_blue };
	template<> inline const wsetcolor wsetcolor::intense_yellow{ color::intense_yellow };
	template<> inline const wsetcolor wsetcolor::intense_magenta{ color::intense_magenta };
	template<> inline const wsetcolor wsetcolor::intense_cyan{ color::intense_cyan };
}

namespace term {
	using color::Layer;
	using color::FormatFlag;
	using color::setcolor_seq;
	using color::setcolor;
	using color::wsetcolor;
}
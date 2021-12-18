/**
 * @file FormatFlag.hpp
 * @author radj307
 * @brief Contains the FormatFlag struct, a pseudo-enum used to pass formatting information to the setcolor functor.
 */
#pragma once
#include <Segments.h>
#include <Sequence.hpp>
#include <var.hpp>

#include <utility>
namespace color {
#pragma region DECLARATION
	/**
	 * @struct FormatFlag
	 * @brief Used by the ColorFormat & setcolor objects to maintain flag types. Does not have any direct comparison operators, as a FormatFlag should never contain multiple flags. (See the ColorFormat struct)
	 */
	struct FormatFlag {
	private:
		const unsigned char _format;
	public:
		/**
		 * @brief Default Constructor.
		 * @param format	- Index value. Must follow the bitwise flag format where each value is double the previous one.
		 */
		constexpr FormatFlag(unsigned char format) : _format{ std::move(format) } {}

		/** @brief Return this instance's flag value */
		constexpr operator const unsigned char() const { return _format; }

		/** @brief Bitwise AND */
		constexpr const unsigned char operator&(const unsigned char& o) const { return static_cast<unsigned char>(!!_format & !!o); }
		/** @brief Bitwise OR  */
		constexpr const unsigned char operator|(const unsigned char& o) const { return static_cast<unsigned char>(!!_format | !!o); }
		/** @brief Bitwise XOR */
		constexpr const unsigned char operator^(const unsigned char& o) const { return static_cast<unsigned char>(!!_format ^ !!o); }
	};
#pragma endregion DECLARATION
#pragma region DEFINITIONS
	/** @brief No special formatting */
	static constexpr const FormatFlag NONE{ 0u };
	/** @brief Resets colors only */
	static constexpr const FormatFlag RESET{ 1u };
	/** @brief Bold printed text. */
	static constexpr const FormatFlag BOLD{ 2u };
	/** @brief Unset the bold flag specifically. */
	static constexpr const FormatFlag NO_BOLD{ 4u };
	/** @brief Underline printed text. */
	static constexpr const FormatFlag UNDERLINE{ 8u };
	/** @brief Unset the underline flag specifically. */
	static constexpr const FormatFlag NO_UNDERLINE{ 16u };
	/** @brief Invert foreground & background colors of printed text. */
	static constexpr const FormatFlag INVERT{ 32u };
	/** @brief Unset the invert flag specifically. */
	static constexpr const FormatFlag NO_INVERT{ 64u };
#pragma endregion DEFINITIONS

	/**
	 * @brief Accepts any number of integrals, and returns the result of calling bitwise OR on each of them.
	 * @tparam ...VT		- Variadic Templated Type (Integral)
	 * @param ...numbers	- Numbers to bitwise-merge
	 * @returns unsigned char
	 */
	template<typename... VT>
	inline static constexpr unsigned char bitmerge(const VT&... numbers) noexcept
	{
		unsigned char flag{ 0u };
		for (auto& i : var::variadic_accumulate<unsigned long long>(numbers...))
			flag |= static_cast<unsigned char>(i);
		return flag;
	}

	inline static const ANSI::Sequence
		bold{ ANSI::make_sequence(ANSI::CSI, ANSI::SGR_BOLD, ANSI::END) },
		no_bold{ ANSI::make_sequence(ANSI::CSI, ANSI::SGR_NO_BOLD, ANSI::END) },
		underline{ ANSI::make_sequence(ANSI::CSI, ANSI::SGR_UNDERLINE, ANSI::END) },
		no_underline{ ANSI::make_sequence(ANSI::CSI, ANSI::SGR_NO_UNDERLINE, ANSI::END) },
		invert{ ANSI::make_sequence(ANSI::CSI, ANSI::SGR_NEGATIVE, ANSI::END) },
		no_invert{ ANSI::make_sequence(ANSI::CSI, ANSI::SGR_POSITIVE, ANSI::END) },
		reset_f{ ANSI::make_sequence(ANSI::CSI, ANSI::SGR_DEFAULT_FORE, ANSI::END) },
		reset_b{ ANSI::make_sequence(ANSI::CSI, ANSI::SGR_DEFAULT_BACK, ANSI::END) },
		reset{ ANSI::make_sequence(ANSI::CSI, ANSI::SGR_DEFAULT_FORE, ANSI::END, ANSI::CSI, ANSI::SGR_DEFAULT_BACK, ANSI::END) },
		reset_all{ ANSI::make_sequence(ANSI::CSI, ANSI::SGR_RESET, ANSI::END) };


	/**
	 * @struct ColorFormat
	 * @brief A mutable format flag instance. Used by the setcolor struct to maintain any number of independent FormatFlag instances.
	 */
	struct ColorFormat {
	private:
		unsigned char _fmt; ///< @brief Current flag value
	public:
		/**
		 * @brief Variadic bitmerge constructor.
		 * @tparam ...VT	- Variadic Templated Type (FormatFlag)
		 * @param ...flags	- At least two format flag instances that will be merged together with a bitwise OR operation.
		 */
		template<typename... VT>
		ColorFormat(const VT&... flags) : _fmt{ bitmerge(flags...) } {}

		/** @brief Retrieve the current value of this flag */
		constexpr operator unsigned char() const { return _fmt; }

		constexpr bool contains(const FormatFlag& flag) const
		{
			return (_fmt & flag) != 0;
		}

		ANSI::Sequence Sequence() const
		{
			// TODO: Fix this function, add functions for quickly sending sequences to terminal
			// ISSUE: ANSI::END characters are being dumped even when the sequence is empty
			// ISSUE: Chaining SGR operators doesn't work on windows.

			using namespace ANSI;
			ANSI::Sequence seq;

			const auto append{ [&seq](auto&& s) { if (!seq.empty()) seq += ';'; return seq += s; } };

			if (contains(RESET))
				append(reset);
			if (contains(BOLD))
				append(bold);
			if (contains(NO_BOLD))
				append(no_bold);
			if (contains(UNDERLINE))
				append(underline);
			if (contains(NO_UNDERLINE))
				append(no_underline);
			if (contains(INVERT))
				append(invert);
			if (contains(NO_INVERT))
				append(no_invert);

			return seq;
		}
		ANSI::wSequence wSequence() const
		{
			ANSI::wSequence wseq;
			for (auto& ch : Sequence())
				wseq += static_cast<wchar_t>(ch);
			return wseq;
		}
	};

}
#ifndef COLOR_NO_GLOBALS
/// @brief Allows specifying the color::FormatFlag object with a shorter syntax. Define "COLOR_NO_GLOBALS" to disable.
using Format = color::FormatFlag;
#endif
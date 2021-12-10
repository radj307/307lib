/**
 * @file FormatFlag.hpp
 * @author radj307
 * @brief Contains the FormatFlag struct, a pseudo-enum used to pass formatting information to the setcolor functor.
 */
#pragma once
#include <utility>
#include <var.hpp>
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

		// Declare enum vars
		static const FormatFlag ///< @brief When adding new entries, make sure to add an equivalent statement in color::setcolor::operator<<
			NONE,				///< @brief No formatting
			BOLD,				///< @brief Bolt text
			RESET_BOLD,			///< @brief Reset bolded text
			INVERT,
			RESET_INVERT,
			UNDERLINE,
			RESET_UNDERLINE;
	};
#pragma endregion DECLARATION
#pragma region DEFINITIONS
	/** @brief No special formatting */
	inline constexpr const FormatFlag FormatFlag::NONE{ 0u };
	/** @brief Bold printed text. */
	inline constexpr const FormatFlag FormatFlag::BOLD{ 1u };
	/** @brief Unset the bold flag specifically. */
	inline constexpr const FormatFlag FormatFlag::RESET_BOLD{ 2u };
	/** @brief Invert foreground & background colors of printed text. */
	inline constexpr const FormatFlag FormatFlag::INVERT{ 4u };
	/** @brief Unset the invert flag specifically. */
	inline constexpr const FormatFlag FormatFlag::RESET_INVERT{ 8u };
	/** @brief Underline printed text. */
	inline constexpr const FormatFlag FormatFlag::UNDERLINE{ 16u };
	/** @brief Unset the underline flag specifically. */
	inline constexpr const FormatFlag FormatFlag::RESET_UNDERLINE{ 32u };
#pragma endregion DEFINITIONS

	/**
	 * @brief Accepts any number of integrals, and returns the result of calling bitwise OR on each of them.
	 * @tparam ...VT		- Variadic Templated Type (Integral)
	 * @param ...numbers	- Numbers to bitwise-merge
	 * @returns unsigned char
	 */
	template<typename... VT> requires std::conjunction_v<std::is_integral<VT>...>
	inline static constexpr unsigned char bitmerge(const VT&... numbers) noexcept
	{
		unsigned char flag{ 0u };
		for (auto& i : var::variadic_accumulate<unsigned long long>(numbers...))
			flag |= static_cast<unsigned char>(i);
		return flag;
	}

	/**
	 * @struct ColorFormat
	 * @brief A mutable format flag instance. Used by the setcolor struct to maintain any number of independent FormatFlag instances.
	 */
	struct ColorFormat {
	private:
		unsigned char _fmt; ///< @brief Current flag value
	public:
		/**
		 * @brief Single-flag constructor.
		 * @param flag	- A premade FormatFlag instance.
		 */
		ColorFormat(const FormatFlag& flag) : _fmt{ flag.operator const unsigned char() } {}

		/**
		 * @brief Variadic bitmerge constructor.
		 * @tparam ...VT	- Variadic Templated Type (FormatFlag)
		 * @param ...flags	- At least two format flag instances that will be merged together with a bitwise OR operation.
		 */
		template<typename... VT> requires (sizeof...(VT) > 1) && std::conjunction_v<std::is_same<VT, FormatFlag>...>
		ColorFormat(const VT&... flags) : _fmt{ bitmerge(flags...) } {}

		/** @brief Retrieve the current value of this flag */
		constexpr operator unsigned char() const { return _fmt; }

		/**
		 * @brief Check if this flag contains a FormatFlag.
		 * @param flag	- A FormatFlag instance to compare against.
		 * @returns bool
		 */
		constexpr bool operator==(const FormatFlag& flag) const
		{
			return operator&(flag.operator const unsigned char()) != 0;
		}
		/// @brief Inverse Comparison Operator. Uses perfect-forwarding to return the negated result of operator==
		constexpr bool operator!=(auto&& o) const { return !operator==(std::forward<decltype(o)>(o)); }

		/// @brief Bitwise OR operator.
		constexpr unsigned char operator|(const unsigned char& o) const { return static_cast<unsigned char>(!!_fmt | !!o); }
		/// @brief Bitwise AND operator.
		constexpr unsigned char operator&(const unsigned char& o) const { return static_cast<unsigned char>(!!_fmt & !!o); }
		/// @brief Bitwise XOR operator.
		constexpr unsigned char operator^(const unsigned char& o) const { return static_cast<unsigned char>(!!_fmt ^ !!o); }
		/// @brief Bitwise OR setter-operator.
		constexpr unsigned char operator|=(const unsigned char& o) { return _fmt = static_cast<unsigned char>(!!_fmt | !!o); }
		/// @brief Bitwise AND setter-operator.
		constexpr unsigned char operator&=(const unsigned char& o) { return _fmt = static_cast<unsigned char>(!!_fmt & !!o); }
		/// @brief Bitwise XOR setter-operator.
		constexpr unsigned char operator^=(const unsigned char& o) { return _fmt = static_cast<unsigned char>(!!_fmt ^ !!o); }
	};
}
#ifndef COLOR_NO_GLOBALS
/// @brief Allows specifying the color::FormatFlag object with a shorter syntax. Define "COLOR_NO_GLOBALS" to disable.
using Format = color::FormatFlag;
#endif
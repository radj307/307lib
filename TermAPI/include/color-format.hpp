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

	struct format {
		/**
		 * @class		FlagBase
		 * @brief		Base Formatting "flag" object, uses bit fields to store various types of formatting data.
		 * @tparam T	Variable type to use. This essentially defines the maximum number of individual flags. Default is char (1 byte, max value 256)
		 */
		template<typename T>
		class FlagBase {
			T _v;
		public:
			/**
			 * @brief	Value Constructor
			 * @param v	Input Value
			 */
			CONSTEXPR FlagBase(const T& v) : _v{ v } {}
			CONSTEXPR FlagBase(const FlagBase<T>&) = default; // copy ctor
			CONSTEXPR FlagBase(FlagBase<T>&&) noexcept = default; // move ctor
			CONSTEXPR virtual ~FlagBase() noexcept = default; // virtual destructor

			CONSTEXPR FlagBase<T>& operator=(const FlagBase<T>&) = default; // copy operator
			CONSTEXPR FlagBase<T>& operator=(FlagBase<T>&&) noexcept = default; // move operator

			/**
			 * @brief	Retrieve the flag's stored value. This operator is implicitly called when possible.
			 * @returns	T 
			 */
			CONSTEXPR operator T() const { return _v; }
			/**
			 * @brief		Check if this flag's bitfield contains a given flag.
			 * @tparam U	Any type with the same size as T
			 * @param o		Other FlagBase-derived object.
			 * @returns		bool
			 */
			template<var::same_size<T> U> CONSTEXPR bool contains(const FlagBase<U>& o) const { return (_v & static_cast<T>(o)) != 0; }
			/**
			 * @brief		Check if this flag's bitfield contains a given flag.
			 * @tparam U	Any type with the same size as T
			 * @param v		Other value.
			 * @returns		bool
			 */
			template<var::same_size<T> U> CONSTEXPR bool contains(const U& v) const { return (_v & v) != 0; }

			// Bitwise operators
			CONSTEXPR FlagBase<T> operator|(const T& v) const { return FlagBase<T>{ static_cast<T>(_v | v) }; }
			CONSTEXPR FlagBase<T> operator&(const T& v) const { return FlagBase<T>{ static_cast<T>(_v & v) }; }
			CONSTEXPR FlagBase<T> operator^(const T& v) const { return FlagBase<T>{ static_cast<T>(_v ^ v) }; }

			// Bitwise setter operators
			CONSTEXPR FlagBase<T>& operator|=(const T& v) const { _v |= v; return *this; }
			CONSTEXPR FlagBase<T>& operator&=(const T& v) const { _v &= v; return *this; }
			CONSTEXPR FlagBase<T>& operator^=(const T& v) const { _v ^= v; return *this; }

			/**
			 * @brief		Set the flag to a new value.
			 * @param v		Another FlagBase-derived type with the same size.
			 * @returns		FlagBase<T>&
			 */
			template<var::same_size<T> U>
			FlagBase<T>& set(const FlagBase<U>& v)
			{
				_v = v;
				return *this;
			}
			/**
			 * @brief		Set the flag to a new value.
			 * @param v		A value with a.
			 * @returns		FlagBase<T>&
			 */
			template<var::same_size<T> U> 
			FlagBase<T>& set(const U& v)
			{
				_v = v;
				return *this;
			}
			template<std::integral U> requires (!var::same_size<T, U>)
			FlagBase<T>& set(const U& v) // resulting type of bitwise operations is int
			{
				_v = static_cast<T>(v);
				return *this;
			}
			/**
			 * @brief	Retrieve the escape sequence to apply this flag's value(s).
			 * @returns	ANSI::Sequence
			 */
			ANSI::Sequence Sequence() const
			{
				ANSI::Sequence seq{};
				if (contains(RESET)) { // unset format
					if (contains(BOLD))
						seq += ANSI::make_sequence(ANSI::CSI, ANSI::SGR_NO_BOLD, ANSI::END);
					if (contains(UNDERLINE))
						seq += ANSI::make_sequence(ANSI::CSI, ANSI::SGR_NO_UNDERLINE, ANSI::END);
					if (contains(INVERT))
						seq += ANSI::make_sequence(ANSI::CSI, ANSI::SGR_POSITIVE, ANSI::END);
				}
				else { // set format
					if (contains(BOLD))
						seq += ANSI::make_sequence(ANSI::CSI, ANSI::SGR_BOLD, ANSI::END);
					if (contains(UNDERLINE))
						seq += ANSI::make_sequence(ANSI::CSI, ANSI::SGR_UNDERLINE, ANSI::END);
					if (contains(INVERT))
						seq += ANSI::make_sequence(ANSI::CSI, ANSI::SGR_NEGATIVE, ANSI::END);
				}
				return seq;
			}
			/**
			 * @brief	Retrieve the escape sequence to apply this flag's value(s).
			 * @returns	ANSI::wSequence
			 */
			ANSI::wSequence wSequence() const
			{
				ANSI::wSequence seq{};
				if (contains(RESET)) { // unset format
					if (contains(BOLD))
						seq += ANSI::make_sequence<ANSI::wSequence>(ANSI::CSI, ANSI::SGR_NO_BOLD, ANSI::END);
					if (contains(UNDERLINE))
						seq += ANSI::make_sequence<ANSI::wSequence>(ANSI::CSI, ANSI::SGR_NO_UNDERLINE, ANSI::END);
					if (contains(INVERT))
						seq += ANSI::make_sequence<ANSI::wSequence>(ANSI::CSI, ANSI::SGR_POSITIVE, ANSI::END);
				}
				else { // set format
					if (contains(BOLD))
						seq += ANSI::make_sequence<ANSI::wSequence>(ANSI::CSI, ANSI::SGR_BOLD, ANSI::END);
					if (contains(UNDERLINE))
						seq += ANSI::make_sequence<ANSI::wSequence>(ANSI::CSI, ANSI::SGR_UNDERLINE, ANSI::END);
					if (contains(INVERT))
						seq += ANSI::make_sequence<ANSI::wSequence>(ANSI::CSI, ANSI::SGR_NEGATIVE, ANSI::END);
				}
				return seq;
			}
		};
		/// @brief	FlagBase derivative with an immutable (constant) value.
		using Flag = const FlagBase<unsigned char>;

		#pragma region DefineFlags
		// Base Utility Types
		inline static const Flag NONE{ 0u };
		inline static const Flag RESET{ 1u };
		// Base Format Types
		inline static const Flag BOLD{ 2u };
		inline static const Flag UNDERLINE{ 4u };
		inline static const Flag INVERT{ 8u };
		// Complex Types
		inline static const Flag NO_BOLD{ RESET | BOLD };
		inline static const Flag NO_UNDERLINE{ RESET | UNDERLINE };
		inline static const Flag NO_INVERT{ RESET | INVERT };
		#pragma endregion DefineFlags

		/// @brief	FlagBase derivative with a mutable value.
		using MutableFlag = FlagBase<unsigned char>;
	};

	// SGR formatting sequences
	inline static const ANSI::Sequence
		bold{ ANSI::make_sequence(ANSI::CSI, ANSI::SGR_BOLD, ANSI::END) },
		no_bold{ ANSI::make_sequence(ANSI::CSI, ANSI::SGR_NO_BOLD, ANSI::END) },
		underline{ ANSI::make_sequence(ANSI::CSI, ANSI::SGR_UNDERLINE, ANSI::END) },
		no_underline{ ANSI::make_sequence(ANSI::CSI, ANSI::SGR_NO_UNDERLINE, ANSI::END) },
		invert{ ANSI::make_sequence(ANSI::CSI, ANSI::SGR_NEGATIVE, ANSI::END) },
		no_invert{ ANSI::make_sequence(ANSI::CSI, ANSI::SGR_POSITIVE, ANSI::END) },
		reset_f{ ANSI::make_sequence(ANSI::CSI, ANSI::SGR_DEFAULT_FORE, ANSI::END) },
		reset_b{ ANSI::make_sequence(ANSI::CSI, ANSI::SGR_DEFAULT_BACK, ANSI::END) },
		reset_fmt{ ANSI::make_sequence(no_bold, no_underline, no_invert) },
		reset{ ANSI::make_sequence(reset_f, reset_b, reset_fmt) },
		reset_all{ ANSI::make_sequence(ANSI::CSI, ANSI::SGR_RESET, ANSI::END) };

	inline static const ANSI::wSequence
		wbold{ ANSI::make_sequence<ANSI::wSequence>(ANSI::CSI, ANSI::SGR_BOLD, ANSI::END) },
		wno_bold{ ANSI::make_sequence<ANSI::wSequence>(ANSI::CSI, ANSI::SGR_NO_BOLD, ANSI::END) },
		wunderline{ ANSI::make_sequence<ANSI::wSequence>(ANSI::CSI, ANSI::SGR_UNDERLINE, ANSI::END) },
		wno_underline{ ANSI::make_sequence<ANSI::wSequence>(ANSI::CSI, ANSI::SGR_NO_UNDERLINE, ANSI::END) },
		winvert{ ANSI::make_sequence<ANSI::wSequence>(ANSI::CSI, ANSI::SGR_NEGATIVE, ANSI::END) },
		wno_invert{ ANSI::make_sequence<ANSI::wSequence>(ANSI::CSI, ANSI::SGR_POSITIVE, ANSI::END) },
		wreset_f{ ANSI::make_sequence<ANSI::wSequence>(ANSI::CSI, ANSI::SGR_DEFAULT_FORE, ANSI::END) },
		wreset_b{ ANSI::make_sequence<ANSI::wSequence>(ANSI::CSI, ANSI::SGR_DEFAULT_BACK, ANSI::END) },
		wreset_fmt{ ANSI::make_sequence<ANSI::wSequence>(wno_bold, wno_underline, wno_invert) },
		wreset{ ANSI::make_sequence<ANSI::wSequence>(wreset_f, wreset_b, wreset_fmt) },
		wreset_all{ ANSI::make_sequence<ANSI::wSequence>(ANSI::CSI, ANSI::SGR_RESET, ANSI::END) };

	// backwards compatibility using statements
	using FormatFlag = format::Flag;
}
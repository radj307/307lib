#pragma once
#include <sysarch.h>
#include <var.hpp>

#define _USE_MATH_DEFINES
#include <cmath>
#ifdef OS_WIN // M$VC doesn't include math constants in <cmath>, so include <math.h> to fix this broke asf implementation
#include <math.h>
#endif
#include <utility>
#include <string>
#include <algorithm>
#include <numeric>
#include <ratio>
#include <chrono>

namespace math {
	/**
	 * @brief		Retrieve the machine epsilon value for the given type using numeric_limits.
	 * @tparam T	Input floating-point type
	 * @returns		T
	 */
	template<std::floating_point T>
	INLINE CONSTEXPR T getEpsilon() noexcept { return std::numeric_limits<T>::epsilon(); }

	/**
	 * @brief		Checks if the given floating-point numbers are closer to being equal than the machine epsilon value.
	 * @tparam T	Input Floating-Point Type.
	 * @param l		First comparison value.
	 * @param r		Second comparison value.
	 * @returns		T
	 */
	template<std::floating_point T> [[nodiscard]] INLINE static CONSTEXPR bool equal(T const& l, T const& r) noexcept
	{
		const T diff{ l - r };
		return (diff < static_cast<T>(0.0) ? -diff : diff) < getEpsilon<T>();
	}
	/**
	 * @brief		Checks if the given numbers are equal. This function only exists to allow SFINAE handling of integral types
	 * @tparam T	Input Integral Type.
	 * @param l		First comparison value.
	 * @param r		Second comparison value.
	 * @returns		T
	 */
	template<std::integral T> [[nodiscard]] INLINE static CONSTEXPR bool equal(T const& l, T const& r) noexcept { return l == r; }

	/**
	 * @brief			This is a `constexpr` version of the pow function from \<cmath\>.
	 * @tparam T		Any numeric type, including integrals and floating-points.
	 * @param value		Input value.
	 * @param power		Exponent value.
	 * @returns T		The result of the expression ( value ^ power ). _(exponent, not XOR)_
	 */
	template<var::numeric T> [[nodiscard]] INLINE static CONSTEXPR T pow(const T& value, const T& power = static_cast<T>(2)) noexcept
	{
		T v{ value };
		for (unsigned i{ 0u }, end{ static_cast<unsigned>(power) - 1u }; i < end; ++i)
			v *= value;
		return v;
	}

	/**
	 * @brief			Calculate the result of a floating-point modulo operation.
	 * @tparam T		Floating-Point Type
	 * @param value		Input Floating-Point Value.
	 * @param modulo	Input Floating-Point Modulo.
	 * @returns	T		Resulting value of the modulo operation.
	 */
	template<std::floating_point T>
	[[nodiscard]] CONSTEXPR static T mod(const T& value, const T& modulo)
	{
#		ifdef OS_WIN
		if CONSTEXPR(std::same_as<T, long double>) // long double
			return std::fmodl(value, modulo);
		else if CONSTEXPR(std::same_as<T, double>) // double
			return std::fmod(value, modulo);
		else // float
			return std::fmodf(value, modulo);
#		else
		return std::fmod(value, modulo);
#		endif
	}

	/**
	 * @brief			Calculate the result of an integral modulo operation. This overload exists to allow interoperability between integral & floating-point values when using the mod function.
	 * @tparam T		Integral Type
	 * @param value		Input Value.
	 * @param modulo	Input Modulo.
	 * @returns			T
	 */
	template<std::integral T>
	[[nodiscard]] CONSTEXPR static T mod(const T& value, const T& modulo)
	{
		return value % modulo;
	}

	/**
	 * @brief				Normalize a given value from its current range to a new one.
	 * @tparam T			Any number type.
	 * @param value			Input Value.
	 * @param old_range		The number range to translate the given value from.
	 * @param new_range		The number range to translate the given value to.
	 * @returns				T
	 */
	template<var::numeric T, var::numeric RT = T>
	[[nodiscard]] static CONSTEXPR RT normalize(const T& value, const std::pair<T, T>& old_range, const std::pair<T, T>& new_range = { 1, 1 })
	{
		return static_cast<RT>(new_range.first) + (static_cast<RT>(value) - static_cast<RT>(old_range.first)) * static_cast<RT>(static_cast<RT>(new_range.second) - static_cast<RT>(new_range.first)) / (static_cast<RT>(old_range.second) - static_cast<RT>(old_range.first));
	}
	/**
	 * @brief				Scale a given value from its current range to a new one.
	 * @tparam T			Any number type.
	 * @param value			Input Value.
	 * @param old_range		The number range to translate the given value from.
	 * @param new_range		The number range to translate the given value to.
	 * @returns				T
	 */
	template<var::numeric T, var::numeric RT = T>
	[[nodiscard]] static CONSTEXPR RT scale(const T& value, const std::pair<T, T>& old_range, const std::pair<T, T>& new_range = { 1, 1 })
	{
		return normalize(value, old_range, new_range);
	}

	template<typename T> [[nodiscard]] static CONSTEXPR T max_value()
	{
		if CONSTEXPR(std::unsigned_integral<T>)
			return{ 255 * sizeof(T) };
		else return { 128 * sizeof(T) };
	}

	/**
	 * @brief		Constant-time math abs function. (Removes negative sign from numbers)
	 * @tparam T	Input Type
	 * @param val	Input Value
	 * @returns		T
	 */
	template<var::arithmetic T> [[nodiscard]] static CONSTEXPR T abs(const T& val) { return val > 0 ? val : -val; }

#if LANG_CPP >= 17
#define MATH_HPP_AVERAGE_FUNCTION_SIG(begin, end) std::reduce(begin, end)
#else
#define MATH_HPP_AVERAGE_FUNCTION_SIG(begin, end) std::accumulate(begin, end, T{ 0 })
#endif

	/**
	 * @brief		Get the average of a vector of numbers.
	 * @param vec	Input vector of any arithmetic type.
	 * @returns		T
	 *\n			The average of all of the numbers in the vector.
	 */
	template<var::arithmetic T, typename IteratorT> [[nodiscard]] static CONSTEXPR T average(const IteratorT& begin, const IteratorT& end)
	{
		return MATH_HPP_AVERAGE_FUNCTION_SIG(begin, end) / static_cast<T>(std::distance(begin, end));
	}

	/**
	 * @brief		Get the average of a variadic list of numbers.
	 * @param vec	Any number of values of type T.
	 * @returns		T
	 *\n			The average of all of the numbers in the vector.
	 */
	template<var::arithmetic T, std::same_as<T>... Ts> [[nodiscard]] static CONSTEXPR T average(T&& fst, Ts&&... rest)
	{
		return average<T>(std::vector<T>{ std::forward<T>(fst), std::forward<Ts>(rest)... });
	}

	/**
	 * @brief				Calculate the average duration from a list of durations.
	 * @tparam Rep			Duration Type Rep.
	 * @tparam Period		Duration Type Period.
	 * @tparam DurationT	Duration Type.
	 * @param durations		Vector of durations.
	 * @returns				DurationT
	 */
	template<typename Rep, typename Period = std::ratio<1L, 1L>, std::same_as<std::chrono::duration<Rep, Period>> DurationT = std::chrono::duration<Rep, Period>>
	[[nodiscard]] static CONSTEXPR DurationT average(const std::vector<DurationT>& durations)
	{
		const DurationT sum{ MATH_HPP_AVERAGE_FUNCTION_SIG(durations.begin(), durations.end()) };
		return sum / durations.size();
	}

	/**
	 * @brief				Get the difference between two durations.
	 * @tparam Rep			Duration Type Rep.
	 * @tparam Period		Duration Type Period.
	 * @tparam DurationT	Duration Type.
	 * @param left			First input duration.
	 * @param right			Second input duration.
	 * @returns				DurationT
	*/
	template<typename Rep, typename Period = std::ratio<1L, 1L>, std::derived_from<std::chrono::duration<Rep, Period>> DurationT = std::chrono::duration<Rep, Period>>
	[[nodiscard]] static CONSTEXPR DurationT difference(DurationT const& left, DurationT const& right)
	{
		return DurationT{ std::abs(left.count() - right.count()) };
	}

	/**
	 * @brief			Split an integral type between 2 types that are half the size of the input type.
	 *\n				Note: This function uses memory address manipulation.
	 * @tparam Input	An input type.
	 * @tparam Output	An output type that is half the size of the input type.
	 * @param value		An input value to split between 2 half-sized types.
	 * @returns			std::pair<Output, Output>
	 *\n				  first:	High-order Byte(s).
	 *\n				  second:	Low-order Byte(s).
	 */
	template<typename Output, typename Input> requires (sizeof(Output) == sizeof(Input) / 2)
		inline CONSTEXPR std::pair<Output, Output> byteSplit(const Input& value) noexcept
	{
		return{ *((Output*)&(value)+1), *((Output*)&(value)+0) };
	}

	/**
	 * @brief				Joins two half-sized types into a single type that is double its size.
	 *\n					Note: This function uses memory address manipulation.
	 * @tparam Input		Input type.
	 * @tparam Output		Output type that is exactly 2x the size of the input type.
	 * @param highBytes		The high-order bytes to join.
	 * @param lowBytes		The low-order bytes to join.
	 * @returns				Output
	 */
	template<typename Output, typename Input> requires (sizeof(Output) == sizeof(Input) * 2)
		inline CONSTEXPR Output byteJoin(const Input& hiBytes, const Input& loBytes) noexcept
	{
		Output value{ 0 };
		*((Input*)&(value)+1) = hiBytes;
		*((Input*)&(value)+0) = loBytes;
		return value;
	}
	/**
	 * @brief				Joins two half-sized types into a single type that is double its size.
	 *\n					Note: This function uses memory address manipulation.
	 * @tparam Input		Input type.
	 * @tparam Output		Output type that is exactly 2x the size of the input type.
	 * @param bytes			A pair of 2 input types where
	 *\n					  first:	High-order byte(s).
	 *\n					  second:	Low-order byte(s).
	 * @returns				Output
	 */
	template<typename Output, typename Input> requires (sizeof(Output) == sizeof(Input) * 2)
		inline CONSTEXPR Output byteJoin(const std::pair<Input, Input>& bytes) noexcept
	{
		return join<Output, Input>(bytes.first, bytes.second);
	}
}
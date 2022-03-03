#pragma once
#include <sysarch.h>

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
	 * @brief			Calculate the result of a floating-point modulo operation.
	 * @tparam T		Floating-Point Type
	 * @param value		Input Floating-Point Value.
	 * @param modulo	Input Floating-Point Modulo.
	 * @returns			T
	 */
	template<std::floating_point T>
	[[nodiscard]] CONSTEXPR static T mod(const T& value, const T& modulo)
	{
		#ifdef OS_WIN
		if constexpr (std::same_as<T, long double>) // long double
			return std::fmodl(value, modulo);
		else if constexpr (std::same_as<T, double>) // double
			return std::fmod(value, modulo);
		else // float
			return std::fmodf(value, modulo);
		#else
		return std::fmod(value, modulo);
		#endif
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
	template<typename T, typename RT = T>
	[[nodiscard]] static CONSTEXPR RT normalize(const T& value, const std::pair<T, T>& old_range, const std::pair<T, T>& new_range = { 1, 1 })
	{
		return static_cast<RT>(new_range.first) + (static_cast<RT>(value) - static_cast<RT>(old_range.first)) * static_cast<RT>(static_cast<RT>(new_range.second) - static_cast<RT>(new_range.first)) / (static_cast<RT>(old_range.second) - static_cast<RT>(old_range.first));
	}

	template<typename T> [[nodiscard]] static CONSTEXPR T max_value()
	{
		if constexpr (std::unsigned_integral<T>)
			return{ 255 * sizeof(T) };
		else return { 128 * sizeof(T) };
	}

	/**
	 * @brief		Constant-time math abs function. (Removes negative sign from numbers)
	 * @tparam T	Input Type
	 * @param val	Input Value
	 * @returns		T
	 */
	template<var::arithmetic T> [[nodiscard]] static constexpr T abs(const T& val) { return val > 0 ? val : -val; }

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
	template<var::arithmetic T, typename IteratorT> [[nodiscard]] static constexpr T average(const IteratorT& begin, const IteratorT& end)
	{
		return MATH_HPP_AVERAGE_FUNCTION_SIG(begin, end) / static_cast<T>(std::distance(begin, end));
	}

	/**
	 * @brief		Get the average of a variadic list of numbers.
	 * @param vec	Any number of values of type T.
	 * @returns		T
	 *\n			The average of all of the numbers in the vector.
	 */
	template<var::arithmetic T, std::same_as<T>... Ts> [[nodiscard]] static constexpr T average(T&& fst, Ts&&... rest)
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
	[[nodiscard]] static constexpr DurationT average(const std::vector<DurationT>& durations)
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
	[[nodiscard]] static constexpr DurationT difference(DurationT const& left, DurationT const& right)
	{
		return DurationT{ std::abs(left.count() - right.count()) };
	}

}
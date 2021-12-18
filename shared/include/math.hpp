#pragma once
#include <sysarch.h>
#include <cmath>
#include <utility>
#include <string>
#include <algorithm>

namespace math {
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
}
#pragma once
#include <sysarch.h>
#include <make_exception.hpp>

#include <chrono>
#include <algorithm>

namespace debug {
	// High resolution clock for the current platform.
	using clock = std::chrono::high_resolution_clock;

	/// @brief	Gets the (approximate) amount of time, in nanoseconds, that the specified function takes to execute.
	template<class Fn, typename... Args>
	CONSTEXPR std::chrono::nanoseconds profile_once(Fn const& func, Args&&... args)
	{
		// get start time
		const auto start_time{ clock::now() };

		// call the function
		func(std::forward<Args>(args)...);

		// get stop time
		const auto stop_time{ clock::now() };

		// return difference
		return stop_time - start_time;
	}

	/**
	 * @brief			Calculates the average of the specified range of numeric values.
	 * @tparam Iter	  -	The type of iterator.
	 * @param begin	  -	Iterator for the beginning of the range of values.
	 * @param end	  -	Iterator for the (exclusive) end of the range of values.
	 * @returns			The average of the specified range of values.
	 */
	template<class Iter>
	long double average(Iter const& begin, Iter const& end)
	{
		uint64_t n{ 0 };
		long double mean{ 0 };
		for (auto it{ begin }; it != end; ++it) {
			mean += (*it - mean) / ++n;
		}
		return mean;
	}

	/**
	 * @brief				Gets the (approximate) average amount of time that
	 *						 the specified function takes to execute.
	 * @tparam Fn		  -	The type of function to profile.
	 * @tparam ...Args	  -	The type(s) of arguments of the function.
	 * @param count		  -	The number of times to call the function.
	 * @param func		  -	The function to profile.
	 * @param ...args	  -	The argument(s) to pass to the function.
	 * @returns				The average elapsed time, in nanoseconds.
	 */
	template<class Fn, typename... Args>
	CONSTEXPR std::chrono::duration<long double, std::nano> profile_avg(unsigned const count, Fn const& func, Args&&... args)
	{
		std::vector<long long> elapsed;
		elapsed.reserve(count);

		for (auto i{ 0u }; i < count; ++i) {
			elapsed.emplace_back(profile_once<Fn, Args...>(func, std::forward<Args>(args)...).count());
		}

		// return average
		return std::chrono::duration<long double, std::nano>{ average(elapsed.begin(), elapsed.end()) };
	}
}

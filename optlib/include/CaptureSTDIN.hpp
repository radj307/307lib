/**
 * @file	CaptureSTDIN.hpp
 * @author	radj307
 * @brief	Contains functions to facilitate capturing piped input from the STDIN stream without blocking.
 */
#pragma once
#include <sysarch.h>
#include <str.hpp>

#include <vector>
#include <string>
#include <sstream>
#include <iostream>

#ifndef OS_WIN
 /**
  * @brief		Check if the given read file descriptor has pending data.
  * @param fd	The target file descriptor. (STDIN = 0)
  * @returns	bool
  *\n			true	There is pending input.
  *\n			false	There is no pending input.
  */
INLINE bool hasPendingInput(const int& fd)
{
	struct timespec timeout { 0L, 0L };
	fd_set fds{}; // create a file descriptor set
	FD_ZERO(&fds);
	FD_SET(fd, &fds); // stdin file descriptor is 0
	fflush(NULL); // flush input buffer
	return pselect(fd + 1, &fds, nullptr, nullptr, &timeout, nullptr) == 1;
}
#else
 // Dummy function for windows that always returns false
INLINE CONSTEXPR bool hasPending(const int& fd) { return false; }
#endif

/**
 * @brief	Read all available input from STDIN without blocking.
 * @returns	std::vector<std::string>
 */
INLINE std::vector<std::string> CaptureSTDIN(const size_t& reserve_sz = 26ull)
{
	INLINE CONSTEXPR const auto STDIN_FD{ 0 };
	if (!hasPendingInput(STDIN_FD))
		return{};
	
	// lambda that gets the next word from STDIN
	const auto& getNextWord{ [&]() -> std::string {
		std::string s;
		if (std::cin.good())
			str::getline(std::cin, s, ' ', '\n', '\t');
		return s;
	} };

	std::vector<std::string> vec;
	vec.reserve(reserve_sz);
	while (std::cin.good() && hasPendingInput(0)) {// keep looping until there is no more pending data
		if (auto word{ getNextWord() }; !word.empty())
			vec.emplace_back(std::move(word));
		if (vec.capacity() + 2ull > reserve_sz)
			vec.reserve(vec.size() + reserve_sz);
	}
	vec.shrink_to_fit();
	return vec;
}

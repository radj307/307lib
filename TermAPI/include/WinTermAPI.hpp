#pragma once
#include <sysarch.h>
#include <make_exception.hpp>

#include <iostream>
#include <string>
#include <bitset>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace term {
	INLINE CONSTEXPR const int STDIN_HANDLE{ 0 };
	INLINE CONSTEXPR const int STDOUT_HANDLE{ 1 };
	INLINE CONSTEXPR const int STDERR_HANDLE{ 2 };

	template<int fd> requires (fd == STDIN_HANDLE || fd == STDOUT_HANDLE || fd == STDERR_HANDLE) [[nodiscard]] inline bool EnableANSI() noexcept
	{
		const auto set_mode{ [](const HANDLE& stdhandle, const DWORD& flags) -> bool {
			DWORD mode{0};
			if (!GetConsoleMode(stdhandle, &mode))
				return false;
			mode |= flags;
			return SetConsoleMode(stdhandle, mode);
		} }; // set_mode
		if constexpr (fd == STDIN_HANDLE) // STDIN
			return set_mode(std::forward<HANDLE>(GetStdHandle(STD_INPUT_HANDLE)), ENABLE_VIRTUAL_TERMINAL_INPUT);
		else if constexpr (fd == STDOUT_HANDLE) // STDOUT
			return set_mode(std::forward<HANDLE>(GetStdHandle(STD_OUTPUT_HANDLE)), ENABLE_VIRTUAL_TERMINAL_PROCESSING);
		else if constexpr (fd == STDERR_HANDLE) // STDERR
			return set_mode(std::forward<HANDLE>(GetStdHandle(STD_ERROR_HANDLE)), ENABLE_VIRTUAL_TERMINAL_PROCESSING);
		return false; // this is only here to quiet the compiler, it isn't possible to hit it.
	}

	template<bool INCLUDES_STDIN = false>
	inline std::ostream& EnableANSI(std::ostream& os)
	{
		if ((INCLUDES_STDIN && !EnableANSI<STDIN_HANDLE>()) || !EnableANSI<STDOUT_HANDLE>() || !EnableANSI<STDERR_HANDLE>())
			throw make_exception("EnableANSI():\tFailed to enable Windows virtual terminal sequence support!");
		return os;
	}
}
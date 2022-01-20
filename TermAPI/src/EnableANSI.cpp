#include "../include/EnableANSI.hpp"
#ifdef OS_WIN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
term::FDHANDLE::type term::EnableANSI(const FDHANDLE& fdh) noexcept
{
	const auto& setmode{
		[](HANDLE&& stdhandle, const DWORD& flags) -> bool {
			DWORD mode{0};
			if (!GetConsoleMode(std::forward<HANDLE>(stdhandle), &mode))
				return false;
			return SetConsoleMode(std::forward<HANDLE>(stdhandle), (mode | flags));
		}
	};
	FDHANDLE successful{ 0 };
	if (fdh.contains(FDHANDLE::STDIN)) // STDIN
		if (setmode(std::forward<HANDLE>(GetStdHandle(STD_INPUT_HANDLE)), ENABLE_VIRTUAL_TERMINAL_INPUT))
			successful |= FDHANDLE::STDIN;
	if (fdh.contains(FDHANDLE::STDOUT)) // STDOUT
		if (setmode(std::forward<HANDLE>(GetStdHandle(STD_OUTPUT_HANDLE)), ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN))
			successful |= FDHANDLE::STDOUT;
	if (fdh.contains(FDHANDLE::STDERR)) // STDERR
		if (setmode(std::forward<HANDLE>(GetStdHandle(STD_ERROR_HANDLE)), ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN))
			successful |= FDHANDLE::STDERR;
	return successful;
}
#else
term::FDHANDLE::type term::EnableANSI(const FDHANDLE& fdh) noexcept { return (FDHANDLE::STDIN | FDHANDLE::STDOUT | FDHANDLE::STDERR); }
#endif

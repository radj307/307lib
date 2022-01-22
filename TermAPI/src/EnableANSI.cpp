#include "../include/EnableANSI.hpp"

#ifdef OS_WIN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // include windows.h in the source file to prevent pollution

bool term::enable_fd_modes(void* hndl, const unsigned long& modes) noexcept
{
	DWORD mode{ 0ul };
	return (
		hndl != nullptr // don't allow null handles
		&& GetConsoleMode(hndl, &mode) // short-circuit and return false if GetConsoleMode fails
		&& (
		(mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0 // return true if ANSI sequences are already enabled
		|| SetConsoleMode(hndl, mode | modes) // return true if SetConsoleMode is successful
		)
	);
}

bool term::enable_fd(const term::HandleFD& handle_fd) noexcept
{
	using enum HandleFD;
	switch (handle_fd) {
	case STDIN:
		return enable_fd_modes(GetStdHandle(STD_INPUT_HANDLE), ENABLE_VIRTUAL_TERMINAL_INPUT);
	case STDOUT:
		return enable_fd_modes(GetStdHandle(STD_OUTPUT_HANDLE), ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	case STDERR:
		return enable_fd_modes(GetStdHandle(STD_ERROR_HANDLE), ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	default:
		return false;
	}
}
#endif

#include "../include/EnableANSI.hpp"

#ifdef OS_WIN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // include windows.h in the source file to prevent pollution

#include <mutex>

/// @brief	std::once_flag instance for the STDOUT handle
static std::once_flag $_STDOUT_$;
/// @brief	std::once_flag instance for the STDERR handle
static std::once_flag $_STDERR_$;
/// @brief	std::once_flag instance for the STDIN handle
static std::once_flag $_STDIN_$;

#define $_vtOUTPUTMODE_$ ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN
#define $_vtINPUTMODE_$ ENABLE_VIRTUAL_TERMINAL_INPUT

inline bool enable_fd_modes(void* hndl, const int modes) noexcept
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

void term::enable_fd(const term::HandleFD& handle_fd) noexcept
{
	switch (handle_fd) {
	case HandleFD::STDIN:
		std::call_once($_STDIN_$, enable_fd_modes, GetStdHandle(STD_INPUT_HANDLE), $_vtINPUTMODE_$);
		break;
	case HandleFD::STDOUT:
		std::call_once($_STDOUT_$, enable_fd_modes, GetStdHandle(STD_OUTPUT_HANDLE), $_vtOUTPUTMODE_$);
		break;
	case HandleFD::STDERR:
		std::call_once($_STDERR_$, enable_fd_modes, GetStdHandle(STD_ERROR_HANDLE), $_vtOUTPUTMODE_$);
		break;
	default:break;
	}
}
#endif

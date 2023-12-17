/**
 * @file	hasPendingDataSTDIN.h
 * @author	radj307
 * @brief	Contains the hasPendingDataSTDIN() function, a cross-platform function
 *\n		that checks if STDIN has pending data, allowing piped input on Linux and Windows.
 */
#pragma once
#include <sysarch.h>

#ifdef OS_WIN // Windows
#include <cstdio>
#include <io.h>
#elif defined(OS_LINUX) || defined(OS_MAC) // POSIX
#include <unistd.h>
#include <sys/socket.h>
#endif

 /**
 * @brief		Checks if STDIN has any data waiting to be read.
 *              NOTE: DO NOT USE THIS FOR DETECTING KEYBOARD INPUT ON WINDOWS
 * @attention	Do not use this function for checking if the user has pressed a key on ___windows only___!
 * @returns		true when STDIN has pending data; otherwise, false.
 */
[[nodiscard]] INLINE bool hasPendingDataSTDIN()
{
#if defined(OS_WIN)
	// Return true when the STDIN file descriptor hasn't been redirected to a file.
	// This works even when redirecting output from a file (which is only supported by cmd, not powershell or pwsh)
	return !_isatty(_fileno(stdin));
#else
	struct timespec timeout { 0l, 0l };
	fd_set fds{};
	FD_ZERO(&fds);
	FD_SET(0, &fds);
	// return true when select says 1 fd (stdin) is ready.
	return pselect(0 + 1, &fds, nullptr, nullptr, &timeout, nullptr) == 1;
#endif
}

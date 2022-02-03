/**
 * @file	hasPendingDataSTDIN.h
 * @author	radj307
 * @brief	Contains the hasPendingDataSTDIN() function, a cross-platform function
 *\n		that checks if STDIN has pending data, allowing piped input on Linux and Windows.
 */
#pragma once
#include <sysarch.h>

#ifdef OS_WIN // Windows: use 
#include <cstdio>
#include <io.h>
#elif defined(OS_LINUX) || defined(OS_MAC) // POSIX
#include <unistd.h>
#include <sys/socket.h>
#endif

 /**
 * @brief	Checks if there is a pending read operation on the STDIN stream.
 *\n		This function works on both POSIX & Windows.
 * @returns	bool
 *\n		true	There is pending data in the STDIN stream.
 *\n		false	The STDIN stream is empty.
 */
[[nodiscard]] INLINE bool hasPendingDataSTDIN()
{
#	if defined(OS_WIN)
	// Return true when the STDIN file descriptor is waiting
	return !_isatty(_fileno(__acrt_iob_func(0)));
#	elif defined(OS_LINUX) || defined(OS_MAC) // POSIX
	struct timespec timeout { 0l, 0l };
	fd_set fds{};
	FD_ZERO(&fds);
	FD_SET(0, &fds);
	// return true when select says 1 fd (stdin) is ready.
	return pselect(0 + 1, &fds, nullptr, nullptr, &timeout, nullptr) == 1;
#	else
	static_assert(false, "<hasPendingDataSTDIN.h>:  Cannot determine which Operating System is being used, see <sysarch.h>");
#	endif
}

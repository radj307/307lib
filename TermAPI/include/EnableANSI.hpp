/**
 * @file	EnableANSI.hpp
 * @author	radj307
 * @brief	Contains functions for enabling ANSI sequences on Windows.
 *\n		When included & used on linux, the functions do nothing, and cause no errors.
 */
#pragma once
#include <sysarch.h>
#include <make_exception.hpp>

#include <istream>
#include <ostream>

namespace term {
	#ifdef OS_WIN
	/**
	 * @brief		Enable ANSI escape sequences for std file descriptor streams (STDIN, STDOUT, STDERR).
	 * @param hndl	File Descriptor Handle. (See _GetStdHandle_)
	 * @returns		bool
	 *\n			true	Success
	 *\n			false	Failure
	 */
	bool enable_fd_modes(void*, const unsigned long&) noexcept;

	/**
	 * @brief	Handle file descriptors
	 */
	enum class HandleFD : unsigned char {
		STDIN,
		STDOUT,
		STDERR,
	};

	bool enable_fd(const HandleFD&) noexcept;
	#endif // OS_WIN

	/**
	 * @brief		Enable ANSI output sequences for STDOUT & STDERR.
	 * @param os	Target Output Stream
	 * @returns		std::ostream&
	 */
	inline std::ostream& EnableANSI(std::ostream& os) noexcept
	{
		#ifdef OS_WIN
		enable_fd(HandleFD::STDOUT);
		enable_fd(HandleFD::STDERR);
		#endif
		return os;
	}

	/**
	 * @brief		Enable ANSI output sequences for STDIN.
	 * @param os	Target Output Stream
	 * @returns		std::ostream&
	 */
	inline std::istream& EnableANSI(std::istream& is) noexcept
	{
		#ifdef OS_WIN
		enable_fd(HandleFD::STDIN);
		#endif
		return is;
	}
}

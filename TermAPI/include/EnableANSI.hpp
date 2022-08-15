/**
 * @file	EnableANSI.hpp
 * @author	radj307
 * @brief	Contains functions for enabling ANSI sequences on Windows.
 *\n		When included & used on linux, the functions do nothing, and cause no errors.
 */
#pragma once
#include <sysarch.h>
#include <make_exception.hpp>
#include <var.hpp>

#include <istream>
#include <ostream>

namespace term {
	#ifdef OS_WIN
	/**
	 * @brief	Handle file descriptors
	 */
	enum class HandleFD : unsigned char {
		STDIN,
		STDOUT,
		STDERR,
	};
	void enable_fd(const HandleFD&) noexcept;
	#endif // OS_WIN

	/**
	 * @brief		Enable ANSI output sequences for STDOUT & STDERR. Uses an internal synchronization object to ensure it is only called once.
	 *\n			This method does nothing if it wasn't compiled for Windows.
	 * @param os	Target Output Stream
	 * @returns		std::basic_ostream<TChar, TCharTraits>&
	 */
	template<var::valid_char TChar, std::derived_from<std::char_traits<TChar>> TCharTraits = std::char_traits<TChar>>
	std::basic_ostream<TChar, TCharTraits>& EnableANSI(std::basic_ostream<TChar, TCharTraits>& os) noexcept
	{
		#ifdef OS_WIN
		enable_fd(HandleFD::STDOUT);
		enable_fd(HandleFD::STDERR);
		#endif
		return os;
	}

	/**
	 * @brief		Enable ANSI output sequences for STDIN. Uses an internal synchronization object to ensure it is only called once.
	 *\n			This method does nothing if it wasn't compiled for Windows.
	 * @param os	Target Output Stream
	 * @returns		std::basic_ostream<TChar, TCharTraits>&
	 */
	template<var::valid_char TChar, std::derived_from<std::char_traits<TChar>> TCharTraits = std::char_traits<TChar>>
	std::basic_istream<TChar, TCharTraits>& EnableANSI(std::basic_istream<TChar, TCharTraits>& is) noexcept
	{
		#ifdef OS_WIN
		enable_fd(HandleFD::STDIN);
		#endif
		return is;
	}

	/**
	 * @brief		Enable ANSI output sequences for STDIN, STDOUT, & STDERR. Uses an internal synchronization object to ensure it is only called once.
	 *\n			This method does nothing if it wasn't compiled for Windows.
	 * @param os	Target Input/Output Stream
	 * @returns		std::basic_iostream<TChar, TCharTraits>&
	 */
	template<var::valid_char TChar, std::derived_from<std::char_traits<TChar>> TCharTraits = std::char_traits<TChar>>
	std::basic_iostream<TChar, TCharTraits>& EnableANSI(std::basic_iostream<TChar, TCharTraits>& ios) noexcept
	{
	#ifdef OS_WIN
		enable_fd(HandleFD::STDIN);
		enable_fd(HandleFD::STDOUT);
		enable_fd(HandleFD::STDERR);
	#endif
		return ios;
	}
}

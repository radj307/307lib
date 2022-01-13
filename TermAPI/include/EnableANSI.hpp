#pragma once
#include <sysarch.h>
#include <make_exception.hpp>

#include <ostream>

namespace term {
	/**
	 * @struct	FDHANDLE
	 * @brief	File descriptor handle bitfield flag object.
	 */
	struct FDHANDLE {
		using type = unsigned char;
	private:
		type _v;
	public:
		CONSTEXPR FDHANDLE(const type& v) : _v{ v } {}
		CONSTEXPR operator type() const { return _v; }
		/**
		 * @brief	Check if this FDHANDLE instance contains a given type.
		 * @param o	Other type to compare
		 * @returns	bool
		 */
		CONSTEXPR bool contains(const type& o) const { return (_v & o) != 0; }
		CONSTEXPR FDHANDLE& operator|=(const type& o) { _v |= o; return *this; }
		CONSTEXPR FDHANDLE& operator&=(const type& o) { _v &= o; return *this; }
		static const FDHANDLE STDIN, STDOUT, STDERR;
	};
	/// @brief	STDIN Handle
	INLINE const FDHANDLE FDHANDLE::STDIN{ static_cast<FDHANDLE::type>(1u) };
	/// @brief	STDOUT Handle
	INLINE const FDHANDLE FDHANDLE::STDOUT{ static_cast<FDHANDLE::type>(2u) };
	/// @brief	STDERR Handle
	INLINE const FDHANDLE FDHANDLE::STDERR{ static_cast<FDHANDLE::type>(4u) };

	/**
	 * @brief		Convert from standard file descriptor integer values to FDHANDLE.
	 * @param fd	File Descriptor for STDIN (0), STDOUT (1), or STDERR (2).
	 * @returns		FDHANDLE
	 */
	inline FDHANDLE make_fdhandle(const int& fd) noexcept(false)
	{
		switch (fd) {
		case 0:
			return FDHANDLE::STDIN;
		case 1:
			return FDHANDLE::STDOUT;
		case 2:
			return FDHANDLE::STDERR;
		default:
			throw make_exception("Invalid file descriptor: ", fd);
		}
	}

	/**
	 * @brief		Enables ANSI escape sequence support on Windows, otherwise does nothing.
	 * @param fdh	Target file descriptor handle(s)
	 * @returns		FDHANDLE::type
	 *\n			Contains the bitfield flags of all handles that were successfully updated.
	 */
	FDHANDLE::type EnableANSI(const FDHANDLE&) noexcept;

	/**
	 * @brief		Enables ANSI escape sequence support on Windows, otherwise does nothing.
	 * @param os	(implicit) Target Output Stream.
	 * @returns		std::ostream&
	 */
	inline std::ostream& EnableANSI(std::ostream& os) noexcept
	{
		#ifdef OS_WIN
		const FDHANDLE fd(FDHANDLE::STDIN | FDHANDLE::STDOUT | FDHANDLE::STDERR);
		if (EnableANSI(fd) == fd) {
			// successfully changed mode for all targeted file descriptors. Do something?
			// Note that attempting to enable VT support when it is already enabled will return an error
		}
		#endif
		return os;
	}
}

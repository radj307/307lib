#pragma once
#include <sysarch.h>
namespace xlog {
	INLINE CONSTEXPR const char* debug_header{ "[DEBUG]" };
	INLINE CONSTEXPR const char* info_header{ "[INFO]" };
	INLINE CONSTEXPR const char* log_header{ "[LOG]" };
	INLINE CONSTEXPR const char* msg_header{ "[MSG]" };
	INLINE CONSTEXPR const char* warn_header{ "[WARN]" };
	INLINE CONSTEXPR const char* error_header{ "[ERROR]" };
	INLINE CONSTEXPR const char* crit_header{ "[CRIT]" };

	INLINE CONSTEXPR const char* debug{ "\033[38;5;99m[DEBUG]\033[38;5;7m" };
	INLINE CONSTEXPR const char* info{ "\033[38;5;246m[INFO]\033[38;5;7m" };
	INLINE CONSTEXPR const char* log{ "\033[38;5;7m[LOG]\033[38;5;7m" };
	INLINE CONSTEXPR const char* msg{ "\033[38;5;2m[MSG]\033[38;5;7m" };
	INLINE CONSTEXPR const char* warn{ "\033[38;5;208m[WARN]\033[38;5;7m" };
	INLINE CONSTEXPR const char* error{ "\033[38;5;1m[ERROR]\033[38;5;7m" };
	INLINE CONSTEXPR const char* crit{ "\033[38;5;88m[CRIT]\033[38;5;7m" };
}
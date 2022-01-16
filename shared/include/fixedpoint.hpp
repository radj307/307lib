#pragma once
#include <sysarch.h>
#include <var.hpp>

namespace math {
	template<var::arithmetic T> [[nodiscard]] INLINE CONSTEXPR T scale(const T& n) { return n >> 8; }
	template<var::arithmetic T> [[nodiscard]] INLINE CONSTEXPR T unscale(const T& n) { return n << 8; }


}

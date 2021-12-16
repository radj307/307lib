#pragma once
namespace term {
	namespace _internal {
		inline static bool CURSOR_MIN_AXIS{ 1 };
	}
	inline int setOriginPos(const bool& min_axis = 1)
	{
		const auto copy{ _internal::CURSOR_MIN_AXIS };
		_internal::CURSOR_MIN_AXIS = min_axis;
		return copy;
	}
}
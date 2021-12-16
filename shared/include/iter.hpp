#pragma once
#include <iterator>
#include <concepts>

namespace iter {
	template<class IteratorT, class FunctionT>
	static constexpr FunctionT for_each(const IteratorT& begin, const IteratorT& end, const FunctionT& func)
	{
		for (auto it{ begin }; it != end; ++it)
			func(*it, std::distance(it, end) == 1ull);
		return func;
	}
}
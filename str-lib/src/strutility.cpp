#include "strutility.hpp"


template<template<class, class> class ContType, class T>
static std::ostream& str::op::operator<<(std::ostream& os, const ContType<T, std::allocator<T>>& cont) noexcept
{
	try {
		for (const auto& i : cont)
			os << i << ' ';
	} catch (...) {}
	return os;
}

template<class T> [[nodiscard]] constexpr bool str::pos_valid(const T pos)
{
	return static_cast<size_t>(pos) != std::string::npos;
}


template<std::integral RT, template<class, class> class Cont> [[nodiscard]] RT str::get_longest_string(const Cont<std::string, std::allocator<std::string>>& cont, RT offset)
{
	size_t longest{ 0llu };
	for (auto& it : cont)
		if (const auto sz{ it.size() }; sz > longest)
			longest = sz;
	return offset + static_cast<RT>(longest);
}
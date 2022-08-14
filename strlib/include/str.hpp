/**
 * @file	str.hpp
 * @author	radj307
 * @brief	Includes all of the str namespace extension headers, and additional functions related to string manipulation & conversions.
 */
#pragma once
#include <stringify.hpp>
#include <strconv.hpp>			// str library component that includes string conversion functions.
#include <strmanip.hpp>			// str library component that includes string manipulation functions.
#include <strlocale.hpp>		// str library component that includes <locale>-based string functions.
#include <strutility.hpp>
#include <strmath.hpp>
#include <strcompare.hpp>
#include <var.hpp>

#include <iomanip>				// for std::setw & other std::iostream manipulation functions.



namespace str {
	/**
	 * @brief
	 * @tparam ContainerT	- Container Type. Accepts most STL containers that hold a single type, such as: std::vector, std::set, etc.
	 * @tparam ElemT		- Templated Element Type. Must be compatible with ostream::operator<<.
	 * @tparam ...VT		- Variadic Templated Separators. These are inserted in order between every element in the container.
	 * @param container		- Input Container. Element type must have an overload for operator<< to allow insertion into an ostream.
	 * @param ...separators	- Any number of separators. These are inserted in order between every element. This can be used to apply formatting, insert spaces, or anything else. Note that separators are only inserted BETWEEN elements, and will never be trailing!
	 * @returns std::string
	 */
	template<template<class, class> class ContainerT, class ElemT, var::streamable... VT> requires std::convertible_to<ElemT, std::string>
	[[nodiscard]] constexpr static const std::string join(const ContainerT<ElemT, std::allocator<ElemT>>& container, VT... separators)
	{
		std::stringstream buffer;
		for (auto element{ container.begin() }; element != container.end(); ++element) {
			if (element != container.begin())
				(buffer << ... << separators);
			buffer << static_cast<std::string>(*element);
		}
		return std::move(buffer.str());
	}

	template<var::valid_char TChar = char, std::derived_from<std::char_traits<TChar>> TCharTraits = std::char_traits<TChar>, std::derived_from<std::allocator<TChar>> TAlloc = std::allocator<TChar>, var::streamable<std::basic_stringstream<TChar, TCharTraits, TAlloc>>... Ts>
	[[nodiscard]] constexpr static const std::basic_string<TChar, TCharTraits, TAlloc> join_with_delimiter(std::basic_string<TChar, TCharTraits, TAlloc> const& prefix, std::basic_string<TChar, TCharTraits, TAlloc> const& suffix, std::basic_string<TChar, TCharTraits, TAlloc> const& delimiter, Ts&&... s)
	{
		return join(std::vector<std::basic_string<TChar, TCharTraits, TAlloc>>{ str::stringify<TChar, TCharTraits, TAlloc>(prefix, std::forward<Ts>(s), suffix)... }, delimiter);
	}
	template<var::valid_char TChar = char, std::derived_from<std::char_traits<TChar>> TCharTraits = std::char_traits<TChar>, std::derived_from<std::allocator<TChar>> TAlloc = std::allocator<TChar>, var::streamable<std::basic_stringstream<TChar, TCharTraits, TAlloc>>... Ts>
	[[nodiscard]] constexpr static const std::basic_string<TChar, TCharTraits, TAlloc> join_with_delimiter(const TChar* prefix, const TChar* suffix, const TChar* delimiter, Ts&&... s)
	{
		return join(std::vector<std::basic_string<TChar, TCharTraits, TAlloc>>{ str::stringify<TChar, TCharTraits, TAlloc>(prefix, std::forward<Ts>(s), suffix)... }, delimiter);
	}

	template<var::valid_string_or_convertible T, class Pred, var::valid_string_or_convertible... Ts>
	static T compare(const Pred& predicate, const T& fst, const Ts&... strings)
	{
		std::unique_ptr<T> ptr{ nullptr };
		for (auto& it : var::variadic_accumulate<T>(fst, strings...))
			if (ptr.get() == nullptr || predicate(*ptr.get(), it))
				ptr = std::make_unique<T>(it);
		return { ptr != nullptr ? *ptr.get() : T{} };
	}
	template<var::valid_string_or_convertible T, var::valid_string_or_convertible... Ts>
	static T longest(const T& fst, const Ts&... strings) { return compare<T>([](const T& l, const T& r) { return l.size() < r.size(); }, fst, strings...); }
	template<var::valid_string_or_convertible T, var::valid_string_or_convertible... Ts>
	static T shortest(const T& fst, const Ts&... strings) { return compare<T>([](const T& l, const T& r) { return l.size() > r.size(); }, fst, strings...); }

	template<var::valid_string T, template<class, class> class Cont>
	static Cont<T, std::allocator<T>>::const_iterator longest(const Cont<T, std::allocator<T>>& strings)
	{
		auto longest{ strings.end() };
		for (auto str{ strings.begin() }; str != strings.end(); ++str)
			if (longest == strings.end() || str->size() > longest->size())
				longest = str;
		return longest;
	}
	template<var::valid_string T, template<class, class> class Cont>
	static Cont<T, std::allocator<T>>::const_iterator shortest(const Cont<T, std::allocator<T>>& strings)
	{
		auto shortest{ strings.end() };
		for (auto str{ strings.begin() }; str != strings.end(); ++str)
			if (shortest == strings.end() || str->size() < shortest->size())
				shortest = str;
		return shortest;
	}

	template<bool index, var::valid_string T, template<class...> class Cont>
	static Cont<std::pair<T, T>, std::allocator<std::pair<T, T>>>::const_iterator longest(const Cont<std::pair<T, T>, std::allocator<std::pair<T, T>>>& strings)
	{
		auto longest{ strings.end() };
		for (auto strpr{ strings.begin() }; strpr != strings.end(); ++strpr)
			if (longest == strings.end() || (!index ? strpr->first.size() : strpr->second.size()) > (!index ? longest->first.size() : longest->second.size()))
				longest = strpr;
		return longest;
	}
	template<bool index, var::valid_string T, template<class...> class Cont>
	static Cont<std::pair<T, T>, std::allocator<std::pair<T, T>>>::const_iterator shortest(const Cont<std::pair<T, T>, std::allocator<std::pair<T, T>>>& strings)
	{
		auto shortest{ strings.end() };
		for (auto strpr{ strings.begin() }; strpr != strings.end(); ++strpr)
			if (shortest == strings.end() || (!index ? strpr->first.size() : strpr->second.size()) < (!index ? shortest->first.size() : shortest->second.size()))
				shortest = strpr;
		return shortest;
	}

	/**
	 * @brief			Get the longest string in a container of string tuples.
	 * @tparam index	Which index to call std::get on each time. Ex: 0 will compare the first string in each tuple.
	 * @tparam ...vT	Variadic type contained by the tuple.
	 * @tparam Cont		Container type containing std::tuples.
	 * @param strings	Container
	 * @returns			Cont::const_iterator
	 */
	template<size_t index, var::valid_string... Ts, template<class...> class Cont>
	static Cont<std::tuple<Ts...>, std::allocator<std::tuple<Ts...>>>::const_iterator longest(const Cont<std::tuple<Ts...>, std::allocator<std::tuple<Ts...>>>& strings)
	{
		static_assert(index < sizeof...(Ts), "str::longest()\tCannot specify out-of-bounds index!");
		auto longest{ strings.end() };
		for (auto strtpl{ strings.begin() }; strtpl != strings.end(); ++strtpl)
			if (longest == strings.end() || std::get<index>(*strtpl).size() > std::get<index>(*longest).size())
				longest = strtpl;
		return longest;
	}
	/**
	 * @brief			Get the shortest string in a container of string tuples.
	 * @tparam index	Which index to call std::get on each time. Ex: 0 will compare the first string in each tuple.
	 * @tparam ...vT	Variadic type contained by the tuple.
	 * @tparam Cont		Container type containing std::tuples.
	 * @param strings	Container
	 * @returns			Cont::const_iterator
	 */
	template<size_t index, var::valid_string... Ts, template<class...> class Cont>
	static Cont<std::tuple<Ts...>, std::allocator<std::tuple<Ts...>>>::const_iterator shortest(const Cont<std::tuple<Ts...>, std::allocator<std::tuple<Ts...>>>& strings)
	{
		static_assert(index < sizeof...(Ts), "str::longest()\tCannot specify out-of-bounds index!");
		auto shortest{ strings.end() };
		for (auto strtpl{ strings.begin() }; strtpl != strings.end(); ++strtpl)
			if (shortest == strings.end() || std::get<index>(*strtpl).size() < std::get<index>(*shortest).size())
				shortest = strtpl;
		return shortest;
	}

	/**
	 * @brief Compare 2 strings.
	 * @tparam T1				- Convertible to string type.
	 * @tparam T2				- Convertible to string type.
	 * @param left				- Comparison target 1
	 * @param right				- Comparison target 2
	 * @param case_sensitive	- When false, both strings are first converted to lowercase, then compared.
	 * @returns bool
	 */
	template<class T1, class T2> requires std::convertible_to<T1, std::string>&& std::convertible_to<T2, std::string>
	static bool compare(T1 left, T2 right, const bool case_sensitive = true)
	{
		const std::string l{ case_sensitive ? left : tolower(std::string(left)) }, r{ case_sensitive ? right : tolower(std::string(right)) };
		return l == r;
	}

#pragma region concepts
	/**
	 * @concept ConvertibleStringT
	 * @brief Allows types that are constructor-convertible to std::string
	 * @tparam T	- Input Type
	 */
	template<class... T> concept ConvertibleStringT = std::constructible_from<std::string, T...>;
#pragma endregion concepts

	/**
	 * @struct Printable
	 * @brief A generic wrapper for any number of any types. Allows functions to return an inline-printable value so the function can be used in an output stream operator<< statement.
	 * @tparam ...VT	- Variadic Templated Types.
	 */
	template<class... VT>
	struct Printable {
		const std::string _div;
		const std::tuple<VT...> _values;
		/**
		 * @brief Default Constructor
		 * @param divider	- This string will be inserted between each value, excluding before the first, and after the last, value.
		 * @param ...values	- Arguments to print out, in order.
		 */
		constexpr Printable(std::string divider, const VT&... values) : _div{ std::move(divider) }, _values{ values... } {}
		constexpr operator const std::tuple<VT...>() const { return _values; }
		friend std::ostream& operator<<(std::ostream& os, const Printable<VT...>& obj)
		{
			constexpr const size_t sz{ sizeof...(VT) };
			for (size_t i{ 0ull }; i < sz; ++i) {
				os << std::get<i>(obj._values);
				if (i < sz - 1ull)
					os << obj._div;
			}
			return os;
		}
	};

	/**
	 * @brief std::string::find function that returns an optional instead of std::string::npos
	 * @param str		- String to search within.
	 * @param search	- Character(s) to search for.
	 * @returns std::optional<size_t>
	 */
	inline std::optional<size_t> optional_find(const std::string& str, auto&& search)
	{
		if (const auto pos{ str.find(std::forward<decltype(search)>(search)) }; pos_valid(pos))
			return pos;
		return std::nullopt;
	}

	template<typename... VT> requires (sizeof...(VT) > 0)
		static std::optional<size_t> optional_find(const std::string& str, VT&&... search)
	{
		for (auto& s : var::variadic_accumulate<std::string>(std::string{ std::forward<decltype(search)>(search) }...))
			if (const auto pos{ optional_find(str, s) }; pos.has_value())
				return pos;
		return std::nullopt;
	}

	using format::indent;
}

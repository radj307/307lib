/**
 * @file	str.hpp
 * @author	radj307
 * @brief	Includes all of the str namespace extension headers, and additional functions related to string manipulation & conversions.
 */
#pragma once
#include <strconv.hpp>			// str library component that includes string conversion functions.
#include <strmanip.hpp>			// str library component that includes string manipulation functions.
#include <strlocale.hpp>		// str library component that includes <locale>-based string functions.
#include <var.hpp>

#include <iomanip>				// for std::setw & other std::iostream manipulation functions.

 /// Define DISABLE_STR_LITERALS to disable adding std::string_literals to the global namespace.
/**
 * @def		DISABLE_STR_LITERALS
 * @brief	Disables the "using namespace std::string_literals;" from being declared in the global namespace.
 */
#ifndef DISABLE_STR_LITERALS
#define DISABLE_STR_LITERALS // for doxygen to see the definition
#undef DISABLE_STR_LITERALS
using namespace std::string_literals;
#endif

/**
 * @brief Test that checks if a given char is a quotation mark. (Accepts single or double quotes.)
 * @param c	- Input char
 * @returns bool
 */
inline constexpr bool isquote(const char c)
{
	switch (c) {
	case '\"': [[fallthrough]];
	case '\'':
		return true;
	default:
		return false;
	}
}

namespace str {
	/**
	 *	disable "use of a moved from object 'buffer'", speed comparison testing shows a major performance
	 *	increase when using return-move() for these functions. (~300000ns)
	 */
#pragma warning (disable: 26800) 
	 /**
	  * @brief			Creates a stringstream, inserts all of the given arguments, then move-returns the resulting stringstream.
	  * @tparam ...Ts	Variadic Templated Arguments.
	  * @param ...args	Arguments to insert into the stream, in order.
	  * @returns		std::stringstream
	  */
	template<var::Streamable... Ts>
	[[nodiscard]] static std::stringstream streamify(Ts&&... args)
	{
		std::stringstream buffer;
		(buffer << ... << std::forward<Ts>(args));
		return std::move(buffer);
	}

	/**
	 * @brief Creates a temporary stringstream, inserts all of the given arguments, then returns the result of the stringstream's str() function.
	 * @tparam ...Ts	- Variadic Templated Arguments.
	 * @param ...args	- Arguments to insert into the stream, in order. Nearly anything can be included here, so long as it has an operator<< stream insertion operator.
	 * @returns std::string
	 */
	template<var::Streamable... Ts>
	[[nodiscard]] constexpr static const std::string stringify(Ts&&... args)
	{
		if constexpr (var::none<Ts...>)
			return{};
		std::stringstream buffer;	// init stringstream
		(buffer << ... << std::forward<Ts>(args));	// insert variadic arguments in order
		return std::move(buffer.str());		// return as string
	}

	/**
	 * @brief
	 * @tparam ContainerT	- Container Type. Accepts most STL containers that hold a single type, such as: std::vector, std::set, etc.
	 * @tparam ElemT		- Templated Element Type. Must be compatible with ostream::operator<<.
	 * @tparam ...VT		- Variadic Templated Separators. These are inserted in order between every element in the container.
	 * @param container		- Input Container. Element type must have an overload for operator<< to allow insertion into an ostream.
	 * @param ...separators	- Any number of separators. These are inserted in order between every element. This can be used to apply formatting, insert spaces, or anything else. Note that separators are only inserted BETWEEN elements, and will never be trailing!
	 * @returns std::string
	 */
	template<template<class, class> class ContainerT, class ElemT, var::Streamable... VT> requires std::convertible_to<ElemT, std::string>
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

	inline static size_t count(std::stringstream& ss, char delim)
	{
		size_t count{ 0ull };
		for (std::string sbuf; std::getline(ss, sbuf, delim); ++count) {}
		return count;
	}

	/**
	 * @brief Creates a temporary stringstream and inserts all of the given arguments in sequential order, then uses the given delimiter to split the resulting string into a vector of strings.
	 * @tparam DelimType	- Input Delimiter Type
	 * @tparam ...VT		- Variadic Templated Arguments.
	 * @param delimiter		- Delimiter to use when splitting the result.
	 * @param ...args		- Arguments to insert into the stream in sequential order.
	 * @returns std::vector<std::string>
	 */
	template<class DelimType, var::Streamable... Ts>
	[[nodiscard]] constexpr static const std::vector<std::string> stringify_split(const DelimType& delimiter, Ts&&... args)
	{
		std::stringstream buffer;
		(buffer << ... << std::forward<Ts>(args)) << delimiter;
		std::vector<std::string> vec;
		vec.reserve(count(buffer, delimiter));
		for (std::string sub{}; str::getline(buffer, sub, delimiter); vec.emplace_back(sub)) {}
		vec.shrink_to_fit();
		return std::move(vec);
	}

	/**
	 * @brief Creates a temporary stringstream and inserts all of the given arguments in sequential order, then uses the given delimiter to split the resulting string into a vector of strings.
	 * @tparam DelimType	- Input Delimiter Type
	 * @tparam ...VT		- Variadic Templated Arguments.
	 * @param delimiter		- Delimiter to use when splitting the result.
	 * @param ...args		- Arguments to insert into the stream in sequential order.
	 * @returns std::vector<std::string>
	 */
	template<class DelimType, var::Streamable T>
	[[nodiscard]] constexpr static const std::vector<std::string> stringify_split(const DelimType& delimiter, const std::vector<T>& args)
	{
		std::stringstream buffer;
		size_t count{ 0ull };
		for (auto it{ args.begin() }; it != args.end(); ++it, ++count) {
			buffer << *it;
			if (std::distance(it, args.end()) > 1)
				buffer << ' ';
		}
		std::vector<std::string> vec;
		vec.reserve(count);
		for (std::string sub{}; str::getline(buffer, sub, delimiter); vec.emplace_back(sub)) {}
		vec.shrink_to_fit();
		return std::move(vec);
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
#pragma warning (default: 26800) // re-enable moved-from-object warning

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

	/**
	 * @struct VariableIndentation
	 * @brief Uses the std::setw() function to create a variable amount of indentation in an output stream.
	 */
	struct VariableIndentation {
		const std::streamsize _max_width, _sub_width;
		const char _fill;

		/**
		 * @brief Default Constructor
		 * @param max_width			- This is the total amount of desired indentation, in characters.
		 * @param subtract_width	- This is the amount of width that has already been used, in characters. (To align multiple lines of varying length, this should be the length of the preceeding string, or 0 if there is no preceeding text.)
		 * @param fill_character	- Character to use as indent.
		 */
		constexpr VariableIndentation(std::streamsize max_width, std::streamsize subtract_width = 0, char fill_character = ' ') : _max_width{ std::move(max_width) }, _sub_width{ std::move(subtract_width) }, _fill{ std::move(fill_character) } {}

		// Returns the actual amount of indentation
		constexpr operator std::streamsize() const { return _max_width - _sub_width; }

		// Insert the indentation into the given output stream
		friend std::ostream& operator<<(std::ostream& os, const VariableIndentation& indent)
		{
			if (indent._max_width == 0ull)
				return os;
			return os << std::setfill(indent._fill) << std::setw(indent.operator std::streamsize()) << indent._fill;
		}
	};
	using VIndent = VariableIndentation;

	/**
	 * @brief				Split a string into a pair of strings by position. This is a simple wrapper to return a string split operation as a pair of strings.
	 * @param str			Input String to Split.
	 * @param pos			Position in the string to split.
	 * @param discard_pos	When true, the character at the given position is discarded, otherwise the character is the first character of the second return value.
	 * @returns				std::pair<std::string, std::string>
	 */
	inline WINCONSTEXPR const std::pair<std::string, std::string> split(const std::string& str, const size_t& pos, const bool& discard_pos = false)
	{
		return{ str.substr(0ull, pos), str.substr(pos + !!discard_pos) };
	}

	/**
	 * @brief		Split a string into a pair of strings by finding the first occurrence of a given delimiter.
	 * @param str	Input String to Split.
	 * @param delim	Delimiter to search for. The delimiter is discarded from the string.
	 * @returns		std::pair<std::string, std::string>
	 */
	inline WINCONSTEXPR const std::pair<std::string, std::string> split(const std::string& str, const char& delim, const unsigned& occurrence = 0ull)
	{
		unsigned matched{ 0u };
		for (size_t pos{ str.find(delim) }; pos_valid(pos); pos = str.find(delim, pos + 1ull))
			if (occurrence == matched++)
				return{ str.substr(0ull, pos), str.substr(pos + 1ull) };
		return{ str, {} };
	}

	/**
	 * @brief		Split a string into a pair of strings by finding the first occurrence of a given delimiter.
	 * @param str	Input String to Split.
	 * @param delim	Delimiter to search for. The delimiter is discarded from the string.
	 * @returns		std::pair<std::string, std::string>
	 */
	inline WINCONSTEXPR const std::pair<std::string, std::string> rsplit(const std::string& str, const char& delim, const unsigned& occurrence = 0ull)
	{
		unsigned matched{ 0u };
		for (size_t pos{ str.rfind(delim) }; pos_valid(pos); pos = str.rfind(delim, pos + 1ull))
			if (occurrence == matched++)
				return{ str.substr(0ull, pos), str.substr(pos + 1ull) };
		return{ str, {} };
	}

	/**
	 * @brief				Remove trailing characters from a string.
	 * @tparam DelimT		Variadic Input Types.
	 * @param float_str		Input String.
	 * @param delims...		At least one character to remove from the end of the string. These are casted to char before comparing!
	 * @returns				std::string_view
	 */
	template<std::same_as<char>... DelimT>
	inline static std::string strip_trailing(const std::string& str, const DelimT&... delims)
	{
		static_assert(sizeof...(DelimT) > 0, "strip_trailing() requires at least one delimiter char!");
		size_t i{ 0ull };
		for (auto rit{ str.rbegin() }; rit != str.rend(); ++rit) {
			if (var::variadic_or(*rit == static_cast<char>(delims)...))
				++i;
			else break;
		}
		return str.substr(0ull, str.size() - i);
	}
	/**
	 * @brief				Remove preceeding characters from a string.
	 * @tparam DelimT		Variadic Input Types.
	 * @param float_str		Input String.
	 * @param delims...		At least one character to remove from the beginning of the string. These are casted to char before comparing!
	 * @returns				std::string_view
	 */
	template<std::same_as<char>... DelimT>
	inline static std::string strip_preceeding(const std::string& str, const DelimT&... delims)
	{
		static_assert(sizeof...(DelimT) > 0, "strip_preceeding() requires at least one delimiter char!");
		size_t i{ 0ull };
		for (auto it{ str.begin() }; it != str.end(); ++it) {
			if (var::variadic_or(*it == static_cast<char>(delims)...))
				++i;
			else break;
		}
		return str.substr(i);
	}

	/**
	 * @brief			Remove all specified characters from the given string.
	 * @param str		Input String
	 * @param delims	A string containing all of the blacklisted characters.
	 * @returns			std::string
	 */
	inline WINCONSTEXPR static std::string strip(std::string s, const std::string_view& delims)
	{
		if (!s.empty() && !delims.empty())
			s.erase(std::remove_if(s.begin(), s.end(), [&delims](auto&& ch) {return delims.find(ch) != std::string::npos; }), s.end());
		return s;
	}

	/**
	 * @brief			Remove all specified characters from the given string.
	 * @param str		Input String
	 * @param ...delims	Any number of characters to remove from the given string.
	 * @returns			std::string
	 */
	template<std::same_as<char>... DelimT>
	inline WINCONSTEXPR static std::string strip(std::string s, const DelimT&... delims)
	{
		s.erase(std::remove_if(s.begin(), s.end(), [&delims...](auto&& ch) { return var::variadic_or(ch == delims...); }), s.end());
		return s;
	}

	/**
	 * @brief				Variadic variant of the getline function.
	 * @tparam Ch			Char Type
	 * @tparam Tr			Char Traits
	 * @tparam A			Allocator Type
	 * @tparam DelimT...	Any number of types that are the same or convertible to Ch
	 * @param is			Input stream ref
	 * @param str			Output string ref
	 * @param delimiters	Any number of characters to use as delimiters.
	 * @returns				basic_istream<Ch, Tr>&
	 */
	template<class Ch, class Tr, class A, var::same_or_convertible<Ch>... DelimT>
	std::basic_istream<Ch, Tr>& getline(std::basic_istream<Ch, Tr>& is, std::basic_string<Ch, Tr, A>& str, DelimT const&... delims)
	{
		std::string::size_type nread{ 0 }; // number of characters read
		if (std::istream::sentry(is, true)) {
			std::streambuf* sbuf{ is.rdbuf() };
			str.clear();
			while (nread < str.max_size()) {
				auto c1{ sbuf->sbumpc() };
				if (Tr::eq_int_type(c1, Tr::eof())) {
					is.setstate(std::istream::eofbit);
					break;
				}
				++nread;
				if (const Ch ch{ Tr::to_char_type(c1) }; !var::variadic_or(ch == delims...))
					str.push_back(ch);
				else break;
			}
		}
		if (nread == 0 || nread >= str.max_size())
			is.setstate(std::istream::failbit);
		return is;
	}

	/**
	 * @brief				Check if the given string starts with the given characters.
	 * @param str			Input String.
	 * @param ...prefix		Any number of types that can be represented with a string to check for.
	 * @returns				bool
	 */
	template<var::Streamable<std::stringstream>... Ts>
	inline WINCONSTEXPR bool startsWith(const std::string& str, Ts&&... prefix)
	{
		if constexpr (sizeof...(Ts) == 0ull)
			return false;
		else if (str.empty())
			return false;

		const std::string& comp{ stringify(std::forward<Ts>(prefix)...) };

		if (comp.size() > str.size())
			return false;

		size_t matches{ 0ull };
		for (auto itstr{ str.begin() }, itstrEnd{ str.end() }, itcomp{ comp.begin() }, itcompEnd{ comp.end() }; itstr < itstrEnd && itcomp < itcompEnd; ++itstr, ++itcomp) {
			if (*itstr == *itcomp)
				++matches;
			else return false;
		}
		return matches == comp.size();
	}

	/**
	 * @brief				Check if the given string ends with the given characters.
	 * @param str			Input String.
	 * @param ...suffix		Any number of types that can be represented with a string to check for.
	 *\n					The resolved strings are checked in reverse sequential order.
	 * @returns				bool
	 */
	template<var::Streamable<std::stringstream>... Ts>
	inline WINCONSTEXPR bool endsWith(const std::string& str, Ts&&... suffix)
	{
		if constexpr (sizeof...(Ts) == 0ull)
			return false;
		else if (str.empty())
			return false;

		const std::string& comp{ stringify(std::forward<Ts>(suffix)...) };

		if (comp.size() > str.size())
			return false;

		size_t matches{ 0ull };
		for (auto itstr{ str.rbegin() }, itstrEnd{ str.rend() }, itcomp{comp.rbegin()}, itcompEnd{ comp.rend() }; itstr < itstrEnd && itcomp < itcompEnd; ++itstr, ++itcomp) {
			if (*itstr == *itcomp)
				++matches;
			else return false;
		}
		return matches == comp.size();
	}

	/**
	 * @brief				Check if the given string is equal to at least one other string.
	 * @param str			Input string to compare.
	 * @param ...compare	Comparison Strings.
	 * @returns				bool
	 */
	template<bool IGNORE_CASE = false, var::all_same_or_convertible<std::string>... Ts>
	inline WINCONSTEXPR bool equalsAny(const std::string& str, Ts&&... compare)
	{
		if constexpr (IGNORE_CASE) {
			const auto& lc{ str::tolower(str) };
			return var::variadic_or(lc == str::tolower(compare)...);
		}
		return var::variadic_or(str == compare...);
	}
	/**
	 * @brief				Check if the given string is equal to at least one other string.
	 * @param str			Input string to compare.
	 * @param ...compare	Comparison Strings.
	 * @returns				bool
	 */
	template<bool IGNORE_CASE = false, var::all_same_or_convertible<std::wstring, char>... Ts>
	inline WINCONSTEXPR bool equalsAny(const std::wstring& str, Ts&&... compare)
	{
		if constexpr (IGNORE_CASE) {
			const auto& lc{ str::tolower(str) };
			return var::variadic_or(lc == str::tolower(compare)...);
		}
		return var::variadic_or(str == compare...);
	}

	/**
	 * @brief				Check if a given string matches any of a list of strings.
	 * @tparam MatchCase	When true, matches are case sensitive, otherwise they are case insensitive.
	 * @tparam StrT			String Type.
	 * @tparam Ts...		Variadic number of types that are the same as StrT.
	 * @param str			Input String
	 * @param matches		At least one string to compare to str.
	 * @returns bool
	 */
	template<bool MatchCase = true, var::valid_string StrT, var::same_or_convertible<StrT>... Ts> requires var::at_least_one<Ts...>
	inline WINCONSTEXPR bool matches_any(const StrT& str, const Ts&... matches)
	{
		if constexpr (!MatchCase)
			return var::variadic_or((str::tolower(str) == str::tolower(matches))...);
		return var::variadic_or((str == matches)...);
	}
}

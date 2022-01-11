/**
 * @file	strmanip.hpp
 * @author	radj307
 * @brief	Contains an assortment of string manipulation functions & methods.
 *\n		This contains the majority of str namespace functions that do not use
 *\n		variadic templates or other more advanced concepts. (such as concepts)
 */
#pragma once
#include <algorithm>
#include <functional>
#include <map>
#include <optional>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <strutility.hpp>

 /**
  * @namespace	str
  * @brief		Contains various string manipulation, conversion, and parsing functions
  */
namespace str {
	/**
	 * @brief		Reverse the characters of a given string.
	 * @param str	Input String
	 * @returns		std::string
	 */
	inline std::string reverse(const std::string_view& str)
	{
		std::string tmp;
		tmp.reserve(str.size());
		for (auto ch{ str.rbegin() }; ch != str.rend(); ++ch)
			tmp.push_back(*ch);
		tmp.shrink_to_fit();
		return tmp;
	}

	/**
	 * @brief			Split a string into a vector of characters.
	 * @tparam CharT	Character Type
	 * @tparam Traits	Character Traits
	 * @param str		Input String
	 * @returns			std::vector<CharT>
	 */
	template<typename CharT, typename Traits>
	inline std::vector<CharT> split_to_vector(const std::basic_string<CharT, Traits, std::allocator<CharT>>& str)
	{
		std::vector<CharT> vec;
		vec.reserve(str.size());
		for (auto& it : str)
			vec.emplace_back(it);
		vec.shrink_to_fit();
		return vec;
	}

	/**
	 * @brief Find any of a given set of delimiters in a string.
	 * @param ln	- String to search.
	 * @param dels	- Delimiters to search for.
	 * @param off	- Position to begin searching at.
	 * @returns std::string::iterator
	 */
	inline std::string::iterator find_any(std::string ln, const std::string_view& dels, const std::string::iterator& off)
	{
		return std::find_if(off, ln.end(), [&dels](const char c) -> bool { return str::pos_valid(dels.find(c)); });
	}
	/**
	 * @brief		Find any of a given set of delimiters in a string.
	 * @param ln	String to search.
	 * @param dels	Delimiters to search for.
	 * @returns		std::string::iterator
	 */
	inline std::string::iterator find_any(std::string ln, const std::string_view& dels)
	{
		return find_any(std::move(ln), dels);
	}

	/**
	 * @brief					Removes comments and preceeding/trailing whitespace from a given string.
	 * @param str				Input string.
	 * @param comment_chars		Characters that should be treated as line comments, everything that appears after one of these characters is removed.
	 * @param whitespace_chars	Characters that should be treated as whitespace, trailing/preceeding whitespace is removed.
	 * @returns					std::string
	 */
	inline std::string strip_line(std::string str, const std::string_view& comment_chars = {}, const std::string_view& whitespace_chars = " \t\r\n\v")
	{
		if (const int dPos{ static_cast<signed>(str.find_first_of(comment_chars)) }; !str.empty() && pos_valid(dPos)) // remove comments first
			str.erase(str.begin() + dPos, str.end());
		const auto is_whitespace{ [&whitespace_chars](const char c) { return pos_valid(whitespace_chars.find(c)); } };
		if (const int first{ static_cast<signed>(str.find_first_not_of(whitespace_chars)) }; !str.empty() && pos_valid(first))
			str.erase(std::remove_if(str.begin(), str.begin() + first, is_whitespace), str.begin() + first); // remove from beginning of string to 1 before first non-whitespace char
		else return{};
		if (const int last{ static_cast<signed>(str.find_last_not_of(whitespace_chars)) }; !str.empty() && pos_valid(last))
			str.erase(std::remove_if(str.begin() + last, str.end(), is_whitespace), str.end()); // remove from 1 after last non-whitespace char to the end of the string
		return str;
	}
	/**
	 * @function strip_line(std::string, const Param&)
	 * @brief Removes more complex comments ( ex. C/JSON/html comments ) and preceeding/trailing whitespace from a given string.
	 * @param str				- Input string.
	 * @param comment_del		- String that should be treated as a comment.
	 * @param whitespace_chars	- Characters that should be treated as whitespace, trailing/preceeding whitespace is removed.
	 * @returns std::string
	 */
	inline std::string strip_line_multiCharComment(std::string str, const std::vector<std::string>& comment_del = {}, const std::string_view& whitespace_chars = " \t\r\n\v")
	{
		for (auto& del : comment_del)
			if (const int dPos{ static_cast<signed>(str.find(del)) }; !str.empty() && pos_valid(dPos)) // remove comments first
				str.erase(str.begin() + dPos, str.end());
		const auto is_whitespace{ [&whitespace_chars](const char c) { return pos_valid(whitespace_chars.find(c)); } };
		if (const int first{ static_cast<signed>(str.find_first_not_of(whitespace_chars)) }; !str.empty() && pos_valid(first))
			str.erase(std::remove_if(str.begin(), str.begin() + first, is_whitespace), str.begin() + first); // remove from beginning of string to 1 before first non-whitespace char
		else return{};
		if (const int last{ static_cast<signed>(str.find_last_not_of(whitespace_chars)) }; !str.empty() && pos_valid(last))
			str.erase(std::remove_if(str.begin() + last, str.end(), is_whitespace), str.end()); // remove from 1 after last non-whitespace char to the end of the string
		return str;
	}

	/**
	 * @function concat(T&&...)
	 * @brief Concatenate any number of string/char* types into a single string.
	 * @arg ...T	- Variadic String Types
	 * @param str	- An arbitrary number of strings.
	 * @returns string
	 */
	template<typename... T> [[nodiscard]] static std::string concat(T&&... str)
	{
		if constexpr (sizeof...(str) > 1) {
			std::stringstream ss;
			(ss << ... << std::forward<T>(str));
			return ss.str();
		}
		return{};
	}

	/**
	 * @function align_center(string&, unsigned int)
	 * @brief Centers a string in the specified number of characters by addings spaces on either side.
	 * @param str	- Input String, must be shorter in length than param setw.
	 * @param setw	- Number of characters in the returned string.
	 * @returns string
	 */
	inline std::string align_center(const std::string_view& str, const unsigned int setw)
	{
		if (str.size() < setw) {
			const auto width{ (setw - str.size()) / 2 }, mod{ (str.size() - 1) % 2 };
			std::string ret(width - 1u, ' ');
			ret += str;
			ret += std::string(width - mod, ' ');
			return ret;
		}
		return std::string{ str };
	}

#pragma endregion center_str

	/**
	 * @function remove_whitespace(string)
	 * @brief Inline wrapper for the erase-remove idiom that removes spaces.
	 * @param str	- Pass-by-value string
	 * @returns string
	 */
	inline std::string remove_whitespace(std::string str) noexcept
	{
	#if defined(OS_LINUX) || LANG_CPP <= 17
		str.erase(std::remove_if(str.begin(), str.end(), isspace), str.end());
	#elif LANG_CPP >= 20
		str.erase(std::ranges::remove_if(str, isspace).begin(), str.end());
	#endif
		return str;
	}

	/**
	 * @function getline(basic_istream<Ch, Tr>&, basic_string<Ch, Tr, A>&, Pred)
	 * @brief Alternative std::getline that accepts a predicate function rather than a char delimiter.
	 * @tparam Ch	- Char Type
	 * @tparam Tr	- Char Traits
	 * @tparam A	- Allocator type
	 * @tparam Pred	- Predicate type
	 * @param is	- Input stream
	 * @param str	- Output string
	 * @param p		- Predicate
	 * @returns basic_istream<Ch, Tr>&
	*/
	template <class Ch, class Tr, class A, class Pred> std::basic_istream<Ch, Tr>& getline_pred(std::basic_istream<Ch, Tr>& is, std::basic_string<Ch, Tr, A>& str, Pred p)
	{
		std::string::size_type nread{ 0 };
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
				if (const Ch ch{ Tr::to_char_type(c1) }; !p(ch))
					str.push_back(ch);
				else break;
			}
		}
		if (nread == 0 || nread >= str.max_size())
			is.setstate(std::istream::failbit);
		return is;
	}

	/**
	 * @function getline(basic_istream<Ch, Tr>&, basic_string<Ch, Tr, A>&, string&)
	 * @brief Alternative std::getline that accepts a string of delimiters rather than a single char.
	 * @tparam Ch		- Char Type
	 * @tparam Tr		- Char Traits
	 * @tparam A		- Allocator Type
	 * @param is		- Input Stream
	 * @param str		- Output String
	 * @param delims	- String of Delimiters
	 * @returns basic_istream<Ch, Tr>&
	 */
	template<class Ch, class Tr, class A>
	std::basic_istream<Ch, Tr>& getline(std::basic_istream<Ch, Tr>& is, std::basic_string<Ch, Tr, A>& str, const std::string& delimiters)
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
				if (const Ch ch{ Tr::to_char_type(c1) }; !std::any_of(delimiters.begin(), delimiters.end(), [&ch](const char c) { return c == ch; }))
					str.push_back(ch);
				else break;
			}
		}
		if (nread == 0 || nread >= str.max_size())
			is.setstate(std::istream::failbit);
		return is;
	}

	using std::getline;

	/**
	 * @function split_by_words(string&)
	 * @brief Splits a given string into a vector of strings, with one word per index.
	 * @param line	- Input string
	 * @returns vector<string>
	*/
	inline std::vector<std::string> split_by_words(const std::string& line)
	{
		std::vector<std::string> vec;
		std::stringstream ss{ line };
		for (std::string parse{}; getline_pred(ss, parse, isspace); parse.clear())
			if (!parse.empty())
				vec.emplace_back(std::move(parse));
		return vec;
	}

	/**
	 * @function sanitize(string, F)
	 * @brief Sanitize a string with a single predicate function. This is called by remove_all_of().
	 * @tparam F	- Type returned by predicate function.
	 * @param str	- Input string.
	 * @param pred	- Input predicate function.
	 * @returns string
	 */
	template<typename F>
	static std::string sanitize(std::string str, F pred) noexcept
	{
		try {
			str.erase(std::remove_if(str.begin(), str.end(), pred), str.end());
		} catch (...) {}
		return str;
	}

	/**
	 * @function remove_all_of(string, F)
	 * @brief Inline wrapper for the erase-remove idiom that accepts a lambda predicate.
	 * @param str		- Input string.
	 * @param pred		- Valid predicate functions for std::remove_if. (Accepts 1 int/char, returns 1 int/char).
	 * @returns string
	 */
	template<typename F>
	constexpr std::string remove_all_of(std::string str, F pred) noexcept { return sanitize(str, pred); };

	/**
	 * @function remove_all(string&, string&)
	 * @brief Remove all occurrences of a given list of delimiters from a given string.
	 * @param str		- String to remove delimiters from.
	 * @param delims	- Delimiter list.
	 * @returns string&
	 */
	inline std::string remove_all(std::string str, const std::string& delims)
	{
	#if defined(OS_LINUX) || LANG_CPP <= 17
		str.erase(std::remove_if(str.begin(), str.end(), [&delims](const char c) -> bool { return delims.find(c) != std::string::npos; }), str.end());
	#elif LANG_CPP >= 20
		str.erase(std::ranges::remove_if(str, [&delims](const char c) -> bool { return delims.find(c) != std::string::npos; }).begin(), str.end());
	#endif
		return str;
	}

	/**
	 * @function apply_to_each(string&, )
	 * @brief Applies a given list of functions to each character in a given string.
	 * @param str		- String ref to modify
	 * @param funclist	- List of functions to apply
	 */
	inline void apply_to_each(std::string& str, const std::vector<std::function<int(int)>, std::allocator<std::function<int(int)>>>& funclist) noexcept
	{
		for (const auto& func : funclist)
			for (auto& it : str)
				try {
				it = static_cast<char>(func(it));
			} catch (std::exception&) {}
	}

	/**
	 * @function parseWithDelim(string&, char, optional<vector<function<int(int)>>>)
	 * @brief Parse a given string with a given delimiter, and return a vector of strings.
	 * @param str		- String to parse
	 * @param delim		- Delimiter to split string with
	 * @param remove_if	- (Default: nullopt) When defined, applies the erase-remove idiom with the given functions.
	 * @returns vector<string>
	 */
	template<template<class, class> class ContType>
	[[nodiscard]] static std::vector<std::string> parseWithDelim(std::string str, const char delim = ',', std::optional<ContType<std::function<int(int)>, std::allocator<std::function<int(int)>>>> remove_if = std::nullopt) noexcept
	{
		std::vector<std::string> vec;
		if (!str.empty()) {
			if (remove_if.has_value())
				remove_all_of(str, remove_if.value());
			std::stringstream ss{ str + delim }; // copy str & append a delim
			for (std::string parse{}; std::getline(ss, parse, delim); parse.clear())
				vec.emplace_back(parse);
		}
		return vec;
	}
#pragma region sstreams
	/**
	 * @function toVector(stringstream&)
	 * @brief Converts a stringstream to a vector of strings, where each string contains one line from the stream.
	 * @param data	- Stringstream ref to convert.
	 * @param delim	- Optional char delimiter
	 * @returns vector<string>
	 */
	inline std::vector<std::string> toVector(std::stringstream data, std::optional<char> delim = std::nullopt)
	{
		std::vector<std::string> vec;
		for (std::string parse; delim.has_value() ? std::getline(data, parse, delim.value()) : std::getline(data, parse); vec.push_back(parse)) {}
		return vec;
	}

	/**
	 * @function toMap\(stringstream&, string&, char\)
	 * @brief Parses a stringstream to a map of string keys & string vars
	 * @param data		- Stringstream ref
	 * @param delims	- String containing delimiter chars
	 * @param lineDelim	- (Default: '\\n') Character to use as delimiter between map entries
	 * @param noSpace	- (Default: true) When true, removes all whitespace after parsing each value.
	 * @returns map\<string, string\>
	 */
	inline std::map<std::string, std::string> toMap(std::stringstream& data, const std::string& delims, const char lineDelim = '\n', const bool noSpace = true) noexcept
	{
		std::map<std::string, std::string> map;
		for (std::string parse{}; std::getline(data, parse, lineDelim); parse.clear()) {
			if (const auto dPos{ parse.find_first_of(delims) }; dPos != std::string::npos && dPos != parse.size())
				map.insert(std::make_pair(noSpace ? remove_whitespace(parse.substr(0, dPos)) : parse.substr(0, dPos), noSpace ? remove_whitespace(parse.substr(dPos + 1)) : parse.substr(dPos + 1)));
		}
		return map;
	}

	/**
	 * @function getLength(stringstream)
	 * @brief Returns the length of a given stringstream.
	 * @param ss	- Ref to target stringstream
	 * @returns	stringstream::pos_type
	 */
	inline std::stringstream::pos_type getStreamLength(std::stringstream& ss) noexcept
	{
		try {
			ss.seekg(std::ios::end);
			const auto length{ ss.tellg() };
			ss.seekg(std::ios::beg);
			return length;
		} catch (...) { return{ 0u }; }
	}
#pragma endregion sstreams
#pragma region extractString
	/**
	 * @function extractString(string&, pair<char, char>, bool, bool)
	 * @brief Returns a substring from the first appearance of a pair of delimiters, if no delimiter is found, uses the beginning/end of the given str.
	 * @param line			- The string to extract from.
	 * @param delims		- ( first = opening delim ) ( second = closing delim )
	 * @param includeDelim	- (Default: false) When true, the returned string will contain first & last delims.
	 * @returns optional<string>
	 */
	[[nodiscard]] inline std::optional<std::string> extractString(const std::string& line, const std::pair<std::string, std::string> delims, const bool includeDelim = false) noexcept
	{
		try {
			// ReSharper disable once CppUseStructuredBinding
			const auto d1{ [&line, &delims](const std::string::size_type off) -> std::string::size_type {
				if (const auto dPos{ line.find_first_of(delims.first, off) }; dPos != std::string::npos)
					return dPos;
				return 0;
			}(0u)
			};
			if (const auto d2{ [&line, &delims](const std::string::size_type off) -> std::string::size_type {
				if (const auto dPos{ line.find_first_of(delims.second, off) }; dPos != std::string::npos)
					return dPos;
					return line.size();
				}(d1 + 1u)
				}; d1 != std::string::npos && d2 != std::string::npos && d1 < d2)
				return std::string{ line.substr(d1 + !includeDelim, d2 - d1 - !includeDelim) };
		} catch (...) {}
		return std::nullopt;
	}

	/**
	 * @function extractString(string&, char, char, bool, bool)
	 * @brief Returns a contiguous substring from the first/last 2 appearances of the given delims.
	 * @param line			- The string to extract from.
	 * @param delims		- String of delims used to determine the open/close delim
	 * @param includeDelim	- (Default: false) When true, the returned string will contain first & last delims.
	 * @returns optional<string>
	 */
	[[nodiscard]] inline std::optional<std::string> extractString(const std::string& line, const std::string& delims, const bool includeDelim = false) noexcept
	{
		return extractString(line, std::make_pair(delims, delims), includeDelim);
	}

	/**
	 * @function extractString(string&, pair<char, char>)
	 * @brief Returns a substring from the first appearance of a given delimiter to the last.
	 * @param line			- The string to extract from.
	 * @param delim			- ( first = opening delim ) ( second = closing delim )
	 * @returns optional<string>
	 */
	[[nodiscard]] inline std::optional<std::string> extractString(const std::string& line, const std::pair<const char, const char> delim) noexcept
	{
		try {// ReSharper disable once CppUseStructuredBinding
			if (const std::pair dPos(line.find(delim.first), line.rfind(delim.second)); dPos.first != std::string::npos && dPos.second != std::string::npos && dPos.first < dPos.second)
				return{ line.substr(dPos.first + 1, dPos.second - dPos.first - 1) };
		} catch (...) {}
		return std::nullopt;
	}

	/**
	 * @function extractStringUntil(string&, char)
	 * @brief Returns a substr between the beginning/end of a given string and the first occurrence of a given delimiter
	 * @param line		- The string to extract from
	 * @param delim		- Character used to determine the end of the substr
	 * @param begin		- (Default: true) When true, extracts from the start of the string, else the back.
	 * @param reverse	- (Default: false) When true, searches for the delim from the opposite side first
	 * @returns optional<string>
	 */
	[[nodiscard]] inline std::optional<std::string> extractStringUntil(const std::string& line, const char delim, const bool begin = true, const bool reverse = false) noexcept
	{
		try {
			if (const auto delimPos{ begin ? reverse ? line.rfind(delim) : line.find(delim) : reverse ? line.find(delim) : line.rfind(delim) }; delimPos != std::string::npos)
				return{ begin ? line.substr(0, delimPos) : line.substr(delimPos + 1) };
		} catch (...) {}
		return std::nullopt;
	}

	/**
	 * @function extractString(string&, pair<char, char>, bool, bool)
	 * @brief Returns 3 substr, the first being the string until the first delim, the second being between the given delims, and the 3rd being the remaining string after the second delim.
	 * @param line			- The string to extract from.
	 * @param delim			- ( first = opening delim ) ( second = closing delim )
	 * @param forward		- ( true = forward search ) ( false = reverse search )
	 * @param includeDelim	- (Default: false) When true, the returned string will contain first & last delims.
	 * @returns optional<tuple<string, string, string>>
	 */
	[[nodiscard]] inline std::optional<std::tuple<std::string, std::string, std::string>> extractAllStrings(const std::string& line, const std::pair<const char, const char> delim, const bool forward, const bool includeDelim = false) noexcept
	{
		try {
			const auto d1{ forward ? line.find(delim.first) : line.rfind(delim.first) };
			if (const auto d2{ forward ? line.find(delim.second, d1 + 1) : line.rfind(delim.second, d1 - 1) }; d1 != std::string::npos && d2 != std::string::npos && d1 < d2)
				return std::tuple<std::string, std::string, std::string>{ { line.substr(0, d1 + includeDelim) }, { line.substr(d1 + !includeDelim, d2 - d1 - !includeDelim) }, { line.substr(d2 + !includeDelim) } };
		} catch (...) {}
		return std::nullopt;
	}
#pragma endregion extractString
#pragma region contains
	/**
	 * @function contains(char)
	 * @brief Check if a given string exists within another string
	 * @param str	- String to search in
	 * @param ch	- Char to search for
	 * @returns bool
	 */
	[[nodiscard]] inline bool contains(const std::string& str, const char ch) noexcept
	{
		return str.find(ch) != std::string::npos;
	}

	/**
	 * @function contains(char)
	 * @brief Check if any of a list of delimiters is present in a given string
	 * @param str	 - String to search in
	 * @param delims - String of delimiters
	 * @returns bool - ( true = At least one delimiter was found ) ( false = None of the given delimiters exist in the string )
	 */
	[[nodiscard]] inline bool contains(const std::string& str, const std::string& delims) noexcept
	{
		return str.find_first_of(delims) != std::string::npos;
	}
#pragma endregion contains
#pragma region longestLength
	/**
	 * @function longestLength(vector<string>&)
	 * @brief Returns the length of the longest string in a vector
	 * @param vec	- A vector of strings
	 * @returns string::size_type
	 */
	[[nodiscard]] inline std::string::size_type longestLength(const std::vector<std::string>& vec) noexcept
	{
		std::string::size_type longest{ 0 };
		for (const auto& it : vec)
			if (it.size() > longest)
				longest = it.size();
		return longest;
	}

	template<template<class, class> class cont_type> [[nodiscard]] static std::string::size_type longestLength(const cont_type<std::string, std::allocator<std::string>>& container) noexcept
	{
		std::string::size_type longest{ 0 };
		for (const auto& it : container)
			if (it.size() > longest)
				longest = it.size();
		return std::move(longest);
	}
#pragma endregion longestLength
}
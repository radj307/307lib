/**
 * @file	var.hpp
 * @author	radj307
 * @brief	Contains helper methods for variadic templates & tuples, as well as general-use concepts.
 */
#pragma once
#include <sysarch.h>
#include <vector>
#include <string>
#include <tuple>
#if CPP >= 20
#include <concepts> // also includes <type_traits>
#else
#include <type_traits>
#endif

 /**
  * @namespace var
  * @brief Contains helper methods for variadic templated functions, as well as std::tuple.
  */
namespace var {
#if CPP >= 20
#pragma region Concepts
	/**
	 * @concept		Streamable
	 * @brief		Check if the given type can be inserted into an output stream.
	 * @tparam T	A type to attempt to insert into the stream.
	 */
	template<typename T> concept Streamable = requires(T obj)
	{
		std::declval<std::stringstream&>() << obj;
	};
	template<typename T> concept wStreamable = requires(T obj)
	{
		std::declval<std::wstringstream&>() << obj;
	};
	/**
	 * @brief			Check if a variadic templated type has the same number of arguments as a given value.
	 * @tparam compsize	The size to compare.
	 * @tparam ...VT	Input Variadic Templated Type
	 * @returns			bool
	 */
	template<auto compsize, class... VT> requires std::unsigned_integral<decltype(compsize)>
	inline static constexpr const bool same_size = (sizeof...(VT) == compsize);
	/**
	 * @brief			Check if a variadic templated type has more arguments than a given number.
	 * @tparam compsize	One before the number of arguments VT must contain before returning true.
	 * @tparam ...VT	Input Variadic Templated Type
	 * @returns			bool
	 */
	template<auto compsize, class... VT> requires std::unsigned_integral<decltype(compsize)>
	inline static constexpr const bool more_than = (sizeof...(VT) > compsize);
	/**
	 * @brief			Check if a variadic templated type has less arguments than a given number.
	 * @tparam compsize	One after the number of arguments VT must contain before returning false.
	 * @tparam ...VT	Input Variadic Templated Type
	 * @returns			bool
	 */
	template<auto compsize, class... VT> requires std::unsigned_integral<decltype(compsize)>
	inline static constexpr const bool less_than = (sizeof...(VT) < compsize);
	template<class... VT>
	inline static constexpr const bool more_than_one = more_than<1, VT...>;
	/**
	 * @concept			at_least_one
	 * @brief			Check if a variadic templated type has at least one included argument.
	 * @tparam ...VT	Input Variadic Templated Type
	 * @returns			bool
	 */
	template<class... VT> concept at_least_one = (sizeof...(VT) > 0);
	/**
	 * @concept			all_same
	 * @brief			Concept that checks if all of the given variadic templated types are of the same specified type.
	 * @tparam T		The type to compare against each variadic type.
	 * @tparam VT...	Variadic Templated Input Types.
	 */
	template<typename T, typename... VT> concept all_same = (std::same_as<T, VT> && ...);
	/**
	 * @concept			any_same
	 * @brief			Concept that checks if all of the given variadic templated types have the same type as any of the specified types.
	 * @tparam T		The type to compare against each variadic type.
	 * @tparam VT...	Variadic Templated Input Types.
	 */
	template<typename T, typename... VT> concept any_same = (std::same_as<T, VT> || ...);
	//template<typename Here, typename Next, typename... Rest> concept any_same = ((std::same_as<Here, Rest> || ...) || any_same<Next, Rest..., Here>);
	/**
	 * @concept			all_convertible_to
	 * @brief			Check if all variadic templated types are able to be converted to a single specified type.
	 * @tparam To		Type that all variadic types must be able to be implicitly converted to.
	 * @tparam From...	Variadic types to check
	 */
	template<typename To, typename... From> concept all_convertible_to = (std::convertible_to<From, To> && ...);
	/**
	 * @concept			none
	 * @brief			Check if the number of included variadic arguments is 0.
	 * @tparam VT...	Variadic Templated Types
	 */
	template<class... VT> concept none = (sizeof...(VT) == 0);
	/**
	 * @concept			none_same
	 * @brief			Check if a specified type was not included in a variadic pack.
	 * @tparam T		Type that shouldn't appear in variadic templated types.
	 * @tparam VT...	Variadic Templated Types.
	 */
	template<class T, class... VT> concept none_same = (!std::same_as<T, VT> && ...);
	/**
	 * @concept		same_or_convertible
	 * @brief		Checks if the given types are the same, or if type From can be converted to type To.
	 * @tparam From	The type to convert from, or the same type as To.
	 * @tparam To	The type to convert to, or the same type as From.
	 */
	template<class From, class To> concept same_or_convertible = std::same_as<From, To> || std::convertible_to<From, To>;
	/**
	 * @concept		same_or_biconvertible
	 * @brief		Checks if the given types are the same, or if one can be converted to the other. (Checks both ways)
	 * @tparam T1	First Type. Order doesn't matter.
	 * @tparam T2	Second Type. Order doesn't matter.
	 */
	template<class T1, class T2> concept same_or_biconvertible = std::same_as<T1, T2> || (std::convertible_to<T1, T2> && std::convertible_to<T2, T1>);
	/**
	 * @concept			all_same_or_convertible
	 * @brief			Checks if all of the given types are the same as or convertible to another given type.
	 * @tparam T		Predicate type. At least one of the variadic types must be the same as, or convertible to, this type.
	 * @tparam VT...	Input Variadic Types
	 */
	template<class T, class... VT > concept all_same_or_convertible = (same_or_biconvertible <T, VT> && ...);
	/**
	 * @concept			any_same_or_convertible
	 * @brief			Checks if any of the given types are the same as or convertible to another given type.
	 * @tparam T		Predicate type. At least one of the variadic types must be the same as, or convertible to, this type.
	 * @tparam VT...	Input Variadic Types
	 */
	template<class T, class... VT > concept any_same_or_convertible = (same_or_biconvertible <T, VT> || ...);
	/**
	 * @concept		not_same
	 * @brief		Checks that the given types are not the same.
	 * @param T1	First Input Type
	 * @param T2	Second Input Type
	 */
	template<typename T1, typename T2> concept not_same = std::negation_v<std::is_same<T1, T2>>;
#pragma endregion Concepts
#pragma region StringConcepts
	/**
	 * @concept		valid_char
	 * @brief		Allows all standard char types as of C++20, including signed/unsigned char, wchar_t, UTF-8, UTF-16, & UTF-32 types.
	 * @tparam T	Input Type.
	 */
	template<class T> concept valid_char =
		std::same_as<T, signed char>
		|| std::same_as<T, unsigned char>
		|| std::same_as<T, char>
		|| std::same_as<T, wchar_t>
		|| std::same_as<T, char8_t>
		|| std::same_as<T, char16_t>
		|| std::same_as<T, char32_t>;
	/**
	 * @concept		valid_string
	 * @brief		Allows standard string types, including narrow/wide, UTF-8, UTF-16, & UTF-32 types.
	 * @tparam T	Input Type.
	 */
	template<class T> concept valid_string =
		std::same_as<T, std::string>
		|| std::same_as<T, std::wstring>
		|| std::same_as<T, std::u8string>
		|| std::same_as<T, std::u16string>
		|| std::same_as<T, std::u32string>;
	template<class T> concept valid_string_or_convertible = valid_string<T> || std::convertible_to<T, std::string> || std::convertible_to<T, std::wstring> || std::convertible_to<T, std::u8string> || std::convertible_to<T, std::u16string> || std::convertible_to<T, std::u32string>;
	/**
	 * @concept		valid_string_view
	 * @brief		Allows standard string_view types, including narrow/wide, UTF-8, UTF-16, & UTF-32 types.
	 * @tparam T	Input Type.
	 */
	template<class T> concept valid_string_view =
		std::same_as<T, std::string_view>
		|| std::same_as<T, std::wstring_view>
		|| std::same_as<T, std::u8string_view>
		|| std::same_as<T, std::u16string_view>
		|| std::same_as<T, std::u32string_view>;
	/**
	 * @concept		valid_stringbuf
	 * @brief		Allows standard stringbuf types, including narrow/wide types.
	 * @tparam T	Input Type.
	 */
	template<class T> concept valid_stringbuf =
		std::same_as<T, std::stringbuf>
		|| std::same_as<T, std::wstringbuf>;
	/**
	 * @concept		valid_stringstream
	 * @brief		Allows standard stringstream types, including i/o, narrow/wide types.
	 * @tparam T	Input Type.
	 */
	template<class T> concept valid_stringstream =
		std::same_as<T, std::stringstream>
		|| std::same_as<T, std::istringstream>
		|| std::same_as<T, std::ostringstream>
		|| std::same_as<T, std::wstringstream>
		|| std::same_as<T, std::wistringstream>
		|| std::same_as<T, std::wostringstream>;
	/**
	 * @concept		valid_streambuf
	 * @brief		Allows standard streambuf types, including narrow/wide types.
	 * @tparam T	Input Type.
	 */
	template<class T> concept valid_streambuf = std::same_as<T, std::streambuf> || std::same_as<T, std::wstreambuf>;
	/**
	 * @concept		valid_streampos
	 * @brief		Allows standard streampos types, including narrow/wide, UTF-8, UTF-16, &UTF-32 types.
	 * @tparam T	Input Type.
	 */
	template<class T> concept valid_streampos =
		std::same_as<T, std::streampos>
		|| std::same_as<T, std::wstreampos>
		|| std::same_as<T, std::u8streampos>
		|| std::same_as<T, std::u16streampos>
		|| std::same_as<T, std::u32streampos>;
#pragma endregion StringConcepts
#endif

#pragma region variadic
	/**
	 * @brief			Non-templated std::disjunction that accepts any number of boolean
	 *\n 				conditions and returns true when at least one of them evaluates to true.
	 * @tparam ...T		Variadic Template Parameter Pack.
	 * @param booleans	Any number of boolean values to compare.
	 * @returns			bool
	 */
	template<typename...T> [[nodiscard]] inline static constexpr bool variadic_or(const T... booleans)
	{
		return ((booleans) || ...);
	}
	/**
	 * @brief			Non-templated std::conjunction that accepts any number of boolean
	 *\n				conditions, and returns true when all of them evaluate to true.
	 * @tparam ...T		Variadic Template Parameter Pack.
	 * @param booleans	Any number of boolean values to compare.
	 * @returns			bool
	 */
	template<typename...T> [[nodiscard]] inline static constexpr bool variadic_and(const T... booleans)
	{
		return ((booleans) && ...);
	}

	/**
	 * @brief			Retrieve the largest value in a variadic pack.
	 * @tparam T		First value type, all additional values are casted to this type before comparison.
	 * @tparam ...vT	Additional Value Type(s).
	 * @param n			First value. All additional values are casted to the same type before comparison.
	 * @param ...rest	At least one additional value.
	 * @returns			T
	 */
	template<typename T, typename... vT>
	[[nodiscard]] inline static constexpr T largest(const T& n, const vT&... rest) noexcept
	{
		const auto larger{ [] <typename T>(const T & n, const T & bound) -> T { return n > bound ? n : bound; } };
		return (larger<T>(n, larger<T>(static_cast<T>(rest), static_cast<T>(0))));
	}

	/**
	 * @brief			Retrieve the smallest value in a variadic pack.
	 * @tparam T		First value type, all additional values are casted to this type before comparison.
	 * @tparam ...vT	Additional Value Type(s).
	 * @param n			First value. All additional values are casted to the same type before comparison.
	 * @param ...rest	At least one additional value.
	 * @returns			T
	 */
	template<typename T, typename... vT>
	[[nodiscard]] inline static constexpr T smallest(const T& n, const vT&... rest) noexcept
	{
		const auto smaller{ [] <typename T>(const T & n, const T & bound) -> T { return n < bound ? n : bound; } };
		return (smaller<T>(n, smaller(static_cast<T>(rest), static_cast<T>(0))));
	}

#if CPP >= 20
	/**
	 * @brief			Variadic templated helper function that returns a vector of templated element types.
	 *\n				All types must be convertible to the specified output type.
	 *\n				The function can also be overloaded to return any container type with a matching ctor.
	 * @tparam T		Return / Cast type
	 * @tparam RT		Return Type, must have a constructor that can receive variadic input.
	 * @tparam ...VT	Variadic Templated Types
	 * @param ...vars	The variables to accumulate.
	 * @returns			std::vector<T>
	 */
	template<class T, class RT = std::vector<T>, typename... VT>
	[[nodiscard]] inline static constexpr const RT variadic_accumulate(VT... vars)
	{
		return RT{ static_cast<T>(vars)... };
	}

	template<valid_char Out, valid_char In>
	static constexpr const Out convert_char_t(const In& ch)
	{
		if constexpr (sizeof(In) <= sizeof(Out))
			return{ ch };
		else if constexpr (sizeof(In) > sizeof(Out))
			return static_cast<Out>(ch);
		return { '\0' }; // this should never be reached, but it quiets the compiler
	}

	/**
	 * @brief			Variadic templated helper function that returns a string from a variadic pack of characters.
	 * @tparam StrT		The type of string to return.
	 * @tparam T		Templated Char Types.
	 * @param ch		Characters.
	 * @returns			StrT
	 */
	template<valid_string StrT, valid_char... T>
	[[nodiscard]] inline static constexpr const StrT string_accumulate(const T&... ch)
	{
		return StrT{ static_cast<StrT::value_type>(ch)... };
	}
#endif

#pragma endregion variadic
#pragma region tuple
	/**
	 * @brief					Compare each element in a tuple against a given variable.
	 * @tparam CompareType		Type that can be compared to all elements in the tuple.
	 * @tparam ...TupleTypes	Types in the given tuple.
	 * @param tuple				A tuple containing elements which can be compared for equality with the given comparison type.
	 * @param compare			A variable to compare to each element in the tuple.
	 * @returns					bool
	 */
	template<size_t i = 0, typename CompareType, typename... TupleTypes> requires (std::equality_comparable_with<CompareType, TupleTypes> && ...)
		[[nodiscard]] constexpr bool tuple_or(const std::tuple<TupleTypes...>& tuple, const CompareType& compare)
	{
		return std::get<i>(tuple) == compare || (sizeof...(TupleTypes) > i + 1ull) && tuple_or<i + 1ull>(tuple, compare);
	}
	/**
	 * @brief					Compare a variadic tuple against at least one comparable type.
	 * @tparam ...CompareTypes	Variadic templated types
	 * @tparam ...TupleTypes	Variadic templated tuple types
	 * @param tuple				Tuple of values to compare
	 * @param ...compare		Values to compare
	 * @returns					bool
	 */
	template<typename... CompareTypes, typename... TupleTypes> requires (sizeof...(CompareTypes) > 0)
		[[nodiscard]] constexpr bool tuple_or(const std::tuple<TupleTypes...>& tuple, const CompareTypes&... compare)
	{
		return (tuple_or(tuple, compare) || ...);
	}
	template<size_t i = 0ull, typename... TupleTypesLeft, typename... TupleTypesRight>
	[[nodiscard]] constexpr bool tuple_or(const std::tuple<TupleTypesLeft...>& left, const std::tuple<TupleTypesRight...>& right)
	{
		return tuple_or(left, std::get<i>(right)) || (sizeof...(TupleTypesRight) > i + 1) && tuple_or<i + 1>(left, right);
	}

	/**
	 * @brief			Check if two tuples containing the same element types are equivalent.
	 * @tparam ...Types	Variadic Templated Types. Both tuples must contain the same types in the same order.
	 * @param l			First tuple to compare.
	 * @param r			Second tuple to compare.
	 * @returns			bool
	 */
	template<size_t i = 0ull, typename... Types>
	[[nodiscard]] constexpr bool tuple_and(const std::tuple<Types...>& l, const std::tuple<Types...>& r)
	{
		return std::get<i>(l) == std::get<i>(r)
			&& (sizeof...(Types) == i || tuple_and<i + 1ull>(l, r)); // if this is the last element, short circuit and stop recursing
	}
#pragma endregion tuple
}
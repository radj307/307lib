/**
 * @file	var.hpp
 * @author	radj307
 * @brief	Contains helper methods for variadic templated functions.
 */
#pragma once
#include <sysarch.h>
#if CPP >= 20
#include <concepts>
#include <vector>
#include <tuple>
#include <strconcepts.hpp>
 /**
  * @namespace var
  * @brief Contains helper methods for variadic templated functions, as well as std::tuple.
  */
namespace var {
#pragma region variadic
	/**
	 * @brief			Non-templated std::disjunction that accepts any number of boolean
	 *\n 				conditions and returns true when any of the conditions are true.
	 * @tparam ...T		Variadic Template Parameter Pack.
	 * @param booleans	Any number of boolean values to compare.
	 * @returns			bool
	 */
	template<typename...T> [[nodiscard]] inline static constexpr bool variadic_or(const T... booleans)
	{
		return ((booleans) || ...);
	}
	/**
	 * @brief			Boolean AND comparison that accepts any number of variadic boolean arguments.
	 * @tparam ...T		Variadic Template Parameter Pack.
	 * @param booleans	Any number of boolean values to compare.
	 * @returns			bool
	 */
	template<typename...T> [[nodiscard]] inline static constexpr bool variadic_and(const T... booleans)
	{
		return ((booleans) && ...);
	}

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
	template<class T, class RT = std::vector<T>, class... VT> requires std::conjunction_v<std::is_convertible<VT, T>...>
	[[nodiscard]] inline static constexpr const RT variadic_accumulate(VT... vars)
	{
		return RT{ static_cast<T>(vars)... };
	}

	template<typename T> concept is_char = std::same_as<T, char> || std::same_as<T, wchar_t>;
	template<typename T> concept is_char_or_convertible = is_char<T> || std::convertible_to<T, char> || std::convertible_to<T, wchar_t>;
	template<str::concepts::valid_string StrT, is_char_or_convertible... T>
	[[nodiscard]] inline static constexpr const StrT string_accumulate(const T&... ch)
	{
		return std::string{ static_cast<StrT::value_type>(ch)... };
	}

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
#pragma region Concepts
	/**
	 * @brief			Check if a variadic templated type has the same number of arguments as a given value.
	 * @tparam compsize	The size to compare.
	 * @tparam ...VT	Input Variadic Templated Type
	 * @returns			bool
	 */
	template<auto compsize, class... VT> requires std::integral<decltype(compsize)>
	inline static constexpr const bool same_size = (sizeof...(VT) == compsize);
	/**
	 * @brief			Check if a variadic templated type has more arguments than a given number.
	 * @tparam compsize	One before the number of arguments VT must contain before returning true.
	 * @tparam ...VT	Input Variadic Templated Type
	 * @returns			bool
	 */
	template<auto compsize, class... VT> requires std::integral<decltype(compsize)>
	inline static constexpr const bool larger_size = (sizeof...(VT) > compsize);
	/**
	 * @brief			Check if a variadic templated type has less arguments than a given number.
	 * @tparam compsize	One after the number of arguments VT must contain before returning false.
	 * @tparam ...VT	Input Variadic Templated Type
	 * @returns			bool
	 */
	template<auto compsize, class... VT> requires std::integral<decltype(compsize)>
	inline static constexpr const bool smaller_size = (sizeof...(VT) < compsize);
	template<class... VT>
	inline static constexpr const bool more_than_one = larger_size<1, VT...>;
	/**
	 * @brief			Check if a variadic templated type has at least one included argument.
	 * @tparam ...VT	Input Variadic Templated Type
	 * @returns			bool
	 */
	 //	template<class... VT>
	 //	inline static constexpr const bool at_least_one = (sizeof...(VT) > 0);
	template<class... VT> concept at_least_one = (sizeof...(VT) > 0);
	/**
	 * @brief		Concept that checks if all of the (minimum 1) given variadic templated types are of the same specified type.
	 * @tparam T	The type to compare against each variadic type.
	 * @tparam VT	Variadic Templated Input Types.
	 */
	template<class T, class... VT> concept all_same = at_least_one<VT...> && std::conjunction_v<std::is_same<T, VT>...>;
	/**
	 * @brief			Concept that checks if all of the (minimum 1) given variadic templated types have the same type as any of the specified types.
	 * @tparam T		The type to compare against each variadic type.
	 * @tparam VT...	Variadic Templated Input Types.
	 */
	template<class T, class... VT> concept any_same = at_least_one<VT...> && std::disjunction_v<std::is_same<T, VT>...>;
	/**
	 * @concept			none
	 * @brief			Concept that checks if no variadic templated types were included.
	 * @tparam VT...	Variadic Templated Types
	 */
	template<class... VT> concept none = (sizeof...(VT) == 0);
	/**
	 * @concept			none_of
	 * @brief			Checks if a list of variadic templated types does not contain a specified type.
	 * @tparam T		Type that shouldn't appear in variadic templated types.
	 * @tparam VT...	Variadic Templated Types.
	 */
	template<class T, class... VT> concept none_of = std::conjunction_v<std::negation<std::is_same<T, VT>>...>;
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
}
#else
#pragma message("<var.hpp> requires C++20 and was disabled.")
#endif
/**
 * @file	var.hpp
 * @author	radj307
 * @brief	Contains helper methods for variadic templates & tuples, as well as C++20 concepts.
 *\n		I can't seem to find any documentation on how exactly concept templates are deduced,
 *\n		but from my testing & std concepts like std::derived_from it appears that when used
 *\n		instead of `typename`|`class`, the test type is inserted into the __FIRST__ template
 *\n		parameter.
 *\n		That's pretty great because it allows variadic concepts like var::any_same.
 */
#pragma once
#include <sysarch.h>
#include <vector>
#include <string>
#include <utility>
#include <tuple>
#if LANG_CPP >= 20
#include <concepts> // also includes <type_traits>
#else
#include <type_traits>
#endif

#include <functional>

 /**
  * @namespace var
  * @brief Contains helper methods for variadic templated functions, as well as std::tuple.
  */
namespace var {
	/**
	 * @brief
	 * @tparam Pred
	 * @param begin
	 * @param end
	 * @param fn	A function that accepts (IteratorT::value_type) and returns an integral or boolean.
	 * @returns		size_t
	 */
	template<class Pred, std::input_or_output_iterator IteratorT> static CONSTEXPR size_t count(IteratorT begin, IteratorT end, const Pred& fn) noexcept(false)
	{
		size_t counter{ 0ull };
		for (auto it{ begin }; it != end; ++it) {
			auto n{ fn(*it) };
			using T = std::decay_t<decltype(n)>;
			if constexpr (std::same_as<T, bool> || std::integral<T>)
				counter += static_cast<size_t>(n);
		}
		return counter;
	}

#	pragma region ConstexprTests
	////////////////////////////////// BEGIN / Constexpr Tests /////////////////////////////////////////////
	/**
	 * @brief			Check if a variadic templated type has the same number of arguments as a given value.
	 * @tparam compsize	The size to compare.
	 * @tparam ...VT	Input Variadic Templated Type
	 */
	template<auto compsize, typename... Ts> struct is_same_as {
		/// @brief	std::true_type when true, std::false_type when false.
		using type = std::conditional<(sizeof...(Ts) == compsize), std::true_type, std::false_type>;
	};
	/// @brief	is_same_as::type macro.
	template<auto compsize, typename... Ts> using is_same_as_t = is_same_as<compsize, Ts...>::type;

	/**
	 * @brief			Check if a variadic templated type has more arguments than a given number.
	 * @tparam compsize	One before the number of arguments VT must contain before returning true.
	 * @tparam ...VT	Input Variadic Templated Type
	 */
	template<auto compsize, typename... Ts> struct is_more_than {
		/// @brief	std::true_type when true, std::false_type when false.
		using type = std::conditional<(sizeof...(Ts) > compsize), std::true_type, std::false_type>;
	};
	/// @brief	is_more_than::type macro.
	template<auto compsize, typename... Ts> using is_more_than_t = is_more_than<compsize, Ts...>::type;

	template<typename... Ts> using is_more_than_one = is_more_than_t<1ull, Ts...>;

	/**
	 * @brief			Check if a variadic templated type has less arguments than a given number.
	 * @tparam compsize	One after the number of arguments VT must contain before returning false.
	 * @tparam ...VT	Input Variadic Templated Type
	 */
	template<auto compsize, typename... Ts> struct is_less_than {
		/// @brief	std::true_type when true, std::false_type when false.
		using type = std::conditional<(sizeof...(Ts) < compsize), std::true_type, std::false_type>;
	};
	/// @brief	is_less_than::type macro.
	template<auto compsize, typename... Ts> using is_less_than_t = is_less_than<compsize, Ts...>::type;
	////////////////////////////////// END / Constexpr Tests /////////////////////////////////////////////
#	pragma endregion ConstexprTests

#	if LANG_CPP >= 20
#	pragma region Concepts
	////////////////////////////////// BEGIN / Concepts /////////////////////////////////////////////
#	pragma region DeclvalTest_Concepts
	////////////////////////////////// BEGIN / Misc Concepts /////////////////////////////////////////////
	/**
	 * @concept		has_default
	 * @brief		Check if the given type has a default value or default constructor.
	 * @tparam T	Input Type.
	 */
	template<typename T>
	concept has_default = std::constructible_from<T>;
	////////////////////////////////// END / Misc Concepts /////////////////////////////////////////////
#	pragma endregion DeclvalTest_Concepts
#	pragma region DeclvalTest_Concepts
	////////////////////////////////// BEGIN / std::declval Test Concepts /////////////////////////////////////////////
	/**
	 * @concept		Streamable
	 * @brief		Check if the given type can be inserted into an output stream.
	 * @tparam T	A type to attempt to insert into the stream.
	 */
	template<typename T, class StreamType = std::stringstream>
	concept Streamable = requires(T obj)
	{
		std::declval<StreamType&>() << obj;
	};
	/**
	 * @concept			callable
	 * @brief			Uses the `std::invocable` concept to test if type `T` is a callable.
	 * @tparam T		Input Type.
	 * @tparam Args...	Optional argument types required by the callable.
	 */
	template<class T, typename... Args>
	concept callable = std::invocable<T, Args...>;
	////////////////////////////////// END / std::declval Test Concepts /////////////////////////////////////////////
#	pragma endregion DeclvalTest_Concepts
#	pragma region Equatable_Concepts
	////////////////////////////////// BEGIN / "Equatable Concepts" /////////////////////////////////////////////
	/**
	 * @concept			same_or_equatable
	 * @brief			Check if the given types are the same type, or if they can be compared.
	 *\n				Comparable check returns true if either ( Left::operator== ) or ( Right::operator== ) can perform the comparison.
	 * @tparam Left:	First Input Type.
	 * @tparam Right:	Second Input Type.
	 */
	template<typename Left, typename Right> concept same_or_equatable = requires (Left l, Right r)
	{
		std::same_as<Left, Right> || std::declval<Left>() == std::declval<Right>() || std::declval<Right>() == std::declval<Left>();
	};
	////////////////////////////////// END / "Equatable Concepts" /////////////////////////////////////////////
#	pragma endregion Equatable_Concepts
#	pragma region Type_Concepts	
	////////////////////////////////// BEGIN / "Type Concepts" /////////////////////////////////////////////
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
	 * @concept		enumerator
	 * @brief		Checks if the given type is an enum.
	 * @tparam T	Input Type
	 */
	template<typename T> concept enumerator = std::is_enum_v<T>;
	/**
	 * @concept		arithmetic
	 * @brief		Checks if the given type can be used in arithmetic operations.
	 * @tparam T	Input Type
	 */
	template<typename T> concept arithmetic = std::is_arithmetic_v<T>;
	/**
	 * 
	 * @concept		numeric
	 * @brief		Checks if the given type can be used in arithmetic operations.
	 * @tparam T	Input Type
	 */
	template<typename T> concept numeric = arithmetic<T>;
	/**
	 * @concept		fundamental
	 * @brief		Checks if the given type is a fundamental type.
	 * @tparam T	Input Type
	 */
	template<typename T> concept fundamental = std::is_fundamental_v<T>;
	////////////////////////////////// END / "Type Concepts" /////////////////////////////////////////////
#	pragma endregion Type_Concepts
#	pragma region Variadic_Count_Concepts
	////////////////////////////////// BEGIN / Variadic Count Concepts /////////////////////////////////////////////
	/**
	 * @concept			more_than
	 * @brief			Check if the number of variadic arguments received is greater than a given threshold.
	 * @tparam compsize	Pass test when there are more than this number of variadic arguments.
	 * @tparam Ts...	Any number of variadic template arguments.
	 */
	template<size_t THRESHOLD, typename... Ts> concept more_than = (sizeof...(Ts) > THRESHOLD);
	/**
	 * @concept			at_least_one
	 * @brief			Check if a variadic templated type has at least one included argument.
	 * @tparam ...Ts	Input Variadic Templated Type
	 */
	template<class... Ts> concept at_least_one = more_than<0ull, Ts...>;
	/**
	 * @concept			less_than
	 * @brief			Check if the number of variadic arguments received is less than a given threshold.
	 * @tparam compsize	Pass test when there are less than this number of variadic arguments.
	 * @tparam Ts...	Any number of variadic template arguments.
	 */
	template<size_t THRESHOLD, typename... Ts> concept less_than = (sizeof...(Ts) < THRESHOLD);
	/**
	 * @concept			none
	 * @brief			Check if the number of included variadic arguments is 0.
	 * @tparam Ts...	Variadic Templated Types
	 */
	template<class... Ts> concept none = (sizeof...(Ts) == 0ull);
	////////////////////////////////// END / Variadic Count Concepts /////////////////////////////////////////////
#	pragma endregion Variadic_Count_Concepts
#	pragma region VariadicType_Concepts
	////////////////////////////////// BEGIN / Variadic "Type Concepts" /////////////////////////////////////////////
	/**
	 * @concept			derived_from_any
	 * @brief			Concept that checks if the given type is derived from any of the specified types.
	 * @tparam Type		Input Type (This is provided during template deduction).
	 * @tparam Ts...	Potential base type(s).
	 */
	template<typename Type, typename... Ts> concept derived_from_any = ((std::derived_from<Type, Ts>) || ...);
	/**
	 * @concept			derived_from_all
	 * @brief			Concept that checks if the given type is derived from all of the specified types.
	 * @tparam Type		Input Type (This is provided during template deduction).
	 * @tparam Ts...	Potential base type(s).
	 */
	template<typename Type, typename... Ts> concept derived_from_all = ((std::derived_from<Type, Ts>) && ...);
	/**
	 * @concept			not_same
	 * @brief			Concept that checks if none of the given variadic types match a given type.
	 * @tparam Type		Type that all variadic types must not match to pass.
	 * @tparam Ts...	Variadic types to compare.
	 */
	template<typename Type, typename... Ts> concept not_same = ((!std::same_as<Type, Ts>) && ...);
	/**
	 * @concept			all_same
	 * @brief			Concept that checks if all of the given variadic types match a given type.
	 * @tparam Type		Type that all variadic types must match to pass.
	 * @tparam Ts...	Variadic types to compare.
	 */
	template<typename Type, typename... Ts> concept all_same = (std::same_as<Type, Ts> && ...);
	/**
	 * @concept			any_same
	 * @brief			Concept that checks if any of the given variadic types match a given type.
	 * @tparam Type		Type that any variadic type must match to pass.
	 * @tparam Ts...	Variadic types to compare.
	 */
	template<typename Type, typename... Ts> concept any_same = (std::same_as<Type, Ts> || ...);
	/**
	 * @concept			convertible_from_all
	 * @brief			Concept that checks if all given input types can be converted to the specified output type.
	 * @tparam To		This is provided at compile-time.
	 * @tparam From...	These are provided during template resolution.
	 */
	template<typename To, typename... From> concept convertible_from_all = ((std::convertible_to<From, To>) && ...);
	/**
	 * @concept			convertible_from_any
	 * @brief			Concept that checks if any given input types can be converted to the specified output type.
	 * @tparam To		This is provided at compile-time.
	 * @tparam From...	These are provided during template resolution.
	 */
	template<typename To, typename... From> concept convertible_from_any = ((std::convertible_to<From, To>) || ...);
	/**
	 * @concept			convertible_from
	 * @brief			Concept that checks if any given input types can be converted to the specified output type.
	 * @tparam To		This is provided at compile-time.
	 * @tparam From...	These are provided during template resolution.
	 */
	template<typename To, typename... From> concept convertible_from = convertible_from_any<To, From...>;
	/**
	 * @concept			not_convertible_from
	 * @brief			Concept that checks if none of the given input types can be converted to the specified output type.
	 * @tparam To		This is provided at compile-time.
	 * @tparam From...	These are provided during template resolution.
	 */
	template<typename To, typename... From> concept not_convertible_from = (!convertible_from_any<To, From...>);
	/**
	 * @concept			not_convertible
	 * @brief			Concept that checks if a given input type cannot be converted to any of the given output types.
	 * @tparam From		Input type.
	 * @tparam To...	Any number of output types.
	 */
	template<typename From, typename... To> concept not_convertible = ((!std::convertible_to<From, To>) && ...);
	/**
	 * @concept			not_convertible_to
	 * @brief			Concept that checks if none of the given input types can be converted to a given output type.
	 * @tparam To		Output type.
	 * @tparam From...	Any number of input types.
	 */
	template<typename To, typename... From> concept not_convertible_to = ((!std::convertible_to<From, To>) && ...);
	/**
	 * @concept			all_convertible
	 * @brief			Concept that checks if the given input type can be converted to all of the given output types.
	 * @tparam From		Input type.
	 * @tparam To...	Any number of output types.
	 */
	template<typename From, typename... To> concept all_convertible = (std::convertible_to<From, To> && ...);
	/**
	 * @concept			all_convertible_to
	 * @brief			Concept that checks if all of the given input types can be converted to the given output type.
	 * @tparam To		Output type.
	 * @tparam From...	Any number of input types.
	 */
	template<typename To, typename... From> concept all_convertible_to = (std::convertible_to<From, To> && ...);
	/**
	 * @concept			any_convertible
	 * @brief			Concept that checks if the given input type can be converted to any of the given output types.
	 * @tparam From		Input type.
	 * @tparam To...	Any number of output types.
	 */
	template<typename From, typename... To> concept any_convertible = (std::convertible_to<From, To> || ...);
	/**
	 * @concept			any_convertible_to
	 * @brief			Concept that checks if any of the given input types can be converted to the given output type.
	 * @tparam To		Output type.
	 * @tparam From...	Any number of input types.
	 */
	template<typename To, typename... From> concept any_convertible_to = (std::convertible_to<From, To> || ...);
	/**
	 * @concept			not_same_or_convertible
	 * @brief			Concept that checks if the given input type does not match and is not convertible to any of the given output types.
	 * @tparam To		Output type.
	 * @tparam From...	Any number of input types.
	 */
	template<typename From, typename... To> concept not_same_or_convertible = ((!std::same_as<From, To> && !std::convertible_to<From, To>) && ...);
	/**
	 * @concept			not_same_or_convertible_to
	 * @brief			Concept that checks if none of the given input types match or are convertible to a given output type.
	 * @tparam To		Output type.
	 * @tparam From...	Any number of input types.
	 */
	template<typename To, typename... From> concept not_same_or_convertible_to = ((!std::same_as<From, To> && !std::convertible_to<From, To>) && ...);
	/**
	 * @concept			all_same_or_convertible
	 * @brief			Concept that checks if a given input type can be converted to all of the given output types.
	 * @tparam From		Input type.
	 * @tparam To...	Any number of output types.
	 */
	template<typename From, typename... To> concept all_same_or_convertible = (same_or_convertible<From, To> && ...);
	/**
	 * @concept			all_same_or_convertible
	 * @brief			Concept that checks if all of the given input types can be converted to the given output type.
	 * @tparam To		Output type.
	 * @tparam From...	Any number of input types.
	 */
	template<typename To, typename... From> concept all_same_or_convertible_to = (same_or_convertible<From, To> && ...);
	/**
	 * @concept			any_same_or_convertible
	 * @brief			Concept that checks if a given input type can be converted to any of the given output types.
	 * @tparam From		Input type.
	 * @tparam To...	Any number of output types.
	 */
	template<typename From, typename... To> concept any_same_or_convertible = (same_or_convertible <From, To> || ...);
	/**
	 * @concept			any_same_or_convertible_to
	 * @brief			Concept that checks if any of the given input types can be converted to the given output type.
	 * @tparam To		Output type.
	 * @tparam From...	Any number of input types.
	 */
	template<typename To, typename... From> concept any_same_or_convertible_to = (same_or_convertible<From, To> || ...);
	////////////////////////////////// END / Variadic "Type Concepts" /////////////////////////////////////////////
#	pragma endregion VariadicType_Concepts
#	pragma region ByteSize_Concepts
	////////////////////////////////// BEGIN / ByteSize Concepts /////////////////////////////////////////////
	/**
	 * @concept		same_size
	 * @brief		Checks if the given types are the same size in bytes.
	 * @tparam L	Input Type 1
	 * @tparam R	Input Type 2
	 */
	template<typename L, typename R> concept same_size = (sizeof(L) == sizeof(R));
	/**
	 * @concept		smaller_size
	 * @brief		Checks if type 1 is smaller than type 2 in bytes.
	 * @tparam L	Input Type 1
	 * @tparam R	Input Type 2
	 */
	template<typename L, typename R> concept smaller_size = (sizeof(L) < sizeof(R));
	/**
	 * @concept		smaller_size
	 * @brief		Checks if type 1 is larger than type 2 in bytes.
	 * @tparam L	Input Type 1
	 * @tparam R	Input Type 2
	 */
	template<typename L, typename R> concept larger_size = (sizeof(L) > sizeof(R));
	/**
	 * @concept		smaller_size
	 * @brief		Checks if type 1 is smaller or equal than type 2 in bytes.
	 * @tparam L	Input Type 1
	 * @tparam R	Input Type 2
	 */
	template<typename L, typename R> concept smaller_equal_size = (sizeof(L) <= sizeof(R));
	/**
	 * @concept		smaller_size
	 * @brief		Checks if type 1 is larger or equal than type 2 in bytes.
	 * @tparam L	Input Type 1
	 * @tparam R	Input Type 2
	 */
	template<typename L, typename R> concept larger_equal_size = (sizeof(L) >= sizeof(R));
	////////////////////////////////// BEGIN / ByteSize Concepts /////////////////////////////////////////////
#	pragma endregion ByteSize_Concepts
#	pragma region StringConcepts
	////////////////////////////////// BEGIN / String Concepts /////////////////////////////////////////////
	/**
	 * @concept		valid_char
	 * @brief		Allows all standard char types as of C++20, including signed/unsigned char, wchar_t, UTF-8, UTF-16, & UTF-32 types.
	 * @tparam T	Test Type.
	 */
	template<class T> concept valid_char = any_same<T, signed char, unsigned char, char, wchar_t, char8_t, char16_t, char32_t>;
	/**
	 * @concept		valid_string
	 * @brief		Allows standard string types, including narrow/wide, UTF-8, UTF-16, & UTF-32 types.
	 * @tparam T	Test Type.
	 */
	template<class T> concept valid_string = any_same<T, std::string, std::wstring, std::u8string, std::u16string, std::u32string>;
	template<class T> concept valid_string_or_cstr = std::same_as<std::remove_cv_t<T>, char*> || any_same<T, std::string, std::wstring, std::u8string, std::u16string, std::u32string>;
	/**
	 * @concept		valid_string_or_convertible
	 * @brief		Allows standard string types, and any type that can be implicitly converted to string.
	 * @tparam T	Test Type.
	 */
	template<class T> concept valid_string_or_convertible = valid_string<T> || any_convertible<T, std::string, std::wstring, std::u8string, std::u16string, std::u32string>;
	/**
	 * @concept		valid_string_view
	 * @brief		Allows standard string_view types, including narrow/wide, UTF-8, UTF-16, & UTF-32 types.
	 * @tparam T	Test Type.
	 */
	template<class T> concept valid_string_view = any_same<T, std::string_view, std::wstring_view, std::u8string_view, std::u16string_view, std::u32string_view>;

	/**
	 * @concept		valid_stringbuf
	 * @brief		Allows standard stringbuf types, including narrow/wide types.
	 * @tparam T	Test Type.
	 */
	template<class T> concept valid_stringbuf = std::same_as<T, std::stringbuf> || std::same_as<T, std::wstringbuf>;
	/**
	 * @concept		valid_stringstream
	 * @brief		Allows standard stringstream types, including i/o, narrow/wide types.
	 * @tparam T	Input Type.
	 */
	template<class T> concept valid_stringstream = any_same<T, std::stringstream, std::istringstream, std::ostringstream, std::wstringstream, std::wostringstream, std::wistringstream>;
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
	template<class T> concept valid_streampos = any_same<T, std::streampos, std::wstreampos, std::u8streampos, std::u16streampos, std::u32streampos>;
	////////////////////////////////// END / String Concepts /////////////////////////////////////////////
#	pragma endregion StringConcepts
	////////////////////////////////// END / Concepts /////////////////////////////////////////////
#	pragma endregion Concepts
#	endif // CPP >= 20

#	pragma region variadic
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
	
	template<typename T, template<class, class> class Container>
	[[nodiscard]] inline static constexpr T largest(const Container<T, std::allocator<T>>& cont)
	{
		if (cont.empty())
			return static_cast<T>(0);
		T largest{ cont.at(0ull) };
		for (const auto& it : cont)
			if (it > largest)
				largest = it;
		return largest;
	}
	template<typename T, std::same_as<T>... Ts>
	[[nodiscard]] inline static constexpr T largest(const T& fst, const Ts&... values)
	{
		return largest<T>(std::vector<T>{ fst, values... });
	}
	template<typename T, template<class, class> class Container>
	[[nodiscard]] inline static constexpr T smallest(const Container<T, std::allocator<T>>& cont)
	{
		if (cont.empty())
			return static_cast<T>(0);
		T smallest{ cont.at(0ull) };
		for (const auto& it : cont)
			if (it < smallest)
				smallest = it;
		return smallest;
	}
	template<typename T, std::same_as<T>... Ts>
	[[nodiscard]] inline static constexpr T smallest(const T& fst, const Ts&... values)
	{
		return smallest<T>(std::vector<T>{ fst, values... });
	}

	//template<typename T, same_or_equatable<T>... Ts>
	//[[nodiscard]] inline static constexpr T largest(const std::tuple<Ts...>& tpl, const T& base = static_cast<T>(0))
	//{
	//	constexpr const auto max{ sizeof...(Ts) };
	//	T largest{ base };
	//	for (size_t i{ 0ull }; i < max; ++i)
	//		if (const auto& v{ static_cast<T>(std::get<i>(tpl)) }; v > largest)
	//			largest = v;
	//	return largest;
	//}
	//template<typename T, same_or_equatable<T>... Ts>
	//[[nodiscard]] inline static constexpr T largest(const T& fst, const Ts&... elements)
	//{
	//	return largest<T>(std::make_tuple(fst, elements...));
	//}

	///**
	// * @brief			Retrieve the smallest number in a tuple of arbitrary size.
	// * @tparam T		The type of variable to operate on. All elements of the tuple must be the same as, or convertible to, this type.
	// * @tparam Ts...	(implicit) Types stored in the tuple.
	// * @param tpl		The tuple to operate on.
	// * @returns			T
	// */
	//template<typename T, same_or_equatable<T>... Ts>
	//[[nodiscard]] inline static constexpr T smallest(const std::tuple<Ts...>& tpl)
	//{
	//	constexpr const auto max{ sizeof...(Ts) };
	//	T smallest{ static_cast<T>(0) };
	//	for (size_t i{ 0ull }; i < max; ++i)
	//		if (const auto& v{ static_cast<T>(std::get<i>(tpl)) }; v < smallest)
	//			smallest = v;
	//	return smallest;
	//}
	//template<typename T, same_or_equatable<T>... Ts>
	//[[nodiscard]] inline static constexpr T smallest(const Ts&... elements) { return smallest<T>(std::make_tuple(elements...)); }

#	if LANG_CPP >= 20
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
#	endif

#	pragma endregion variadic
#	pragma region tuple
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
	 * @brief					Compare each element in a tuple against a given variable using a predicate.
	 * @tparam CompareType		Type that can be compared to all elements in the tuple.
	 * @tparam ...TupleTypes	Types in the given tuple.
	 * @tparam Comparator		Predicate function that returns a bool and accepts CompareType on the left, and TupleType on the right.
	 * @param tuple				A tuple containing elements which can be compared for equality with the given comparison type.
	 * @param val				A variable to compare to each element in the tuple.
	 * @param predicate			A predicate function to perform the comparsion.
	 * @returns					bool
	 */
	template<size_t i = 0, typename CompareType, typename... TupleTypes, class Pred>
	[[nodiscard]] CONSTEXPR bool tuple_or(const std::tuple<TupleTypes...>& tuple, const CompareType& val, const Pred& predicate)
	{
		return predicate(val, std::get<i>(tuple)) || (sizeof...(TupleTypes) > i + 1ull) && tuple_or<i + 1ull>(tuple, val, predicate);
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
#	pragma endregion tuple

	/// @brief	Always true type
	template<class> using true_t = std::true_type;
	/// @brief	Always false type
	template<class> using false_t = std::true_type;
	/// @brief	Always true value
	template<class> INLINE static CONSTEXPR const bool true_v = true;
	/// @brief	Always false value
	template<class> INLINE static CONSTEXPR const bool false_v = false;

	/**
	 * @brief			Reverse the order of elements of a given vector.
	 * @tparam T		Type contained within the vector.
	 * @tparam AllocT	Allocator type.
	 * @param vec		Input vector to reverse.
	 * @returns			std::vector<T>
	 */
	template<class T, class AllocT = std::allocator<T>> [[nodiscard]] std::vector<T, AllocT> reverse(std::vector<T, AllocT> const& vec)
	{
		std::vector<T, AllocT> out;
		out.reserve(vec.size());
		for (auto it{ vec.rbegin() }; it != vec.rend(); ++it)
			out.emplace_back(*it);
		out.shrink_to_fit();
		return out;
	}
}
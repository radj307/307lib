#pragma once
/**
 * @file	opt3.hpp
 * @author	radj307
 * @brief	Contains the opt3 namespace & its contents; a fully-featured commandline argument parser, container, & API.
 *\n		This is 307lib's 3rd-generation argument parser, and is intended to replace ParamsAPI2.
 */
#include <sysarch.h>
#include <var.hpp>
#include <make_exception.hpp>
#include <predicate.hpp>
#include <strcore.hpp>

#include <concepts>
#include <compare>
#include <string>
#include <variant>
#include <optional>
#include <set>
#include <map>
#include <execution>

 /**
  * @namespace	opt3
  * @brief		Contains a commandline argument parser & container object.
  */
namespace opt3 {
	template<var::valid_char TChar, std::derived_from<std::char_traits<TChar>> TCharTraits = std::char_traits<TChar>, std::derived_from<std::allocator<TChar>> TAlloc = std::allocator<TChar>>
	std::basic_string<TChar, TCharTraits, TAlloc> trim_preceding(std::basic_string<TChar, TCharTraits, TAlloc> str, std::vector<TChar> const& chars)
	{
		std::basic_string<TChar, TCharTraits, TAlloc> chars_s;
		for (const auto& it : chars) chars_s += it;
		if (const auto& beg{ str.find_first_not_of(chars_s) }; beg < str.size())
			return str.substr(beg);
		return str;
	}
	template<var::valid_char TChar, std::derived_from<std::char_traits<TChar>> TCharTraits = std::char_traits<TChar>, std::derived_from<std::allocator<TChar>> TAlloc = std::allocator<TChar>>
	std::basic_string<TChar, TCharTraits, TAlloc> trim_trailing(std::basic_string<TChar, TCharTraits, TAlloc> str, std::vector<TChar> const& chars)
	{
		std::basic_string<TChar, TCharTraits, TAlloc> chars_s;
		for (const auto& it : chars) chars_s += it;
		if (const auto& end{ str.find_last_not_of(chars_s) }; end < str.size())
			return str.substr(0ull, end + 1ull);
		return str;
	}
	template<var::valid_char TChar, std::derived_from<std::char_traits<TChar>> TCharTraits = std::char_traits<TChar>, std::derived_from<std::allocator<TChar>> TAlloc = std::allocator<TChar>>
	std::basic_string<TChar, TCharTraits, TAlloc> trim(std::basic_string<TChar, TCharTraits, TAlloc> str, std::vector<TChar> const& chars = { $c(TChar, ' '), $c(TChar, '\t'), $c(TChar, '\v'), $c(TChar, '\r'), $c(TChar, '\n') })
	{
		return trim_trailing(trim_preceding(str, chars), chars);
	}
	/**
	 * @struct	basic_vstring
	 * @brief	Variable-string object with constructor overloads that accept a single character, c-strings, & strings. If the filesystem library is included (before this header), std::filesystem::path types are also allowed.
	 */
	template<var::valid_char TChar, std::derived_from<std::char_traits<TChar>> TCharTraits = std::char_traits<TChar>, std::derived_from<std::allocator<TChar>> TAlloc = std::allocator<TChar>>
	struct basic_vstring : public std::basic_string<TChar, TCharTraits, TAlloc> {
		using base = std::basic_string<TChar, TCharTraits, TAlloc>;
		using base::base;
		/// @brief	Accepts a single character of type TChar
		CONSTEXPR basic_vstring(const TChar& c) : base(1ull, c) {}
		/// @brief	Accepts a string of TChars
		CONSTEXPR basic_vstring(const std::basic_string<TChar, TCharTraits, TAlloc>& s) : base(s) {}
		/// @brief	Accepts a c-string of TChars
		CONSTEXPR basic_vstring(const TChar* cs) : base(cs) {}
	#ifdef _FILESYSTEM_
		CONSTEXPR basic_vstring(const std::filesystem::path& p) requires var::any_same<TChar, char, signed char, unsigned char> : base(p.generic_string()) {}
		CONSTEXPR basic_vstring(const std::filesystem::path& p) requires std::same_as<TChar, wchar_t> : base(p.generic_wstring()) {}
		CONSTEXPR basic_vstring(const std::filesystem::path& p) requires std::same_as<TChar, char8_t> : base(p.generic_u8string()) {}
		CONSTEXPR basic_vstring(const std::filesystem::path& p) requires std::same_as<TChar, char16_t> : base(p.generic_u16string()) {}
		CONSTEXPR basic_vstring(const std::filesystem::path& p) requires std::same_as<TChar, char32_t> : base(p.generic_u32string()) {}
	#endif

		CONSTEXPR bool is_single_char() const { return this->size() == 1ull; }
		CONSTEXPR char get_single_char() const { return this->front(); }

		CONSTEXPR operator std::basic_string<TChar, TCharTraits, TAlloc>() const noexcept { return *this; }
	#ifdef _FILESYSTEM_
		CONSTEXPR operator std::filesystem::path() const noexcept { return std::filesystem::path{ this->operator std::basic_string<TChar, TCharTraits, TAlloc>() }; }
	#endif
	};
	/// @brief	Narrow-char width string type.
	using vstring = basic_vstring<char, std::char_traits<char>, std::allocator<char>>;

	namespace _internal {
		/// @brief	The underlying value type in opt3::Parameter.
		using parameter_t = std::string;
		/// @brief	The underlying value type in opt3::Flag.
		using flag_t = std::pair<char, std::optional<std::string>>;
		/// @brief	The underlying value type in opt3::Option.
		using option_t = std::pair<std::string, std::optional<std::string>>;
	}

	/**
	 * @struct	base_arg
	 * @brief	An empty struct that serves as the templateless base type for the basic_arg_t struct.
	 */
	struct base_arg {};

	/**
	 * @struct		basic_arg_t
	 * @brief		Basic argument wrapper object that contains a templated variable type with some useful methods.
	 * @tparam T	The type of the underlying value member.
	 */
	template<typename T>
	struct basic_arg_t : public base_arg {
	protected:
		/// @brief	The underlying value member.
		T _value;

	public:
		/**
		 * @brief		Default Constructor.
		 *\n			Only available for types that do not require arguments to construct.
		 */
		CONSTEXPR basic_arg_t() noexcept requires(std::constructible_from<T>) : _value{} {}
		/**
		 * @brief			Value-Move Constructor
		 * @param value		rvalue reference of the value to assign to this object.
		 */
		CONSTEXPR basic_arg_t(T&& value) noexcept : _value{ std::move(value) } {}
		/**
		 * @brief			Value-Copy Constructor
		 * @param value		The value to assign to this object.
		 */
		CONSTEXPR basic_arg_t(T const& value) : _value{ value } {}

		/// @brief	Get the name of this argument.
		CONSTEXPR std::string name() const requires (std::same_as<T, _internal::parameter_t>) { return _value; }
		/// @brief	Get the name of this argument.
		CONSTEXPR std::string name() const requires (std::same_as<T, _internal::flag_t>) { return vstring{ _value.first }; }
		/// @brief	Get the name of this argument.
		CONSTEXPR std::string name() const requires (std::same_as<T, _internal::option_t>) { return{ _value.first }; }

		/// @brief	Get the capture value of this argument as its actual type.
		CONSTEXPR std::optional<std::string> getValue() const requires(var::any_same<T, _internal::flag_t, _internal::option_t>) { return _value.second; }

		/// @brief	Check if this argument has a captured value. (always returns false for parameters)
		CONSTEXPR bool has_capture() const requires (std::same_as<T, _internal::parameter_t>) { return false; }
		/// @brief	Check if this argument has a captured value.
		CONSTEXPR bool has_capture() const requires (var::any_same<T, _internal::flag_t, _internal::option_t>) { return _value.second.has_value(); }

		/**
		 * @brief				Retrieve the capture value of this argument if it exists; otherwise returns the given string instead.
		 * @param defaultValue	A string to return when a capture value is not available.
		 * @returns				The captured value of this argument or defaultValue if one wasn't available.
		 */
		CONSTEXPR std::string capture_or(std::string const& defaultValue) const requires (var::any_same<T, _internal::flag_t, _internal::option_t>) { return _value.second.value_or(defaultValue); }
		/**
		 * @brief				Retrieve the capture value of this argument.
		 * @returns				The captured value of this argument.
		 * @throws ex::except	This argument does not contain a captured value.
		 */
		CONSTEXPR std::string capture() const requires (var::any_same<T, _internal::flag_t, _internal::option_t>) { return _value.second.value(); }
	};

	/// @brief	Constraint that allows any types derived from base_arg.
	template<typename T> concept valid_arg = std::derived_from<T, base_arg>;

	/// @brief	Parameter argument type; these are any arguments without special meaning.
	using Parameter = basic_arg_t<_internal::parameter_t>;
	/// @brief	Flag argument type; these are single-character arguments that can be chained together and can capture values when enabled.
	using Flag = basic_arg_t<_internal::flag_t>;
	/// @brief	Option argument type; these are multi-character arguments that can capture values when enabled.
	using Option = basic_arg_t<_internal::option_t>;

	/**
	 * @struct	variantarg
	 * @brief	An abstraction wrapper around std::variant that directly exposes methods from the underlying basic_arg_t struct.
	 *\n		This allows the programmer to interact with std::variant argument types as if they weren't actually variants.
	 */
	struct variantarg : public std::variant<Parameter, Flag, Option> {
		using base = std::variant<Parameter, Flag, Option>;
		using base::base;

		/**
		 * @brief		Compare the name of this argument to the given name.
		 * @param name	The name to compare this argument to.
		 * @returns		true when names match; otherwise false.
		 */
		WINCONSTEXPR bool compare_name(vstring const& name) const noexcept
		{
			return this->name() == name;
		}

		/**
		 * @brief
		 * @tparam TVisitor
		 * @param visitor		A function or lambda to use as the visitor when calling ``
		 * @returns				The value returned by the visitor function.
		 */
		template<typename TVisitor>
		CONSTEXPR auto visit(const TVisitor& visitor) const noexcept
		{
			return std::visit(visitor, static_cast<base>(*this));
		}

		/**
		 * @brief		Gets the name of this argument.
		 * @returns		The name of this argument, excluding any prefixes that were stripped during parsing.
		 */
		std::string name() const noexcept;
		/**
		 * @brief		Directly gets the capture value of this argument as the type it is stored as; or std::nullopt for Parameter types.
		 * @returns		The captured value of this argument if it has one; otherwise std::nullopt.
		 */
		std::optional<std::string> getValue() const noexcept;
		/**
		 * @brief		Checks if this argument has a captured value.
		 * @returns		true when this argument has a captured value; otherwise false.
		 */
		bool has_capture() const noexcept;
		/**
		 * @brief				Gets the capture value of this argument if it exists; otherwise returns the given string instead.
		 * @param defaultValue	The default string value to return when this argument doesn't have a capture value.
		 * @returns				std::string
		 */
		std::string capture_or(std::string const&) const noexcept;
		/**
		 * @brief				Gets the capture value of this argument.
		 * @returns				The capture value of this argument.
		 * @throws
		 */
		std::string capture() const noexcept(false);

		/**
		 * @brief		Check if this variantarg instance's type is the same as a given type.
		 * @tparam T	A valid_arg type to compare to.
		 * @returns		true when this variantarg's type is the same as type T; otherwise false.
		 */
		template<valid_arg T> CONSTEXPR bool is_type() const noexcept { return std::holds_alternative<T>(*this); }
		/**
		 * @brief		Check if this variantarg instance's type is the same as any of the given types.
		 * @tparam Ts	Any number of valid_arg types to compare to.
		 * @returns		true when this variantarg's type is the same as any of the given types; otherwise false.
		 */
		template<valid_arg... Ts> CONSTEXPR bool is_any_type() const noexcept { return var::variadic_or(std::holds_alternative<Ts>(*this)...); }

		/**
		 * @brief		Comparison Operator
		 * @param l		A variantarg instance to compare
		 * @param r		Another variantarg instance to compare to
		 * @returns		true when both l and r have the same name and capture value; otherwise false.
		 */
		friend bool operator==(const variantarg& l, const variantarg& r)
		{
			return l.compare_name(r.name())
				&& l.has_capture() == r.has_capture()
				&& (l.has_capture() ? l.capture() == r.capture() : true);
		}
	};
	std::string variantarg::name() const noexcept
	{
		return this->visit([](auto&& value) -> std::string {
			using T = std::decay_t<decltype(value)>;

		if constexpr (std::same_as<T, Parameter>)
			return value.name();
		else if constexpr (std::same_as<T, Flag>)
			return value.name();
		else if constexpr (std::same_as<T, Option>)
			return value.name();
		//else static_assert(false, "opt3::variantarg:  Visitor doesn't handle all possible types!");
						   });
	}
	std::optional<std::string> variantarg::getValue() const noexcept
	{
		return this->visit([](auto&& value) -> std::optional<std::string> {
			using T = std::decay_t<decltype(value)>;

		if constexpr (std::same_as<T, Parameter>)
			return std::nullopt;
		else if constexpr (std::same_as<T, Flag>)
			return value.getValue();
		else if constexpr (std::same_as<T, Option>)
			return value.getValue();
		//else static_assert(false, "opt3::variantarg:  Visitor doesn't handle all possible types!");
						   });
	}
	bool variantarg::has_capture() const noexcept
	{
		return this->visit([](auto&& value) -> bool {
			using T = std::decay_t<decltype(value)>;

		if constexpr (std::same_as<T, Parameter>)
			return value.has_capture();
		else if constexpr (std::same_as<T, Flag>)
			return value.has_capture();
		else if constexpr (std::same_as<T, Option>)
			return value.has_capture();
		//else static_assert(false, "opt3::variantarg:  Visitor doesn't handle all possible types!");
						   });
	}
	std::string variantarg::capture_or(std::string const& defaultValue) const noexcept
	{
		return this->visit([&defaultValue](auto&& value) -> std::string {
			using T = std::decay_t<decltype(value)>;

		if constexpr (std::same_as<T, Parameter>)
			return defaultValue;
		else if constexpr (std::same_as<T, Flag>)
			return value.capture_or(defaultValue);
		else if constexpr (std::same_as<T, Option>)
			return value.capture_or(defaultValue);
		//else static_assert(false, "opt3::variantarg:  Visitor doesn't handle all possible types!");
						   });
	}
	std::string variantarg::capture() const noexcept(false)
	{
		return this->visit([](auto&& value) -> std::string {
			using T = std::decay_t<decltype(value)>;

		if constexpr (std::same_as<T, Parameter>)
			throw make_exception("opt3::variantarg:  Cannot retrieve capture value from an argument of type parameter!");
		else if constexpr (std::same_as<T, Flag>)
			return value.capture();
		else if constexpr (std::same_as<T, Option>)
			return value.capture();
		//else static_assert(false, "opt3::variantarg:  Visitor doesn't handle all possible types!");
						   });
	}


	/**
	 * @brief				Instantiates a variantarg of the specified type, with the given values.
	 * @tparam TValue		The type of the underlying value.  This is passed to the templated basic_arg_t struct.
	 * @tparam T			Any type that is the same as, or is implicitly convertible to, std::string or char
	 * @param name			The name of the argument.
	 * @param capture		An optional capture value for this argument.
	 *\n					Note that when TReturn is _internal::parameter_t
	 * @returns				A variantarg containing an argument of the specified type.
	 */
	template<var::any_same<_internal::parameter_t, _internal::flag_t, _internal::option_t> TValue>
	WINCONSTEXPR variantarg make_argument(vstring&& name, std::optional<std::string>&& capture = std::nullopt) noexcept(false)
	{
		if constexpr (std::same_as<TValue, _internal::parameter_t>)
			return{ basic_arg_t<_internal::parameter_t>{ std::forward<vstring>(name) } };
		else if constexpr (std::same_as<TValue, _internal::flag_t>)
			return{ basic_arg_t<_internal::flag_t>{ std::make_pair(std::forward<vstring>(name).get_single_char(), std::forward<std::optional<std::string>>(capture))  } };
		else if constexpr (std::same_as<TValue, _internal::option_t>)
			return{ basic_arg_t<_internal::option_t>{ std::make_pair(std::forward<vstring>(name), std::forward<std::optional<std::string>>(capture))} };
		else throw make_exception("make_argument() failed:  Invalid target type specified!");
	}
	/**
	 * @brief				Instantiates a variantarg of the specified type, with the given values.
	 *\n					This overload accepts actual argument types instead of their underlying types, and translates them to the appropriate underlying type.
	 * @tparam TArgument	The type of argument to instantiate.
	 * @tparam T			Any type that is the same as, or is implicitly convertible to, std::string or char
	 * @param name			The name of the argument.
	 * @param capture		An optional capture value for this argument.
	 *\n					Note that when TReturn is _internal::parameter_t
	 * @returns				A variantarg containing an argument of the specified type.
	 */
	template<var::any_same<Parameter, Flag, Option> TArgument>
	WINCONSTEXPR variantarg make_argument(vstring&& name, std::optional<std::string>&& capture = std::nullopt) noexcept(false)
	{
		if constexpr (std::same_as<TArgument, Parameter>)
			return make_argument<_internal::parameter_t>(std::forward<vstring>(name), std::forward<std::optional<std::string>>(capture));
		else if constexpr (std::same_as<TArgument, Flag>)
			return make_argument<_internal::flag_t>(std::forward<vstring>(name), std::forward<std::optional<std::string>>(capture));
		else if constexpr (std::same_as<TArgument, Option>)
			return make_argument<_internal::option_t>(std::forward<vstring>(name), std::forward<std::optional<std::string>>(capture));
		else throw make_exception("make_argument() failed:  Invalid target type specified!");
	}

	/**
	 * @struct	arg_container
	 * @brief	A container type derived directly from std::vector that exposes utility methods for managing & interacting with argument lists.
	 *\n		This object contains the bulk of the code in the entire opt3 library.
	 *\n		Additionally, this object and all derived types may be declared `const` without any loss of functionality, since commandline arguments are only parsed once.
	 */
	struct arg_container : public std::vector<variantarg> {
		using base = std::vector<variantarg>;
		using base::base;

	#pragma region subvec
		/**
		 * @brief			Retrieves (copies) the specified segment of this argument container into a new instance.
		 * @param begin		(Inclusive) An iterator pointing to the first element in the subvec.
		 * @param end		(Exclusive) An iterator pointing to the element AFTER the last element in the subvec.
		 * @returns			A new arg_container instance containing the specified segment.
		 */
		WINCONSTEXPR arg_container subvec(const std::vector<variantarg>::const_iterator& begin, const std::vector<variantarg>::const_iterator& end) const
		{
			return arg_container{ begin, end };
		}
		/**
		 * @brief			Retrieves (copies) the specified segment of this argument container into a new instance.
		 * @param off		The index of the first element in the subvec.
		 * @param count		The total number of elements to include in the subvec. (Must be greater than 0; Default: 1)
		 * @returns			A new arg_container instance containing the specified segment.
		 */
		WINCONSTEXPR arg_container subvec(const size_t& off, const size_t& count = 1ull) const
		{
			if (count == 0ull)
				throw make_exception("subvec() failed:  Cannot create a subvec with a total of 0 elements!");
			return arg_container{ this->begin() + off, this->begin() + (off + count) };
		}
	#pragma endregion subvec

	#pragma region find_if
		/**
		 * @brief			Finds the first element for which the given predicate returns true.
		 * @param pred		A predicate function to apply to each element.
		 * @returns			An iterator to the first matching element, or the ending iterator if no matches were found.
		 */
		template<valid_arg... TFilterTypes, std::predicate<variantarg> TPredicate>
		WINCONSTEXPR std::vector<variantarg>::const_iterator find_if(const TPredicate& pred) const
		{
			constexpr bool match_any_type{ sizeof...(TFilterTypes) == 0ull };
			for (auto it{ this->begin() }, end{ this->end() }; it != end; ++it) {
				if ((match_any_type || it->is_any_type<TFilterTypes...>()) && pred(*it)) {
					return it;
				}
			}
			return this->end();
		}
		/**
		 * @brief			Finds the first element for which any of the given predicates return true.
		 * @param preds		Any number of predicate functions to apply to each element.
		 * @returns			An iterator to the first matching element, or the ending iterator if no matches were found.
		 */
		template<valid_arg... TFilterTypes, std::predicate<variantarg>... TPredicates>
		WINCONSTEXPR std::vector<variantarg>::const_iterator find_if_any(const TPredicates&... preds) const
		{
			constexpr bool match_any_type{ sizeof...(TFilterTypes) == 0ull };
			for (auto it{ this->begin() }, end{ this->end() }; it != end; ++it)
				if ((match_any_type || it->is_any_type<TFilterTypes...>()) && var::variadic_or(preds(*it)...))
					return it;
			return this->end();
		}
		/**
		 * @brief			Finds the first element for which all of the given predicates return true.
		 * @param preds		Any number of predicate functions to apply to each element.
		 * @returns			An iterator to the first matching element, or the ending iterator if no matches were found.
		 */
		template<valid_arg... TFilterTypes, std::predicate<variantarg>... TPredicates>
		WINCONSTEXPR std::vector<variantarg>::const_iterator find_if_all(const TPredicates&... preds) const
		{
			constexpr bool match_any_type{ sizeof...(TFilterTypes) == 0ull };
			for (auto it{ this->begin() }, end{ this->end() }; it != end; ++it)
				if ((match_any_type || it->is_any_type<TFilterTypes...>()) && var::variadic_and(preds(*it)...))
					return it;
			return this->end();
		}
	#pragma endregion find_if

	#pragma region rfind_if
		/**
		 * @brief			Finds the first element for which the given predicate returns true.
		 * @param pred		A predicate function to apply to each element.
		 * @returns			An iterator to the first matching element, or the ending iterator if no matches were found.
		 */
		template<valid_arg... TFilterTypes, std::predicate<variantarg> TPredicate>
		WINCONSTEXPR std::vector<variantarg>::const_reverse_iterator rfind_if(const TPredicate& pred) const
		{
			constexpr bool match_any_type{ sizeof...(TFilterTypes) == 0ull };
			for (auto it{ this->rbegin() }, end{ this->rend() }; it != end; ++it) {
				if ((match_any_type || it->is_any_type<TFilterTypes...>()) && pred(*it)) {
					return it;
				}
			}
			return this->rend();
		}
		/**
		 * @brief			Finds the first element for which any of the given predicates return true.
		 * @param preds		Any number of predicate functions to apply to each element.
		 * @returns			An iterator to the first matching element, or the ending iterator if no matches were found.
		 */
		template<valid_arg... TFilterTypes, std::predicate<variantarg>... TPredicates>
		WINCONSTEXPR std::vector<variantarg>::const_reverse_iterator rfind_if_any(const TPredicates&... preds) const
		{
			constexpr bool match_any_type{ sizeof...(TFilterTypes) == 0ull };
			for (auto it{ this->rbegin() }, end{ this->rend() }; it != end; ++it)
				if ((match_any_type || it->is_any_type<TFilterTypes...>()) && var::variadic_or(preds(*it)...))
					return it;
			return this->rend();
		}
		/**
		 * @brief			Finds the first element for which all of the given predicates return true.
		 * @param preds		Any number of predicate functions to apply to each element.
		 * @returns			An iterator to the first matching element, or the ending iterator if no matches were found.
		 */
		template<valid_arg... TFilterTypes, std::predicate<variantarg>... TPredicates>
		WINCONSTEXPR std::vector<variantarg>::const_reverse_iterator rfind_if_all(const TPredicates&... preds) const
		{
			constexpr bool match_any_type{ sizeof...(TFilterTypes) == 0ull };
			for (auto it{ this->rbegin() }, end{ this->rend() }; it != end; ++it)
				if ((match_any_type || it->is_any_type<TFilterTypes...>()) && var::variadic_and(preds(*it)...))
					return it;
			return this->rend();
		}
	#pragma endregion rfind_if

	#pragma region find
		template<valid_arg... TFilterTypes>
		WINCONSTEXPR std::vector<variantarg>::const_iterator find(vstring const& name) const noexcept
		{
			constexpr bool match_any_type{ sizeof...(TFilterTypes) == 0ull };
			for (auto it{ this->begin() }, end{ this->end() }; it != end; ++it) {
				if ((match_any_type || it->is_any_type<TFilterTypes...>()) && it->compare_name(name)) {
					return it;
				}
			}
			return this->end();
		}
		template<valid_arg... TFilterTypes, var::same_or_convertible<vstring>... Ts>
		WINCONSTEXPR std::vector<variantarg>::const_iterator find_any(Ts&&... names) const noexcept
		{
			constexpr bool
				match_any_type{ sizeof...(TFilterTypes) == 0ull },
				match_any_name{ sizeof...(Ts) == 0ull };
			for (auto it{ this->begin() }, end{ this->end() }; it != end; ++it) {
				if ((match_any_type || it->is_any_type<TFilterTypes...>()) && (match_any_name || var::variadic_or(it->compare_name(std::forward<Ts>(names))...))) {
					return it;
				}
			}
			return this->end();
		}
		template<valid_arg... TFilterTypes, var::same_or_convertible<vstring>... Ts>
		WINCONSTEXPR std::vector<std::vector<variantarg>::const_iterator> find_all(Ts&&... names) const noexcept
		{
			constexpr bool
				match_any_type{ sizeof...(TFilterTypes) == 0ull },
				match_any_name{ sizeof...(Ts) == 0ull };
			std::vector<std::vector<variantarg>::const_iterator> vec;
			constexpr size_t block_size{ sizeof...(names) };
			vec.reserve(block_size);
			for (auto it{ this->begin() }, end{ this->end() }; it != end; ++it) {
				if ((match_any_type || it->is_any_type<TFilterTypes...>()) && (match_any_name || var::variadic_or(it->compare_name(std::forward<Ts>(names))...))) {
					vec.emplace_back(it);
					if (vec.size() == vec.capacity())// increase capacity:
						vec.reserve(vec.size() + block_size);
				}
			}
			vec.shrink_to_fit();
			return vec;
		}
	#pragma endregion find

	#pragma region rfind
		template<valid_arg... TFilterTypes>
		WINCONSTEXPR std::vector<variantarg>::const_reverse_iterator rfind(vstring const& name) const noexcept
		{
			constexpr bool match_any_type{ sizeof...(TFilterTypes) == 0ull };
			for (auto it{ this->rbegin() }, end{ this->rend() }; it != end; ++it) {
				if ((match_any_type || it->is_any_type<TFilterTypes...>()) && it->compare_name(name)) {
					return it;
				}
			}
			return this->rend();
		}
		template<valid_arg... TFilterTypes, var::same_or_convertible<vstring>... Ts>
		WINCONSTEXPR std::vector<variantarg>::const_reverse_iterator rfind_any(Ts&&... names) const noexcept
		{
			constexpr bool
				match_any_type{ sizeof...(TFilterTypes) == 0ull },
				match_any_name{ sizeof...(Ts) == 0ull };
			for (auto it{ this->rbegin() }, end{ this->rend() }; it != end; ++it) {
				if ((match_any_type || it->is_any_type<TFilterTypes...>()) && (match_any_name || var::variadic_or(it->compare_name(std::forward<Ts>(names))...))) {
					return it;
				}
			}
			return this->rend();
		}
		template<valid_arg... TFilterTypes, var::same_or_convertible<vstring>... Ts>
		WINCONSTEXPR std::vector<std::vector<variantarg>::const_reverse_iterator> rfind_all(Ts&&... names) const noexcept
		{
			constexpr bool
				match_any_type{ sizeof...(TFilterTypes) == 0ull },
				match_any_name{ sizeof...(Ts) == 0ull };
			std::vector<std::vector<variantarg>::const_reverse_iterator> vec;
			constexpr size_t block_size{ sizeof...(names) };
			vec.reserve(block_size);
			for (auto it{ this->rbegin() }, end{ this->rend() }; it != end; ++it) {
				if ((match_any_type || it->is_any_type<TFilterTypes...>()) && (match_any_name || var::variadic_or(it->compare_name(std::forward<Ts>(names))...))) {
					vec.emplace_back(it);
					if (vec.size() == vec.capacity())// increase capacity:
						vec.reserve(vec.size() + block_size);
				}
			}
			vec.shrink_to_fit();
			return vec;
		}
	#pragma endregion rfind

	#pragma region get
		/**
		 * @brief					Gets the specified argument of the specified type.
		 * @tparam TFilterTypes...	Any number of types to limit the returned result types to.  When left empty, all types are considered matching.
		 * @param name				The name of the argument to search for.
		 * @returns					The first variantarg object with the given name if it exists; otherwise std::nullopt.
		 */
		template<valid_arg... TFilterTypes>
		CONSTEXPR std::optional<variantarg> get(vstring const& name) const noexcept
		{
			if (const auto& it{ this->find<TFilterTypes...>(name) }; it != this->end())
				return *it;
			return std::nullopt;
		}
		/**
		 * @brief					Gets any of the specified arguments of the specified type(s).
		 * @tparam TFilterTypes...	Any number of types to limit the returned result types to.  When left empty, all types are considered matching.
		 * @tparam Ts...			Variadic types that are the same as or are convertible to vstring.  When left empty, all names are considered matching.
		 * @param name				The name of the argument to search for.
		 * @returns					The first variantarg object with the given name if it exists; otherwise std::nullopt.
		 */
		template<valid_arg... TFilterTypes, var::same_or_convertible<vstring>... Ts>
		CONSTEXPR std::optional<variantarg> get_any(Ts&&... names) const noexcept
		{
			if (const auto& it{ this->find_any<TFilterTypes...>(std::forward<Ts>(names)...) }; it != this->end())
				return *it;
			return std::nullopt;
		}
		/**
		 * @brief					Gets all of the specified arguments of the specified type(s).
		 * @tparam TFilterTypes...	Any number of types to limit the returned result types to.  When left empty, all types are considered matching.
		 * @tparam Ts...			Variadic types that are the same as or are convertible to vstring.  When left empty, all names are considered matching.
		 * @param name				The name of the argument to search for.
		 * @returns					The first variantarg object with the given name if it exists; otherwise std::nullopt.
		 */
		template<valid_arg... TFilterTypes, var::same_or_convertible<vstring>... Ts>
		CONSTEXPR std::vector<variantarg> get_all(Ts&&... names) const noexcept
		{
			std::vector<variantarg> vec;
			const auto& all{ this->find_all<TFilterTypes...>(std::forward<Ts>(names)...) };
			vec.reserve(all.size());
			for (const auto& it : all)
				vec.emplace_back(*it);
			vec.shrink_to_fit();
			return vec;
		}
	#pragma endregion get

	#pragma region rget
		/**
		 * @brief					Gets the specified argument of the specified type.  This overload searches in reverse.
		 * @tparam TFilterTypes...	Any number of types to limit the returned result types to.  When left empty, all types are considered matching.
		 * @param name				The name of the argument to search for.
		 * @returns					The first variantarg object with the given name if it exists; otherwise std::nullopt.
		 */
		template<valid_arg... TFilterTypes>
		CONSTEXPR std::optional<variantarg> rget(vstring const& name) const noexcept
		{
			if (const auto& it{ this->rfind<TFilterTypes...>(name) }; it != this->end())
				return *it;
			return std::nullopt;
		}
		/**
		 * @brief					Gets any of the specified arguments of the specified type(s).  This overload searches in reverse.
		 * @tparam TFilterTypes...	Any number of types to limit the returned result types to.  When left empty, all types are considered matching.
		 * @tparam Ts...			Variadic types that are the same as or are convertible to vstring.  When left empty, all names are considered matching.
		 * @param name				The name of the argument to search for.
		 * @returns					The first variantarg object with the given name if it exists; otherwise std::nullopt.
		 */
		template<valid_arg... TFilterTypes, var::same_or_convertible<vstring>... Ts>
		CONSTEXPR std::optional<variantarg> rget_any(Ts&&... names) const noexcept
		{
			if (const auto& it{ this->rfind_any<TFilterTypes...>(std::forward<Ts>(names)...) }; it != this->end())
				return *it;
			return std::nullopt;
		}
		/**
		 * @brief					Gets all of the specified arguments of the specified type(s).  This overload searches in reverse.
		 * @tparam TFilterTypes...	Any number of types to limit the returned result types to.  When left empty, all types are considered matching.
		 * @tparam Ts...			Variadic types that are the same as or are convertible to vstring.  When left empty, all names are considered matching.
		 * @param name				The name of the argument to search for.
		 * @returns					The first variantarg object with the given name if it exists; otherwise std::nullopt.
		 */
		template<valid_arg... TFilterTypes, var::same_or_convertible<vstring>... Ts>
		CONSTEXPR std::vector<variantarg> rget_all(Ts&&... names) const noexcept
		{
			std::vector<variantarg> vec;
			const auto& all{ this->rfind_all<TFilterTypes...>(std::forward<Ts>(names)...) };
			vec.reserve(all.size());
			for (const auto& it : all)
				vec.emplace_back(*it);
			vec.shrink_to_fit();
			return vec;
		}
	#pragma endregion rget

	#pragma region getv
		/**
		 * @brief					Gets the captured value from the first matching argument.
		 *\n						Only arguments with values are considered to be matching, all other arguments are skipped; including if their name does match.
		 * @tparam TFilterTypes...	Any number of types to limit the returned result types to.  When left empty, all types are considered matching.
		 * @param name				The name of the argument to search for.
		 * @returns					The capture value of the first matching argument if found; otherwise std::nullopt.
		 */
		template<valid_arg... TFilterTypes>
		CONSTEXPR std::optional<std::string> getv(vstring const& name) const noexcept
		{
			static_assert(!var::any_same<Parameter, TFilterTypes...>, "opt3::arg_container::getv() cannot be used to get non-Flags or non-Options!");
			constexpr bool match_any_type{ sizeof...(TFilterTypes) == 0ull };
			for (auto it{ this->begin() }, end{ this->end() }; it != end; ++it) {
				if (!it->has_capture()) continue;
				if ((match_any_type || it->is_any_type<TFilterTypes...>()) && it->compare_name(name)) {
					if (it->is_type<Parameter>())
						return it->name();
					else
						return it->capture();
				}
			}
			return std::nullopt;
		}
		/**
		 * @brief					Gets the captured value from the first matching argument.
		 *\n						Only arguments with values are considered to be matching, all other arguments are skipped; including if their name does match.
		 * @tparam TFilterTypes...	Any number of types to limit the returned result types to.  When left empty, all types are considered matching.
		 * @param names				The name of the argument to search for.
		 * @returns					The capture value of the first matching argument if found; otherwise std::nullopt.
		 */
		template<valid_arg... TFilterTypes, var::same_or_convertible<vstring>... Ts>
		CONSTEXPR std::optional<std::string> getv_any(Ts&&... names) const noexcept
		{
			static_assert(!var::any_same<Parameter, TFilterTypes...>, "opt3::arg_container::getv() cannot be used to get non-Flags or non-Options!");
			constexpr bool
				match_any_type{ sizeof...(TFilterTypes) == 0ull },
				match_any_name{ sizeof...(Ts) == 0ull };
			for (auto it{ this->begin() }, end{ this->end() }; it != end; ++it) {
				if (!it->has_capture()) continue;
				if ((match_any_type || it->is_any_type<TFilterTypes...>()) && ((match_any_name || var::variadic_or(it->compare_name(std::forward<Ts>(names))...)))) {
					if (it->is_type<Parameter>())
						return it->name();
					else
						return it->capture();
				}
			}
			return std::nullopt;
		}
		/**
		 * @brief					Gets the captured value from the first matching argument.
		 *\n						Only arguments with values are considered to be matching, all other arguments are skipped; including if their name does match.
		 * @tparam TFilterTypes...	Any number of types to limit the returned result types to.  When left empty, all types are considered matching.
		 * @param names				The name of the argument to search for.
		 * @returns					The capture value of the first matching argument if found; otherwise std::nullopt.
		 */
		template<valid_arg... TFilterTypes, var::same_or_convertible<vstring>... Ts>
		CONSTEXPR std::vector<std::string> getv_all(Ts&&... names) const noexcept
		{
			constexpr bool
				match_any_type{ sizeof...(TFilterTypes) == 0ull },
				match_any_name{ sizeof...(Ts) == 0ull };
			std::vector<std::string> vec;
			constexpr size_t block_size{ sizeof...(Ts) };
			vec.reserve(block_size);
			for (auto it{ this->begin() }, end{ this->end() }; it != end; ++it) {
				if (!it->has_capture() && !it->is_type<Parameter>()) continue;
				if ((match_any_type || it->is_any_type<TFilterTypes...>()) && ((match_any_name || var::variadic_or(it->compare_name(std::forward<Ts>(names))...)))) {
					if (it->is_type<Parameter>())
						vec.emplace_back(it->name());
					else
						vec.emplace_back(it->capture());
					if (vec.size() == vec.capacity())
						vec.reserve(vec.size() + block_size);
				}
			}
			vec.shrink_to_fit();
			return vec;
		}
	#pragma endregion getv

	#pragma region rgetv
		/**
		 * @brief					Gets the captured value from the first matching argument.
		 *\n						Only arguments with values are considered to be matching, all other arguments are skipped; including if their name does match.
		 * @tparam TFilterTypes...	Any number of types to limit the returned result types to.  When left empty, all types are considered matching.
		 * @param name				The name of the argument to search for.
		 * @returns					The capture value of the first matching argument if found; otherwise std::nullopt.
		 */
		template<valid_arg... TFilterTypes>
		CONSTEXPR std::optional<std::string> rgetv(vstring const& name) const noexcept
		{
			constexpr bool match_any_type{ sizeof...(TFilterTypes) == 0ull };
			for (auto it{ this->rbegin() }, end{ this->rend() }; it != end; ++it) {
				if (!it->has_capture()) continue;
				if ((match_any_type || it->is_any_type<TFilterTypes...>()) && it->compare_name(name)) {
					if (it->is_type<Parameter>())
						return it->name();
					else
						return it->capture();
				}
			}
			return std::nullopt;
		}
		/**
		 * @brief					Gets the captured value from the first matching argument.
		 *\n						Only arguments with values are considered to be matching, all other arguments are skipped; including if their name does match.
		 * @tparam TFilterTypes...	Any number of types to limit the returned result types to.  When left empty, all types are considered matching.
		 * @param names				The name of the argument to search for.
		 * @returns					The capture value of the first matching argument if found; otherwise std::nullopt.
		 */
		template<valid_arg... TFilterTypes, var::same_or_convertible<vstring>... Ts>
		CONSTEXPR std::optional<std::string> rgetv_any(Ts&&... names) const noexcept
		{
			constexpr bool
				match_any_type{ sizeof...(TFilterTypes) == 0ull },
				match_any_name{ sizeof...(Ts) == 0ull };
			for (auto it{ this->rbegin() }, end{ this->rend() }; it != end; ++it) {
				if (!it->has_capture()) continue;
				if ((match_any_type || it->is_any_type<TFilterTypes...>()) && ((match_any_name || var::variadic_or(it->compare_name(std::forward<Ts>(names))...)))) {
					if (it->is_type<Parameter>())
						return it->name();
					else
						return it->capture();
				}
			}
			return std::nullopt;
		}
		/**
		 * @brief					Gets the captured value from the first matching argument.
		 *\n						Only arguments with values are considered to be matching, all other arguments are skipped; including if their name does match.
		 * @tparam TFilterTypes...	Any number of types to limit the returned result types to.  When left empty, all types are considered matching.
		 * @param names				The name of the argument to search for.
		 * @returns					The capture value of the first matching argument if found; otherwise std::nullopt.
		 */
		template<valid_arg... TFilterTypes, var::same_or_convertible<vstring>... Ts>
		CONSTEXPR std::vector<std::string> rgetv_all(Ts&&... names) const noexcept
		{
			constexpr bool
				match_any_type{ sizeof...(TFilterTypes) == 0ull },
				match_any_name{ sizeof...(Ts) == 0ull };
			std::vector<std::string> vec;
			constexpr size_t block_size{ sizeof...(Ts) };
			vec.reserve(block_size);
			for (auto it{ this->rbegin() }, end{ this->rend() }; it != end; ++it) {
				if (!it->has_capture()) continue;
				if ((match_any_type || it->is_any_type<TFilterTypes...>()) && ((match_any_name || var::variadic_or(it->compare_name(std::forward<Ts>(names))...)))) {
					if (it->is_type<Parameter>())
						vec.emplace_back(it->name());
					else
						vec.emplace_back(it->capture());
					if (vec.size() == vec.capacity())
						vec.reserve(vec.size() + block_size);
				}
			}
			vec.shrink_to_fit();
			return vec;
		}
	#pragma endregion rgetv

	#pragma region castget
		/**
		 * @brief					Get the specified argument casted to the specified type.
		 *\n						This overload is only available when TReturn specifies a type that is implicitly convertible from `variantarg`.
		 * @tparam TReturn			The desired return type.
		 * @tparam TFilterTypes...	Any number of types to limit the returned result types to.  When left empty, all types are considered matching.
		 * @param name				The name of the target argument.
		 * @returns					The specified argument, casted to the specified type; or std::nullopt if it wasn't found.
		 */
		template<var::convertible_from<variantarg> TReturn, valid_arg... TFilterTypes>
		CONSTEXPR std::optional<TReturn> castget(vstring const& name) const noexcept
		{
			if (const auto& v{ this->get<TFilterTypes...>(name) }; v.has_value())
				return static_cast<TReturn>(v.value());
			return std::nullopt;
		}
		/**
		 * @brief					Get the specified argument casted to the specified type.
		 * @tparam TReturn			The desired return type.
		 * @tparam TFilterTypes...	Any number of types to limit the returned result types to.  When left empty, all types are considered matching.
		 * @param name				The name of the target argument.
		 * @param converter			A conversion function that accepts type variantarg and returns type TReturn.
		 * @returns					The specified argument, casted to the specified type; or std::nullopt if it wasn't found.
		 */
		template<typename TReturn, valid_arg... TFilterTypes>
		CONSTEXPR std::optional<TReturn> castget(const std::function<TReturn(variantarg)>& converter, vstring const& name) const
		{
			if (const auto& v{ this->get<TFilterTypes...>(name) }; v.has_value())
				return converter(v.value());
			return std::nullopt;
		}
		/**
		 * @brief					Get the specified argument casted to the specified type.
		 *\n						This overload is only available when TReturn specifies a type that is implicitly convertible from variantarg.
		 * @tparam TReturn			The desired return type.
		 * @tparam TFilterTypes...	Any number of types to limit the returned result types to.  When left empty, all types are considered matching.
		 * @param name				The name of the target argument.
		 * @returns					The specified argument, casted to the specified type; or std::nullopt if it wasn't found.
		 */
		template<var::convertible_from<variantarg> TReturn, valid_arg... TFilterTypes, var::same_or_convertible<vstring>... Ts>
		CONSTEXPR std::optional<TReturn> castget_any(Ts&&... names) const noexcept
		{
			if (const auto& v{ this->get_any<TFilterTypes...>(std::forward<Ts>(names)...) }; v.has_value())
				return static_cast<TReturn>(v.value());
			return std::nullopt;
		}
		/**
		 * @brief					Get the specified argument casted to the specified type.
		 * @tparam TReturn			The desired return type.
		 * @tparam TFilterTypes...	Any number of types to limit the returned result types to.  When left empty, all types are considered matching.
		 * @param converter			A conversion function that accepts type variantarg and returns type TReturn.
		 * @param name				The name of the target argument.
		 * @returns					The specified argument, casted to the specified type; or std::nullopt if it wasn't found.
		 */
		template<typename TReturn, valid_arg... TFilterTypes, var::same_or_convertible<vstring>... Ts>
		CONSTEXPR std::optional<TReturn> castget_any(const std::function<TReturn(variantarg)>& converter, Ts&&... names) const
		{
			if (const auto& v{ this->get_any<TFilterTypes...>(std::forward<Ts>(names)...) }; v.has_value())
				return converter(v.value());
			return std::nullopt;
		}
		/**
		 * @brief					Get all of the specified argument(s), casted to the specified type.
		 *\n						This overload is only available when TReturn specifies a type that is implicitly convertible from variantarg.
		 * @tparam TReturn			The desired return type.
		 * @tparam TFilterTypes...	Any number of types to limit the returned result types to.  When left empty, all types are considered matching.
		 * @param names				The name(s) of the target argument(s) (Excluding prefix dashes).
		 * @returns					A vector containing all matching arguments.
		 */
		template<var::convertible_from<variantarg> TReturn, valid_arg... TFilterTypes, var::same_or_convertible<vstring>... Ts>
		CONSTEXPR std::vector<TReturn> castget_all(Ts&&... names) const noexcept
		{
			std::vector<TReturn> vec;
			const auto& all{ this->get_all<TFilterTypes...>(std::forward<Ts>(names)...) };
			vec.reserve(all.size());
			for (const auto& it : all)
				vec.emplace_back(static_cast<TReturn>(it));
			vec.shrink_to_fit();
			return vec;
		}
		/**
		 * @brief					Get all of the specified argument(s), casted to the specified type.
		 * @tparam TReturn			The desired return type.
		 * @tparam TFilterTypes...	Any number of types to limit the returned result types to.  When left empty, all types are considered matching.
		 * @param converter			A conversion function that accepts type variantarg and returns type TReturn.
		 * @param names				The name(s) of the target argument(s) (Excluding prefix dashes).
		 * @returns					A vector containing all matching arguments.
		 */
		template<typename TReturn, valid_arg... TFilterTypes, var::same_or_convertible<vstring>... Ts>
		CONSTEXPR std::vector<TReturn> castget_all(const std::function<TReturn(variantarg)>& converter, Ts&&... names) const
		{
			std::vector<TReturn> vec;
			const auto& all{ this->get_all<TFilterTypes...>(std::forward<Ts>(names)...) };
			vec.reserve(all.size());
			for (const auto& it : all)
				vec.emplace_back(converter(it));
			vec.shrink_to_fit();
			return vec;
		}
	#pragma endregion castget

	#pragma region castgetv
		/**
		 * @brief					Get the specified argument's capture value, casted to the specified type.
		 *\n						This overload is only available when TReturn specifies a type that is implicitly convertible from vstring.
		 * @tparam TReturn			The desired return type.
		 * @tparam TFilterTypes...	Any number of types to limit the returned result types to.  When left empty, all types are considered matching.
		 * @param name				The name of the target argument.
		 * @returns					The specified argument's capture value, casted to the specified type; or std::nullopt if the argument wasn't found, or didn't have a capture value.
		 */
		template<var::convertible_from<vstring> TReturn, valid_arg... TFilterTypes>
		CONSTEXPR std::optional<TReturn> castgetv(vstring const& name) const noexcept
		{
			if (const auto& v{ this->getv<TFilterTypes...>(name) }; v.has_value())
				return static_cast<TReturn>(v.value());
			return std::nullopt;
		}
		/**
		 * @brief					Get the specified argument's capture value, casted to the specified type.
		 * @tparam TReturn			The desired return type.
		 * @tparam TFilterTypes...	Any number of types to limit the returned result types to.  When left empty, all types are considered matching.
		 * @param name				The name of the target argument.
		 * @param converter			A conversion function that accepts type vstring and returns type TReturn.
		 * @returns					The specified argument's capture value, casted to the specified type; or std::nullopt if the argument wasn't found, or didn't have a capture value.
		 */
		template<typename TReturn, valid_arg... TFilterTypes>
		CONSTEXPR std::optional<TReturn> castgetv(const std::function<TReturn(vstring)>& converter, vstring const& name) const
		{
			if (const auto& v{ this->getv<TFilterTypes...>(name) }; v.has_value())
				return converter(v.value());
			return std::nullopt;
		}
		/**
		 * @brief					Get the specified argument's capture value, casted to the specified type.
		 *\n						This overload is only available when TReturn specifies a type that is implicitly convertible from vstring.
		 * @tparam TReturn			The desired return type.
		 * @tparam TFilterTypes...	Any number of types to limit the returned result types to.  When left empty, all types are considered matching.
		 * @param names				The name(s) of the target argument(s) (Excluding prefix dashes).
		 * @returns					The first specified argument, casted to the specified type; or std::nullopt if it wasn't found.
		 */
		template<var::convertible_from<vstring> TReturn, valid_arg... TFilterTypes, var::same_or_convertible<vstring>... Ts>
		CONSTEXPR std::optional<TReturn> castgetv_any(Ts&&... names) const noexcept
		{
			if (const auto& v{ this->getv_any<TFilterTypes...>(std::forward<Ts>(names)...) }; v.has_value())
				return static_cast<TReturn>(v.value());
			return std::nullopt;
		}
		/**
		 * @brief					Get the specified argument's capture value, casted to the specified numeric type.
		 *\n						This overload is only available when TReturn specifies a type that is implicitly convertible from vstring.
		 * @tparam TReturn			The desired return type.
		 * @tparam TFilterTypes...	Any number of types to limit the returned result types to.  When left empty, all types are considered matching.
		 * @param names				The name(s) of the target argument(s) (Excluding prefix dashes).
		 * @returns					The first specified argument, casted to the specified type; or std::nullopt if it wasn't found.
		 */
		template<var::numeric TReturn, valid_arg... TFilterTypes, var::same_or_convertible<vstring>... Ts>
		CONSTEXPR std::optional<TReturn> castgetv_any(Ts&&... names) const noexcept
		{
			if (const auto& v{ this->getv_any<TFilterTypes...>(std::forward<Ts>(names)...) }; v.has_value())
				return str::tonumber<TReturn>(v.value());
			return std::nullopt;
		}
		/**
		 * @brief					Get the specified argument's capture value, casted to bool.
		 *\n						This overload is only available when TReturn specifies a type that is implicitly convertible from vstring.
		 * @tparam TReturn			The desired return type.
		 * @tparam TFilterTypes...	Any number of types to limit the returned result types to.  When left empty, all types are considered matching.
		 * @param names				The name(s) of the target argument(s) (Excluding prefix dashes).
		 * @returns					The first specified argument, casted to the specified type; or std::nullopt if it wasn't found.
		 */
		template<std::same_as<bool> TReturn, valid_arg... TFilterTypes, var::same_or_convertible<vstring>... Ts>
		CONSTEXPR std::optional<TReturn> castgetv_any(Ts&&... names) const noexcept
		{
			if (const auto& v{ this->getv_any<TFilterTypes...>(std::forward<Ts>(names)...) }; v.has_value())
				return str::tobool(str::trim(v.value()));
			return std::nullopt;
		}
		/**
		 * @brief					Get the specified argument's capture value, casted to the specified type.
		 * @tparam TReturn			The desired return type.
		 * @tparam TFilterTypes...	Any number of types to limit the returned result types to.  When left empty, all types are considered matching.
		 * @param converter			A conversion function that accepts type vstring and returns type TReturn.
		 * @param names				The name(s) of the target argument(s) (Excluding prefix dashes).
		 * @returns					The first specified argument, casted to the specified type; or std::nullopt if it wasn't found.
		 */
		template<typename TReturn, valid_arg... TFilterTypes, var::same_or_convertible<vstring>... Ts>
		CONSTEXPR std::optional<TReturn> castgetv_any(const std::function<TReturn(vstring)>& converter, Ts&&... names) const
		{
			if (const auto& v{ this->getv_any<TFilterTypes...>(std::forward<Ts>(names)...) }; v.has_value())
				return converter(v.value());
			return std::nullopt;
		}
		/**
		 * @brief					Get all of the specified arguments' capture value(s).
		 *\n						This overload is only available when TReturn specifies a type that is implicitly convertible from vstring.
		 * @tparam TReturn			The desired return type.
		 * @tparam TFilterTypes...	Any number of types to limit the returned result types to.  When left empty, all types are considered matching.
		 * @param names				The name(s) of the target argument(s) (Excluding prefix dashes).
		 * @returns					A vector containing the capture value(s) of all matches.
		 */
		template<var::convertible_from<vstring> TReturn, valid_arg... TFilterTypes, var::same_or_convertible<vstring>... Ts>
		CONSTEXPR std::vector<TReturn> castgetv_all(Ts&&... names) const noexcept
		{
			std::vector<TReturn> vec;
			const auto& all{ this->getv_all<TFilterTypes...>(std::forward<Ts>(names)...) };
			vec.reserve(all.size());
			for (const auto& it : all)
				vec.emplace_back(static_cast<TReturn>(it));
			vec.shrink_to_fit();
			return vec;
		}
		/**
		 * @brief					Get the specified argument casted to the specified type.
		 * @tparam TReturn			The desired return type.
		 * @tparam TFilterTypes...	Any number of types to limit the returned result types to.  When left empty, all types are considered matching.
		 * @param converter			A conversion function that accepts type vstring and returns type TReturn.
		 * @param names				The name(s) of the target argument(s) (Excluding prefix dashes).
		 * @returns					A vector containing the capture value(s) of all matches.
		 */
		template<typename TReturn, valid_arg... TFilterTypes, var::same_or_convertible<vstring>... Ts>
		CONSTEXPR std::vector<TReturn> castgetv_all(const std::function<TReturn(vstring)>& converter, Ts&&... names) const
		{
			std::vector<TReturn> vec;
			const auto& all{ this->getv_all<TFilterTypes...>(std::forward<Ts>(names)...) };
			vec.reserve(all.size());
			for (const auto& it : all)
				vec.emplace_back(converter(it));
			vec.shrink_to_fit();
			return vec;
		}
	#pragma endregion castgetv

	#pragma region check
		/**
		 * @brief			Checks if the specified argument was included or not.
		 * @param name		The name of the argument to check for (Excluding prefix dashes).
		 * @returns			True when the argument was included; otherwise false.
		 */
		template<valid_arg... TFilterTypes>
		CONSTEXPR bool check(vstring const& name) const noexcept
		{
			return this->find<TFilterTypes...>(name) != this->end();
		}
		/**
		 * @brief			Checks if the specified argument was included or not.
		 * @param names		The name of the argument to check for (Excluding prefix dashes).
		 * @returns			True when the argument was included; otherwise false.
		 */
		template<valid_arg... TFilterTypes, var::same_or_convertible<vstring>... Ts>
		CONSTEXPR bool check_any(Ts&&... names) const noexcept
		{
			return this->find_any<TFilterTypes...>(std::forward<Ts>(names)...) != this->end();
		}
		/**
		 * @brief			Checks if the specified argument was included or not.
		 * @param names		The name of the argument to check for (Excluding prefix dashes).
		 * @returns			True when the argument was included; otherwise false.
		 */
		template<valid_arg... TFilterTypes, var::same_or_convertible<vstring>... Ts>
		CONSTEXPR bool check_all(Ts&&... names) const noexcept
		{
			std::vector<vstring> nameVec{ std::forward<Ts>(names)... };
			return std::all_of(nameVec.begin(), nameVec.end(), [this](auto&& name) { return check(std::forward<decltype(name)>(name)); });
		}
		/**
		 * @brief		Checks if the specified Option was included.
		 * @param name	The name to check for (Excluding prefix dashes).
		 * @returns		true when the Option was included; otherwise false.
		 */
		CONSTEXPR bool checkopt(vstring const& name) const noexcept { return this->check<Option>(name); }
		/**
		 * @brief		Checks if the specified Flag was included.
		 * @param name	The name to check for (Excluding prefix dashes).
		 * @returns		true when the Flag was included; otherwise false.
		 */
		CONSTEXPR bool checkflag(vstring const& name) const noexcept { return this->check<Flag>(name); }
		/**
		 * @brief		Checks if the specified Parameter was included.
		 * @param name	The name to check for (Excluding prefix dashes).
		 * @returns		true when the Parameter was included; otherwise false.
		 */
		CONSTEXPR bool checkparam(vstring const& name) const noexcept { return this->check<Parameter>(name); }
		/**
		 * @brief		Checks if the specified capturing option (Flag or Option) was included.
		 * @param name	The name to check for (Excluding prefix dashes).
		 * @returns		true when the specified name was included; otherwise false.
		 */
		CONSTEXPR bool checkcap(vstring const& name) const noexcept { return this->check<Flag, Option>(name); }
	#pragma endregion check

	#pragma region checkv
		/**
		 * @brief			Checks if the specified argument was included AND captured an argument.
		 * @param name		The name of the argument to check for (Excluding prefix dashes).
		 * @returns			True when the argument was included; otherwise false.
		 */
		template<valid_arg... TFilterTypes>
		CONSTEXPR bool checkv(std::string const& value, vstring const& name) const noexcept
		{
			if (const auto& v{ this->getv<TFilterTypes...>(name) }; v.has_value())
				return v.value() == value;
			return false;
		}
		/**
		 * @brief			Checks if any of the specified arguments were included AND captured an argument.
		 * @param names		The name of the argument to check for (Excluding prefix dashes).
		 * @returns			True when the argument was included; otherwise false.
		 */
		template<valid_arg... TFilterTypes, var::same_or_convertible<vstring>... Ts>
		CONSTEXPR bool checkv_any(std::string const& value, Ts&&... names) const noexcept
		{
			if (const auto& v{ this->getv_any<TFilterTypes...>(std::forward<Ts>(names)...) }; v.has_value())
				return v.value() == value;
			return false;
		}
		/**
		 * @brief			Checks if all of the specified arguments were included AND captured an argument.
		 * @param names		The name of the argument to check for (Excluding prefix dashes).
		 * @returns			True when the argument was included; otherwise false.
		 */
		template<valid_arg... TFilterTypes, var::same_or_convertible<vstring>... Ts>
		CONSTEXPR bool checkv_all(std::string const& value, Ts&&... names) const noexcept
		{
			for (const auto& it : this->get_all<TFilterTypes...>(std::forward<Ts>(names)...))
				if (!it.has_value() || it.value() != value)
					return false;
			return true;
		}
	#pragma endregion checkv

	#pragma region get_duplicates
		/**
		 * @brief
		 * @tparam TExecutionPolicy
		 * @param equality_comparer
		 * @returns						A subcontainer that contains all of the duplicate entries as determined by the given predicate.
		 */
		template<std::predicate<variantarg, variantarg> TCompare, typename TExecutionPolicy = std::execution::parallel_unsequenced_policy, std::derived_from<std::allocator<variantarg>> TAlloc = std::allocator<variantarg>>
		CONSTEXPR arg_container get_duplicates(const TCompare& equality_comparer) const
		{
			const std::set<variantarg> distinct{ this->begin(), this->end() };
			std::vector<variantarg> duplicates{};
			duplicates.reserve(this->size());
			std::set_difference<TExecutionPolicy>(TExecutionPolicy{}, this->begin(), this->end(), distinct.begin(), distinct.end(), duplicates.begin(), equality_comparer);
			return arg_container{ duplicates.begin(), duplicates.end() };
		}
	#pragma endregion get_duplicates
	};

	/// @brief	unsigned char type
	using uchar = unsigned char;

#pragma region Enums
	/**
	 * @enum	CaptureStyle
	 * @brief	Defines various argument capturing styles
	 */
	$make_typed_bitfield(CaptureStyle, uchar,
						 /// @brief	No captures are allowed under any circumstances. If a capture is appended using an equals ('=') sign, the capture string is inserted into the argument list as a parameter as if the equals sign was replaced with a space on the commandline.
						 Disabled = 0,
						 /// @brief	Captures are only allowed when appended to the argument using an equals ('=') sign. Since CaptureStyle is a bitfield, this may be combined with CaptureStyle::Optional or CaptureStyle::Required to force them to only accept input via appending with an equals sign ('=').
						 EqualsOnly = 1,
						 /// @brief	Captures whenever possible, but does not require capture input. Arguments of type Flag & Option are never captured by other flags or options.
						 Optional = 2,
						 /// @brief	Requires capture input. If a capture is not provided, an exception is thrown by the parser that includes the name of the argument, and a brief message informing the user to provide a capture argument.
						 Required = 4,
						 );

	/**
	 * @enum	ConflictStyle
	 * @brief	Determines the circumstances in which two arguments from the same template group will conflict with each other.
	 */
	enum class ConflictStyle : uchar {
		/// @brief	Arguments do not conflict.
		None = 0,
		/// @brief	Both arguments can be present at the same time, but **only one** is allowed to capture, otherwise a conflict exception is thrown.
		CapturesConflict = 1,
		/// @brief	Only one of the arguments can be present at the same time.
		Conflict = 2,
	};
#pragma endregion Enums

	/**
	 * @brief
	 * @tparam T
	 */
	template<var::any_same<_internal::flag_t, _internal::option_t> T>
	struct basic_arg_template : public base_arg {
		vstring name;
		std::optional<CaptureStyle> captureStyle;

		CONSTEXPR basic_arg_template(vstring const& name, const std::optional<CaptureStyle>& captureStyle = std::nullopt) : name{ name }, captureStyle{ captureStyle } {}
	};

	/// @brief		An argument template for `Option`s.
	using TemplateOption = basic_arg_template<_internal::option_t>;
	/// @brief		An argument template for `Flag`s.
	using TemplateFlag = basic_arg_template<_internal::flag_t>;

	/**
	 * @struct	variant_template
	 * @brief	An abstraction wrapper around `basic_arg_template` that accepts types `TemplateOption` & `TemplateFlag`
	 */
	class variant_template : public std::variant<TemplateFlag, TemplateOption> {
	protected:
		using base_t = std::variant<TemplateFlag, TemplateOption>;

	public:
		template<typename... Ts> requires std::constructible_from<base_t, Ts...> CONSTEXPR variant_template(Ts&&... args) : base_t(std::forward<Ts>(args)...) {}

		/**
		 * @brief
		 * @tparam TVisitor
		 * @param visitor		A function or lambda to use as the visitor when calling ``
		 * @returns				The value returned by the visitor function.
		 */
		template<typename TVisitor>
		CONSTEXPR auto visit(const TVisitor& visitor) const noexcept
		{
			return std::visit(visitor, static_cast<base_t>(*this));
		}

		CONSTEXPR std::optional<CaptureStyle> getCaptureStyle() const;
		WINCONSTEXPR vstring name() const;

		template<typename T> CONSTEXPR bool is_type() const noexcept { return std::holds_alternative<T>(*this); }
		template<typename... Ts> CONSTEXPR bool is_any_type() const noexcept { return var::variadic_or(std::holds_alternative<Ts>(*this)...); }
		CONSTEXPR bool is_flag() const noexcept { return is_type<TemplateFlag>(); }
		CONSTEXPR bool is_option() const noexcept { return is_type<TemplateOption>(); }
	};
	inline CONSTEXPR std::optional<CaptureStyle> variant_template::getCaptureStyle() const
	{
		return this->visit([](auto&& value) -> std::optional<CaptureStyle> {
			using T = std::decay_t<decltype(value)>;

		if constexpr (std::same_as<T, TemplateFlag>)
			return value.captureStyle;
		else if constexpr (std::same_as<T, TemplateOption>)
			return value.captureStyle;
						   });
	}
	inline WINCONSTEXPR vstring variant_template::name() const
	{
		return this->visit([](auto&& value) -> std::string {
			using T = std::decay_t<decltype(value)>;

		if constexpr (std::same_as<T, TemplateFlag>)
			return value.name;
		else if constexpr (std::same_as<T, TemplateOption>)
			return value.name;
		return std::string{};
						   });
	}
	inline std::ostream& operator<<(std::ostream& os, const variant_template& vt)
	{
		if (vt.is_option())
			os << "--";
		else if (vt.is_flag())
			os << '-';
		return os << vt.name();
	}

	namespace _internal {
		/// @brief	Returns the given variant_template.
		inline WINCONSTEXPR variant_template resolve_template(variant_template const& vt)
		{
			return vt;
		}
		/// @brief	Constructs a new variant_template instance from the given vstring. Single-character vstrings are parsed as flags, while multi-character vstrings are parsed as options.
		inline WINCONSTEXPR variant_template resolve_template(vstring const& vs)
		{
			if (vs.is_single_char())
				return variant_template{ TemplateFlag{ vs } };
			else return variant_template{ TemplateOption{ vs } };
		}
	}

	/**
	 * @struct	arg_conflict
	 * @brief	Simple container object for a single argument 'conflict'; a much simpler form of argument definition that includes a name & optional `ConflictStyle`.
	 */
	struct arg_conflict {
		/// @brief	The name of the conflicting argument.
		vstring name;
		/// @brief	Optional conflict style override for this conflict only.
		std::optional<ConflictStyle> style{ std::nullopt };

		/**
		 * @brief					Creates a new arg_conflict instance.
		 * @param name				The name of the conflicting argument.
		 * @param conflictStyle		An optional conflict style that overrides the group's default for this conflict only.
		 */
		WINCONSTEXPR arg_conflict(vstring const& name, std::optional<ConflictStyle> const& conflictStyle) : name{ name }, style{ conflictStyle } {}
		/**
		 * @brief					Creates a new arg_conflict instance.
		 * @param name				The name of the conflicting argument.
		 * @param conflictStyle		A conflict style that overrides the group's default for this conflict only.
		 */
		WINCONSTEXPR arg_conflict(vstring const& name, ConflictStyle const& conflictStyle) : name{ name }, style{ conflictStyle } {}
		/**
		 * @brief					Creates a new arg_conflict instance.
		 * @param name				The name of the conflicting argument.
		 */
		WINCONSTEXPR arg_conflict(vstring const& name) : name{ name } {}

		WINCONSTEXPR arg_conflict(arg_conflict const&) = default;
		WINCONSTEXPR arg_conflict(arg_conflict&&) noexcept = default;
		~arg_conflict() = default;
		WINCONSTEXPR arg_conflict& operator=(arg_conflict const&) = default;
		WINCONSTEXPR arg_conflict& operator=(arg_conflict&&) noexcept = default;

		WINCONSTEXPR operator vstring() const noexcept { return name; }
	};

	namespace _internal {
		static struct {
		private:
			mutable size_t rolling{ 0 };
		public:
			/// @returns	Next unassigned ID number.
			CONSTEXPR size_t next() const noexcept { return rolling++; }
			/// @brief		Resets the sequencer to 0.
			CONSTEXPR void reset() const noexcept { rolling = 0; }
		} arg_id_sequencer;
	}

	/**
	 * @struct	variant_template_group
	 * @brief	Container object for a *group* of argument templates that all perform a similar function.
	 *\n		A template group may also **conflict** with members of another template group; this is handled differently depending on the winning `ConflictStyle`
	 */
	struct variant_template_group {
		using conflict_list_t = std::vector<arg_conflict>;

		/// @brief	This argument group's identifier number.
		size_t _id;
		/// @brief	Argument templates defined by this group.
		std::vector<variant_template> templates;
		/// @brief	Default capture style for argument templates that don't override it.
		CaptureStyle _defaultCaptureStyle;
		/// @brief	Default conflict style for conflict definitions that don't override it.
		ConflictStyle _defaultConflictStyle;
		/// @brief	An optional *(inclusive)* maximum argument count.
		std::optional<size_t> _max;
		/// @brief	An *(inclusive)* minimum argument count.
		size_t _min;
		/// @brief	Conflicting argument definitions for this template group.
		conflict_list_t conflicts;

		/**
		 * @brief					Creates a new variant_template_group instance with the specified ID.
		 * @param id				An explicit ID number to use for the template. Note that the default constructor automatically increments an internal unsigned integer starting at 0 for implicit IDs, and that these shouldn't overlap.
		 * @param defaultCaptureStyle		Determines how arguments from this template group can capture input.
		 * @param defaultConflictStyle		Determines how multiple arguments with the same ID are handled.
		 * @param templates			Any number of `variant_template`s to include in the group
		 */
		WINCONSTEXPR variant_template_group(const size_t& id, const CaptureStyle& defaultCaptureStyle, const ConflictStyle& defaultConflictStyle, const std::vector<variant_template>& templates, const size_t& min = 0, const std::optional<size_t> max = std::nullopt) :
			_id{ id },
			templates{ templates },
			_defaultCaptureStyle{ defaultCaptureStyle },
			_defaultConflictStyle{ defaultConflictStyle },
			_max{ max },
			_min{ min }
		{}
		/**
		 * @brief					Creates a new variant_template_group instance with the specified ID.
		 * @param id				An explicit ID number to use for the template. Note that the default constructor automatically increments an internal unsigned integer starting at 0 for implicit IDs, and that these shouldn't overlap.
		 * @param defaultCaptureStyle		Determines how arguments from this template group can capture input.
		 * @param templates			Any number of `variant_template`s to include in the group
		 */
		WINCONSTEXPR variant_template_group(const size_t& id, const CaptureStyle& defaultCaptureStyle, const std::vector<variant_template>& templates, const size_t& min = 0, const std::optional<size_t> max = std::nullopt) :
			_id{ id },
			templates{ templates },
			_defaultCaptureStyle{ defaultCaptureStyle },
			_defaultConflictStyle{ ConflictStyle::Conflict },
			_max{ max },
			_min{ min }
		{}
		/**
		 * @brief					Creates a new variant_template_group instance with the specified ID.
		 * @param id				An explicit ID number to use for the template. Note that the default constructor automatically increments an internal unsigned integer starting at 0 for implicit IDs, and that these shouldn't overlap.
		 * @param defaultConflictStyle		Determines how multiple arguments with the same ID are handled.
		 * @param templates			Any number of `variant_template`s to include in the group
		 */
		WINCONSTEXPR variant_template_group(const size_t& id, const ConflictStyle& defaultConflictStyle, const std::vector<variant_template>& templates, const size_t& min = 0, const std::optional<size_t> max = std::nullopt) :
			_id{ id },
			templates{ templates },
			_defaultCaptureStyle{ CaptureStyle::Optional },
			_defaultConflictStyle{ defaultConflictStyle },
			_max{ max },
			_min{ min }
		{}
		/**
		 * @brief					Creates a new variant_template_group instance with the specified ID.
		 * @param id				An explicit ID number to use for the template. Note that the default constructor automatically increments an internal unsigned integer starting at 0 for implicit IDs, and that these shouldn't overlap.
		 * @param templates			Any number of `variant_template`s to include in the group
		 */
		WINCONSTEXPR variant_template_group(const size_t& id, const std::vector<variant_template>& templates, const size_t& min = 0, const std::optional<size_t> max = std::nullopt) :
			_id{ id },
			templates{ templates },
			_defaultCaptureStyle{ CaptureStyle::Optional },
			_defaultConflictStyle{ ConflictStyle::Conflict },
			_max{ max },
			_min{ min }
		{}

		/**
		 * @brief					Creates a new variant_template_group instance with an automatically-assigned ID.
		 * @param defaultCaptureStyle		Determines how arguments from this template group can capture input.
		 * @param defaultConflictStyle		Determines how multiple arguments with the same ID are handled.
		 * @param templates			Any number of `variant_template`s to include in the group
		 */
		WINCONSTEXPR variant_template_group(const CaptureStyle& defaultCaptureStyle, const ConflictStyle& defaultConflictStyle, const std::vector<variant_template>& templates, const size_t& min = 0, const std::optional<size_t> max = std::nullopt) :
			_id{ _internal::arg_id_sequencer.next() },
			templates{ templates },
			_defaultCaptureStyle{ defaultCaptureStyle },
			_defaultConflictStyle{ defaultConflictStyle },
			_max{ max },
			_min{ min }
		{}
		/**
		 * @brief					Creates a new variant_template_group instance with an automatically-assigned ID.
		 * @param defaultCaptureStyle		Determines how arguments from this template group can capture input.
		 * @param templates			Any number of `variant_template`s to include in the group
		 */
		WINCONSTEXPR variant_template_group(const CaptureStyle& defaultCaptureStyle, const std::vector<variant_template>& templates, const size_t& min = 0, const std::optional<size_t> max = std::nullopt) :
			_id{ _internal::arg_id_sequencer.next() },
			templates{ templates },
			_defaultCaptureStyle{ defaultCaptureStyle },
			_defaultConflictStyle{ ConflictStyle::Conflict },
			_max{ max },
			_min{ min }
		{}
		/**
		 * @brief					Creates a new variant_template_group instance with an automatically-assigned ID.
		 * @param defaultConflictStyle		Determines how multiple arguments with the same ID are handled.
		 * @param templates			Any number of `variant_template`s to include in the group
		 */
		WINCONSTEXPR variant_template_group(const ConflictStyle& defaultConflictStyle, const std::vector<variant_template>& templates, const size_t& min = 0, const std::optional<size_t> max = std::nullopt) :
			_id{ _internal::arg_id_sequencer.next() },
			templates{ templates },
			_defaultCaptureStyle{ CaptureStyle::Optional },
			_defaultConflictStyle{ defaultConflictStyle },
			_max{ max },
			_min{ min }
		{}
		/**
		 * @brief					Creates a new variant_template_group instance with an automatically-assigned ID.
		 * @param templates			Any number of `variant_template`s to include in the group
		 */
		WINCONSTEXPR variant_template_group(const std::vector<variant_template>& templates, const size_t& min = 0, const std::optional<size_t> max = std::nullopt) :
			_id{ _internal::arg_id_sequencer.next() },
			templates{ templates },
			_defaultCaptureStyle{ CaptureStyle::Optional },
			_defaultConflictStyle{ ConflictStyle::Conflict },
			_max{ max },
			_min{ min }
		{}


		WINCONSTEXPR CaptureStyle get_capture_style_of(vstring const& name) const noexcept
		{
			if (const auto& it{ std::find_if(this->templates.begin(), this->templates.end(), [&name](auto&& vTemplate) { return vTemplate.name() == name; }) }; it != this->templates.end())
				return it->getCaptureStyle().value_or(this->_defaultCaptureStyle);
			else return this->_defaultCaptureStyle;
		}
		WINCONSTEXPR ConflictStyle get_conflict_style_of(vstring const& name) const noexcept
		{
			if (const auto& it{ std::find_if(this->conflicts.begin(), this->conflicts.end(), [&name](auto&& conflict) { return conflict.name == name; }) }; it != this->conflicts.end())
				return it->style.value_or(this->_defaultConflictStyle);
			else return this->_defaultConflictStyle;
		}

		/**
		 * @brief		Sets the default capture style of this group.
		 * @param cs	A default capture style for argument templates that don't override it.
		 * @returns		The reference of this instance.
		 */
		CONSTEXPR variant_template_group& SetCaptureStyle(const CaptureStyle& cs) noexcept
		{
			_defaultCaptureStyle = cs;
			return *this;
		}
		/**
		 * @brief		Sets the default conflict style of this group.
		 * @param cs	A default conflict style for conflict definitions that don't override it.
		 * @returns		The reference of this instance.
		 */
		CONSTEXPR variant_template_group& SetConflictStyle(const ConflictStyle& cs) noexcept
		{
			_defaultConflictStyle = cs;
			return *this;
		}
		/**
		 * @brief		Sets the minimum number of arguments from this group that must be specified.
		 * @param min	Minimum argument count.
		 *\n			If the user doesn't specify (any) argument from this group at least this many times, an exception is thrown by the parser.
		 * @returns		The reference of this instance.
		 */
		CONSTEXPR variant_template_group& SetMin(const size_t min) noexcept
		{
			this->_min = min;
			return *this;
		}
		/**
		 * @brief		Sets the maximum number of arguments from this group that can be specified.
		 * @param min	Optional maximum argument count.
		 *\n			If the user specifies (any) argument from this group more than this many times, an exception is thrown by the parser.
		 * @returns		The reference of this instance.
		 */
		CONSTEXPR variant_template_group& SetMax(const std::optional<size_t>& max) noexcept
		{
			this->_max = max;
			return *this;
		}
		/**
		 * @brief		Specifies argument names that conflict with all arguments from this group.
		 * @param names	Any number of argument names or arg_conflict definitions.
		 * @returns		The reference of this instance.
		 */
		template<var::any_same_or_convertible<vstring, arg_conflict>... Ts>
		CONSTEXPR variant_template_group& SetConflicts(Ts&&... names)
		{
			const conflict_list_t tmp{ arg_conflict{ std::forward<Ts>(names) }... };
			conflicts.reserve(conflicts.size() + tmp.size());
			conflicts.insert(conflicts.end(), tmp.begin(), tmp.end());
			return *this;
		}

		/// @brief	Ostream insertion operator. Prints all argument templates with prefixes in the format '<FIRST> | <SECOND> | <THIRD>...'
		friend std::ostream& operator<<(std::ostream& os, const variant_template_group& vtg)
		{
			bool fst{ true };
			for (const auto& vt : vtg.templates) {
				if (fst) fst = false;
				else os << " | ";
				os << vt;
			}
			return os;
		}
	};

#pragma region make_template
	/**
	 * @brief					Creates a new variant_template_group instance.
	 * @param captureStyle		Determines how arguments from this template group can capture input.
	 * @param conflictStyle		Determines how multiple arguments with the same ID are handled.
	 * @param templates			Any number of `variant_template`s to include in the group.
	 * @returns					A new variant_template_group instance with the specified parameters.
	 */
	template<var::any_same_or_convertible<variant_template, vstring>... Ts>
	inline constexpr variant_template_group make_template(const CaptureStyle captureStyle, const ConflictStyle conflictStyle, Ts&&... args)
	{
		return variant_template_group{
			captureStyle,
			conflictStyle,
			std::vector<variant_template>{ _internal::resolve_template(std::forward<Ts>(args))... }
		};
	}
	/**
	 * @brief					Creates a new variant_template_group instance.
	 * @param conflictStyle		Determines how multiple arguments with the same ID are handled.
	 * @param captureStyle		Determines how arguments from this template group can capture input.
	 * @param templates			Any number of `variant_template`s to include in the group.
	 * @returns					A new variant_template_group instance with the specified parameters.
	 */
	template<var::any_same_or_convertible<variant_template, vstring>... Ts>
	inline constexpr variant_template_group make_template(const ConflictStyle conflictStyle, const CaptureStyle captureStyle, Ts&&... args)
	{
		return variant_template_group{
			captureStyle,
			conflictStyle,
			std::vector<variant_template>{ _internal::resolve_template(std::forward<Ts>(args))... }
		};
	}
	/**
	 * @brief					Creates a new variant_template_group instance.
	 * @param captureStyle		Determines how arguments from this template group can capture input.
	 * @param templates			Any number of `variant_template`s to include in the group.
	 * @returns					A new variant_template_group instance with the specified parameters.
	 */
	template<var::any_same_or_convertible<variant_template, vstring>... Ts>
	inline constexpr variant_template_group make_template(const CaptureStyle captureStyle, Ts&&... args)
	{
		return variant_template_group{
			captureStyle,
			std::vector<variant_template>{ _internal::resolve_template(std::forward<Ts>(args))... }
		};
	}
	/**
	 * @brief					Creates a new variant_template_group instance.
	 * @param conflictStyle		Determines how multiple arguments with the same ID are handled.
	 * @param templates			Any number of `variant_template`s to include in the group.
	 * @returns					A new variant_template_group instance with the specified parameters.
	 */
	template<var::any_same_or_convertible<variant_template, vstring>... Ts>
	inline constexpr variant_template_group make_template(const ConflictStyle conflictStyle, Ts&&... args)
	{
		return variant_template_group{
			conflictStyle,
			std::vector<variant_template>{ _internal::resolve_template(std::forward<Ts>(args))... }
		};
	}
	/**
	 * @brief					Creates a new variant_template_group instance.
	 * @param templates			Any number of `variant_template`s to include in the group
	 * @returns					A new variant_template_group instance with the specified parameters.
	 */
	template<var::any_same_or_convertible<variant_template, vstring>... Ts>
	inline constexpr variant_template_group make_template(Ts&&... args)
	{
		return variant_template_group{
			std::vector<variant_template>{ _internal::resolve_template(std::forward<Ts>(args))... }
		};
	}
	/**
	 * @brief					Creates a new variant_template_group instance.
	 * @param g					A pre-constructed variant_template_group instance.
	 * @returns					g
	 */
	inline WINCONSTEXPR variant_template_group make_template(variant_template_group const& g) { return g; }
#pragma endregion make_template

	/**
	 * @concept		valid_capture
	 * @brief		Constraint that only allows types that constitute a valid argument capture definition, such as `variant_template`, `variant_template_group`, or `vstring`.
	 */
	template<typename T> concept valid_capture = var::any_same_or_convertible<T, variant_template, variant_template_group, vstring>;

	/**
	 * @struct	capture_list
	 * @brief	A small wrapper object that inherits from a std::vector.
	 *			TODO: Implement this properly instead of inheriting from std::vector.
	 */
	struct capture_list : std::vector<variant_template_group> {
		using base_t = std::vector<variant_template_group>;
		using iterator_t = std::vector<variant_template_group>::iterator;
		using const_iterator_t = std::vector<variant_template_group>::const_iterator;

	private:
		template<size_t IDX, valid_capture... Ts>
		static base_t& build(base_t& vec, std::tuple<Ts...>&& tpl)
		{
			vec.reserve(vec.size() + 1ull);
			vec.emplace_back(std::move(make_template(std::move(std::get<IDX>(tpl)))));

			if constexpr (IDX + 1ull < sizeof...(Ts))
				return build<IDX + 1ull>(vec, std::forward<std::tuple<Ts...>>(tpl));
			else return vec;
		}
		template<valid_capture... Ts>
		static base_t build(std::tuple<Ts...>&& tpl)
		{
			base_t vec;
			vec = build<0ull>(vec, std::forward<std::tuple<Ts...>>(tpl));
			return vec;
		}

	public:
		/**
		 * @brief		Creates a new capture_list instance with no entries.
		 */
		STRCONSTEXPR capture_list() : base_t() {}
		/**
		 * @brief			Creates a new capture_list instance with the given entries.
		 * @param captures	Any number of valid argument names as strings or chars.
		 */
		template<valid_capture... Ts> requires var::more_than<0, Ts...>
		STRCONSTEXPR capture_list(Ts&&... captures) : base_t(build(std::make_tuple(std::forward<Ts>(captures)...))) {}

		/**
		 * @brief		Gets an iterator pointing to the capture group that contains an argument with the given name.
		 * @param name	Input argument name.
		 * @returns		An iterator pointing to the first capture group that contains the given argument name if one was found; otherwise an iterator pointing to the end position of the capture list.
		 */
		iterator_t get_group_of(vstring const& name)
		{
			if (const auto& it{ std::find_if(this->begin(), this->end(), [&name](auto&& v) { return std::any_of(v.templates.begin(), v.templates.end(), [&name](auto&& vtemplate) { return vtemplate.name() == name; }); }) }; it != this->end())
				return it;
			return this->end();
		}
		/**
		 * @brief		Gets an iterator pointing to the capture group that contains an argument with the given name.
		 * @param name	Input argument name.
		 * @returns		An iterator pointing to the first capture group that contains the given argument name if one was found; otherwise an iterator pointing to the end position of the capture list.
		 */
		const_iterator_t get_group_of(vstring const& name) const
		{
			if (const auto& it{ std::find_if(this->begin(), this->end(), [&name](auto&& v) { return std::any_of(v.templates.begin(), v.templates.end(), [&name](auto&& vtemplate) { return vtemplate.name() == name; }); }) }; it != this->end())
				return it;
			return this->end();
		}

		/**
		 * @brief		Gets an iterator pointing to the capture group with the given id.
		 * @param id	Input capture group ID.
		 * @returns		An iterator pointing to the capture group with the given id if one was found; otherwise an iterator pointing to the end position of the capture list.
		 */
		iterator_t get_group_of(size_t const& id)
		{
			if (const auto& it{ std::find_if(this->begin(), this->end(), [&id](auto&& v) { return v._id == id; }) }; it != this->end())
				return it;
			return this->end();
		}
		/**
		 * @brief		Gets an iterator pointing to the capture group with the given id.
		 * @param id	Input capture group ID.
		 * @returns		An iterator pointing to the capture group with the given id if one was found; otherwise an iterator pointing to the end position of the capture list.
		 */
		const_iterator_t get_group_of(size_t const& id) const
		{
			if (const auto& it{ std::find_if(this->begin(), this->end(), [&id](auto&& v) { return v._id == id; }) }; it != this->end())
				return it;
			return this->end();
		}

		/**
		 * @brief		Gets the CaptureStyle associated with a given argument.
		 * @param name	Input argument name.
		 * @returns		The CaptureStyle associated with the given argument name.
		 */
		CaptureStyle get_capture_style_of(vstring const& name) const
		{
			if (const auto& group{ get_group_of(name) }; group != this->end())
				return group->get_capture_style_of(name);
			return CaptureStyle::Disabled;
		}

		/**
		 * @brief		Checks if the given Option/Flag name was specified in the capture list.
		 * @param name	The name of an Option/Flag to search for.
		 * @returns		true when the given name is present somewhere in the capture list.
		 */
		bool is_present(vstring const& name) const
		{
			return std::any_of(this->begin(), this->end(), [&name](variant_template_group const& vtGroup) {
				return std::any_of(vtGroup.templates.begin(), vtGroup.templates.end(), [&name](variant_template const& vt) {
								   return vt.name() == name;
							   });
							   });
		}
	};

	/**
	 * @struct	ArgParsingRules
	 * @brief	Allows configuration of the rules used when parsing arguments.
	 *\n		It is highly recommended that you do not call this object's methods as they are intended for use by only the opt3 argument parser.
	 */
	struct ArgParsingRules {
		/**
		 * @brief	List of characters that are considered valid argument prefixes.
		 *\n		Default: { '-' }
		 */
		std::vector<char> delimiters{ '-' };
		/**
		 * @brief	Determines the default capture style used for arguments present in the capture list that do not specify a capture style themselves.
		 *\n		Default: CaptureStyle::Optional
		 */
		CaptureStyle defaultCaptureStyle{ CaptureStyle::Optional };
		/**
		 * @brief	Determines whether options/flags that aren't present in the capture_list are allowed or not. When true, unexpected arguments are allowed; otherwise the action taken depends on the value of convertUnexpectedCaptureArgsToParameters.
		 *\n		Default: true
		 */
		bool allowUnexpectedCaptureArgs{ true };
		/**
		 * @brief	Determines whether unexpected arguments are converted to Parameters or cause an exception to be thrown. When true, unexpected arguments are converted to parameters; otherwise, unexpected arguments trigger an exception.
		 *\n		__This is ignored when allowUnexpectedCaptureArgs is true.__
		 *\n		Default: false
		 */
		bool convertUnexpectedCaptureArgsToParameters{ false };
		/**
		 * @brief	Whether to include the end of args specifier (2x delimiter chars ONLY; ex: '--') as an option in the arguments list.
		 */
		bool includeEndOfArgsSpecifierInOutput{ false };

		/**
		 * @brief	Default Constructor.
		 */
		WINCONSTEXPR ArgParsingRules() {}

	#pragma region Methods
		/**
		 * @brief		Gets all of the characters from the delimiters vector as a string.
		 * @returns		String containing each character in the delimiters vector.
		 */
		[[nodiscard]] WINCONSTEXPR std::string getDelimitersAsString() const
		{
			std::string s;
			s.reserve(delimiters.size());
			for (const auto& c : delimiters)
				s += c;
			return s;
		}
		/**
		 * @brief		Check if the given character is a valid delimiter.
		 * @param c		Input Character
		 * @returns		true when the given character is a valid delimiter; otherwise false.
		 */
		[[nodiscard]] WINCONSTEXPR bool isDelimiter(const char& c) const
		{
			return std::any_of(delimiters.begin(), delimiters.end(), [&c](auto&& delim) { return delim == c; });
		}
		/**
		 * @brief		Check if the given iterator CAN capture the next argument by checking
		 *\n			if the next argument is not prefixed with a '-' or is prefixed with '-' but is also a number.
		 *\n			Does NOT check if the given iterator is present on the capturelist!
		 * @param here	The current iterator position.
		 * @param end	The position of the end of the iterable range.
		 * @returns		bool
		 */
		[[nodiscard]] CONSTEXPR bool canCaptureNext(std::vector<std::string>::const_iterator& here, const std::vector<std::string>::const_iterator& end) const
		{
			return (here != end - 1ll) // incrementing iterator won't go out-of-bounds
				&& !isDelimiter((here + 1ll)->front()); // AND next argument doesn't start with a delimiter
		}
	#pragma endregion Methods
	};

	namespace _internal {
		/// @brief	Checks if captureStyle is set to CaptureStyle::Disabled
		inline CONSTEXPR bool CaptureIsDisabled(const CaptureStyle& captureStyle)
		{
			return static_cast<uchar>(captureStyle) == static_cast<uchar>(CaptureStyle::Disabled);
		}
		/// @brief	Checks if captureStyle contains CaptureStyle::EqualsOnly
		inline CONSTEXPR bool CaptureIsEqualsOnly(const CaptureStyle& captureStyle)
		{
			return (static_cast<uchar>(captureStyle) & static_cast<uchar>(CaptureStyle::EqualsOnly)) != 0;
		}
		/// @brief	Checks if captureStyle contains CaptureStyle::Disabled and/or CaptureStyle::EqualsOnly
		inline CONSTEXPR bool CaptureIsDisabledOrEqualsOnly(const CaptureStyle& captureStyle)
		{
			return CaptureIsDisabled(captureStyle) || CaptureIsEqualsOnly(captureStyle);
		}
		/// @brief	Checks if captureStyle contains CaptureStyle::Required
		inline CONSTEXPR bool CaptureIsRequired(const CaptureStyle& captureStyle)
		{
			return (static_cast<uchar>(captureStyle) & static_cast<uchar>(CaptureStyle::Required)) != 0;
		}
	}

	$DefineExcept(invalid_argument_exception,
				  /// @brief	The name of the invalid argument that caused the exception.
				  std::string argument_name;
	invalid_argument_exception(std::string const& argument_name, std::string const& argument_typename) : ex::except(str::stringify("Argument '", argument_name, "' is not a recognized ", argument_typename, '.')), argument_name{ argument_name } {}
	);

	/**
	 * @brief				Parse commandline arguments into an ArgContainer instance.
	 * @details				### Argument Types
	 *						- Parameters are any arguments that do not begin with a dash '-' character that were not captured by another argument type.
	 *						- Options are arguments that begin with 2 dash '-' characters, and can capture additional arguments if the option name appears in the capture list.
	 *						- Flags are arguments that begin with a single dash '-' character, are a single character in length, and can capture additional arguments. Flags can appear alone, or in "chains" where each character is treated as an individual flag. In a flag chain, only the last flag can capture additional arguments.
	 *						### Capture Rules
	 *						- Only options/flags specified in the capture list are allowed to capture additional arguments. Capture list entries should not include a delimiter prefix.
	 *						- Options/Flags cannot be captured under any circumstance. ex: "--opt --opt captured" results in "--opt", & "--opt" + "captured".
	 *						- If a flag in a chain should capture an argument (either with an '=' delimiter or by context), it must appear at the end of the chain.
	 *						- Any captured arguments do not appear in the argument list by themselves, and must be accessed through the argument that captured them.
	 * @param args			Commandline arguments as a vector of strings, in order and including argv[0].
	 * @param captures		A `capture_list` instance specifying which arguments are allowed to capture other arguments as their parameters
	 * @param parsingRules	An `ArgParsingRules` instance that provides the parser with a configuration
	 * @returns				ArgContainer
	 */
	inline arg_container parse(std::vector<std::string>&& args, capture_list captures, const ArgParsingRules& parsingRules)
	{
		using namespace _internal;

		// remove empty arguments, which are possible when passing arguments from automated testing applications
		args.erase(std::remove_if(args.begin(), args.end(), [](auto&& s) { return s.empty(); }), args.end());

		arg_container cont{};
		cont.reserve(args.size());

		bool endOfArgsReached{ false };

		for (std::vector<std::string>::const_iterator it{ args.begin() }; it != args.end(); ++it) {
			std::string arg{ *it };
			if (!endOfArgsReached) {
				// check for capture args
				if (arg.size() > 1 && parsingRules.isDelimiter(arg.at(0))) {
					// is flag or option
					arg = arg.substr(1);

					if (parsingRules.isDelimiter(arg.at(0))) {
						// is option
						arg = arg.substr(1);

						if (arg.empty()) {
							// is end of args specifier ("--" by default)
							endOfArgsReached = true;
							if (parsingRules.includeEndOfArgsSpecifierInOutput) {
								arg = *it;
								goto JUMP_TO_PARAMETER;
							}
							else continue; //< don't add this to the args list
						}

						if (const auto eqPos{ arg.find('=') }; eqPos != std::string::npos) {// argument contains an equals sign, split string
							auto opt{ arg.substr(0ull, eqPos) }, cap{ arg.substr(eqPos + 1ull) };

							if (!parsingRules.allowUnexpectedCaptureArgs && !captures.is_present(opt)) {
								if (parsingRules.convertUnexpectedCaptureArgsToParameters)
									cont.emplace_back(Parameter{ *it });
								else throw make_custom_exception_explicit<invalid_argument_exception>(*it, "option");
							}
							else if (const auto& captureStyle{ captures.get_capture_style_of(opt) }; !CaptureIsDisabled(captureStyle))
								cont.emplace_back(Option{ std::make_pair(std::move(opt), std::move(cap)) });
							else {
								if (CaptureIsRequired(captureStyle))
									throw make_exception("Expected a capture argument for option '", opt, "'!");
								cont.emplace_back(Option{ std::make_pair(std::move(opt), std::nullopt) });
								if (!cap.empty()) {
									arg = cap;
									goto JUMP_TO_PARAMETER; // skip flag case, add invalid capture as a parameter
								}
							}
						}
						else {
							if (!parsingRules.allowUnexpectedCaptureArgs && !captures.is_present(arg)) {
								if (parsingRules.convertUnexpectedCaptureArgsToParameters)
									cont.emplace_back(Parameter{ *it });
								else throw make_custom_exception_explicit<invalid_argument_exception>(*it, "option");
							}
							else if (const auto& captureStyle{ captures.get_capture_style_of(arg) }; !CaptureIsDisabledOrEqualsOnly(captureStyle) && parsingRules.canCaptureNext(it, args.end())) // argument can capture next arg
								cont.emplace_back(Option{ std::make_pair(arg, *++it) });
							else {
								if (CaptureIsRequired(captureStyle))
									throw make_exception("Expected a capture argument for option '", arg, "'!");
								cont.emplace_back(Option{ std::make_pair(arg, std::nullopt) });
							}
						}
					}
					else {
						// is flag
						std::optional<Flag> capt{ std::nullopt }; // this can contain a flag if there is a capturing flag at the end of a chain
						std::string invCap{}; //< for invalid captures that should be treated as parameters

						if (const auto eqPos{ arg.find('=') }; eqPos != std::string::npos) {
							invCap = arg.substr(eqPos + 1ull); // get string following '=', use invCap in case flag can't capture
							if (const auto flag{ arg.substr(eqPos - 1ull, 1ull) }; !CaptureIsDisabled(captures.get_capture_style_of(flag))) {
								capt = Flag{ std::make_pair(flag.front(), invCap) }; // push the capturing flag to capt, insert into vector once all other flags in this chain are parsed
								arg = arg.substr(0ull, eqPos - 1ull); // remove last flag, '=', and captured string from arg
								invCap.clear(); // flag can capture, clear invCap
							}
							else
								arg = arg.substr(0ull, eqPos); // remove everything from eqPos to arg.end()
						}

						// iterate through characters in arg
						bool convertToParameter{ false };
						std::vector<Flag> flagsBuffer;
						flagsBuffer.reserve(arg.size());
						for (auto fl{ arg.begin() }; fl != arg.end(); ++fl) {
							if (!parsingRules.allowUnexpectedCaptureArgs && !captures.is_present(*fl)) {
								if (parsingRules.convertUnexpectedCaptureArgsToParameters) {
									convertToParameter = true;
									continue;
								}
								else throw make_custom_exception_explicit<invalid_argument_exception>(std::string{ 1ull, *fl }, "flag");
							}

							const auto& captureStyle{ captures.get_capture_style_of(std::string(1ull, *fl)) };
							// If this is the last char, and it can capture
							if (fl == arg.end() - 1ll && !CaptureIsDisabledOrEqualsOnly(captureStyle) && parsingRules.canCaptureNext(it, args.end()))
								flagsBuffer.emplace_back(Flag{ std::make_pair(*fl, *++it) });
							else {// not last char, or can't capture
								if (CaptureIsRequired(captureStyle))
									throw make_exception("Expected a capture argument for flag '", *fl, "'!");
								flagsBuffer.emplace_back(Flag{ std::make_pair(*fl, std::nullopt) });
							}
						}
						if (convertToParameter)
							cont.emplace_back(Parameter{ *it });
						else {
							for (auto&& flag : flagsBuffer) {
								cont.emplace_back(std::move(flag));
							}
						}
						if (capt.has_value()) // flag captures are always at the end, but parsing them first puts them out of chronological order.
							cont.emplace_back(std::move(capt.value()));
						if (invCap.empty())
							continue;
						else arg = invCap; // set argument to invalid capture and fallthrough to add it as a parameter
						goto JUMP_TO_PARAMETER;
					}
				}
				// else is parameter
				else goto JUMP_TO_PARAMETER;
			}
			else {
			JUMP_TO_PARAMETER:
				cont.emplace_back(arg);
			}
		}
		cont.shrink_to_fit();

		// count each argument
		std::map<capture_list::iterator_t, std::pair<size_t, size_t>> counts;
		for (const auto& varg : cont) {
			if (auto groupOf{ captures.get_group_of(varg.name()) }; groupOf != captures.end()) {
				auto& g{ counts[groupOf] };

				++g.first; //< increment arg counter
				if (varg.has_capture())
					++g.second; //< increment capture counter

				// validate max arg count limits
				if (groupOf->_max.has_value() && g.first > groupOf->_max.value()) {
					throw make_exception("Argument '", *groupOf, "' was specified too many times! (Expected a maximum of ", groupOf->_max.value(), ")");
				}
			}
		}
		for (const auto& [group, counters] : counts) {
			// validate arg min limits
			if (const auto& min{ group->_min }; counters.first < min)
				throw make_exception("Expected argument '", *group, "' at least ", min, " time!", (min == 1 ? "" : "s"));

			// validate arg conflicts
			if (counters.first == 0 || group->conflicts.empty())
				continue;

			for (const auto& conflict : group->conflicts) {
				if (const auto& conflictGroup{ captures.get_group_of(conflict.name) }; conflictGroup != captures.end() && counts.contains(conflictGroup)) {

					const auto& conflictStyle{ conflict.style.value_or(group->_defaultConflictStyle) };
					if (conflictStyle == ConflictStyle::None)
						continue;

					const auto& [argCount, capCount] { counts.at(conflictGroup) };

					switch (conflictStyle) {
					case ConflictStyle::CapturesConflict:
						if (counters.second > 0ull && capCount > 0ull)
							throw make_exception("Only one of the following arguments can accept input at the same time: [\n",
												 indent(14), *group, '\n',
												 indent(14), *conflictGroup, '\n',
												 indent(10), ']');
						break;
					case ConflictStyle::Conflict:
						if (counters.first > 0ull && argCount > 0ull)
							throw make_exception("Argument        ( ", *group, " )\n",
												 indent(10), "conflicts with: ( ", *conflictGroup, " )"
							);
						break;
					default:break;
					}
				}
			}
		}

		return cont;
	}
	/**
	 * @brief		Make a std::vector of std::strings from a char** array.
	 * @param sz	Size of the array.
	 * @param arr	Array.
	 * @param off	The index to start at. Any elements that are skipped are ignored.
	 * @returns		std::vector<std::string>
	 */
	inline WINCONSTEXPR static std::vector<std::string> vectorize(const int& sz, char** arr, const int& off = 0)
	{
		std::vector<std::string> vec;
		vec.reserve(sz);
		for (int i{ off }; i < sz; ++i)
			vec.emplace_back(std::string{ std::move(arr[i]) });
		vec.shrink_to_fit();
		return vec;
	}

	/**
	 * @struct	arg_manager2
	 * @brief	Implements the generation 3.5 commandline argument container & parser.
	 *\n		Use this with argc & argv to quickly & easily parse commandline arguments into a container for later use.
	 */
	struct arg_manager : arg_container {
		using base = arg_container;

		/**
		 * @brief					Parsing Constructor.
		 * @param argc				Argument array size from main.
		 * @param argv				Argument array from main.
		 * @param captureArguments	Argument names that should be able to capture. Do not include delimiter prefixes, they will be stripped.\n Argument types must meet the `valid_capture` requirement.
		 */
		template<valid_capture... TCaptures>
		WINCONSTEXPR arg_manager(const int argc, char** argv, TCaptures&&... captureArguments) :
			base(parse(vectorize(argc, argv, 1), capture_list{ make_template(std::forward<TCaptures>(captureArguments))... }, ArgParsingRules{}))
		{}

		/**
		 * @brief					Parsing Constructor.
		 * @param argc				Argument array size from main.
		 * @param argv				Argument array from main.
		 * @param ruleset			Used to define additional constraints & default settings for the argument parser.
		 * @param captureArguments	Argument names that should be able to capture. Do not include delimiter prefixes, they will be stripped.\n Argument types must meet the `valid_capture` requirement.
		 */
		template<valid_capture... TCaptures>
		WINCONSTEXPR arg_manager(const int argc, char** argv, ArgParsingRules const& ruleset, TCaptures&&... captureArguments) :
			base(parse(vectorize(argc, argv, 1), capture_list{ make_template(std::forward<TCaptures>(captureArguments))... }, ruleset))
		{}

		/**
		 * @brief			Value-Move Constructor.
		 * @param args		rvalue reference of an arg_container instance to move to this instance.
		 */
		WINCONSTEXPR explicit arg_manager(arg_container&& args) : base(std::move(args)) {}

		/**
		 * @brief				Parse the given rvalue references into a new arg_manager instance.
		 * @param arguments		A vector of arguments. (See vectorize() for a simple way to convert argc/argv to a vector)
		 * @param captureList	The CaptureList instance that defines which arguments may capture & the ruleset to use when parsing.
		 * @returns				An arg_manager instance containing the parsed arguments.
		 */
		static arg_manager parse(std::vector<std::string>&& arguments, capture_list&& captureList, ArgParsingRules const& ruleset = {})
		{
			return arg_manager{ ::opt3::parse(std::forward<std::vector<std::string>>(arguments), std::forward<capture_list>(captureList), ruleset) };
		}
	};

	using ArgManager = arg_manager;
}
namespace opt3_literals {
	opt3::variant_template operator ""_requireCapture(const char* s, size_t)
	{
		return opt3::variant_template{ opt3::TemplateOption{ s, opt3::CaptureStyle::Required } };
	}
	opt3::variant_template operator ""_requireCapture(char c)
	{
		return opt3::variant_template{ opt3::TemplateFlag{ c, opt3::CaptureStyle::Required } };
	}
	opt3::variant_template operator ""_reqcap(const char* s, size_t)
	{
		return opt3::variant_template{ opt3::TemplateOption{ s, opt3::CaptureStyle::Required } };
	}
	opt3::variant_template operator ""_reqcap(char c)
	{
		return opt3::variant_template{ opt3::TemplateFlag{ c, opt3::CaptureStyle::Required } };
	}

	opt3::variant_template operator ""_optionalCapture(const char* s, size_t)
	{
		return opt3::variant_template{ opt3::TemplateOption{ s, opt3::CaptureStyle::Optional } };
	}
	opt3::variant_template operator ""_optionalCapture(char c)
	{
		return opt3::variant_template{ opt3::TemplateFlag{ c, opt3::CaptureStyle::Optional } };
	}
	opt3::variant_template operator ""_optcap(const char* s, size_t)
	{
		return opt3::variant_template{ opt3::TemplateOption{ s, opt3::CaptureStyle::Optional } };
	}
	opt3::variant_template operator ""_optcap(char c)
	{
		return opt3::variant_template{ opt3::TemplateFlag{ c, opt3::CaptureStyle::Optional } };
	}

	opt3::variant_template operator ""_noCapture(const char* s, size_t)
	{
		return opt3::variant_template{ opt3::TemplateOption{ s, opt3::CaptureStyle::Disabled } };
	}
	opt3::variant_template operator ""_noCapture(char c)
	{
		return opt3::variant_template{ opt3::TemplateFlag{ c, opt3::CaptureStyle::Disabled } };
	}
	opt3::variant_template operator ""_nocap(const char* s, size_t)
	{
		return opt3::variant_template{ opt3::TemplateOption{ s, opt3::CaptureStyle::Disabled } };
	}
	opt3::variant_template operator ""_nocap(char c)
	{
		return opt3::variant_template{ opt3::TemplateFlag{ c, opt3::CaptureStyle::Disabled } };
	}
}

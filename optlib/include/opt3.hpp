#pragma once
/**
 * @file	opt3.hpp
 * @author	radj307
 * @brief	Contains the opt3 namespace & its contents; a fully-featured commandline argument parser, container, & API.
 *\n		This is 307lib's 3rd-generation argument parser, and is intended to replace ParamsAPI2.
 */
#include <sysarch.h>
#include <str.hpp>
#include <vstring.hpp>

#include <concepts>
#include <compare>
#include <string>
#include <variant>
#include <optional>

 /**
  * @namespace	opt3
  * @brief		Contains a commandline argument parser & container object.
  */
namespace opt3 {
	using shared::basic_vstring;
	using shared::vstring;

	/// @brief	The underlying value type of Parameters.
	using parameter_t = std::string;
	/// @brief	The underlying value type of Flags.
	using flag_t = std::pair<char, std::optional<std::string>>;
	/// @brief	The underlying value type of Options.
	using option_t = std::pair<std::string, std::optional<std::string>>;

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
		CONSTEXPR std::string name() const requires (std::same_as<T, parameter_t>) { return _value; }
		/// @brief	Get the name of this argument.
		CONSTEXPR std::string name() const requires (std::same_as<T, flag_t>) { return vstring{ _value.first }; }
		/// @brief	Get the name of this argument.
		CONSTEXPR std::string name() const requires (std::same_as<T, option_t>) { return{ _value.first }; }

		CONSTEXPR std::optional<std::string> getValue() const requires(var::any_same<T, flag_t, option_t>) { return _value.second; }

		/// @brief	Check if this argument has a captured value. (always returns false for parameters)
		CONSTEXPR bool has_value() const requires (std::same_as<T, parameter_t>) { return false; }
		/// @brief	Check if this argument has a captured value.
		CONSTEXPR bool has_value() const requires (var::any_same<T, flag_t, option_t>) { return _value.second.has_value(); }

		/**
		 * @brief				Retrieve the capture value of this argument if it exists; otherwise returns the given string instead.
		 * @param defaultValue	A string to return when a capture value is not available.
		 * @returns				The captured value of this argument or defaultValue if one wasn't available.
		 */
		CONSTEXPR std::string value_or(std::string const& defaultValue) const requires (var::any_same<T, flag_t, option_t>) { return _value.second.value_or(defaultValue); }
		/**
		 * @brief				Retrieve the capture value of this argument.
		 * @returns				The captured value of this argument.
		 * @throws ex::except	This argument does not contain a captured value.
		 */
		CONSTEXPR std::string value() const requires (var::any_same<T, flag_t, option_t>) { return _value.second.value(); }
	};

	/// @brief	Constraint that allows any types derived from base_arg.
	template<typename T> concept valid_arg = std::derived_from<T, base_arg>;

	/// @brief	Parameter argument type; these are any arguments without special meaning.
	using Parameter = basic_arg_t<parameter_t>;
	/// @brief	Flag argument type; these are single-character arguments that can be chained together and can capture values when enabled.
	using Flag = basic_arg_t<flag_t>;
	/// @brief	Option argument type; these are multi-character arguments that can capture values when enabled.
	using Option = basic_arg_t<option_t>;

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

		template<typename TVisitor>
		WINCONSTEXPR auto visit(const TVisitor& visitor) const noexcept
		{
			return std::visit(visitor, static_cast<base>(*this));
		}

		/**
		 * @brief	Gets the name of this argument.
		 * @returns	The name of this argument, excluding any prefixes that were stripped during parsing.
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
		bool has_value() const noexcept;
		/**
		 * @brief				Gets the capture value of this argument if it exists; otherwise returns the given string instead.
		 * @param defaultValue	The default string value to return when this argument doesn't have a capture value.
		 * @returns				std::string
		 */
		std::string value_or(std::string const&) const noexcept;
		/**
		 * @brief				Gets the capture value of this argument.
		 * @returns				The capture value of this argument.
		 * @throws
		 */
		std::string value() const noexcept(false);

		template<valid_arg T> CONSTEXPR bool is_type() const noexcept { return std::holds_alternative<T>(*this); }
		template<valid_arg... Ts> CONSTEXPR bool is_any_type() const noexcept { return var::variadic_or(std::holds_alternative<Ts>(*this)...); }
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
	bool variantarg::has_value() const noexcept
	{
		return this->visit([](auto&& value) -> bool {
			using T = std::decay_t<decltype(value)>;

			if constexpr (std::same_as<T, Parameter>)
				return value.has_value();
			else if constexpr (std::same_as<T, Flag>)
				return value.has_value();
			else if constexpr (std::same_as<T, Option>)
				return value.has_value();
			//else static_assert(false, "opt3::variantarg:  Visitor doesn't handle all possible types!");
						   });
	}
	std::string variantarg::value_or(std::string const& defaultValue) const noexcept
	{
		return this->visit([&defaultValue](auto&& value) -> std::string {
			using T = std::decay_t<decltype(value)>;

			if constexpr (std::same_as<T, Parameter>)
				return defaultValue;
			else if constexpr (std::same_as<T, Flag>)
				return value.value_or(defaultValue);
			else if constexpr (std::same_as<T, Option>)
				return value.value_or(defaultValue);
			//else static_assert(false, "opt3::variantarg:  Visitor doesn't handle all possible types!");
						   });
	}
	std::string variantarg::value() const noexcept(false)
	{
		return this->visit([](auto&& value) -> std::string {
			using T = std::decay_t<decltype(value)>;

			if constexpr (std::same_as<T, Parameter>)
				throw make_exception("opt3::variantarg:  Cannot retrieve capture value from an argument of type parameter!");
			else if constexpr (std::same_as<T, Flag>)
				return value.value();
			else if constexpr (std::same_as<T, Option>)
				return value.value();
			//else static_assert(false, "opt3::variantarg:  Visitor doesn't handle all possible types!");
						   });
	}







	/**
	 * @brief				Instantiates a variantarg of the specified type, with the given values.
	 * @tparam TValue		The type of the underlying value.  This is passed to the templated basic_arg_t struct.
	 * @tparam T			Any type that is the same as, or is implicitly convertible to, std::string or char
	 * @param name			The name of the argument.
	 * @param capture		An optional capture value for this argument.
	 *\n					Note that when TReturn is parameter_t
	 * @returns				A variantarg containing an argument of the specified type.
	 */
	template<var::any_same<parameter_t, flag_t, option_t> TValue>
	WINCONSTEXPR variantarg make_argument(vstring&& name, std::optional<std::string>&& capture = std::nullopt) noexcept(false)
	{
		if constexpr (std::same_as<TValue, parameter_t>)
			return{ basic_arg_t<parameter_t>{ std::forward<vstring>(name) } };
		else if constexpr (std::same_as<TValue, flag_t>)
			return{ basic_arg_t<flag_t>{ std::make_pair(std::forward<vstring>(name).get_single_char(), std::forward<std::optional<std::string>>(capture))  } };
		else if constexpr (std::same_as<TValue, option_t>)
			return{ basic_arg_t<option_t>{ std::make_pair(std::forward<vstring>(name), std::forward<std::optional<std::string>>(capture))} };
		else throw make_exception("make_argument() failed:  Invalid target type specified!");
	}
	/**
	 * @brief				Instantiates a variantarg of the specified type, with the given values.
	 *\n					This overload accepts actual argument types instead of their underlying types, and translates them to the appropriate underlying type.
	 * @tparam TArgument	The type of argument to instantiate.
	 * @tparam T			Any type that is the same as, or is implicitly convertible to, std::string or char
	 * @param name			The name of the argument.
	 * @param capture		An optional capture value for this argument.
	 *\n					Note that when TReturn is parameter_t
	 * @returns				A variantarg containing an argument of the specified type.
	 */
	template<var::any_same<Parameter, Flag, Option> TArgument>
	WINCONSTEXPR variantarg make_argument(vstring&& name, std::optional<std::string>&& capture = std::nullopt) noexcept(false)
	{
		if constexpr (std::same_as<TArgument, Parameter>)
			return make_argument<parameter_t>(std::forward<vstring>(name), std::forward<std::optional<std::string>>(capture));
		else if constexpr (std::same_as<TArgument, Flag>)
			return make_argument<flag_t>(std::forward<vstring>(name), std::forward<std::optional<std::string>>(capture));
		else if constexpr (std::same_as<TArgument, Option>)
			return make_argument<option_t>(std::forward<vstring>(name), std::forward<std::optional<std::string>>(capture));
		else throw make_exception("make_argument() failed:  Invalid target type specified!");
	}

	/**
	 * @struct	arg_container
	 * @brief	A container type derived directly from std::vector that exposes utility methods for managing & interacting with argument lists.
	 *\n		This object contains the bulk of the code in the entire opt3 library.
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
		WINCONSTEXPR std::vector<variantarg>::const_iterator find(vstring const& name) const
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
		WINCONSTEXPR std::vector<variantarg>::const_iterator find_any(Ts&&... names) const
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
		WINCONSTEXPR std::vector<std::vector<variantarg>::const_iterator> find_all(Ts&&... names) const
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
		WINCONSTEXPR std::vector<variantarg>::const_reverse_iterator rfind(vstring const& name) const
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
		WINCONSTEXPR std::vector<variantarg>::const_reverse_iterator rfind_any(Ts&&... names) const
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
		WINCONSTEXPR std::vector<std::vector<variantarg>::const_reverse_iterator> rfind_all(Ts&&... names) const
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
		CONSTEXPR std::optional<variantarg> get(vstring const& name) const
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
		CONSTEXPR std::optional<variantarg> get_any(Ts&&... names) const
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
		CONSTEXPR std::vector<variantarg> get_all(Ts&&... names) const
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
		CONSTEXPR std::optional<variantarg> rget(vstring const& name) const
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
		CONSTEXPR std::optional<variantarg> rget_any(Ts&&... names) const
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
		CONSTEXPR std::vector<variantarg> rget_all(Ts&&... names) const
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
		CONSTEXPR std::optional<std::string> getv(vstring const& name) const
		{
			static_assert(!var::any_same<Parameter, TFilterTypes...>, "opt3::arg_container::getv() cannot be used to get non-Flags or non-Options!");
			constexpr bool match_any_type{ sizeof...(TFilterTypes) == 0ull };
			for (auto it{ this->begin() }, end{ this->end() }; it != end; ++it) {
				if ((match_any_type || it->is_any_type<TFilterTypes...>()) && it->compare_name(name)) {
					if (it->is_type<Parameter>())
						return it->name();
					else if (it->has_value())
						return it->value();
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
		CONSTEXPR std::optional<std::string> getv_any(Ts&&... names) const
		{
			static_assert(!var::any_same<Parameter, TFilterTypes...>, "opt3::arg_container::getv() cannot be used to get non-Flags or non-Options!");
			constexpr bool
				match_any_type{ sizeof...(TFilterTypes) == 0ull },
				match_any_name{ sizeof...(Ts) == 0ull };
			for (auto it{ this->begin() }, end{ this->end() }; it != end; ++it) {
				if ((match_any_type || it->is_any_type<TFilterTypes...>()) && ((match_any_name || var::variadic_or(it->compare_name(std::forward<Ts>(names))...)))) {
					if (it->is_type<Parameter>())
						return it->name();
					else if (it->has_value())
						return it->value();
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
		CONSTEXPR std::vector<std::string> getv_all(Ts&&... names) const
		{
			constexpr bool
				match_any_type{ sizeof...(TFilterTypes) == 0ull },
				match_any_name{ sizeof...(Ts) == 0ull };
			std::vector<std::string> vec;
			constexpr size_t block_size{ sizeof...(Ts) };
			vec.reserve(block_size);
			for (auto it{ this->begin() }, end{ this->end() }; it != end; ++it) {
				if ((match_any_type || it->is_any_type<TFilterTypes...>()) && ((match_any_name || var::variadic_or(it->compare_name(std::forward<Ts>(names))...)))) {
					if (it->is_type<Parameter>())
						vec.emplace_back(it->name());
					else if (it->has_value())
						vec.emplace_back(it->value());
					else continue;
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
		CONSTEXPR std::optional<std::string> rgetv(vstring const& name) const
		{
			constexpr bool match_any_type{ sizeof...(TFilterTypes) == 0ull };
			for (auto it{ this->rbegin() }, end{ this->rend() }; it != end; ++it) {
				if ((match_any_type || it->is_any_type<TFilterTypes...>()) && it->compare_name(name)) {
					if (it->is_type<Parameter>())
						return it->name();
					else if (it->has_value())
						return it->value();
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
		CONSTEXPR std::optional<std::string> rgetv_any(Ts&&... names) const
		{
			constexpr bool
				match_any_type{ sizeof...(TFilterTypes) == 0ull },
				match_any_name{ sizeof...(Ts) == 0ull };
			for (auto it{ this->rbegin() }, end{ this->rend() }; it != end; ++it) {
				if ((match_any_type || it->is_any_type<TFilterTypes...>()) && ((match_any_name || var::variadic_or(it->compare_name(std::forward<Ts>(names))...)))) {
					if (it->is_type<Parameter>())
						return it->name();
					else if (it->has_value())
						return it->value();
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
		CONSTEXPR std::vector<std::string> rgetv_all(Ts&&... names) const
		{
			constexpr bool
				match_any_type{ sizeof...(TFilterTypes) == 0ull },
				match_any_name{ sizeof...(Ts) == 0ull };
			std::vector<std::string> vec;
			constexpr size_t block_size{ sizeof...(Ts) };
			vec.reserve(block_size);
			for (auto it{ this->rbegin() }, end{ this->rend() }; it != end; ++it) {
				if ((match_any_type || it->is_any_type<TFilterTypes...>()) && ((match_any_name || var::variadic_or(it->compare_name(std::forward<Ts>(names))...)))) {
					if (it->is_type<Parameter>())
						vec.emplace_back(it->name());
					else if (it->has_value())
						vec.emplace_back(it->value());
					else continue;
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
		 *\n						This overload is only available when TReturn specifies a type that is implicitly convertible from variantarg.
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
		CONSTEXPR std::optional<TReturn> castget(const std::function<TReturn(variantarg)>& converter, vstring const& name) const noexcept
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
		CONSTEXPR std::optional<TReturn> castget_any(const std::function<TReturn(variantarg)>& converter, Ts&&... names) const noexcept
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
		 * @param names				The name(s) of the target argument(s).
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
		 * @param names				The name(s) of the target argument(s).
		 * @returns					A vector containing all matching arguments.
		 */
		template<typename TReturn, valid_arg... TFilterTypes, var::same_or_convertible<vstring>... Ts>
		CONSTEXPR std::vector<TReturn> castget_all(const std::function<TReturn(variantarg)>& converter, Ts&&... names) const noexcept
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
		CONSTEXPR std::optional<TReturn> castgetv(const std::function<TReturn(vstring)>& converter, vstring const& name) const noexcept
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
		 * @param names				The name(s) of the target argument(s).
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
		 * @brief					Get the specified argument's capture value, casted to the specified type.
		 * @tparam TReturn			The desired return type.
		 * @tparam TFilterTypes...	Any number of types to limit the returned result types to.  When left empty, all types are considered matching.
		 * @param converter			A conversion function that accepts type vstring and returns type TReturn.
		 * @param names				The name(s) of the target argument(s).
		 * @returns					The first specified argument, casted to the specified type; or std::nullopt if it wasn't found.
		 */
		template<typename TReturn, valid_arg... TFilterTypes, var::same_or_convertible<vstring>... Ts>
		CONSTEXPR std::optional<TReturn> castgetv_any(const std::function<TReturn(vstring)>& converter, Ts&&... names) const noexcept
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
		 * @param names				The name(s) of the target argument(s).
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
		 * @param names				The name(s) of the target argument(s).
		 * @returns					A vector containing the capture value(s) of all matches.
		 */
		template<typename TReturn, valid_arg... TFilterTypes, var::same_or_convertible<vstring>... Ts>
		CONSTEXPR std::vector<TReturn> castgetv_all(const std::function<TReturn(vstring)>& converter, Ts&&... names) const noexcept
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
		 * @param name		The name of the argument to check for.
		 * @returns			True when the argument was included; otherwise false.
		 */
		template<valid_arg... TFilterTypes>
		CONSTEXPR bool check(vstring const& name) const
		{
			return this->find<TFilterTypes...>(name) != this->end();
		}
		/**
		 * @brief			Checks if the specified argument was included or not.
		 * @param names		The name of the argument to check for.
		 * @returns			True when the argument was included; otherwise false.
		 */
		template<valid_arg... TFilterTypes, var::same_or_convertible<vstring>... Ts>
		CONSTEXPR bool check_any(Ts&&... names) const
		{
			return this->find_any<TFilterTypes...>(std::forward<Ts>(names)...) != this->end();
		}
		/**
		 * @brief			Checks if the specified argument was included or not.
		 * @param names		The name of the argument to check for.
		 * @returns			True when the argument was included; otherwise false.
		 */
		template<valid_arg... TFilterTypes, var::same_or_convertible<vstring>... Ts>
		CONSTEXPR bool check_all(Ts&&... names) const
		{
			std::vector<vstring> nameVec{ std::forward<Ts>(names)... };
			return std::all_of(nameVec.begin(), nameVec.end(), [this](auto&& name) { return check(std::forward<decltype(name)>(name)); });
		}
		/**
		 * @brief		Checks if the specified Option was included.
		 * @param name	The name to check for.
		 * @returns		true when the Option was included; otherwise false.
		 */
		CONSTEXPR bool checkopt(vstring const& name) { return this->check<Option>(name); }
		/**
		 * @brief		Checks if the specified Flag was included.
		 * @param name	The name to check for.
		 * @returns		true when the Flag was included; otherwise false.
		 */
		CONSTEXPR bool checkflag(vstring const& name) { return this->check<Flag>(name); }
		/**
		 * @brief		Checks if the specified Parameter was included.
		 * @param name	The name to check for.
		 * @returns		true when the Parameter was included; otherwise false.
		 */
		CONSTEXPR bool checkparam(vstring const& name) { return this->check<Parameter>(name); }
	#pragma endregion check

	#pragma region checkv
		/**
		 * @brief			Checks if the specified argument was included or not.
		 * @param name		The name of the argument to check for.
		 * @returns			True when the argument was included; otherwise false.
		 */
		template<valid_arg... TFilterTypes>
		CONSTEXPR bool checkv(std::string const& value, vstring const& name) const
		{
			if (const auto& v{ this->getv<TFilterTypes...>(name) }; v.has_value())
				return v.value() == value;
			return false;
		}
		/**
		 * @brief			Checks if the specified argument was included or not.
		 * @param names		The name of the argument to check for.
		 * @returns			True when the argument was included; otherwise false.
		 */
		template<valid_arg... TFilterTypes, var::same_or_convertible<vstring>... Ts>
		CONSTEXPR bool checkv_any(std::string const& value, Ts&&... names) const
		{
			if (const auto& v{ this->getv_any<TFilterTypes...>(std::forward<Ts>(names)...) }; v.has_value())
				return v.value() == value;
			return false;
		}
		/**
		 * @brief			Checks if the specified argument was included or not.
		 * @param names		The name of the argument to check for.
		 * @returns			True when the argument was included; otherwise false.
		 */
		template<valid_arg... TFilterTypes, var::same_or_convertible<vstring>... Ts>
		CONSTEXPR bool checkv_all(std::string const& value, Ts&&... names) const
		{
			for (const auto& it : this->get_all<TFilterTypes...>(std::forward<Ts>(names)...))
				if (!it.has_value() || it.value() != value)
					return false;
			return true;
		}
	#pragma endregion checkv
	};



	/// @brief	unsigned char type
	using uchar = unsigned char;

	/**
	 * @enum	CaptureStyle
	 * @brief	Defines various capture styles
	 */
	enum class CaptureStyle : uchar {
		Disabled = 0,
		EqualsOnly = 1,
		Optional = 2,
		Required = 4,
	};



	/**
	 * @struct	ArgumentParsingRules
	 * @brief	Allows configuration of the rules used when parsing arguments.
	 */
	struct ArgumentParsingRules {
		/**
		 * @brief	This argument is ignored when '-' does not appear in this instance's 'delimiters' vector.
		 *\n		When true, valid numbers that are prefixed with a single dash character (Ex: "-3.14")
		 *			 are assumed to be negative numbers; when false, numbers with a single dash prefix
		 *			 are always considered flags instead of numbers.
		 *\n		Default: true
		 */
		bool assumeNumericDashPrefixIsNegative{ true };
		/**
		 * @brief	List of characters that are considered valid argument prefixes.
		 *\n		This defaults to just '-'; other commonly-used prefixes include '/' & '+',
		 *			 but these must be added manually.
		 */
		std::vector<char> delimiters{ '-' };

		/**
		 * @brief	Determines the default capture style used for arguments present in the capture list that do not specify a capture style themselves.
		 */
		CaptureStyle defaultCaptureStyle{ CaptureStyle::Optional };

		/**
		 * @brief	Default Constructor.
		 */
		WINCONSTEXPR ArgumentParsingRules() {}
		/**
		 * @brief										Constructor.
		 * @param defaultCaptureStyle					Determines the default capture style used for arguments present in the capture list that do not specify a capture style themselves.
		 * @param delimiters							List of characters that are considered valid argument prefixes.
		 * @param assumeNumericDashPrefixIsNegative		This argument is ignored when '-' does not appear in this instance's 'delimiters' vector.
		 *\n											When true, valid numbers that are prefixed with a single dash character (Ex: "-3.14")
		 *												 are assumed to be negative numbers; when false, numbers with a single dash prefix
		 *												 are always considered flags instead of numbers.
		 */
		WINCONSTEXPR ArgumentParsingRules(CaptureStyle const& defaultCaptureStyle, std::vector<char>&& delimiters, bool const& assumeNumericDashPrefixIsNegative = true) : defaultCaptureStyle{ defaultCaptureStyle }, assumeNumericDashPrefixIsNegative{ assumeNumericDashPrefixIsNegative }, delimiters{ std::forward<std::vector<char>>(delimiters) } {}
		/**
		 * @brief										Constructor.
		 * @param delimiters							List of characters that are considered valid argument prefixes.
		 * @param assumeNumericDashPrefixIsNegative		This argument is ignored when '-' does not appear in this instance's 'delimiters' vector.
		 *\n											When true, valid numbers that are prefixed with a single dash character (Ex: "-3.14")
		 *												 are assumed to be negative numbers; when false, numbers with a single dash prefix
		 *												 are always considered flags instead of numbers.
		 */
		WINCONSTEXPR ArgumentParsingRules(std::vector<char>&& delimiters, bool const& assumeNumericDashPrefixIsNegative = true) : assumeNumericDashPrefixIsNegative{ assumeNumericDashPrefixIsNegative }, delimiters{ std::forward<std::vector<char>>(delimiters) } {}
		/**
		 * @brief										Constructor.
		 * @param assumeNumericDashPrefixIsNegative		This argument is ignored when '-' does not appear in this instance's 'delimiters' vector.
		 *\n											When true, valid numbers that are prefixed with a single dash character (Ex: "-3.14")
		 *												 are assumed to be negative numbers; when false, numbers with a single dash prefix
		 *												 are always considered flags instead of numbers.
		 */
		WINCONSTEXPR ArgumentParsingRules(bool const& assumeNumericDashPrefixIsNegative) : assumeNumericDashPrefixIsNegative{ assumeNumericDashPrefixIsNegative } {}

	#pragma region Methods
		[[nodiscard]] WINCONSTEXPR std::string getDelimitersAsString() const
		{
			std::string s;
			s.reserve(delimiters.size());
			for (const auto& c : delimiters)
				s += c;
			s.shrink_to_fit();
			return s;
		}
		/**
		 * @brief		Check if the given character is a valid delimiter, according to the static Settings_ArgParser object.
		 * @param c		Input Character
		 * @returns		bool
		 */
		[[nodiscard]] WINCONSTEXPR bool isDelimiter(const char& c) const
		{
			return std::any_of(delimiters.begin(), delimiters.end(), [&c](auto&& delim) { return delim == c; });
		}
		[[nodiscard]] WINCONSTEXPR size_t countPrefix(const std::string& str, const size_t& max_delims) const
		{
			size_t count{ 0ull };
			for (size_t i{ 0ull }; i < str.size() && i < max_delims; ++i) {
				if (isDelimiter(str.at(i)))
					++count;
				else break;
			}
			return count;
		}
		[[nodiscard]] WINCONSTEXPR std::pair<std::string, size_t> stripPrefix(const std::string& str, const size_t& max_delims = 2ull) const
		{
			const auto count{ countPrefix(str, max_delims) };
			return{ str.substr(count), count };
		}
		/**
		 * @brief		Checks if the given string is a valid integer, floating-point, or hexadecimal number. Hexadecimal numbers must be prefixed with "0x" (or "-0x") to be detected properly.
		 * @param str	Input String
		 * @returns		bool
		 */
		[[nodiscard]] WINCONSTEXPR bool isNumber(std::string str) const
		{
			str = str::trim(str::strip(str, ','));
			if (str.empty())
				return false;

			if (str.starts_with("0x")) {
				return str.size() > 2ull && std::all_of(str.begin() + 2ull, str.end(), str::ishexdigit);
			}
			else {
				const bool is_negative{ str.starts_with('-') };
				if (int decimalCount{ 0 }; std::all_of(str.begin() + static_cast<size_t>(is_negative), str.end(), [&decimalCount](auto&& c) { return (c == '.' ? ++decimalCount < 2 : str::stdpred::isdigit(c)); })) {
					return assumeNumericDashPrefixIsNegative; //< when str is a valid number, return true when assuming it is a number and not a flag
				} // else, return false
			}
			return false;
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
				&& ((here + 1ll)->front() != '-' // AND next argument doesn't start with a dash
					|| isNumber(*(here + 1ll))); // OR next argument is a number
		}
	#pragma endregion Methods
	};

	/**
	 * @struct	basic_capture_wrapper
	 * @brief	Extends vstring with the ability to require specific arguments.
	 */
	template<var::valid_char TChar, std::derived_from<std::char_traits<TChar>> TCharTraits = std::char_traits<TChar>, std::derived_from<std::allocator<TChar>> TAlloc = std::allocator<TChar>>
	struct basic_capture_wrapper {
		basic_vstring<TChar, TCharTraits, TAlloc> _input;
		std::optional<CaptureStyle> captureStyle;
		std::optional<size_t> maxCount;
		size_t minCount{ 0ull };

		WINCONSTEXPR basic_capture_wrapper(const basic_vstring<TChar, TCharTraits, TAlloc>& input, const std::optional<CaptureStyle>& captureStyle = CaptureStyle::Optional, const size_t& minCount = 0ull, const std::optional<size_t>& maxCount = std::nullopt) : _input(input), captureStyle{ captureStyle }, minCount{ minCount }, maxCount{ maxCount } {}

		WINCONSTEXPR basic_capture_wrapper(basic_capture_wrapper<TChar, TCharTraits, TAlloc>&& o) noexcept : _input{ std::move(o._input) }, captureStyle{ std::move(o.captureStyle) }, maxCount{ std::move(o.maxCount) }, minCount{ std::move(o.minCount) } {}
		WINCONSTEXPR basic_capture_wrapper(basic_capture_wrapper<TChar, TCharTraits, TAlloc> const& o) noexcept : _input(o._input), captureStyle{ o.captureStyle }, maxCount{ o.maxCount }, minCount{ o.minCount } {}
		~basic_capture_wrapper() = default;

		WINCONSTEXPR basic_capture_wrapper<TChar, TCharTraits, TAlloc>& operator=(basic_capture_wrapper<TChar, TCharTraits, TAlloc>&& o) noexcept
		{
			_input = std::move(o._input);
			captureStyle = std::move(o.captureStyle);
			maxCount = std::move(o.maxCount);
			minCount = std::move(o.minCount);
			return *this;
		}
		WINCONSTEXPR basic_capture_wrapper<TChar, TCharTraits, TAlloc>& operator=(basic_capture_wrapper<TChar, TCharTraits, TAlloc> const& o) noexcept
		{
			_input = o._input;
			captureStyle = o.captureStyle;
			maxCount = o.maxCount;
			minCount = o.minCount;
			return *this;
		}

		WINCONSTEXPR basic_vstring<TChar, TCharTraits, TAlloc> name() const
		{
			return _input;
		}

		constexpr bool withinRequiredRange(size_t const& count) const
		{
			return count >= minCount && (!maxCount.has_value() || count < maxCount.value());
		}
	};
	using capture_wrapper = basic_capture_wrapper<char, std::char_traits<char>, std::allocator<char>>;

	/**
	 * @brief				Specify that an argument requires captured input while also providing additional context.
	 * @tparam MAX			Limits the number of times this argument may be specified legally.
	 * @tparam MIN			Requires that this argument be specified at least this many times. (Default: 0)
	 * @tparam T			Any type that is a char or string, or is implicitly convertible to a char or string.
	 * @param input			The name of the argument, excluding prefixes. chars are interpreted as flags, while strings are interpreted as options.
	 * @param equalsOnly	When true, this argument may only capture input when directly appended with an equals sign.
	 * @returns				capture_wrapper
	 */
	template<size_t MAX, size_t MIN = 0ull, var::any_same_or_convertible<std::string, char> T>
	inline WINCONSTEXPR capture_wrapper requireCapture(T&& input, const bool& equalsOnly = false)
	{
		return{ std::forward<T>(input), (equalsOnly ? static_cast<CaptureStyle>(static_cast<uchar>(CaptureStyle::Required) | static_cast<uchar>(CaptureStyle::EqualsOnly)) : CaptureStyle::Required), MIN, MAX };
	}
	/**
	 * @brief				Specify that an argument requires captured input while also providing additional context.
	 * @tparam MIN			Requires that this argument be specified at least this many times. (Default: 0)
	 * @tparam T			Any type that is a char or string, or is implicitly convertible to a char or string.
	 * @param input			The name of the argument, excluding prefixes. chars are interpreted as flags, while strings are interpreted as options.
	 * @param equalsOnly	When true, this argument may only capture input when directly appended with an equals sign.
	 * @returns				capture_wrapper
	 */
	template<size_t MIN = 0ull, var::any_same_or_convertible<std::string, char> T>
	inline WINCONSTEXPR capture_wrapper requireCapture(T&& input, const bool& equalsOnly = false)
	{
		return{ std::forward<T>(input), (equalsOnly ? static_cast<CaptureStyle>(static_cast<uchar>(CaptureStyle::Required) | static_cast<uchar>(CaptureStyle::EqualsOnly)) : CaptureStyle::Required), MIN };
	}
	/**
	 * @brief				Specify that an argument does not require captured input while also providing additional context.
	 * @tparam MAX			Limits the number of times this argument may be specified legally.
	 * @tparam MIN			Requires that this argument be specified at least this many times. (Default: 0)
	 * @tparam T			Any type that is a char or string, or is implicitly convertible to a char or string.
	 * @param input			The name of the argument, excluding prefixes. chars are interpreted as flags, while strings are interpreted as options.
	 * @param equalsOnly	When true, this argument may only capture input when directly appended with an equals sign.
	 * @returns				capture_wrapper
	 */
	template<size_t MAX, size_t MIN = 0ull, var::any_same_or_convertible<std::string, char> T>
	inline WINCONSTEXPR capture_wrapper optionalCapture(T&& input, const bool& equalsOnly = false)
	{
		return{ std::forward<T>(input), (equalsOnly ? static_cast<CaptureStyle>(static_cast<uchar>(CaptureStyle::Optional) | static_cast<uchar>(CaptureStyle::EqualsOnly)) : CaptureStyle::Optional), MIN, MAX };
	}
	/**
	 * @brief				Specify that an argument does not require captured input while also providing additional context.
	 * @tparam MIN			Requires that this argument be specified at least this many times. (Default: 0)
	 * @tparam T			Any type that is a char or string, or is implicitly convertible to a char or string.
	 * @param input			The name of the argument, excluding prefixes. chars are interpreted as flags, while strings are interpreted as options.
	 * @param equalsOnly	When true, this argument may only capture input when directly appended with an equals sign.
	 * @returns				capture_wrapper
	 */
	template<size_t MIN = 0ull, var::any_same_or_convertible<std::string, char> T>
	inline WINCONSTEXPR capture_wrapper optionalCapture(T&& input, const bool& equalsOnly = false)
	{
		return{ std::forward<T>(input), (equalsOnly ? static_cast<CaptureStyle>(static_cast<uchar>(CaptureStyle::Optional) | static_cast<uchar>(CaptureStyle::EqualsOnly)) : CaptureStyle::Optional), MIN };
	}

	/**
	 * @brief				Specify that an argument does not accept captured input while also providing additional context.
	 * @tparam MIN			Requires that this argument be specified at least this many times. (Default: 0)
	 * @tparam T			Any type that is a char or string, or is implicitly convertible to a char or string.
	 * @param input			The name of the argument, excluding prefixes. chars are interpreted as flags, while strings are interpreted as options.
	 * @returns				capture_wrapper
	 */
	template<size_t MIN = 0ull, var::any_same_or_convertible<std::string, char> T>
	inline WINCONSTEXPR capture_wrapper noCapture(T&& input)
	{
		return{ std::forward<T>(input), CaptureStyle::Disabled, MIN };
	}
	/**
	 * @brief				Specify that an argument does not accept captured input while also providing additional context.
	 * @tparam MAX			Requires that this argument be specified no more than this many times. (Default: std::nullopt)
	 * @tparam MIN			Requires that this argument be specified at least this many times. (Default: 0)
	 * @tparam T			Any type that is a char or string, or is implicitly convertible to a char or string.
	 * @param input			The name of the argument, excluding prefixes. chars are interpreted as flags, while strings are interpreted as options.
	 * @returns				capture_wrapper
	 */
	template<size_t MAX, size_t MIN = 0ull, var::any_same_or_convertible<std::string, char> T>
	inline WINCONSTEXPR capture_wrapper noCapture(T&& input)
	{
		return{ std::forward<T>(input), CaptureStyle::Disabled, MIN, MAX };
	}

	/**
	 * @struct	CaptureList
	 * @brief	Contains a list of arguments that should be allowed to capture additional arguments. This is used by the parse function.
	 */
	struct CaptureList {
		const ArgumentParsingRules parsingRules;
		/// @brief Vector of input wrapper strings that contains the argument names.
		const std::vector<capture_wrapper> vec;

		template<var::any_same_or_convertible<capture_wrapper, vstring> T>
		static capture_wrapper get_wrapper(const ArgumentParsingRules& parsingRules, T&& item)
		{
			if constexpr (std::same_as<T, capture_wrapper>)
				return item;
			else if constexpr (std::same_as<T, std::string>)
				return capture_wrapper{ str::trim_preceeding(std::forward<T>(item), parsingRules.getDelimitersAsString()) };
			return capture_wrapper{ std::forward<T>(item) };
		}

		/**
		 * @brief							Default Constructor.
		 * @tparam VT...					Variadic Templated Types.
		 * @param ...capturing_arguments	Arguments that should be allowed to capture additional arguments. Names should not contain prefix delimiters, but if they do, they are removed.
		 */
		template<var::any_same_or_convertible<capture_wrapper, vstring>... Ts> constexpr CaptureList(ArgumentParsingRules const& parsingRules, Ts&&... capturing_arguments) : parsingRules{ parsingRules }, vec{ get_wrapper(parsingRules, capture_wrapper{ std::forward<Ts>(capturing_arguments) })... } {}
		template<var::any_same_or_convertible<capture_wrapper, vstring>... Ts> constexpr CaptureList(Ts&&... capturing_arguments) : CaptureList(ArgumentParsingRules{}, capture_wrapper{ std::forward<Ts>(capturing_arguments) }...) {}
		WINCONSTEXPR operator const std::vector<capture_wrapper>() const { return vec; }
		/**
		 * @brief				Checks if a given argument appears in the capture list, and returns the associated capture_wrapper object.
		 * @param name			The name of the target argument. (Note that preceeding delimiters are stripped automatically)
		 * @returns				The capture_wrapper object associated with the given name, or std::nullopt if none were found.
		 */
		std::optional<capture_wrapper> get(std::string const& name) const
		{
			const auto& [s, count] { parsingRules.stripPrefix(name) };
			if (const auto& it{ std::find_if(vec.begin(), vec.end(), [&s](auto&& capwrap) { return capwrap.name() == s; }) }; it != vec.end())
				return *it;
			return std::nullopt;
		}

		CaptureStyle get_capture_style(std::string const& name) const
		{
			if (const auto& wrapper{ get(name) }; wrapper.has_value())
				return wrapper.value().captureStyle.value_or(parsingRules.defaultCaptureStyle);
			return CaptureStyle::Disabled;
		}
	};

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

	/**
	 * @brief			Parse commandline arguments into an ArgContainer instance.
	 *\n				__Argument Types__
	 *\n				- Parameters are any arguments that do not begin with a dash '-' character that were not captured by another argument type.
	 *\n				- Options are arguments that begin with 2 dash '-' characters, and can capture additional arguments if the option name appears in the capture list.
	 *\n				- Flags are arguments that begin with a single dash '-' character, are a single character in length, and can capture additional arguments. Flags can appear alone, or in "chains" where each character is treated as an individual flag. In a flag chain, only the last flag can capture additional arguments.
	 *\n				__Capture Rules__
	 *\n				- Only options/flags specified in the capture list are allowed to capture additional arguments. Capture list entries should not include a delimiter prefix.
	 *\n				- Options/Flags cannot be captured under any circumstance. ex: "--opt --opt captured" results in "--opt", & "--opt" + "captured".
	 *\n				- If a flag in a chain should capture an argument (either with an '=' delimiter or by context), it must appear at the end of the chain.
	 *\n				- Any captured arguments do not appear in the argument list by themselves, and must be accessed through the argument that captured them.
	 * @param args		Commandline arguments as a vector of strings, in order and including argv[0].
	 * @param captures	A CaptureList instance specifying which arguments are allowed to capture other arguments as their parameters
	 * @returns			ArgContainer
	 */
	inline arg_container parse(std::vector<std::string>&& args, const CaptureList& captures)
	{
		// remove empty arguments, which are possible when passing arguments from automated testing applications
		args.erase(std::remove_if(args.begin(), args.end(), [](auto&& s) { return s.empty(); }), args.end());

		arg_container cont{};
		cont.reserve(args.size());

		for (std::vector<std::string>::const_iterator it{ args.begin() }; it != args.end(); ++it) {
			auto [arg, d_count] { captures.parsingRules.stripPrefix(*it, 2ull) };

			switch (d_count) {
			case 2ull: // Option
				if (const auto eqPos{ arg.find('=') }; eqPos != std::string::npos) {// argument contains an equals sign
					auto opt{ arg.substr(0ull, eqPos) }, cap{ arg.substr(eqPos + 1ull) };

					if (const auto& captureStyle{ captures.get_capture_style(opt) }; !CaptureIsDisabled(captureStyle))
						cont.emplace_back(Option(std::make_pair(std::move(opt), std::move(cap))));
					else {
						if (CaptureIsRequired(captureStyle))
							throw make_exception("Expected a capture argument for option '", opt, "'!");
						cont.emplace_back(Option(std::make_pair(std::move(opt), std::nullopt)));
						if (!cap.empty()) {
							arg = cap;
							goto JUMP_TO_PARAMETER; // skip flag case, add invalid capture as a parameter
						}
					}
				}
				else if (const auto& captureStyle{ captures.get_capture_style(arg) }; !CaptureIsDisabledOrEqualsOnly(captureStyle) && captures.parsingRules.canCaptureNext(it, args.end())) // argument can capture next arg
					cont.emplace_back(Option(std::make_pair(arg, *++it)));
				else {
					if (CaptureIsRequired(captureStyle))
						throw make_exception("Expected a capture argument for option '", arg, "'!");
					cont.emplace_back(Option(std::make_pair(arg, std::nullopt)));
				}
				break;
			case 1ull: // Flag
				if (!captures.parsingRules.isNumber(arg)) { // single-dash prefix is not a number
					std::optional<Flag> capt{ std::nullopt }; // this can contain a flag if there is a capturing flag at the end of a chain
					std::string invCap{}; //< for invalid captures that should be treated as parameters
					if (const auto eqPos{ arg.find('=') }; eqPos != std::string::npos) {
						invCap = arg.substr(eqPos + 1ull); // get string following '=', use invCap in case flag can't capture
						if (const auto flag{ arg.substr(eqPos - 1ull, 1ull) }; !CaptureIsDisabled(captures.get_capture_style(flag))) {
							capt = Flag{ std::make_pair(flag.front(), invCap) }; // push the capturing flag to capt, insert into vector once all other flags in this chain are parsed
							arg = arg.substr(0ull, eqPos - 1ull); // remove last flag, '=', and captured string from arg
							invCap.clear(); // flag can capture, clear invCap
						}
						else
							arg = arg.substr(0ull, eqPos); // remove everything from eqPos to arg.end()
					}
					// iterate through characters in arg
					for (auto fl{ arg.begin() }; fl != arg.end(); ++fl) {
						const auto& captureStyle{ captures.get_capture_style(std::string(1ull, *fl)) };
						// If this is the last char, and it can capture
						if (fl == arg.end() - 1ll && !CaptureIsDisabledOrEqualsOnly(captureStyle) && captures.parsingRules.canCaptureNext(it, args.end()))
							cont.emplace_back(Flag(std::make_pair(*fl, *++it)));
						else {// not last char, or can't capture
							if (CaptureIsRequired(captureStyle))
								throw make_exception("Expected a capture argument for flag '", *fl, "'!");
							cont.emplace_back(Flag(std::make_pair(*fl, std::nullopt)));
						}
					}
					if (capt.has_value()) // flag captures are always at the end, but parsing them first puts them out of chronological order.
						cont.emplace_back(std::move(capt.value()));
					if (invCap.empty())
						break;
					else arg = invCap; // set argument to invalid capture and fallthrough to add it as a parameter
				}
				else // this is a negative number, re-add '-' prefix and fallthrough
					arg = *it;
				[[fallthrough]];
			case 0ull:
			JUMP_TO_PARAMETER:
				[[fallthrough]]; // Parameter
			default:
				cont.emplace_back(Parameter(arg));
				break;
			}
		}
		cont.shrink_to_fit();

		// count the number of each included argument:
		std::unordered_map<std::string, size_t> counts;

		for (const auto& varg : cont) {
			varg.visit([&counts](auto&& value) {
				using T = std::decay_t<decltype(value)>;

				if constexpr (std::same_as<T, Option>)
					++counts[value.name()];
				else if constexpr (std::same_as<T, Flag>)
					++counts[value.name()];
					   });
		}

		// validate argument count limits:
		for (const auto& it : captures.vec) {
			const auto& name{ it.name() };
			if (const auto& count{ counts[name] }; !it.withinRequiredRange(count)) {
				throw make_exception("Argument count ", count, " for '", name, "' is out-of-bounds! Expected (min: ", it.minCount, ", max: ", ([&it]() {
					if (it.maxCount.has_value())
						return std::to_string(it.maxCount.value());
					return "(none)"s;
									 }()), ')');
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
	 * @struct	arg_manager
	 * @brief	Implements the 3rd-generation commandline argument container & parser.
	 *\n		Use this with argc & argv to quickly & easily parse commandline arguments into a container for later use.
	 */
	struct arg_manager : arg_container {
		using base = arg_container;

		/**
		 * @brief					Parsing Constructor.
		 * @param argc				Argument array size from main.
		 * @param argv				Argument array from main.
		 * @param captureArguments	Argument names that should be able to capture. Do not include delimiter prefixes, they will be stripped.
		 */
		template<var::any_same_or_convertible<capture_wrapper, vstring>... TCaptures>
		WINCONSTEXPR arg_manager(const int argc, char** argv, TCaptures&&... captureArguments) : base(parse(vectorize(argc, argv, 1), CaptureList{ capture_wrapper{ std::forward<TCaptures>(captureArguments) }... })) {}

		/**
		 * @brief					Parsing Constructor.
		 * @param argc				Argument array size from main.
		 * @param argv				Argument array from main.
		 * @param ruleset			Used to define additional constraints & default settings for the argument parser.
		 * @param captureArguments	Argument names that should be able to capture. Do not include delimiter prefixes, they will be stripped.
		 */
		template<var::any_same_or_convertible<capture_wrapper, vstring>... TCaptures>
		WINCONSTEXPR arg_manager(const int argc, char** argv, const ArgumentParsingRules& ruleset, TCaptures&&... captureArguments) : base(parse(vectorize(argc, argv, 1), CaptureList{ ruleset, capture_wrapper{ std::forward<TCaptures>(captureArguments) }... })) {}

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
		static arg_manager parse(std::vector<std::string>&& arguments, CaptureList&& captureList)
		{
			return arg_manager{ ::opt3::parse(std::forward<std::vector<std::string>>(arguments), std::forward<CaptureList>(captureList)) };
		}
	};

	using ArgManager = arg_manager;
}
namespace opt3_literals {
	opt3::capture_wrapper operator ""_requireCapture(const char* s, size_t)
	{
		return opt3::requireCapture(s);
	}
	opt3::capture_wrapper operator ""_requireCapture(char c)
	{
		return opt3::requireCapture(c);
	}
	opt3::capture_wrapper operator ""_reqcap(const char* s, size_t)
	{
		return opt3::requireCapture(s);
	}
	opt3::capture_wrapper operator ""_reqcap(char c)
	{
		return opt3::requireCapture(c);
	}
	opt3::capture_wrapper operator ""_optionalCapture(const char* s, size_t)
	{
		return opt3::optionalCapture(s);
	}
	opt3::capture_wrapper operator ""_optionalCapture(char c)
	{
		return opt3::optionalCapture(c);
	}
	opt3::capture_wrapper operator ""_optcap(const char* s, size_t)
	{
		return opt3::optionalCapture(s);
	}
	opt3::capture_wrapper operator ""_optcap(char c)
	{
		return opt3::optionalCapture(c);
	}
	opt3::capture_wrapper operator ""_noCapture(const char* s, size_t)
	{
		return opt3::noCapture(s);
	}
	opt3::capture_wrapper operator ""_noCapture(char c)
	{
		return opt3::noCapture(c);
	}
	opt3::capture_wrapper operator ""_nocap(const char* s, size_t)
	{
		return opt3::noCapture(s);
	}
	opt3::capture_wrapper operator ""_nocap(char c)
	{
		return opt3::noCapture(c);
	}
}

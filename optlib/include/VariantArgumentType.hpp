/**
 * @file VariantArgumentType.hpp
 * @author radj307
 * @brief Contains the VariantArgumentType type definition, as well as the Index struct & the get functions for VariantArgumentType objects along with a little C++ wizardry.
 */
#pragma once
#include <Arg.hpp>
#include <var.hpp>
#include <variant>

namespace opt {
	/// @brief Variant type that allows std::monostate (null/all), Parameter, Option, & Flag types, in that order.
	using VariantArgumentType = std::variant<std::monostate, Parameter, Option, Flag>;

	/**
	 * @struct Index
	 * @brief Constant compile-time variant index object. This allows retrieving types from a variant argument with friendly names rather than raw numbers or types.
	 */
	struct Index {
	private:
		const size_t _index;

	public:
		constexpr Index(size_t index) : _index{ std::move(index) } {}
		constexpr const bool operator==(const Index& o) const { return _index == o._index; }
		constexpr const bool operator==(const size_t& o) const { return _index == o; }
		constexpr const bool operator!=(auto&& o) const { return !operator==(std::forward<decltype(o)>(o)); }
		constexpr operator const size_t() const { return _index; }
		static const Index MONOSTATE, PARAMETER, OPTION, FLAG;
	};
	inline constexpr const Index
		Index::MONOSTATE{ 0ull },
		Index::PARAMETER{ 1ull },
		Index::OPTION{ 2ull },
		Index::FLAG{ 3ull };



	template<ValidArg Type>
	constexpr const Index& get_index() noexcept(false)
	{
		if constexpr (std::same_as<Type, Parameter>)
			return Index::PARAMETER;
		else if constexpr (std::same_as<Type, Option>)
			return Index::OPTION;
		else if constexpr (std::same_as<Type, Flag>)
			return Index::FLAG;
		return Index::MONOSTATE;
	}


	template<ValidArg Type>
	[[nodiscard]] inline static constexpr const bool is_type(const VariantArgumentType& arg)
	{
		return (get_index<Type>() == arg.index());
	}
	template<ValidArg... Type> requires (sizeof...(Type) > 1)
		[[nodiscard]] inline static constexpr const bool is_type(const VariantArgumentType& arg)
	{
		return var::variadic_or(is_type<Type>(arg)...);
	}

	WINCONSTEXPR const std::string get_name(const VariantArgumentType& arg)
	{
		switch (arg.index()) {
		case Index::PARAMETER:
			return get<Parameter>(arg).name();
		case Index::OPTION:
			return get<Option>(arg).name();
		case Index::FLAG:
			return get<Flag>(arg).name();
		case Index::MONOSTATE: [[fallthrough]];
		default:return{};
		}
	}

	/**
	 * @brief Retrieve a std::monostate type from a VariantArgumentType instance.
	 * @tparam IntegerT	- Integer type
	 * @tparam index	- The index number of the type to retrieve.
	 * @param arg		- The argument to retrieve a type from.
	 * @returns std::monostate
	 */
	template<typename IntegerT, IntegerT index> requires (index == static_cast<IntegerT>(0)) && std::integral<IntegerT>
	[[nodiscard]] constexpr auto get(const VariantArgumentType& arg)
	{
		return std::get<std::monostate>(arg);
	}
	/**
	 * @brief Retrieve a Parameter type from a VariantArgumentType instance.
	 * @tparam IntegerT	- Integer type
	 * @tparam index	- The index number of the type to retrieve.
	 * @param arg		- The argument to retrieve a type from.
	 * @returns Parameter
	 */
	template<typename IntegerT, IntegerT index> requires (index == static_cast<IntegerT>(1)) && std::integral<IntegerT>
	[[nodiscard]] constexpr auto get(const VariantArgumentType& arg) noexcept(false)
	{
		return std::get<Parameter>(arg);
	}
	/**
	 * @brief Retrieve an Option type from a VariantArgumentType instance.
	 * @tparam IntegerT	- Integer type
	 * @tparam index	- The index number of the type to retrieve.
	 * @param arg		- The argument to retrieve a type from.
	 * @returns Option
	 */
	template<typename IntegerT, IntegerT index> requires (index == static_cast<IntegerT>(2)) && std::integral<IntegerT>
	[[nodiscard]] constexpr auto get(const VariantArgumentType& arg) noexcept(false)
	{
		return std::get<Option>(arg);
	}
	/**
	 * @brief Retrieve a Flag type from a VariantArgumentType instance.
	 * @tparam IntegerT	- Integer type
	 * @tparam index	- The index number of the type to retrieve.
	 * @param arg		- The argument to retrieve a type from.
	 * @returns Flag
	 */
	template<typename IntegerT, IntegerT index> requires (index == static_cast<IntegerT>(3)) && std::integral<IntegerT>
	[[nodiscard]] constexpr auto get(const VariantArgumentType& arg) noexcept(false)
	{
		return std::get<Flag>(arg);
	}

	template<ValidArg T>
	[[nodiscard]] constexpr auto get(const VariantArgumentType& arg) noexcept(false)
	{
		return std::get<T>(arg);
	}

	/**
	 * @brief VariantArgumentType output stream insertion operator.
	 * @param os	- (implicit) Output Stream Reference.
	 * @param varg	- (implicit) VariantArgumentType instance.
	 * @returns std::ostream&
	 */
	inline std::ostream& operator<<(std::ostream& os, const VariantArgumentType& varg)
	{
		switch (varg.index()) {
		case Index::PARAMETER: // Parameter
			os << get<Index::PARAMETER>(varg);
			break;
		case Index::OPTION: // Option
			os << get<Index::OPTION>(varg);
			break;
		case Index::FLAG: // Flag
			os << std::get<Index::FLAG>(varg);
			break;
		case Index::MONOSTATE: [[fallthrough]]; // std::monostate
		default:break;
		}
		return os;
	}
}
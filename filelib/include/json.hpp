#pragma once
#include <TokenRedux.hpp>
#include <var.hpp>

#include <variant>
#include <concepts>

namespace json {
	template<std::integral IntType, std::floating_point RealType>
	struct basic_number {
		using this_t = basic_number<IntType, RealType>;
		using Int = IntType;
		using Real = RealType;
		using Variant = std::variant<Int, Real>;

		Variant value;

		template<var::numeric T>
		constexpr basic_number(T&& n) : value{ std::move(n) } {}
		template<var::numeric T>
		constexpr basic_number(T const& n) : value{ n } {}

		constexpr bool isIntegral() const { return value.index() == 0; }
		constexpr bool isReal() const { return value.index() == 1; }

		this_t& operator=(const this_t& o)
		{
			value = o.value;
			return *this;
		}
		template<std::floating_point T>
		this_t& operator=(const T& v)
		{
			value = static_cast<Real>(v);
			return *this;
		}
		template<std::integral T>
		this_t& operator=(const T& v)
		{
			value = static_cast<Int>(v);
			return *this;
		}

		template<typename ReturnT>
		constexpr this_t operation(const this_t& o, const std::function<ReturnT()>&) const
		{

		}

		constexpr this_t operator+(const this_t& o) const
		{
			std::visit([](auto&& l, auto&& r) {
				using Tl = std::decay_t<decltype(l)>;
				using Tr = std::decay_t<decltype(r)>;


			}, *this, o);
		}

		constexpr operator Variant() const noexcept { return value; }
		constexpr explicit operator Int() const noexcept(false) { return std::get<Int>(value); }
		constexpr explicit operator Real() const noexcept(false) { return std::get<Real>(value); }

		friend std::ostream& operator<<(std::ostream& os, const this_t& n)
		{
			if (auto i = std::get_if<Int>(&n.value))
				os << *i;
			else if (auto r = std::get_if<Real>(&n.value))
				os << *r;
			return os;
		}
	};

	using number32 = basic_number<int32_t, float>;
	using number64 = basic_number<int64_t, double>;

#	ifdef JSON_64
	using number = number64;
#	else
	using number = number32;
#	endif

	using flag_t = unsigned char;
	enum class ValueType : flag_t {
		Null = 0,
		Number,
		String,
		Boolean,
		Array,
		Object,
	};

	using key_t = std::string;

	using number_t = number;
	using string_t = const char*;
	using boolean_t = bool;
	template<typename T>
	using basic_array_t = std::vector<T>;
	template<typename T>
	using basic_object_t = std::map<key_t, T>;

	struct value_t {
		ValueType type;

		value_t() {}
	};

	struct kvpair {
		key_t key;
		value_t value;

		ValueType getValueType() const { return value.type; }
	};
}

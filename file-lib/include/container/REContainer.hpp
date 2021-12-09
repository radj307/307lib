#pragma once
#include <TokenizedContainer.hpp>
#include <TokenizerRE.hpp>
#include <variant>

namespace token::parse {

	struct REElement {
		using ValueT = std::string;
		using SectionT = std::vector<REElement>;
		using VariantT = std::variant<std::monostate, ValueT, SectionT>;

		enum class Type : unsigned char {
			NULL_TYPE = 0u,
			VALUE = 1u,
			SECTION = 2u,
		};
		using enum Type;

		const std::string name;
		const VariantT value;
		const Type type;

		constexpr REElement(const std::string& name, const ValueT& val) : name{ name }, value{ val }, type{ VALUE } {}
		constexpr REElement(const std::string& name, const SectionT& sec) : name{ name }, value{ sec }, type{ SECTION } {}

		friend std::ostream& operator<<(std::ostream& os, const REElement& elem)
		{
			switch (elem.type) {
			case VALUE:
				os << std::get<ValueT>(elem.value);
				break;
			case SECTION:
				for (auto& it : std::get<SectionT>(elem.value))
					os << it << '\n';
				break;
			default:break;
			}
			return os;
		}

		template<typename T> requires std::same_as<T, ValueT> || std::same_as<T, SectionT>
		constexpr const T get_value() const noexcept(false) { return std::get<T>(value); }
		template<typename T> requires std::same_as<T, ValueT> || std::same_as<T, SectionT>
		constexpr const T get_value_or(const T & rv) const noexcept { if (auto v = std::get_if<T>(&value))return *v; return rv; }
	};

	struct REContainer {
		using ContainerType = std::vector<REElement>;
		const ContainerType cont;

		constexpr REContainer(const ContainerType& cont) : cont{ cont } {}
	};
}
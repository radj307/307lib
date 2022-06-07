#include "../include/json/Node.h"

#include <str.hpp>

#include <regex>

using namespace json;

static const std::basic_regex regex_unescaped_quotes("([^\\\\]\")", std::regex_constants::optimize);

/// @brief	Inserts backslashes to escape quotation marks that aren't already escaped in the given string.
std::string escape_string(string_t const& s)
{
	// ((?<!\\\\)\")  //< negative lookbehind isn't supported by std::regex
	// ([^\\\\]\")
	return std::regex_replace(s, regex_unescaped_quotes, "\\$1", std::regex_constants::match_any);
}

std::ostream& json::operator<<(std::ostream& os, const std::pair<string_t, Node>& nodepr)
{
	return os << '"' << escape_string(nodepr.first) << "\": " << nodepr.second;
}
std::ostream& json::operator<<(std::ostream& os, const Node& n)
{
	return std::visit([&os](auto&& value) -> std::ostream& {
		using T = std::decay_t<decltype(value)>;

		if constexpr (std::same_as<T, null_t>)
			os << "null";
		else if constexpr (std::same_as<T, boolean_t>)
			os << str::bool_to_string(value);
		else if constexpr (std::same_as<T, integral_t>)
			os << value;
		else if constexpr (std::same_as<T, real_t>)
			os << str::stringify(std::fixed, value);
		else if constexpr (std::same_as<T, string_t>)
			os << '"' << escape_string(value) << '"';
		else if constexpr (std::same_as<T, array_t>) {
			bool fst{ true };
			os << "[";
			for (const auto& it : value) {
				if (!fst) os << ',';
				else fst = false;
				os << it; //< recurse
			}
			os << "]";
		}
		else if constexpr (std::same_as<T, object_t>) {
			bool fst{ true };
			os << '{';
			for (const auto& pr : value) {
				if (!fst) os << ',';
				else fst = false;
				os << pr;
			}
			os << '}';
		}
		//else static_assert(false, "Visitor lambda does not handle all potential types!");

		return os;
	}, n.value);
}
std::ostream& json::operator<<(std::ostream& os, const NodeType& type)
{
	switch (type._value) {
	case (uchar)NodeType::Null:
		os << "null";
		break;
	case (uchar)NodeType::Boolean:
		os << "bool";
		break;
	case (uchar)NodeType::Number:
		os << "number";
		break;
	case (uchar)NodeType::String:
		os << "string";
		break;
	case (uchar)NodeType::Array:
		os << "array";
		break;
	case (uchar)NodeType::Object:
		os << "object";
		break;
	default:
		os << "undefined";
		break;
	}
	return os;
}

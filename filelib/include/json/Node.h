#pragma once
/**
 * @file	node.h
 * @author	radj307
 * @brief	Contains objects related to JSON nodes, which are singular components of a JSON-formatted file.
 */
#include <sysarch.h>
#include <var.hpp>
#include <str.hpp>
#include <make_exception.hpp>

#include <string>
#include <ostream>
#include <typeinfo>
#include <variant>
#include <vector>
#ifdef OS_WIN
#include <unordered_map>
#elif defined(OS_LINUX)
#include <map>
#endif


 /**
  * @namespace	json
  * @brief		C++ JSON Library namespace.
  */
namespace json {
	/// @brief	Convenience typedef for an unsigned char.
	using uchar = unsigned char;

	struct Node; //< forward declaration of Node
	struct NodeType;

	/// @brief	JSON type used to represent null values.
	using null_t = std::monostate;
	/// @brief	JSON type used to hold boolean values.
	using boolean_t = bool;
	/// @brief	JSON type used to hold integral numbers. This forms the JSON Number type along with real_t.
	using integral_t = long long;
	/// @brief	JSON type used to hold floating-point numbers. This forms the JSON Number type along with integral_t.
	using real_t = long double;
	/// @brief	JSON type used to hold strings; both as values and as keys.
	using string_t = std::string;
	/// @brief	JSON type used to hold nodes in an array.
	using array_t = std::vector<Node>;
	/// @brief	JSON type used to hold the key-node pairs in an object.
#	ifdef OS_WIN
	using object_t = std::unordered_map<string_t, Node>;
#	elif defined(OS_LINUX)
	using object_t = std::map<string_t, Node>;
#	endif
	/// @brief	A variant capable of holding any JSON type.
	using variant_t = std::variant<null_t, boolean_t, integral_t, real_t, string_t, array_t, object_t>; //< The ordering of the types in this variant must be preserved!

	/// @brief	A static constant null_t implementation for use as a JSON literal.
	inline static constexpr null_t null;

	template<typename T> concept json_raw_type = var::any_same_or_convertible<T, null_t, boolean_t, integral_t, real_t, string_t>;
	template<typename T> concept json_container_type = var::any_same_or_convertible<T, array_t, object_t>;
	template<typename T> concept json_type = json_raw_type<T> || json_container_type<T>;

	template<json_type T> constexpr NodeType GetNodeTypeFrom() noexcept;

	constexpr NodeType GetNodeTypeFrom(const size_t&) noexcept;

	/**
	 * @struct	NodeType
	 * @brief	Pseudo-enum struct whose value is used to determine the type contained within a Node.
	 *\n		The values of this enum correspond to the variant::index used to store the types.
	 */
	struct NodeType {
		uchar _value;

		/**
		 * @brief		Constructs a new NodeType object from a copied integral.
		 * @tparam T	Any integral type; this is automatically converted to uchar.
		 * @param value	The integral value to set this NodeType's _value to.
		 */
		template<std::integral T>
		constexpr NodeType(T const& value) : _value{ static_cast<uchar>(value) } {}
		/**
		 * @brief		Constructs a new NodeType object from a moved integral.
		 * @tparam T	Any integral type; this is automatically converted to uchar.
		 * @param value	The integral value to set this NodeType's _value to.
		 */
		template<std::integral T>
		constexpr NodeType(T&& value) noexcept : _value{ std::move(static_cast<uchar>(std::move(value))) } {}

		/**
		 * @brief		Converts from a given type that meets the json_type constraint to a NodeType.
		 * @tparam T	The type to convert from.
		 * @returns		NodeType object determined by type T.
		 */
		template<json_type T>
		constexpr static NodeType from_type() { return GetNodeTypeFrom<T>(); }

		/**
		 * @brief			Converts from the result of variant_t::index() to a NodeType.
		 * @param index		The variant index to convert from.
		 * @returns			NodeType object determined by the variant index.
		 */
		constexpr static NodeType from_index(size_t const& index) { return GetNodeTypeFrom(index); }

		// operators:

		constexpr operator size_t() const& { return static_cast<size_t>(_value); }
		explicit constexpr operator uchar() const& { return static_cast<size_t>(_value); }

		constexpr bool operator==(NodeType&& o) const& { return _value == o._value; }
		constexpr bool operator!=(NodeType&& o) const& { return _value != o._value; }

		static NodeType const Null, Boolean, Number, String, Array, Object, Undefined;
	};

#	pragma region NodeTypeDefs
	/// @brief	Null type; doesn't have any value.
	inline constexpr const NodeType json::NodeType::Null{ 0 };
	/// @brief	Boolean type; contains a 1 or 0.
	inline constexpr const NodeType json::NodeType::Boolean{ 1 };
	/// @brief	Number type; contains any numeric value.
	inline constexpr const NodeType json::NodeType::Number{ 2 };
	/// @brief	String type; contains any string value.
	inline constexpr const NodeType json::NodeType::String{ 3 };
	/// @brief	Array type; contains a variable-length array of Nodes.
	inline constexpr const NodeType json::NodeType::Array{ 4 };
	/// @brief	Object type; contains a list of key-value pairs where the key type is always a string and the value type is always a Node.
	inline constexpr const NodeType json::NodeType::Object{ 5 };
	/// @brief	Undefined type; indicates an error.
	inline constexpr const NodeType json::NodeType::Undefined{ 6 };
#	pragma endregion NodeTypeDefs

	/// @brief	Stream insertion operator for the NodeType struct.\nThis inserts the typename associated with the NodeType.
	std::ostream& operator<<(std::ostream&, const NodeType&);

	/**
	 * @struct	Node
	 * @brief	A singular JSON Node capable of containing any valid type.
	 *\n		Nodes are containers that store a single value.
	 *\n		For nodes that expose STL methods for specific node types, see the NodeView family.
	 */
	struct Node {
		variant_t value;
		NodeType type;

	public:
		/// @brief	Null value constructor.
		Node() : value{ null }, type{ NodeType::Null } {}
		/**
		 * @brief		Value constructor.
		 * @param value	The value to assign to this node.
		 */
		Node(variant_t&& value) noexcept : value{ std::move(value) }, type{ GetNodeTypeFrom(this->value.index()) } {}

		template<json_type T>
		[[nodiscard]] constexpr bool is_type() { return GetNodeTypeFrom<T>() == type; }
		[[nodiscard]] constexpr bool is_null() { return is_type<null_t>(); }
		[[nodiscard]] constexpr bool is_boolean() { return is_type<boolean_t>(); }
		[[nodiscard]] constexpr bool is_number() { return is_type<integral_t>() || is_type<real_t>(); }
		[[nodiscard]] constexpr bool is_array() { return is_type<array_t>(); }
		[[nodiscard]] constexpr bool is_object() { return is_type<object_t>(); }

		template<json_type T>
		Node& operator=(T&& v) noexcept
		{
			value = variant_t{ std::move(v) };
			type = GetNodeTypeFrom<T>();
			return *this;
		}
		template<json_type T>
		Node& operator=(T const& v) noexcept
		{
			value = variant_t{ v };
			type = GetNodeTypeFrom<T>();
			return *this;
		}
		Node& operator=(variant_t const& v) noexcept
		{
			value = v;
			type = GetNodeTypeFrom(value.index());
			return *this;
		}
		Node& operator=(variant_t&& v) noexcept
		{
			value = std::move(v);
			type = GetNodeTypeFrom(value.index());
			return *this;
		}
	};

	/// @brief	Stream insertion operator for key-Node pairs.
	std::ostream& operator<<(std::ostream&, const std::pair<string_t, Node>&);
	/// @brief	Stream insertion operator for the Node type.
	std::ostream& operator<<(std::ostream&, const Node&);

	template<json_type T>
	constexpr NodeType GetNodeTypeFrom() noexcept
	{
		if constexpr (std::same_as<T, boolean_t>)
			return NodeType::Boolean;
		if constexpr (var::any_same<T, integral_t, real_t>)
			return NodeType::Number;
		if constexpr (std::same_as<T, string_t>)
			return NodeType::String;
		if constexpr (std::same_as<T, array_t>)
			return NodeType::Array;
		if constexpr (std::same_as<T, object_t>)
			return NodeType::Object;
		return NodeType::Null;
	}
	constexpr NodeType GetNodeTypeFrom(const size_t& index) noexcept
	{
		switch (index) {
		case 0:
			return NodeType::Null;
		case 1:
			return NodeType::Boolean;
		case 2: [[fallthrough]];
		case 3:
			return NodeType::Number;
		case 4:
			return NodeType::String;
		case 5:
			return NodeType::Array;
		case 6:
			return NodeType::Object;
		default:
			return NodeType::Undefined;
		}
	}
}

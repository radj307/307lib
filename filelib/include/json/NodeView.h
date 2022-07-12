#pragma once
#include "Node.h"

namespace json {
	/**
	 * @struct		NodeView
	 * @tparam T	A type satisfying the json_type constraint to create the NodeView object for.
	 * @brief		A constant wrapper around the Node type that allows the usage of more specialized methods for interacting with raw nodes.
	 */
	template<json_type T>
	struct NodeView {
	protected:
		static void validate_node_type(NodeType const& node_type);

		mutable Node node;
	public:
		NodeView(const Node& n) : node{ n } { validate_node_type(node.type); }
		NodeView(Node&& n) : node{ std::move(n) } { validate_node_type(node.type); }

		NodeView(const T& value) : node{ Node{ value } } { validate_node_type(node.type); }
		NodeView(T&& value) : node{ Node{ std::move(value) } } { validate_node_type(node.type); }

		[[nodiscard]] constexpr operator Node() const& { return node; }
		[[nodiscard]] constexpr operator Node& () { return node; }
		[[nodiscard]] constexpr operator T() const& { return std::get<T>(node.value); }
	};

	/**
	 * @struct		ContainerNode
	 * @tparam T	Any JSON container type.
	 * @brief		Abstract NodeView derivative that implements certain generic STL container methods.
	 */
	template<json_container_type T>
	struct ContainerNode : public NodeView<T> {
		using base = NodeView<T>;
		using base::base;
		/// @brief	Virtual Destructor; this is an abstract object.
		virtual ~ContainerNode() = default;

		[[nodiscard]] constexpr bool empty() const { return std::get<T>(this->node.value).empty(); }
		[[nodiscard]] constexpr size_t size() const { return std::get<T>(this->node.value).size(); }
		[[nodiscard]] constexpr auto at(const size_t& index) const { return std::get<T>(this->node.value).at(index); }

		[[nodiscard]] constexpr auto begin() const { return std::get<T>(this->node.value).begin(); }
		[[nodiscard]] constexpr auto rbegin() const requires std::same_as<T, array_t> { return std::get<T>(this->node.value).rbegin(); }

		[[nodiscard]] constexpr auto end() const { return std::get<T>(this->node.value).end(); }
		[[nodiscard]] constexpr auto rend() const requires std::same_as<T, array_t> { return std::get<T>(this->node.value).end(); }
	};

	/**
	 * @struct	ObjectNode
	 * @brief	Specialization of the ContainerNode struct for object_t.
	 */
	struct ObjectNode : ContainerNode<object_t> {
		using base = ContainerNode<object_t>;
		using base::base;

		/// @brief	Empty Object Constructor.
		ObjectNode() : base(object_t{}) {}

		auto insert(std::pair<string_t, Node>&& pair) { return std::get<object_t>(node.value).insert(std::move(pair)); }
		template<json_type T> auto insert(std::pair<string_t, T>&& pair) { return std::get<object_t>(node.value).insert(std::make_pair(pair.first, Node{ pair.second })); }
		auto insert_or_assign(string_t&& key, Node&& value) { return std::get<object_t>(node.value).insert_or_assign(std::move(key), std::move(value)); }
		template<json_type T> auto insert_or_assign(string_t&& key, T&& value) { return std::get<object_t>(node.value).insert_or_assign(std::move(key), Node{ std::move(value) }); }

		/**
		 * @brief		Get or add a key in the object.
		 * @param key	The keyname of the target Node.
		 *\n			If there is no key with the specified name in the object, a new key is created with the default Node value of null.
		 * @returns		A reference to the specified Node.
		 */
		Node& get(const string_t& key)& { return{ std::get<object_t>(node.value)[key] }; }
		/**
		 * @brief		Get or add a key in the object.
		 * @param key	The keyname of the target Node.
		 *\n			If there is no key with the specified name in the object, a new key is created with the default Node value of null.
		 * @returns		A reference to the specified Node.
		 */
		Node& get(string_t&& key)& { return{ std::get<object_t>(node.value)[std::forward<string_t>(key)] }; }

		/**
		 * @brief		Indexing operator that gets or adds a key in the object map.
		 * @param key	The keyname of the target Node.
		 *\n			If there is no key with the specified name in the object, a new key is created with the default Node value of null.
		 * @returns		A reference to the specified Node.
		 */
		Node& operator[](const string_t& key)& { return this->get(key); }
		/**
		 * @brief		Indexing operator that gets or adds a key in the object map.
		 * @param key	The keyname of the target Node.
		 *\n			If there is no key with the specified name in the object, a new key is created with the default Node value of null.
		 * @returns		A reference to the specified Node.
		 */
		Node& operator[](string_t&& key)& { return this->get(std::forward<string_t>(key)); }

		/**
		 * @brief		Gets or adds a Node in the object with the specified type.
		 * @tparam T	The value type of the target Node.
		 * @param key	The keyname of the target Node.
		 *\n			If there is no key with the specified name in the object, a new key is created with the default Node value of null.
		 * @throws		std::bad_variant_access	The specified Node isn't of the requested type.
		 * @returns		A NodeView struct with the specified type.
		 */
		template<json_type T> NodeView<T> get_as(const string_t& key) const
		{
			auto obj{ std::get<object_t>(node.value) };
			if (obj.contains(key))
				return{ std::get<T>(obj[key].value) };
			else {
				return obj[key] = T{};
			}
		}
		/**
		 * @brief		Gets or adds a Node in the object with the specified type.
		 * @tparam T	The value type of the target Node.
		 * @param key	The keyname of the target Node.
		 *\n			If there is no key with the specified name in the object, a new key is created with the default Node value of null.
		 * @throws		std::bad_variant_access	The specified Node isn't of the requested type.
		 * @returns		A NodeView struct with the specified type.
		 */
		template<json_type T> NodeView<T> get_as(string_t&& key) const
		{
			return{ std::get<T>(std::get<object_t>(node.value)[std::forward<string_t>(key)].value) };
		}
	};
}

#pragma once
// 307lib::TermAPI
#include "LineCharacter.hpp"
#include "term.hpp"

// 307lib::shared
#include <var.hpp>

// STL
#include <vector>
#include <stack>
#include <sstream>

namespace term {
	/**
	 * @brief				Functor that pretty-prints a tree data structure using line drawing
	 *						 characters to an output stream.
	 * @tparam T		  -	Tree node type of the data structure to print.
	 */
	template<typename T>
	class basic_tree_printer {
	protected:
		T root_object;
		/// @brief	The size of a single indentation; or, the width of 1 layer in the tree.
		std::uint16_t indent_sz;

		/// @brief		Gets the child nodes to print from the specified object.
		/// @returns	A std::vector of elements of the same type as the specified object.
		virtual std::vector<T> select_child_nodes(T const& object) const = 0;
		/// @brief	Prints the node using special handling for type T.
		virtual std::ostream& print_node(std::ostream& os, T const& node) const = 0;

		constexpr basic_tree_printer(T const& root_object, std::uint16_t const& indent_sz = 2u) :
			root_object{ root_object },
			indent_sz{ indent_sz }
		{}

	public:
		virtual ~basic_tree_printer() = default;

		friend std::ostream& operator<<(std::ostream& os, basic_tree_printer<T> const& p)
		{
			// create a stack to enumerate the tree depth-first
			std::stack<std::tuple<std::string, T, size_t, size_t>> stack;

			// push the root node onto the stack
			stack.push(std::make_tuple("", p.root_object, 0ull, 0ull));

			while (!stack.empty()) {
				// take the next node off of the stack
				auto [prefix, node, depth, linePositions] { stack.top() };
				stack.pop();

				// print this node with the previously-calculated prefix
				os << prefix;
				p.print_node(os, node);
				os << '\n';

				//< nothing is printed to (os) beyond this line

				if (const auto& children{ p.select_child_nodes(node) }; !children.empty()) {
					// enumerate this node's children & create a string that contains any
					//  prefix box characters to maintain the tree structure's appearance
					auto indentSize{ depth * p.indent_size };

					for (auto it{ children.rbegin() }, it_end{ children.rend() }, it_last{ it }; it != it_end; ++it) {
						std::stringstream ss;

						// enable DEC line drawing characters
						ss << term::EnableLineDrawing;

						// print indentation
						if (indentSize > 0) {
							for (auto i{ 0 }; i < indentSize; i += p.indent_size) {
								if ((linePositions & (1ull << (i / p.indent_size))) != 0) {
									// a vertical line for a previously-printed node is required here
									ss << term::LineCharacter::LINE_VERTICAL;
								}
								else ss << ' ';
								// fill in the rest of the indentation
								if (p.indent_size > 1)
									ss << indent(p.indent_size - 1);
							}
						}

						// print tree structure line drawings for this child node
						if (it == it_last) {
							// this is the last of this node's children to be printed:
							ss << term::LineCharacter::CORNER_BOTTOM_LEFT;
						}
						else {
							// there are still more of this node's children to print:
							ss << term::LineCharacter::JUNCTION_3_WAY_LEFT;
							// make sure any sub-nodes of this child print a vertical bar at this
							//  node's depth in order to maintain the tree structure's appearance:
							linePositions |= (1ull << depth);
						}
						// print horizontal lines until we reach the value position
						for (std::uint16_t i{ 0 }, i_max{ p.indent_size - 1 }; i < i_max; ++i) {
							ss << term::LineCharacter::LINE_HORIZONTAL;
						}

						// disable DEC line drawing characters
						ss << term::DisableLineDrawing;

						stack.push(std::make_tuple(ss.str(), *it, depth + 1, linePositions));
					}
				}
			}

			return os;
		}
	};

	/**
	 * @brief					Prints a nicely-formatted tree
	 * @tparam Node			  -	Type of the tree node used to represent the data structure.
	 * @tparam ChildSelector  -	Type of function that selects the child nodes for an arbitrary node.
	 * @tparam ValueSelector  -	Type of function that selects the value to print for an arbitrary node.
	 */
	template<
		typename Node,
		class ChildSelector,
		class ValueSelector
	> class tree_printer : public basic_tree_printer<Node> {
		ChildSelector child_selector;
		ValueSelector value_selector;

	protected:
		std::vector<Node> select_child_nodes(Node const& object) const override
		{
			return child_selector(object);
		}
		std::ostream& print_node(std::ostream& os, Node const& node) const override
		{
			return os << value_selector(node);
		}

	public:
		constexpr tree_printer(Node const& root, ChildSelector const& childSelector, ValueSelector const& valueSelector) :
			basic_tree_printer<Node>(root),
			child_selector{ childSelector },
			value_selector{ valueSelector }
		{}
		constexpr tree_printer(std::uint16_t const indentationLength, Node const& root, ChildSelector const& childSelector, ValueSelector const& valueSelector) :
			basic_tree_printer<Node>(root, indentationLength),
			child_selector{ childSelector },
			value_selector{ valueSelector }
		{}
	};

	template<var::streamable T>
		requires var::enumerable_of<decltype(std::declval<T>().children), T>
	inline auto print_tree(T const& rootNode, std::uint16_t const indentationLength = 2)
	{
		auto childSelector = [](auto const& node) -> std::vector<T> {
			return node.children;
		};

		auto valueSelector = [](auto const& node) -> T {
			return node;
		};

		return tree_printer<T, decltype(childSelector), decltype(valueSelector)>{ rootNode, childSelector, valueSelector };
	}
	template<typename T, class ChildSelector>
	inline auto print_tree(T const& rootNode, ChildSelector const& childSelector, std::uint16_t const indentationLength = 2)
	{
		auto valueSelector = [](auto const& node) -> T {
			return node;
		};

		return tree_printer<T, ChildSelector, decltype(valueSelector)>{ rootNode, childSelector, valueSelector };
	}
	template<typename T, class ChildSelector, class ValueSelector>
	inline auto print_tree(T const& rootNode, ChildSelector const& childSelector, ValueSelector const& valueSelector, std::uint16_t const indentationLength = 2)
	{
		return tree_printer<T, ChildSelector, ValueSelector>{ rootNode, childSelector, valueSelector };
	}

	/// @brief		Deprecated tree printing functors
	/// @deprecated	Deprecated tree printing functors
	namespace legacy {
		/**
		 * @brief					An ostream functor that prints the specified object as a
		 *							 nicely-formatted tree using DEC line drawing characters,
		 *							 without using recursion.
		 *							See the constructor for more information.
		 * @tparam T			  - The type of object to print. It must have an operator<<
		 *							 for printing to a std::ostream.
		 * @tparam INDENT_LENGTH  - The length of the indentation for each subnode, in characters.
		 */
		template<var::streamable T, std::uint16_t INDENT_LENGTH = 2>
		class print_tree {
			T root;
			std::function<std::vector<T>(T)> child_selector;

		public:
			/**
				* @brief					Creates a new print_recursive instance for the specified rootObject.
				* @param rootObject	  -	The root node of the tree.
				* @param childSelector	  - A function that gets a node's children and returns them as a vector.
				*/
			constexpr print_tree(T&& rootObject, const std::function<std::vector<T>(T)>& childSelector) : root{ std::forward<T>(rootObject) }, child_selector{ childSelector } {}
			/**
				* @brief					Creates a new print_recursive instance for the specified rootObject.
				* @param rootObject	  -	The root node of the tree.
				* @param childSelector	  - A function that gets a node's children and returns them as a vector.
				*/
			constexpr print_tree(T const& rootObject, const std::function<std::vector<T>(T)>& childSelector) : root{ rootObject }, child_selector{ childSelector } {}

			/// @brief	Prints the root object as a tree.
			friend std::ostream& operator<<(std::ostream& os, const print_tree<T, INDENT_LENGTH>& p)
			{
				// create a stack to enumerate the tree depth-first
				std::stack<std::tuple<std::string, T, size_t, size_t>> stack;
				stack.push(std::make_tuple("", p.root, 0ull, 0ull));

				while (!stack.empty()) {
					// get the top item off of the stack
					auto [prefix, current, depth, linePositions] { stack.top() };
					stack.pop();

					os << prefix << current << '\n';

					if (const auto& children{ p.child_selector(current) }; !children.empty()) {
						auto indentSize{ depth * INDENT_LENGTH };

						for (auto it{ children.rbegin() }, it_end{ children.rend() }, it_last{ it }; it != it_end; ++it) {
							std::stringstream ss;

							// print indentation
							if (indentSize > 0) {
								for (auto i{ 0 }; i < indentSize; i += INDENT_LENGTH) {
									if ((linePositions & (1ull << (i / INDENT_LENGTH))) != 0) {
										// a vertical line for a previously-printed node is required here
										ss << term::EnableLineDrawing << term::LineCharacter::LINE_VERTICAL << term::DisableLineDrawing;
									}
									else ss << ' ';
									// fill in the rest of the indentation
									if (INDENT_LENGTH > 1)
										ss << indent(INDENT_LENGTH - 1);
								}
							}

							auto vertLines{ linePositions };
							// print tree structure line drawings
							ss << term::EnableLineDrawing;
							if (it == it_last) {
								// this is the last child
								ss << term::LineCharacter::CORNER_BOTTOM_LEFT;
							}
							else {
								// there are still more children
								ss << term::LineCharacter::JUNCTION_3_WAY_LEFT;
								vertLines |= (1ull << depth); //< sub-nodes must print a vertical line here
							}
							// print horizontal lines until we reach the value position
							for (std::uint16_t i{ 0 }, i_max{ INDENT_LENGTH - 1 }; i < i_max; ++i) {
								ss << term::LineCharacter::LINE_HORIZONTAL;
							}
							ss << term::DisableLineDrawing;

							stack.push(std::make_tuple(ss.str(), *it, depth + 1, vertLines));
						}
					}
				}

				return os;
			}
		};
	}
}

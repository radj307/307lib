#pragma once
#include "LineCharacter.hpp"
#include "term.hpp"

#include <var.hpp>

#include <vector>
#include <stack>
#include <sstream>

/**
 * @brief					Printer object that prints the specified object as a formatted
 *							 tree using DEC line drawing characters, without using recursion.
 *							See the constructor for more information.
 * @tparam T			  - The type of object to print. It must have an operator<<
 *							 for printing to a std::ostream.
 * @tparam INDENT_LENGTH  - The length of the indentation for each subnode, in characters.
 */
template<var::streamable T, std::uint16_t INDENT_LENGTH = 2> requires (INDENT_LENGTH >= 1)
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

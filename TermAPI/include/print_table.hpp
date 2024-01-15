#pragma once
// 307lib::TermAPI
#include "LineCharacter.hpp"	//< for term::LineCharacter
#include "term.hpp"				//< for enable/disable line drawing sequence

// 307lib::shared
#include <indentor.hpp>			//< for shared::indent
#include <make_exception.hpp>	//< for ex::make_exception
#include <var.hpp>				//< for var::iterator_of concept

// STL
#include <vector>		//< for std::vector
#include <string>		//< for std::string
#include <ostream>		//< for std::ostream
#include <functional>	//< for std::function
#include <type_traits>	//< for std::declval

namespace term {

	enum class HorizontalAlignment : std::uint8_t {
		/// @brief	Aligns to the left of the space.
		Left,
		/// @brief	Aligns to the center of the space.
		Center,
		/// @brief	Aligns to the right of the space.
		Right,
	};

	/**
	 * @brief			Pretty-prints a range of items as a table using line drawing characters.
	 * @tparam Iter	  -	The type of iterator to use.
	 * @tparam T	  -	The type of data in the table.
	 */
	template<class Iter, typename T = std::decay_t<decltype(*std::declval<Iter>())>>
	struct print_table {
	private:
		using this_t = print_table<Iter, T>;

		/// @brief	Gets the minimum required widths of each column in the table.
		std::vector<size_t> getColumnWidths() const
		{
			std::vector<size_t> maxWidths;
			maxWidths.reserve(column_defs.size());

			// use the size of the headers as the default
			for (const auto& col : column_defs) {
				maxWidths.emplace_back(col.header.size() + padding * 2);
			}

			// find the longest item and use that width for each column
			for (auto it{ begin }; it != end; ++it) {
				for (size_t i{ 0 }, i_max{ column_defs.size() }; i < i_max; ++i) {
					const auto minWidth{ column_defs[i].getMinWidthForItem(*it, padding) };

					if (auto& currentMax{ maxWidths[i] }; minWidth > currentMax)
						currentMax = minWidth;
				}
			}

			return maxWidths;
		}

	public:
		/// @brief	A column definition that consists of a header and a function that gets the string to print in the column for an item.
		struct table_column {
			/// @brief	A function that returns a std::string for an item of type T.
			using selector_t = std::function<std::string(T)>;

			/// @brief	The text displayed at the top of the column.
			std::string header;
			/// @brief	The alignment of the header string.
			HorizontalAlignment header_alignment;
			/// @brief	Function that selects a std::string from an item of type T.
			selector_t item_selector;
			/// @brief	The alignment of items in the column.
			HorizontalAlignment item_alignment;

			/**
			 * @brief					Creates a new table_column instance with the specified aligned header, item selector function, and item alignment.
			 * @param header		  -	The header text for this column.
			 * @param headerAlignment -	The alignment of the header text for this column.
			 * @param itemSelector	  -	A function that selects string to display in this column from an item of type T.
			 * @param itemAlignment	  -	The alignment of the item text for this column.
			 */
			constexpr table_column(std::string const& header, HorizontalAlignment const headerAlignment, selector_t const& itemSelector, HorizontalAlignment const itemAlignment) :
				header{ header },
				header_alignment{ headerAlignment },
				item_selector{ itemSelector },
				item_alignment{ itemAlignment }
			{}
			/**
			 * @brief					Creates a new table_column instance with the specified aligned header, item selector function, and left item alignment.
			 * @param header		  -	The header text for this column.
			 * @param headerAlignment -	The alignment of the header text for this column.
			 * @param itemSelector	  -	A function that selects string to display in this column from an item of type T.
			 */
			constexpr table_column(std::string const& header, HorizontalAlignment const headerAlignment, selector_t const& itemSelector) :
				header{ header },
				header_alignment{ headerAlignment },
				item_selector{ itemSelector },
				item_alignment{ HorizontalAlignment::Left }
			{}
			/**
			 * @brief					Creates a new table_column instance with the specified left-aligned header, item selector function, and item alignment.
			 * @param header		  -	The header text for this column.
			 * @param itemSelector	  -	A function that selects string to display in this column from an item of type T.
			 * @param itemAlignment	  -	The alignment of the item text for this column.
			 */
			constexpr table_column(std::string const& header, selector_t const& itemSelector, HorizontalAlignment const itemAlignment) :
				header{ header },
				header_alignment{ HorizontalAlignment::Left },
				item_selector{ itemSelector },
				item_alignment{ itemAlignment }
			{}
			/**
			 * @brief					Creates a new table_column instance with the specified left-aligned header, item selector function, and left item alignment.
			 * @param header		  -	The header text for this column.
			 * @param itemSelector	  -	A function that selects string to display in this column from an item of type T.
			 */
			constexpr table_column(std::string const& header, selector_t const& itemSelector) :
				header{ header },
				header_alignment{ HorizontalAlignment::Left },
				item_selector{ itemSelector },
				item_alignment{ HorizontalAlignment::Left }
			{}

			size_t getMinWidthForItem(T const& dataItem, unsigned const padding) const
			{
				return item_selector(dataItem).size() + padding * 2;
			}

			void write_header_to(std::ostream& os, size_t const width, unsigned const padding) const
			{
				write_to(os, header, header_alignment, width, padding);
			}
			void write_item_to(std::ostream& os, T const& dataItem, size_t const width, unsigned const padding) const
			{
				write_to(os, item_selector(dataItem), item_alignment, width, padding);
			}

		private:
			static constexpr void write_to(std::ostream& os, std::string const& str, HorizontalAlignment const alignment, unsigned const width, unsigned const padding)
			{
				switch (alignment) {
				case HorizontalAlignment::Left:
					os << indent(padding) << str << indent(padding) << indent(width, str.size() + padding * 2);
					break;
				case HorizontalAlignment::Center: {
					const auto total_indentsz{ width - str.size() - padding * 2ull }, indentsz{ total_indentsz / 2 };
					os
						<< indent(indentsz)
						<< indent(padding) << str << indent(padding)
						<< indent(indentsz + (total_indentsz % 2 != 0 ? 1 : 0))
						;
					break;
				}
				case HorizontalAlignment::Right:
					os << indent(width, str.size() + padding * 2) << indent(padding) << str << indent(padding);
					break;
				default:
					throw make_exception((int)alignment, " is not a valid value for HorizontalAlignment!");
				}
			}
		};

		/// @brief	Column definitions
		std::vector<table_column> column_defs;
		/// @brief	The begin iterator of the range of items to display.
		Iter begin;
		/// @brief	The (exclusive) end iterator of the range of items to display.
		Iter end;
		/// @brief	The number of padding characters between strings and the table separators. Defaults to 1.
		unsigned padding;

		/**
		 * @brief			Creates a new print_table instance.
		 * @param begin	  -	The begin iterator of the range of items to display in the table.
		 * @param end	  -	The (exclusive) end iterator of the range of items to display in the table.
		 * @param columns -	An initializer list of column definitions.
		 * @param padding -	The amount of space between text and the table separators. Defaults to 1.
		 */
		CONSTEXPR print_table(Iter const& begin, Iter const& end, std::initializer_list<table_column>&& columns, unsigned const padding) :
			begin{ begin },
			end{ end },
			column_defs{ std::move(columns) },
			padding{ padding }
		{}
		/**
		 * @brief			Creates a new print_table instance.
		 * @param begin	  -	The begin iterator of the range of items to display in the table.
		 * @param end	  -	The (exclusive) end iterator of the range of items to display in the table.
		 * @param columns -	An initializer list of column definitions.
		 * @param padding -	The amount of space between text and the table separators. Defaults to 1.
		 */
		CONSTEXPR print_table(Iter const& begin, Iter const& end, std::initializer_list<table_column> const& columns, unsigned const padding) :
			begin{ begin },
			end{ end },
			column_defs{ columns },
			padding{ padding }
		{}
		/**
		 * @brief			Creates a new print_table instance.
		 * @param begin	  -	The begin iterator of the range of items to display in the table.
		 * @param end	  -	The (exclusive) end iterator of the range of items to display in the table.
		 * @param columns -	An initializer list of column definitions.
		 */
		CONSTEXPR print_table(Iter const& begin, Iter const& end, std::initializer_list<table_column>&& columns) :
			begin{ begin },
			end{ end },
			column_defs{ std::move(columns) },
			padding{ 1u }
		{}
		/**
		 * @brief			Creates a new print_table instance.
		 * @param begin	  -	The begin iterator of the range of items to display in the table.
		 * @param end	  -	The (exclusive) end iterator of the range of items to display in the table.
		 * @param columns -	An initializer list of column definitions.
		 */
		CONSTEXPR print_table(Iter const& begin, Iter const& end, std::initializer_list<table_column> const& columns) :
			begin{ begin },
			end{ end },
			column_defs{ columns },
			padding{ 1u }
		{}

		friend std::ostream& operator<<(std::ostream& os, const this_t& t)
		{
			if (t.column_defs.empty()) return os;

			// get table dimensions
			const auto columnWidths{ t.getColumnWidths() };

			/// print header
			// line 0 (top separator):
			os << term::EnableLineDrawing << term::LineCharacter::CORNER_TOP_LEFT;
			bool fst{ true };
			for (const auto& colWidth : columnWidths) {
				if (fst) fst = false;
				else os << term::LineCharacter::JUNCTION_3_WAY_TOP;

				os << indent(colWidth, 0, term::LineCharacter::LINE_HORIZONTAL);
			}
			os << term::LineCharacter::CORNER_TOP_RIGHT << '\n';
			// line 1 (header text):
			os << term::LineCharacter::LINE_VERTICAL;
			fst = true;
			for (int i{ 0 }, i_max{ static_cast<int>(t.column_defs.size()) }; i < i_max; ++i) {
				if (fst) fst = false;
				else os << term::LineCharacter::LINE_VERTICAL;

				os << term::DisableLineDrawing;
				t.column_defs[i].write_header_to(os, columnWidths[i], t.padding);
				os << term::EnableLineDrawing;
			}
			os << term::LineCharacter::LINE_VERTICAL << '\n';
			// line 2 (bottom separator):
			os << term::LineCharacter::JUNCTION_3_WAY_LEFT;
			fst = true;
			for (const auto& colWidth : columnWidths) {
				if (fst) fst = false;
				else os << term::LineCharacter::JUNCTION_4_WAY;

				os << indent(colWidth, 0, term::LineCharacter::LINE_HORIZONTAL);
			}
			os << term::LineCharacter::JUNCTION_3_WAY_RIGHT << '\n';

			/// print data
			for (auto it{ t.begin }; it != t.end; ++it) {
				const auto& item{ *it };

				os << term::LineCharacter::LINE_VERTICAL;
				fst = true;
				for (int i{ 0 }, i_max{ static_cast<int>(t.column_defs.size()) }; i < i_max; ++i) {
					if (fst) fst = false;
					else os << term::LineCharacter::LINE_VERTICAL;

					os << term::DisableLineDrawing;
					t.column_defs[i].write_item_to(os, item, columnWidths[i], t.padding);
					os << term::EnableLineDrawing;
				}
				os << term::LineCharacter::LINE_VERTICAL << '\n';
			}

			/// print bottom separator
			os << term::LineCharacter::CORNER_BOTTOM_LEFT;
			fst = true;
			for (const auto& colWidth : columnWidths) {
				if (fst) fst = false;
				else os << term::LineCharacter::JUNCTION_3_WAY_BOTTOM;

				os << indent(colWidth, 0, term::LineCharacter::LINE_HORIZONTAL);
			}
			os << term::LineCharacter::CORNER_BOTTOM_RIGHT << term::DisableLineDrawing << '\n';

			return os;
		}
	};
}

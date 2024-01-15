#pragma once
// 307lib::TermAPI
#include "term.hpp"		//< for term::setCursorPosition

// 307lib::shared
#include <strcore.hpp>	//< for str::stringify
#include <indentor.hpp>	//< for shared::indentor

// STL
#include <string>
#include <optional>
#include <ostream>

namespace term {
	/// @brief	Stream functor that prints the specified data at the specified offset in an ostream.
	template<typename TChar = char, typename TCharTraits = std::char_traits<TChar>, typename TAlloc = std::allocator<TChar>>
	struct print_at {
	private:
		using this_t = print_at<TChar, TCharTraits, TAlloc>;

		using string_t = std::basic_string<TChar, TCharTraits, TAlloc>;
		using indent_t = shared::basic_indentor<TChar, TCharTraits, TAlloc>;
		using ostream_t = std::basic_ostream<TChar, TCharTraits>;

	public:
		string_t data;
		std::optional<size_t> horizontal_offset;
		std::optional<size_t> vertical_offset;

		/**
		 * @brief			Prints the specified data at the specified col and/or row.
		 * @param col	  -	Horizontal offset. Use std::nullopt to not set the column.
		 * @param row	  -	Vertical offset. Use std::nullopt to not set the row.
		 * @param data... -	Any number of objects with a valid operator<<.
		 */
		print_at(std::optional<size_t> col, std::optional<size_t> row, auto&&... data) :
			data{ str::stringify<TChar, TCharTraits, TAlloc>($fwd(data)...) },
			horizontal_offset{ col },
			vertical_offset{ row }
		{}
		/**
		 * @brief			Prints the specified data at the specified col and/or row.
		 * @param col	  -	Horizontal offset. Use std::nullopt to not set the column.
		 * @param row	  -	Vertical offset. Use std::nullopt to not set the row.
		 * @param data... -	Any number of objects with a valid operator<<.
		 */
		print_at(std::optional<size_t> col, std::optional<size_t> row, auto const&... data) :
			data{ str::stringify<TChar, TCharTraits, TAlloc>(data...) },
			horizontal_offset{ col },
			vertical_offset{ row }
		{}

		friend ostream_t& operator<<(ostream_t& os, const this_t& p)
		{
			if (p.data.empty())
				return os;
			else if (!p.horizontal_offset.has_value() && !p.vertical_offset.has_value())
				return os << p.data;

			// set the vertical offset
			if (p.vertical_offset.has_value())
				os << term::setCursorPosition(1, p.vertical_offset.value());

			// get the horizontal offset
			const auto hOffset{ p.horizontal_offset.value_or(0) };

			// print the data
			if (hOffset > 0) os << indent_t(hOffset);
			TChar ch{};
			for (auto it{ p.data.begin() }, it_end{ p.data.end() }; it != it_end; ++it) {
				switch (ch = *it) {
				case static_cast<TChar>('\n'):
					os << static_cast<TChar>('\n');
					if (it + 1 != it_end)
						os << indent_t(hOffset);
					break;
				default:
					os << ch;
					break;
				}
			}

			return os;
		}
	};

}

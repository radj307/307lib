/**
 * @file	Message.hpp
 * @author	radj307
 * @brief	Contains the Message object definition, and the constant pre-defined message typesf.
 */
#pragma once
#include <sysarch.h>
#include <regex>
#include "indentor.hpp"

namespace term {
	/**
	 * @struct	Message
	 * @brief	Provides a convenience wrapper used to print log message headers.
	 */
	struct Message {
		const char* const body;
		const indent::size_type margin_sz;
		/// @brief	When true, uses regular expressions to only count displayable characters when calculating message lengths.
		const bool use_regex_indent{ true };

		CONSTEXPR Message() : body{ nullptr }, margin_sz{ 10ull } {}
		explicit CONSTEXPR Message(const char* body, const indent::size_type& marginSize = 10ull) : body{ body }, margin_sz{ marginSize } {}

		explicit operator const char* const() const { return body; }
		operator std::string() const { return{ body }; }

		friend std::ostream& operator<<(std::ostream& os, const Message& msg)
		{
			if (msg.body != nullptr) {
				std::string body{ msg.body };
				if (msg.use_regex_indent) {
					std::smatch match;
					if (std::regex_search(body, match, std::basic_regex<char>("\\[[A-Z]+?\\]")))
						body = match.str();
				}
				os << msg.body << indent(msg.margin_sz, body.size());
			}
			return os;
		}
	};

	CONSTEXPR const indent::size_type MessageMarginSize{ 10 };

	CONSTEXPR const Message debug{ "[DEBUG]", MessageMarginSize };
	CONSTEXPR const Message info{ "[INFO]", MessageMarginSize };
	CONSTEXPR const Message log{ "[LOG]", MessageMarginSize };
	CONSTEXPR const Message msg{ "[MSG]", MessageMarginSize };
	CONSTEXPR const Message warn{ "[WARN]", MessageMarginSize };
	CONSTEXPR const Message error{ "[ERROR]", MessageMarginSize };
	CONSTEXPR const Message crit{ "[CRIT]", MessageMarginSize };
	CONSTEXPR const Message fatal{ "[FATAL]", MessageMarginSize };
	CONSTEXPR const Message placeholder{ "", MessageMarginSize };

	CONSTEXPR const Message get_debug(const bool& allow_color = true, const indent::size_type& indentation = 10) noexcept { return(allow_color ? Message("\033[38;5;99m[DEBUG]\033[38;5;7m", indentation) : Message("[DEBUG]", indentation)); }
	CONSTEXPR const Message get_info(const bool& allow_color = true, const indent::size_type& indentation = 10) noexcept { return(allow_color ? Message("\033[38;5;246m[INFO]\033[38;5;7m", indentation) : Message("[INFO]", indentation)); }
	CONSTEXPR const Message get_log(const bool& allow_color = true, const indent::size_type& indentation = 10) noexcept { return(allow_color ? Message("\033[38;5;7m[LOG]\033[38;5;7m", indentation) : Message("[LOG]", indentation)); }
	CONSTEXPR const Message get_msg(const bool& allow_color = true, const indent::size_type& indentation = 10) noexcept { return(allow_color ? Message("\033[38;5;2m[MSG]\033[38;5;7m", indentation) : Message("[MSG]", indentation)); }
	CONSTEXPR const Message get_warn(const bool& allow_color = true, const indent::size_type& indentation = 10) noexcept { return(allow_color ? Message("\033[38;5;208m[WARN]\033[38;5;7m", indentation) : Message("[WARN]", indentation)); }
	CONSTEXPR const Message get_error(const bool& allow_color = true, const indent::size_type& indentation = 10) noexcept { return(allow_color ? Message("\033[38;5;1m[ERROR]\033[38;5;7m", indentation) : Message("[ERROR]", indentation)); }
	CONSTEXPR const Message get_crit(const bool& allow_color = true, const indent::size_type& indentation = 10) noexcept { return(allow_color ? Message("\033[38;5;88m[CRIT]\033[38;5;7m", indentation) : Message("[CRIT]", indentation)); }
	CONSTEXPR const Message get_fatal(const bool& allow_color = true, const indent::size_type& indentation = 10) noexcept { return(allow_color ? Message("\033[38;5;88m[FATAL]\033[38;5;7m", indentation) : Message("[FATAL]", indentation)); }

	CONSTEXPR const Message get_placeholder(const bool& allow_color = true) noexcept { return placeholder; }
}
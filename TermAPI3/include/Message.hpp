#pragma once
#include <Sequence.hpp>
#include <ColorPalette.hpp>

namespace sys::term {
#ifndef TERMAPI_ENABLE_OLD_FUNCTIONS
	namespace message_settings {
		static bool useColorSequencesInMessages{ true };
		static size_t maxMessageSizeIndent{ 8ull };
	}

	/**
	 * @brief			Allows enabling/disabling the use of ANSI color sequences in message functions.
	 * @param enable	When true, sets the message_palette to active. Else, disables message colors.
	 * @returns			bool
	 */
	inline constexpr bool setMessageColorEnabled(const bool& enable)
	{
		const auto copy{ message_settings::useColorSequencesInMessages };
		message_settings::useColorSequencesInMessages = enable;
		return copy;
	}

	/**
	 * @class	Message
	 * @brief	Wrapper for messages.
	 */
	class Message {
		const std::string _message;
		const color::setcolor _color;
		const bool _use_indent;
	public:
		constexpr Message(const std::string& message_prefix, const color::setcolor& color, const bool& use_indent = true) : _message{ message_prefix }, _color{ color }, _use_indent{ use_indent } {}

		std::string as_string() const
		{
			if (message_settings::useColorSequencesInMessages)
				return str::stringify(_color, _message, color::reset, _use_indent ? str::VIndent(message_settings::maxMessageSizeIndent, _message.size()) : str::VIndent(0ull));
			return _message;
		}
		std::string as_string_no_color() const
		{
			return str::stringify(_message, _use_indent ? str::VIndent(message_settings::maxMessageSizeIndent, _message.size()) : str::VIndent(0ull));
		}

		friend std::ostream& operator<<(std::ostream& os, const Message& msg)
		{
			return os << msg.as_string();
		}
	};

	const Message
		info{ "[INFO]", color::setcolor{ color::light_gray } },
		log{ "[LOG]", color::setcolor{ color::white } },
		debug{ "[DEBUG]", color::setcolor{color::light_purple } },
		msg{ "[MSG]",color::setcolor{color::green } },
		warn{ "[WARN]",color::setcolor{color::orange} },
		error{ "[ERROR]", color::setcolor{color::red } },
		crit{ "[CRIT]",color::setcolor{color::dark_red, color::FormatFlag::BOLD } };
#endif
}
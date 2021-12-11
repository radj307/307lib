/**
 * @file	xLog.hpp
 * @author	radj307
 * @brief	Contains the xlog _(eXtensible LOG)_ namespace, a framework for implementing console logs into C++ programs.
 */
#include <str.hpp>		// str-lib
#include <Message.hpp>	// TermAPI

#include <iostream>
#include <string>
#include <optional>

#pragma region UndefineMicrosoftBullshitMacros
#ifdef ERROR
#undef ERROR
#endif
#pragma endregion UndefineMicrosoftBullshitMacros

 /**
  * @def	XLOG_INCLUDE_SELF
  * @brief	When defined before including <xlog.hpp>, DEBUG level logs will include
  */
#ifndef XLOG_INCLUDE_SELF
#define XLOG_INCLUDE_SELF
#undef XLOG_INCLUDE_SELF
#else
#define XLOG_LEVEL_SELF
#endif

  /**
   * @namespace	xlog
   * @brief		Contains all user-facing xlog functions & objects.
   */
namespace xlog {
	/**
	 * @struct	level
	 * @brief	Contains possible LogLevel values that are used by the message whitelisting system.
	 *			# Base Levels
	 *			The LogLevel struct can contain any or all of these values.
	 *			| Name       | Description of Included Information										   |
	 *			| ---------- | --------------------------------------------------------------------------- |
	 *			| `CRITICAL` | Only shows critically-important information.								   |
	 *			| `ERROR`	 | Shows program errors.													   |
	 *			| `WARNING`	 | Shows program warnings.													   |
	 *			| `LOG`		 | Shows program logs.														   |
	 *			| `INFO`	 | Shows program information, and unimportant additional information.		   |
	 *			| `MESSAGE`	 | Shows program messages.                                                     |
	 *			| `DEBUG`	 | Shows debugging information that is usually only useful for the developer.  |
	 *			| `NONE`	 | This isn't technically a log level, just the absence of one. Shows nothing. |
	 *
	 *			# Level Presets
	 *			These are premade combinations of the above Base Levels.
	 *\n		Levels marked with _X_ are shown by the related constant.
	 *			| Name                         | DEBUG | MESSAGE | INFO | LOG | WARNING | ERROR | CRITICAL |
	 *			| ---------------------------- | ----- | ------- | ---- | --- | ------- | ----- | -------- |
	 *			| `OnlyErrors`                 |       |         |      |     |         | X     | X        |
	 *			| `OnlyErrorsAndWarnings`      |       |         |      |     | X       | X     | X        |
	 *			| `OnlyErrorsDebug`            | X     |         |      |     |         | X     | X        |
	 *			| `OnlyErrorsAndWarningsDebug` | X     |         |      |     | X       | X     | X        |
	 *			| `Default`                    |       | X       |      | X   | X       | X     | X        |
	 *			| `All`                        |       | X       | X    | X   | X       | X     | X        |
	 *			| `AllDebug`                   | X     | X       | X    | X   | X       | X     | X        |
	 *			| `NoErrors`                   |       | X       | X    | X   | X       |       |          |
	 *			| `NoErrorsOrWarnings`         |       | X       | X    | X   |         |       |          |
	 *			| `NoErrorsDebug`              | X     | X       | X    | X   | X       |       |          |
	 *			| `NoErrorsOrWarningsDebug`    | X     | X       | X    | X   |         |       |          |
	 */
	struct level {
		level() = delete;
		/**
		 * @struct	LogLevel
		 * @brief	Represents a message flag system that allows the programmer or user to only show specific message types.
		 */
		struct LogLevel {
		private:
			const unsigned char _level; // this is a char because they use one byte and allow multiple values
		public:
			constexpr LogLevel(const unsigned char& level) : _level{ level } {}
			constexpr operator const unsigned char() const { return _level; }
			constexpr bool contains(const unsigned char& l) const { return (_level & l) != 0; }
		};
		/// @brief	Base: Don't show any messages.
		static const LogLevel NONE;
		/// @brief	Base: Show critical error messages.
		static const LogLevel CRITICAL;
		/// @brief	Base: Show error messages.
		static const LogLevel ERROR;
		/// @brief	Base: Show warning messages.
		static const LogLevel WARNING;
		/// @brief	Base: Show log messages.
		static const LogLevel LOG;
		/// @brief	Base: Show informational messages.
		static const LogLevel INFO;
		/// @brief	Base: Show debugging messages.
		static const LogLevel DEBUG;
		/// @brief	Base: Show program messages.
		static const LogLevel MESSAGE;

		/// @brief	Only show CRITICAL & ERROR level messages.
		static const LogLevel OnlyErrors;
		/// @brief	Only show CRITICAL, ERROR, & WARNING level messages.
		static const LogLevel OnlyErrorsAndWarnings;
		/// @brief	Only show CRITICAL & ERROR level messages.
		static const LogLevel OnlyErrorsDebug;
		/// @brief	Only show CRITICAL, ERROR, & WARNING level messages.
		static const LogLevel OnlyErrorsAndWarningsDebug;
		/// @brief Show all levels except for DEBUG.
		static const LogLevel All;
		/// @brief Show all levels including DEBUG.
		static const LogLevel AllDebug;
		/// @brief Shows CRITICAL, ERROR, WARNING, & LOG level messages.
		static const LogLevel Default;
		/// @brief	Show everything except for DEBUG, CRITICAL, & ERROR level messages.
		static const LogLevel NoErrors;
		/// @brief	Show everything except for DEBUG, CRITICAL, ERROR, & WARNING level messages.
		static const LogLevel NoErrorsOrWarnings;
		/// @brief	Show everything except for CRITICAL, & ERROR level messages.
		static const LogLevel NoErrorsDebug;
		/// @brief	Show everything except for CRITICAL, ERROR, & WARNING level messages.
		static const LogLevel NoErrorsOrWarningsDebug;
	};
	constexpr const level::LogLevel
		level::NONE{ 0 },
		level::CRITICAL{ 1 },
		level::ERROR{ 2 },
		level::WARNING{ 4 },
		level::LOG{ 8 },
		level::INFO{ 16 },
		level::DEBUG{ 32 },
		level::MESSAGE{ 64 },
		level::OnlyErrors{ CRITICAL | ERROR },
		level::OnlyErrorsAndWarnings{ CRITICAL | ERROR | WARNING },
		level::OnlyErrorsDebug{ DEBUG | CRITICAL | ERROR },
		level::OnlyErrorsAndWarningsDebug{ DEBUG | OnlyErrorsAndWarnings },
		level::All{ OnlyErrorsAndWarnings | LOG | INFO },
		level::AllDebug{ DEBUG | All },
		level::Default{ OnlyErrorsAndWarnings | LOG | MESSAGE },
		level::NoErrors{ WARNING | LOG | INFO | MESSAGE },
		level::NoErrorsOrWarnings{ LOG | INFO | MESSAGE },
		level::NoErrorsDebug{ DEBUG | NoErrors },
		level::NoErrorsOrWarningsDebug{ DEBUG | NoErrorsOrWarnings };

	/**
	 * @struct				OutputTarget
	 * @brief				Represents a method of outputting the logs generated by the program.
	 * @tparam StreamType	The type of stream used during output operations.
	 */
	template<class StreamType>
	struct OutputTarget {
		StreamType* target{ nullptr };
		OutputTarget(StreamType& output_target) : target{ &output_target } {}

		friend std::istream& operator>>(std::istream& is, const OutputTarget& o) { return is >> (*o.output).rdbuf(); }
		friend std::ostream& operator<<(std::ostream& os, const OutputTarget& o) { return os << (*o.output).rdbuf(); }

		void write(const std::string& str) const
		{
			(*target) << str;
			if (!str.empty() && str.back() != '\n')
				(*target) << '\n';
		}
	};

	/**
	 * @class				xLog
	 * @brief				Logging object.
	 * @tparam StreamType	The type of stream to use as the output target.
	 */
	template<class StreamType = std::ostream>
	class xLog {
	protected:
		OutputTarget<StreamType> _target;
		std::unique_ptr<level::LogLevel> _level;
		bool _add_prefix, _log_self{ false };

		/**
		 * @brief			Format a given message using the current settings.
		 * @param level		The log level associated with this message.
		 * @param message	The message string.
		 * @returns			std::string
		 */
		std::string format(const level::LogLevel& level, const std::string& message) const
		{
			if (!_add_prefix)
				return message;
			const sys::term::Message* message_type{ nullptr };
			switch (level) {
			case level::CRITICAL:
				message_type = &sys::term::crit;
				break;
			case level::ERROR:
				message_type = &sys::term::error;
				break;
			case level::WARNING:
				message_type = &sys::term::warn;
				break;
			case level::LOG:
				message_type = &sys::term::log;
				break;
			case level::INFO:
				message_type = &sys::term::info;
				break;
			case level::MESSAGE:
				message_type = &sys::term::msg;
				break;
			case level::DEBUG:
				message_type = &sys::term::debug;
				break;
			default:
				break;
			}
			if (message_type != nullptr) {
				if constexpr (std::derived_from<StreamType, std::ofstream>)
					return { message_type->as_string_no_color() + message };
				else
					return { message_type->as_string() + message };
			}
			return str::stringify(str::VIndent(sys::term::message_settings::maxMessageSizeIndent), message);
		}

		/**
		 * @brief			Check if the current log level allows a given level.
		 * @param level		Check if the current log level contains this level.
		 * @returns bool
		 */
		bool currentLevelContains(const level::LogLevel& level) const
		{
			if (_level.get() != nullptr)
				return (*_level.get()).contains(level);
			return false;
		}

		/**
		 * @brief				Internal-use-only function that checks if self-logging is enabled, and if so, writes the given message with the DEBUG level.
		 * @param ...message	The message to write to the log.
		 * @returns				bool
		 */
		template<var::Streamable... VT>
		bool self_log(const VT&... message) const
		{
			return _log_self && currentLevelContains(level::DEBUG) && log(level::DEBUG, message...);
		}

	public:
		xLog(const OutputTarget<StreamType>& out = OutputTarget<StreamType>{ std::cerr }, const level::LogLevel& log_level = level::Default, const bool& add_prefix = true) : _target{ out }, _level{ std::make_unique<level::LogLevel>(log_level) }, _add_prefix{ add_prefix } {}

		friend std::istream& operator>>(std::istream& is, const xLog<StreamType>& x) { return is >> x._target; }
		friend std::ostream& operator<<(std::ostream& os, const xLog<StreamType>& x) { return os << x._target; }

		/**
		 * @brief			Set whether messages should automatically be given a colorized prefix such as "[ERROR]".
		 * @param enable	When true, messages include a colorized prefix.
		 * @returns bool	The previous prefix state.
		 */
		bool setPrefixEnabled(const bool& enable)
		{
			const auto copy{ _add_prefix };
			_add_prefix = enable;
			return copy;
		}

		/**
		 * @brief				Set the current whitelist level.
		 * @param level			This level will overwrite the previous one.
		 * @returns LogLevel	The previous whitelist level.
		 */
		level::LogLevel setLevel(const level::LogLevel& level)
		{
			const auto copy{ *_level.release() };
			_level = std::make_unique<level::LogLevel>(level);

			return copy;
		}

		/// @brief	Retrieve the current level whitelist setting.
		[[nodiscard]] level::LogLevel getLevel() const { return *_level.get(); }

		/**
		 * @brief				Create a message and send it to the current output target.
		 * @tparam ...Message	Any number of types that can be inserted into a stream with operator<<.
		 * @param level			This message's level.
		 * @param ...msg		The contents of this message.
		 * @returns				bool
		 */
		template<typename... Message>
		inline bool log(const level::LogLevel& level, const Message&... msg) const
		{
			const auto allowed{ currentLevelContains(level) };
			if (allowed)
				_target.write(format(level, str::stringify(msg...)));
			else
				self_log("Refused a message because the current log level does not allow messages of that type.");
			return allowed;
		}
	};

	using msg_break_t = nullptr_t;
	constexpr const msg_break_t endm = nullptr;

	/**
	 * @class				xLogs
	 * @brief				Extends the xLog object with operator<< stream capabilities.
	 * @tparam StreamType	The type of stream to use as the output target.
	 */
	template<class StreamType = std::ostream>
	class xLogs : public xLog<StreamType> {
		std::unique_ptr<level::LogLevel> _last{ nullptr };
		std::stringstream _buffer;

	public:
		/**
		 * @brief				Constructor
		 * @param out			The output target for logs.
		 * @param log_level		The log level whitelist.
		 * @param add_prefix	When true, log messages are prefixed with their level.
		 */
		xLogs(const OutputTarget<StreamType>& out = OutputTarget<StreamType>{ std::cerr }, const level::LogLevel& log_level = level::Default, const bool& add_prefix = true) : xLog<StreamType>(out, log_level, add_prefix) {}

		/**
		 * @brief		Stream insertion operator for the xLogs class.
		 *\n			Allows using xLogs in the same way as a std::ostream, with some important changes:
		 *\n			- A message level must be specified before messages can be inserted, unless prefixes are disabled.
		 *\n			- Messages are terminated with `endm` / nullptr.
		 *\n			- Messages are only sent to the output target once endm has been received.
		 * @tparam T	Input Type
		 * @param oxl	(implicit) xLogs instance.
		 * @param m		(implicit) Message type.
		 */
		template<var::Streamable T>
		friend xLogs<StreamType>& operator<<(xLogs<StreamType>& oxl, const T& m)
		{
			if constexpr (std::same_as<T, level::LogLevel>)
				oxl._last = std::make_unique<level::LogLevel>(m);
			else if constexpr (std::same_as<T, msg_break_t>) {
				if (!oxl._add_prefix || oxl._last.get() == nullptr)
					oxl.log(level::NONE, oxl._buffer.str());
				else
					oxl.log(*oxl._last.get(), oxl._buffer.str());
				oxl._buffer = {};
			}
			else if (!oxl._add_prefix || oxl._last.get() != nullptr)
				oxl._buffer << m;
			return oxl;
		}
		/**
		 * @brief		Allows inserting an entire output stream into the xLogs instance.
		 * @param oxl	(implicit) xLogs instance.
		 * @param os	(implicit) Output Stream.
		 */
		friend xLogs<StreamType>& operator<<(xLogs<StreamType>& oxl, std::ostream& os)
		{
			oxl << os.rdbuf();
		}
	};
}
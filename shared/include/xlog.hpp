/**
 * @file	xlog.hpp
 * @author	radj307
 * @brief	Contains the xlog namespace, a framework for implementing debug logs into programs.
 *
*//**
 * @struct	LogLevel
 * @brief	The `LogLevel` struct is used to define which types of messages should be included or excluded from the logs.
 *\n		The possible values for LogLevel can be _any or all_ of the following:
 *
 *			| LogLevel  | Description of Included Information										 |
 *			| --------- | -------------------------------------------------------------------------- |
 *			| CRITICAL  | Only shows critically-important information.								 |
 *			| ERROR		| Shows program errors.														 |
 *			| WARNING	| Shows program warnings.													 |
 *			| LOG		| Shows program logs.														 |
 *			| INFO		| Shows program information, and unimportant additional information.		 |
 *			| DEBUG		| Shows debugging information that is usually only useful for the developer. |
 *			| NONE		| Disables all logs.														 |
*//**
 * @class	xLog
 * @brief	This object is scope-based, and tracks the current output method & log level, both of which can be changed at runtime as-needed.
 */
#include <str.hpp>
#include <Message.hpp>

#include <iostream>
#include <string>
#include <optional>

#pragma region UndefineMicrosoftBullshitMacros
#ifdef OUT
#undef OUT
#endif
#ifdef ERROR
#undef ERROR
#endif
#pragma endregion UndefineMicrosoftBullshitMacros

 /**
  * @def		XLOG_INCLUDE_SELF
  * @brief	When defined before including <xlog.hpp>, DEBUG level logs will include
  */
#ifndef XLOG_INCLUDE_SELF
#define XLOG_INCLUDE_SELF
#undef XLOG_INCLUDE_SELF
#endif

  /**
   * @namespace	xlog
   * @brief		Contains all user-accessible xlog functions & objects.
   */
namespace xlog {
	struct LogLevel {
	private:
		const unsigned char _level; // this is a char because they use one byte and allow multiple values
	public:
		constexpr LogLevel(const unsigned char& level) : _level{ level } {}
		constexpr operator const unsigned char() const { return _level; }
		constexpr bool contains(const unsigned char& l) const { return (_level & l) != 0; }
		static const LogLevel NONE, CRITICAL, ERROR, WARNING, LOG, INFO, DEBUG;
	};
	constexpr const LogLevel
		LogLevel::NONE{ 0 },
		LogLevel::CRITICAL{ 1 },
		LogLevel::ERROR{ 2 },
		LogLevel::WARNING{ 4 },
		LogLevel::LOG{ 8 },
		LogLevel::INFO{ 16 },
		LogLevel::DEBUG{ 32 };
	constexpr const LogLevel
		LEVEL_NEGATIVE{ LogLevel::CRITICAL | LogLevel::ERROR | LogLevel::WARNING },
		LEVEL_ALL_NO_DEBUG{ LEVEL_NEGATIVE | LogLevel::LOG | LogLevel::INFO },
		LEVEL_ALL{ LEVEL_ALL_NO_DEBUG | LogLevel::DEBUG };

	template<class StreamType>
	struct OutputTarget {
		StreamType* target{ nullptr };
		OutputTarget(StreamType& output_target) : target{ &output_target } {}

		friend std::istream& operator>>(std::istream& is, const OutputTarget& o) { return is >> (*o.output).rdbuf(); }
		friend std::ostream& operator<<(std::ostream& os, const OutputTarget& o) { return os << (*o.output).rdbuf(); }

		auto write(const std::string& str) const
		{
			(*target) << str;
			if (!str.empty() && str.back() != '\n')
				(*target) << '\n';
		}
		auto flush() const { return target->flush(); }
		auto clear() const { return target->clear(); }
	};

	template<class StreamType = std::ostream>
	class xLog {
		std::unique_ptr<LogLevel> _level;
		OutputTarget<StreamType> _target;
		bool _add_prefix{ true };

		std::string format(const LogLevel& level, const std::string& message) const
		{
			if (!_add_prefix)
				return message;
			switch (level) {
			case LogLevel::CRITICAL:
				return{ sys::term::crit.as_string() + message };
			case LogLevel::ERROR:
				return{ sys::term::error.as_string() + message };
			case LogLevel::WARNING:
				return{ sys::term::warn.as_string() + message };
			case LogLevel::LOG:
				return{ sys::term::log.as_string() + message };
			case LogLevel::INFO:
				return{ sys::term::info.as_string() + message };
			case LogLevel::DEBUG:
				return{ sys::term::debug.as_string() + message };
			default:
				return message;
			}
		}

	public:
		xLog(const LogLevel& log_level, const OutputTarget<StreamType>& out = OutputTarget<StreamType>{ std::cerr }) : _target{ out }, _level{ std::make_unique<LogLevel>(log_level) } {}

		friend std::istream& operator>>(std::istream& is, const xLog<StreamType>& x) { return is >> x._target; }
		friend std::ostream& operator<<(std::ostream& os, const xLog<StreamType>& x) { return os << x._target; }

		bool setIncludePrefix(const bool& enabled)
		{
			const auto copy{ _add_prefix };
			_add_prefix = enabled;
			return copy;
		}
		LogLevel setLevel(const LogLevel& level)
		{
			const auto copy{ *_level.release() };
			_level = std::make_unique<LogLevel>(level);
			return copy;
		}
		LogLevel getLevel() const { return *_level.get(); }

		template<typename... Message>
		inline bool log(const LogLevel& level, const Message&... msg) const
		{
			const auto allowed{ (*_level.get()).contains(level) };
			if (allowed)
				_target.write(format(level, str::stringify(msg...)));
		#ifdef XLOG_INCLUDE_SELF
			else if ((*_level.get()).contains(LogLevel::DEBUG))
				_target.write(format(LogLevel::DEBUG, "Refused a message because the log level did not include it."s));
		#endif
			return allowed;
		}
	};
}
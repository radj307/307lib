#pragma once

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
			std::string as_string_id() const
			{
				return std::to_string(static_cast<short>(_level));
			}
		};
		/// @brief	Base: Don't show any messages.
		static const LogLevel NONE;
		/// @brief	Base: Show critical error messages.
		static const LogLevel CRITICAL;
		/// @brief	Base: Show error messages.
		static const LogLevel ERROR;
		/// @brief	Base: Show warning messages.
		static const LogLevel WARNING;
		/// @brief	Base: Show program messages.
		static const LogLevel MESSAGE;
		/// @brief	Base: Show log messages.
		static const LogLevel LOG;
		/// @brief	Base: Show informational messages.
		static const LogLevel INFO;
		/// @brief	Base: Show debugging messages.
		static const LogLevel DEBUG;

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
		level::MESSAGE{ 8 },
		level::LOG{ 16 },
		level::INFO{ 32 },
		level::DEBUG{ 64 },
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
	 * @brief		Convert a Message instance to its equivalent xlog level. This is hardcoded.
	 * @param msg	Input Message Type.
	 * @returns		level::LogLevel
	 */
	inline level::LogLevel message_to_level(const char* msg)
	{
		if (msg == debug)
			return level::DEBUG;
		if (msg == info)
			return level::INFO;
		if (msg == log)
			return level::LOG;
		if (msg == msg)
			return level::MESSAGE;
		if (msg == warn)
			return level::WARNING;
		if (msg == error)
			return level::ERROR;
		if (msg == crit)
			return level::CRITICAL;
		return level::NONE;
	}
}
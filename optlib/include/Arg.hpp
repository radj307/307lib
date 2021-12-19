#pragma once
#include <str.hpp>
#include <optional>

namespace opt {
	/// @brief The variable type contained within a Parameter instance.
	using ParameterType = std::string;
	/// @brief The variable type contained within an Option instance.
	using OptionType = std::pair<std::string, std::optional<std::string>>;
	/// @brief The variable type contained within a Flag instance.
	using FlagType = std::pair<char, std::optional<std::string>>;

	/// @brief Concept constraint for Arg subtypes. Allows: ( ParameterType | OptionType | FlagType )
	template<typename T> concept ValidArgType = std::same_as<T, ParameterType> || std::same_as<T, OptionType> || std::same_as<T, FlagType>;
	/// @brief Concept constraint for Option/Flag pair subtypes
	template<typename T> concept CanHaveValueArgumentType = std::same_as<T, OptionType> || std::same_as<T, FlagType>;

	/**
	 * @class Arg
	 * @brief Polymorphic argument wrapper class that exposes helper functions.
	 * @tparam T	- Argument Type. Allows ParameterType, OptionType, or FlagType.
	 */
	template<ValidArgType T>
	class Arg {
		T _arg;

	public:
		/// CONSTRUCTORS
		/**
		 * @brief Any-Constructor. Can make Parameters, Options, or Flags depending on input type.
		 * @param arg	- Raw argument from the commandline to wrap with an Arg class.
		 */
		Arg(T arg) : _arg{ std::move(arg) } {}
		/**
		 * @brief Option Constructor.
		 * @param arg		- Argument string.
		 * @param capture	- Optional captured argument. Pass std::nullopt if there is no capture argument.
		 */
		Arg(std::string arg, std::optional<std::string> capture) : _arg{ std::make_pair(std::move(arg), std::move(capture)) } {}
		/**
		 * @brief Flag Constructor.
		 * @param arg		- Argument char.
		 * @param capture	- Optional captured argument. Pass std::nullopt if there is no capture argument.
		 */
		Arg(char c, std::optional<std::string> capture) : _arg{ std::make_pair(std::move(c), std::move(capture)) } {}

		/// OPERATORS

		[[nodiscard]] auto operator==(const Arg<T>& o) const noexcept { return _arg == o._arg; } // operator that checks if _arg is equivalent
		template<ValidArgType U> [[nodiscard]] auto operator==(const Arg<U>& o) const noexcept { return false; } // operator that allows comparing against other Arg types. (always false)
		[[nodiscard]] auto operator!=(auto&& o) const noexcept { return !operator==(std::forward<decltype(o)>(o)); } // not equal operator

		/// @brief std::string conversion operator. Only includes the argument's name, not any captured parameters! Allows Args to be more easily-consumed by the ParamsAPI class.
		[[nodiscard]] constexpr operator const std::string() const noexcept
		{
			if constexpr (std::same_as<T, ParameterType>) // return parameters
				return _arg;
			else if constexpr (std::same_as<T, OptionType>) // Return the first element of options
				return _arg.first;
			else if constexpr (std::same_as<T, FlagType>) // Convert flags to std::string
				return std::string(1ull, _arg.first);
			return{};
		}

		/**
		 * @brief Check if this argument has any of the given types, or if no types are given, always returns true.
		 * @tparam MatchArgType...	- Any number of valid argument types. If no types are specified, always returns true.
		 * @returns bool
		 */
		template<ValidArgType... MatchArgType> [[nodiscard]] constexpr bool has_type() const noexcept { return (sizeof...(MatchArgType) == 0) || std::disjunction_v<std::is_same<T, MatchArgType>...>; }

		/// @brief Retrieve this Arg's name as it appeared on the commandline. @returns const std::string()
		[[nodiscard]] constexpr auto name() const noexcept { return operator const std::string(); }

		/// @brief Check if this Arg has a captured argument. @returns bool
		[[nodiscard]] constexpr bool hasv() const noexcept requires CanHaveValueArgumentType<T> { return (std::same_as<T, OptionType> || std::same_as<T, FlagType>) && _arg.second.has_value(); }
		[[nodiscard]] constexpr bool hasv() const noexcept requires std::same_as<T, ParameterType> { return false; }
		/// @brief Retrieve this argument's optional capture argument. If this argument is a Parameter, returns std::nullopt rather than throwing.
		[[nodiscard]] constexpr const std::optional<std::string> getv() const noexcept requires CanHaveValueArgumentType<T> { return _arg.second; }
		[[nodiscard]] constexpr const std::string getv() const noexcept requires std::same_as<T, ParameterType> { return _arg; }
		[[nodiscard]] constexpr const std::string typegetv() const noexcept(false) { return getv().value(); }
	};

	/// @brief Allows changing some formatting when using the Arg::operator<< / Arg::operator>> methods.
	static struct {
		bool include_captures{ true };
		bool enclose_multiword_with_quotes{ true };
		unsigned char quote_char{ '\"' };
		std::vector<unsigned char> divider{ ' ' };		///< @brief Allows changing the divider between an argument's name and its optional capture argument. Recommended values are ' ' or '=', but any list of characters can be used.
	} Settings_Arg;

	/**
	 * @brief Stream insertion operator for the Arg class. See the Settings_Arg static struct for changing output format settings.
	 * @tparam T	- The type of argument contained within the given Arg class.
	 * @param os	- (implicit) Output Stream.
	 * @param arg	- (implicit) Argument instance.
	 * @returns std::ostream&
	 */
	template<ValidArgType T>
	inline std::ostream& operator<<(std::ostream& os, const Arg<T>& arg)
	{
		auto name{ arg.name() };
		if (Settings_Arg.enclose_multiword_with_quotes && str::pos_valid(name.find_first_of(" \t\v\r\n")))
			name = str::stringify(Settings_Arg.quote_char, name, Settings_Arg.quote_char);
		constexpr const bool is_option{ std::same_as<T, OptionType> }, is_flag{ std::same_as<T, FlagType> };

		if constexpr (is_option)
			os << "--";
		else if constexpr (is_flag)
			os << '-';
		os << name;
		if constexpr (is_option || is_flag)
			if (Settings_Arg.include_captures)
				if (const auto cap{ arg.getv() }; cap.has_value()) {
					const auto& v{ cap.value() };
					for (auto& div : Settings_Arg.divider)
						os << div;
					if (Settings_Arg.enclose_multiword_with_quotes && str::pos_valid(v.find(' '))) // Value has spaces in its argument, enclose it with quotes
						os << Settings_Arg.quote_char << v << Settings_Arg.quote_char;
					else
						os << v; // don't enclose single-words
				}
		return os;
	}

	using Option = Arg<std::pair<std::string, std::optional<std::string>>>;
	using Flag = Arg<std::pair<char, std::optional<std::string>>>;
	using Parameter = Arg<std::string>;

	/**
	 * @brief Allows types: ( Parameter | Option | Flag )
	 */
	template<class T> concept ValidArg = std::same_as<T, Parameter> || std::same_as<T, Option> || std::same_as<T, Flag>;
	/**
	 * @brief Allows types: ( Option | Flag )
	 */
	template<class T> concept CanHaveValueArgument = std::same_as<T, Option> || std::same_as<T, Flag>;
}
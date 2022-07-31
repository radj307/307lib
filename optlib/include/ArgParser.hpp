#pragma once
#include <ArgContainer.hpp>
#include <str.hpp>
#include <charcompare.hpp>
#include <var.hpp>

#include <vector>
#include <concepts>

namespace opt {
	using StrVec = std::vector<std::string>;
	using uchar = unsigned char;

#	pragma region CaptureStyle
	/**
	 * @struct	CaptureStyle
	 * @brief	Pseudo-enum bitfield that defines argument capturing rules.
	 */
	struct CaptureStyle {
	private:
		uchar _value;
	public:
		constexpr CaptureStyle(const uchar& value) : _value{ value } {}
		constexpr operator uchar() const { return _value; }

		constexpr CaptureStyle& operator=(const uchar& o)
		{
			_value = o;
			return *this;
		}

		constexpr bool isDisabled() const { return _value == 0; }
		constexpr bool isEqualsOnly() const { return (_value & EqualsOnly) != 0; }
		constexpr bool isDisabledOrEqualsOnly() const { return isDisabled() || isEqualsOnly(); }
		constexpr bool isRequired() const { return (_value & Required) != 0; }

		CaptureStyle operator|(const uchar& o) const { return{ static_cast<uchar>(_value | o) }; }
		CaptureStyle operator&(const uchar& o) const { return{ static_cast<uchar>(_value & o) }; }
		CaptureStyle operator^(const uchar& o) const { return{ static_cast<uchar>(_value ^ o) }; }

		CaptureStyle operator|=(const uchar& o) { _value |= o; return *this; }
		CaptureStyle operator&=(const uchar& o) { _value &= o; return *this; }
		CaptureStyle operator^=(const uchar& o) { _value ^= o; return *this; }

		static const CaptureStyle Disabled, EqualsOnly, Optional, Required;
	};
	/**
	 * @brief	Argument does not capture additional input.
	 */
	constexpr CaptureStyle CaptureStyle::Disabled{ 0 };
	/**
	 * @brief	Argument can capture additional input, but only when an equals sign is used.
	 */
	constexpr CaptureStyle CaptureStyle::EqualsOnly{ 1 };
	/**
	 * @brief	Argument can capture when available, but does not require additional input.
	 *\n		When used in conjunction with EqualsOnly, this argument will accept additional input only when appended with an equals sign.
	 */
	constexpr CaptureStyle CaptureStyle::Optional{ 2 };
	/**
	 * @brief	Argument requires additional input.
	 *\n		When used in conjunction with EqualsOnly, this argument will require appending a capture parameter with an equals sign.
	 */
	constexpr CaptureStyle CaptureStyle::Required{ 4 };
#	pragma endregion CaptureStyle

	/**
	 * @struct	ArgumentParsingRules
	 * @brief	Allows configuration of the rules used when parsing arguments.
	 */
	struct ArgumentParsingRules {
		/**
		 * @brief	This argument is ignored when '-' does not appear in this instance's 'delimiters' vector.
		 *\n		When true, valid numbers that are prefixed with a single dash character (Ex: "-3.14")
		 *			 are assumed to be negative numbers; when false, numbers with a single dash prefix
		 *			 are always considered flags instead of numbers.
		 *\n		Default: true
		 */
		bool assumeNumericDashPrefixIsNegative{ true };
		/**
		 * @brief	List of characters that are considered valid argument prefixes.
		 *\n		This defaults to just '-'; other commonly-used prefixes include '/' & '+',
		 *			 but these must be added manually.
		 */
		std::vector<char> delimiters{ '-' };

		/**
		 * @brief	Determines the default capture style used for arguments present in the capture list that do not specify a capture style themselves.
		 */
		CaptureStyle defaultCaptureStyle{ CaptureStyle::Optional };

		/**
		 * @brief	Default Constructor.
		 */
		ArgumentParsingRules() {}
		/**
		 * @brief										Constructor.
		 * @param defaultCaptureStyle					Determines the default capture style used for arguments present in the capture list that do not specify a capture style themselves.
		 * @param delimiters							List of characters that are considered valid argument prefixes.
		 * @param assumeNumericDashPrefixIsNegative		This argument is ignored when '-' does not appear in this instance's 'delimiters' vector.
		 *\n											When true, valid numbers that are prefixed with a single dash character (Ex: "-3.14")
		 *												 are assumed to be negative numbers; when false, numbers with a single dash prefix
		 *												 are always considered flags instead of numbers.
		 */
		ArgumentParsingRules(CaptureStyle const& defaultCaptureStyle, std::vector<char>&& delimiters, bool const& assumeNumericDashPrefixIsNegative = true) : defaultCaptureStyle{ defaultCaptureStyle }, assumeNumericDashPrefixIsNegative{ assumeNumericDashPrefixIsNegative }, delimiters{ std::forward<std::vector<char>>(delimiters) } {}
		/**
		 * @brief										Constructor.
		 * @param delimiters							List of characters that are considered valid argument prefixes.
		 * @param assumeNumericDashPrefixIsNegative		This argument is ignored when '-' does not appear in this instance's 'delimiters' vector.
		 *\n											When true, valid numbers that are prefixed with a single dash character (Ex: "-3.14")
		 *												 are assumed to be negative numbers; when false, numbers with a single dash prefix
		 *												 are always considered flags instead of numbers.
		 */
		ArgumentParsingRules(std::vector<char>&& delimiters, bool const& assumeNumericDashPrefixIsNegative = true) : assumeNumericDashPrefixIsNegative{ assumeNumericDashPrefixIsNegative }, delimiters{ std::forward<std::vector<char>>(delimiters) } {}
		/**
		 * @brief										Constructor.
		 * @param assumeNumericDashPrefixIsNegative		This argument is ignored when '-' does not appear in this instance's 'delimiters' vector.
		 *\n											When true, valid numbers that are prefixed with a single dash character (Ex: "-3.14")
		 *												 are assumed to be negative numbers; when false, numbers with a single dash prefix
		 *												 are always considered flags instead of numbers.
		 */
		ArgumentParsingRules(bool const& assumeNumericDashPrefixIsNegative) : assumeNumericDashPrefixIsNegative{ assumeNumericDashPrefixIsNegative } {}

	#pragma region Methods
		[[nodiscard]] std::string getDelimitersAsString() const
		{
			std::string s;
			s.reserve(delimiters.size());
			for (const auto& c : delimiters)
				s += c;
			s.shrink_to_fit();
			return s;
		}
		/**
		 * @brief		Check if the given character is a valid delimiter, according to the static Settings_ArgParser object.
		 * @param c		Input Character
		 * @returns		bool
		 */
		[[nodiscard]] WINCONSTEXPR bool isDelimiter(const char& c) const
		{
			return std::any_of(delimiters.begin(), delimiters.end(), [&c](auto&& delim) { return delim == c; });
		}
		[[nodiscard]] size_t countPrefix(const std::string& str, const size_t& max_delims) const
		{
			size_t count{ 0ull };
			for (size_t i{ 0ull }; i < str.size() && i < max_delims; ++i) {
				if (isDelimiter(str.at(i)))
					++count;
				else break;
			}
			return count;
		}
		[[nodiscard]] std::pair<std::string, size_t> stripPrefix(const std::string& str, const size_t& max_delims = 2ull) const
		{
			const auto count{ countPrefix(str, max_delims) };
			return{ str.substr(count), count };
		}
		/**
		 * @brief		Checks if the given string is a valid integer, floating-point, or hexadecimal number. Hexadecimal numbers must be prefixed with "0x" (or "-0x") to be detected properly.
		 * @param str	Input String
		 * @returns		bool
		 */
		[[nodiscard]] bool isNumber(std::string str) const
		{
			str = str::trim(str::strip(str, ','));
			if (str.empty())
				return false;

			if (str.starts_with("0x")) {
				return str.size() > 2ull && std::all_of(str.begin() + 2ull, str.end(), str::ishexdigit);
			}
			else {
				const bool is_negative{ str.starts_with('-') };
				if (int decimalCount{ 0 }; std::all_of(str.begin() + static_cast<size_t>(is_negative), str.end(), [&decimalCount](auto&& c) { return (c == '.' ? ++decimalCount < 2 : str::stdpred::isdigit(c)); })) {
					return assumeNumericDashPrefixIsNegative; //< when str is a valid number, return true when assuming it is a number and not a flag
				} // else, return false
			}
			return false;
		}
		/**
		 * @brief		Check if the given iterator CAN capture the next argument by checking
		 *\n			if the next argument is not prefixed with a '-' or is prefixed with '-' but is also a number.
		 *\n			Does NOT check if the given iterator is present on the capturelist!
		 * @param here	The current iterator position.
		 * @param end	The position of the end of the iterable range.
		 * @returns		bool
		 */
		[[nodiscard]] bool canCaptureNext(StrVec::const_iterator& here, const StrVec::const_iterator& end) const
		{
			return (here != end - 1ll) // incrementing iterator won't go out-of-bounds
				&& ((here + 1ll)->front() != '-' // AND next argument doesn't start with a dash
					|| isNumber(*(here + 1ll))); // OR next argument is a number
		}
	#pragma endregion Methods
	};

	/**
	 * @struct	CaptureWrapper
	 * @brief	Extends the InputWrapper struct with the ability to require specific arguments.
	 */
	struct CaptureWrapper : InputWrapper {
		using base = InputWrapper;

		std::optional<CaptureStyle> captureStyle;
		std::optional<size_t> maxCount;
		size_t minCount{ 0ull };

		WINCONSTEXPR CaptureWrapper(const std::string& input, const std::optional<CaptureStyle>& captureStyle = CaptureStyle::Optional, const size_t& minCount = 0ull, const std::optional<size_t>& maxCount = std::nullopt) : base(input), captureStyle{ captureStyle }, minCount{ minCount }, maxCount{ maxCount } {}
		WINCONSTEXPR CaptureWrapper(const char input, const std::optional<CaptureStyle>& captureStyle = CaptureStyle::Optional, const size_t& minCount = 0ull, const std::optional<size_t>& maxCount = std::nullopt) : base(input), captureStyle{ captureStyle }, minCount{ minCount }, maxCount{ maxCount } {}

		WINCONSTEXPR CaptureWrapper(CaptureWrapper&& o) noexcept : base(std::move(o._input)), captureStyle{ std::move(o.captureStyle) }, maxCount{ std::move(o.maxCount) }, minCount{ std::move(o.minCount) } {}
		WINCONSTEXPR CaptureWrapper(CaptureWrapper const& o) noexcept : base(o._input), captureStyle{ o.captureStyle }, maxCount{ o.maxCount }, minCount{ o.minCount } {}
		~CaptureWrapper() = default;

		WINCONSTEXPR CaptureWrapper& operator=(CaptureWrapper&& o) noexcept
		{
			_input = std::move(o._input);
			captureStyle = std::move(o.captureStyle);
			maxCount = std::move(o.maxCount);
			minCount = std::move(o.minCount);
			return *this;
		}
		WINCONSTEXPR CaptureWrapper& operator=(CaptureWrapper const& o) noexcept
		{
			_input = o._input;
			captureStyle = o.captureStyle;
			maxCount = o.maxCount;
			minCount = o.minCount;
			return *this;
		}

		WINCONSTEXPR std::string name() const
		{
			return _input;
		}

		constexpr bool withinRequiredRange(size_t const& count) const
		{
			return count >= minCount && (!maxCount.has_value() || count < maxCount.value());
		}
	};
	/**
	 * @brief				Specify that an argument requires captured input while also providing additional context.
	 * @tparam MAX			Limits the number of times this argument may be specified legally.
	 * @tparam MIN			Requires that this argument be specified at least this many times. (Default: 0)
	 * @tparam T			Any type that is a char or string, or is implicitly convertible to a char or string.
	 * @param input			The name of the argument, excluding prefixes. chars are interpreted as flags, while strings are interpreted as options.
	 * @param equalsOnly	When true, this argument may only capture input when directly appended with an equals sign.
	 * @returns				CaptureWrapper
	 */
	template<size_t MAX, size_t MIN = 0ull, var::any_same_or_convertible<std::string, char> T>
	inline WINCONSTEXPR CaptureWrapper reqCapture(T&& input, const bool& equalsOnly = false)
	{
		return{ std::forward<T>(input), (equalsOnly ? (CaptureStyle::Required | CaptureStyle::EqualsOnly) : CaptureStyle::Required), MIN, MAX };
	}
	/**
	 * @brief				Specify that an argument requires captured input while also providing additional context.
	 * @tparam MIN			Requires that this argument be specified at least this many times. (Default: 0)
	 * @tparam T			Any type that is a char or string, or is implicitly convertible to a char or string.
	 * @param input			The name of the argument, excluding prefixes. chars are interpreted as flags, while strings are interpreted as options.
	 * @param equalsOnly	When true, this argument may only capture input when directly appended with an equals sign.
	 * @returns				CaptureWrapper
	 */
	template<size_t MIN = 0ull, var::any_same_or_convertible<std::string, char> T>
	inline WINCONSTEXPR CaptureWrapper reqCapture(T&& input, const bool& equalsOnly = false)
	{
		return{ std::forward<T>(input), (equalsOnly ? (CaptureStyle::Required | CaptureStyle::EqualsOnly) : CaptureStyle::Required), MIN };
	}
	/**
	 * @brief				Specify that an argument does not require captured input while also providing additional context.
	 * @tparam MAX			Limits the number of times this argument may be specified legally.
	 * @tparam MIN			Requires that this argument be specified at least this many times. (Default: 0)
	 * @tparam T			Any type that is a char or string, or is implicitly convertible to a char or string.
	 * @param input			The name of the argument, excluding prefixes. chars are interpreted as flags, while strings are interpreted as options.
	 * @param equalsOnly	When true, this argument may only capture input when directly appended with an equals sign.
	 * @returns				CaptureWrapper
	 */
	template<size_t MAX, size_t MIN = 0ull, var::any_same_or_convertible<std::string, char> T>
	inline WINCONSTEXPR CaptureWrapper optCapture(T&& input, const bool& equalsOnly = false)
	{
		return{ std::forward<T>(input), (equalsOnly ? (CaptureStyle::Optional | CaptureStyle::EqualsOnly) : CaptureStyle::Optional), MIN, MAX };
	}
	/**
	 * @brief				Specify that an argument does not require captured input while also providing additional context.
	 * @tparam MIN			Requires that this argument be specified at least this many times. (Default: 0)
	 * @tparam T			Any type that is a char or string, or is implicitly convertible to a char or string.
	 * @param input			The name of the argument, excluding prefixes. chars are interpreted as flags, while strings are interpreted as options.
	 * @param equalsOnly	When true, this argument may only capture input when directly appended with an equals sign.
	 * @returns				CaptureWrapper
	 */
	template<size_t MIN = 0ull, var::any_same_or_convertible<std::string, char> T>
	inline WINCONSTEXPR CaptureWrapper optCapture(T&& input, const bool& equalsOnly = false)
	{
		return{ std::forward<T>(input), (equalsOnly ? (CaptureStyle::Optional | CaptureStyle::EqualsOnly) : CaptureStyle::Optional), MIN };
	}

	/**
	 * @brief				Specify that an argument does not accept captured input while also providing additional context.
	 * @tparam MIN			Requires that this argument be specified at least this many times. (Default: 0)
	 * @tparam T			Any type that is a char or string, or is implicitly convertible to a char or string.
	 * @param input			The name of the argument, excluding prefixes. chars are interpreted as flags, while strings are interpreted as options.
	 * @returns				CaptureWrapper
	 */
	template<size_t MIN = 0ull, var::any_same_or_convertible<std::string, char> T>
	inline WINCONSTEXPR CaptureWrapper noCapture(T&& input)
	{
		return{ std::forward<T>(input), CaptureStyle::Disabled, MIN };
	}
	/**
	 * @brief				Specify that an argument does not accept captured input while also providing additional context.
	 * @tparam MAX			Requires that this argument be specified no more than this many times. (Default: std::nullopt)
	 * @tparam MIN			Requires that this argument be specified at least this many times. (Default: 0)
	 * @tparam T			Any type that is a char or string, or is implicitly convertible to a char or string.
	 * @param input			The name of the argument, excluding prefixes. chars are interpreted as flags, while strings are interpreted as options.
	 * @returns				CaptureWrapper
	 */
	template<size_t MAX, size_t MIN = 0ull, var::any_same_or_convertible<std::string, char> T>
	inline WINCONSTEXPR CaptureWrapper noCapture(T&& input)
	{
		return{ std::forward<T>(input), CaptureStyle::Disabled, MIN, MAX };
	}

	template<typename T> concept ValidCaptureInputType = std::derived_from<T, InputWrapper> || ValidInputType<T>;

	/**
	 * @struct	CaptureList
	 * @brief	Contains a list of arguments that should be allowed to capture additional arguments. This is used by the parse function.
	 */
	struct CaptureList {
		const ArgumentParsingRules parsingRules;
		/// @brief Vector of input wrapper strings that contains the argument names.
		const std::vector<CaptureWrapper> vec;

		template<ValidCaptureInputType T>
		static CaptureWrapper get_wrapper(const ArgumentParsingRules& parsingRules, T&& item)
		{
			if constexpr (std::same_as<T, CaptureWrapper>)
				return item;
			else if constexpr (std::same_as<T, std::string>)
				return CaptureWrapper{ str::trim_preceeding(std::forward<T>(item), parsingRules.getDelimitersAsString()) };
			return CaptureWrapper{ std::forward<T>(item) };
		}

		/**
		 * @brief							Default Constructor.
		 * @tparam VT...					Variadic Templated Types.
		 * @param ...capturing_arguments	Arguments that should be allowed to capture additional arguments. Names should not contain prefix delimiters, but if they do, they are removed.
		 */
		template<ValidCaptureInputType... Ts> constexpr CaptureList(ArgumentParsingRules const& parsingRules, Ts&&... capturing_arguments) : parsingRules{ parsingRules }, vec{ get_wrapper(parsingRules, std::forward<Ts>(capturing_arguments))... } {}
		template<ValidCaptureInputType... Ts> constexpr CaptureList(Ts&&... capturing_arguments) : CaptureList(ArgumentParsingRules{}, std::forward<Ts>(capturing_arguments)...) {}
		WINCONSTEXPR operator const std::vector<CaptureWrapper>() const { return vec; }
		/**
		 * @brief				Checks if a given argument appears in the capture list, and returns the associated CaptureWrapper object.
		 * @param name			The name of the target argument. (Note that preceeding delimiters are stripped automatically)
		 * @returns				The CaptureWrapper object associated with the given name, or std::nullopt if none were found.
		 */
		std::optional<CaptureWrapper> get(std::string const& name) const
		{
			const auto& [s, count] { parsingRules.stripPrefix(name) };
			if (const auto& it{ std::find(vec.begin(), vec.end(), s) }; it != vec.end())
				return *it;
			return std::nullopt;
		}

		CaptureStyle get_capture_style(std::string const& name) const
		{
			if (const auto& wrapper{ get(name) }; wrapper.has_value())
				return wrapper.value().captureStyle.value_or(parsingRules.defaultCaptureStyle);
			return CaptureStyle::Disabled;
		}
	};

	/**
	 * @brief			Parse commandline arguments into an ArgContainer instance.
	 *\n				__Argument Types__
	 *\n				- Parameters are any arguments that do not begin with a dash '-' character that were not captured by another argument type.
	 *\n				- Options are arguments that begin with 2 dash '-' characters, and can capture additional arguments if the option name appears in the capture list.
	 *\n				- Flags are arguments that begin with a single dash '-' character, are a single character in length, and can capture additional arguments. Flags can appear alone, or in "chains" where each character is treated as an individual flag. In a flag chain, only the last flag can capture additional arguments.
	 *\n				__Capture Rules__
	 *\n				- Only options/flags specified in the capture list are allowed to capture additional arguments. Capture list entries should not include a delimiter prefix.
	 *\n				- Options/Flags cannot be captured under any circumstance. ex: "--opt --opt captured" results in "--opt", & "--opt" + "captured".
	 *\n				- If a flag in a chain should capture an argument (either with an '=' delimiter or by context), it must appear at the end of the chain.
	 *\n				- Any captured arguments do not appear in the argument list by themselves, and must be accessed through the argument that captured them.
	 * @param args		Commandline arguments as a vector of strings, in order and including argv[0].
	 * @param captures	A CaptureList instance specifying which arguments are allowed to capture other arguments as their parameters
	 * @returns			ArgContainer
	 */
	inline ArgContainerType parse(StrVec&& args, const CaptureList& captures)
	{
		// remove empty arguments, which are possible when passing arguments from automated testing applications
		args.erase(std::remove_if(args.begin(), args.end(), [](auto&& s) { return s.empty(); }), args.end());

		ArgContainerType cont{};
		cont.reserve(args.size());

		for (StrVec::const_iterator it{ args.begin() }; it != args.end(); ++it) {
			auto [arg, d_count] { captures.parsingRules.stripPrefix(*it, 2ull) };

			switch (d_count) {
			case 2ull: // Option
				if (const auto eqPos{ arg.find('=') }; eqPos != std::string::npos) {// argument contains an equals sign
					auto opt{ arg.substr(0ull, eqPos) }, cap{ arg.substr(eqPos + 1ull) };

					if (const auto& captureStyle{ captures.get_capture_style(opt) }; !captureStyle.isDisabled())
						cont.emplace_back(opt::Option(std::make_pair(std::move(opt), std::move(cap))));
					else {
						if (captureStyle.isRequired())
							throw make_exception("Expected a capture argument for option '", opt, "'!");
						cont.emplace_back(opt::Option(std::make_pair(std::move(opt), std::nullopt)));
						if (!cap.empty()) {
							arg = cap;
							goto JUMP_TO_PARAMETER; // skip flag case, add invalid capture as a parameter
						}
					}
				}
				else if (const auto& captureStyle{ captures.get_capture_style(arg) }; !captureStyle.isDisabledOrEqualsOnly() && captures.parsingRules.canCaptureNext(it, args.end())) // argument can capture next arg
					cont.emplace_back(opt::Option(std::make_pair(arg, *++it)));
				else {
					if (captureStyle.isRequired())
						throw make_exception("Expected a capture argument for option '", arg, "'!");
					cont.emplace_back(opt::Option(std::make_pair(arg, std::nullopt)));
				}
				break;
			case 1ull: // Flag
				if (!captures.parsingRules.isNumber(arg)) { // single-dash prefix is not a number
					std::optional<opt::Flag> capt{ std::nullopt }; // this can contain a flag if there is a capturing flag at the end of a chain
					std::string invCap{}; //< for invalid captures that should be treated as parameters
					if (const auto eqPos{ arg.find('=') }; eqPos != std::string::npos) {
						invCap = arg.substr(eqPos + 1ull); // get string following '=', use invCap in case flag can't capture
						if (const auto flag{ arg.substr(eqPos - 1ull, 1ull) }; !captures.get_capture_style(flag).isDisabled()) {
							capt = opt::Flag(flag.front(), invCap); // push the capturing flag to capt, insert into vector once all other flags in this chain are parsed
							arg = arg.substr(0ull, eqPos - 1ull); // remove last flag, '=', and captured string from arg
							invCap.clear(); // flag can capture, clear invCap
						}
						else
							arg = arg.substr(0ull, eqPos); // remove everything from eqPos to arg.end()
					}
					// iterate through characters in arg
					for (auto fl{ arg.begin() }; fl != arg.end(); ++fl) {
						const auto& captureStyle{ captures.get_capture_style(std::string(1ull, *fl)) };
						// If this is the last char, and it can capture
						if (fl == arg.end() - 1ll && !captureStyle.isDisabledOrEqualsOnly() && captures.parsingRules.canCaptureNext(it, args.end()))
							cont.emplace_back(opt::Flag(std::make_pair(*fl, *++it)));
						else {// not last char, or can't capture
							if (captureStyle.isRequired())
								throw make_exception("Expected a capture argument for flag '", *fl, "'!");
							cont.emplace_back(opt::Flag(std::make_pair(*fl, std::nullopt)));
						}
					}
					if (capt.has_value()) // flag captures are always at the end, but parsing them first puts them out of chronological order.
						cont.emplace_back(std::move(capt.value()));
					if (invCap.empty())
						break;
					else arg = invCap; // set argument to invalid capture and fallthrough to add it as a parameter
				}
				else // this is a negative number, re-add '-' prefix and fallthrough
					arg = *it;
				[[fallthrough]];
			case 0ull:
			JUMP_TO_PARAMETER:
				[[fallthrough]]; // Parameter
			default:
				cont.emplace_back(opt::Parameter(arg));
				break;
			}
		}
		cont.shrink_to_fit();

		// count the number of each included argument:
		std::unordered_map<std::string, size_t> counts;

		for (const auto& varg : cont) {
			std::visit([&counts](auto&& value) {
				using T = std::decay_t<decltype(value)>;

				if constexpr (std::same_as<T, Option>)
					++counts[value.name()];
				else if constexpr (std::same_as<T, Flag>)
					++counts[value.name()];
					   }, varg);
		}

		// validate argument count limits:
		for (const auto& it : captures.vec) {
			const auto& name{ it.name() };
			if (const auto& count{ counts[name] }; !it.withinRequiredRange(count)) {
				throw make_exception("Argument count ", count, " for '", name, "' is out-of-bounds! Expected (min: ", it.minCount, ", max: ", ([&it]() {
					if (it.maxCount.has_value())
						return std::to_string(it.maxCount.value());
					return "(none)"s;
									 }()), ')');
			}
		}
		return cont;
	}
	/**
	 * @brief		Make a std::vector of std::strings from a char** array.
	 * @param sz	Size of the array.
	 * @param arr	Array.
	 * @param off	The index to start at. Any elements that are skipped are ignored.
	 * @returns		StrVec
	 */
	inline WINCONSTEXPR static StrVec vectorize(const int& sz, char** arr, const int& off = 0)
	{
		StrVec vec;
		vec.reserve(sz);
		for (int i{ off }; i < sz; ++i)
			vec.emplace_back(std::string{ std::move(arr[i]) });
		vec.shrink_to_fit();
		return vec;
	}
}
/**
 * @namespace	opt_literals
 * @brief		Contains literal operator equivalents for types and methods from the opt namespace.
 *				# Usage
 *				```
 *				using namespace opt_literals;
 *				opt::ParamsAPI2 args{ argc, argv, 'h'_opt, "help"_opt, 's'_req, "set"_req };
 *				```
 */
namespace opt_literals {
	/**
	 * @brief		String literal operator that is the equivalent of opt::reqCapture
	 * @returns		A CaptureWrapper object for an opt::Option that requires capture input.
	 */
	opt::CaptureWrapper operator ""_req(const char* s, size_t)
	{
		return opt::reqCapture(std::string{ s });
	}
	/**
	 * @brief		String literal operator that is the equivalent of opt::optCapture
	 * @returns		A CaptureWrapper object for an opt::Option that does not require capture input.
	 */
	opt::CaptureWrapper operator ""_opt(const char* s, size_t)
	{
		return opt::optCapture(std::string{ s });
	}
	/**
	 * @brief		String literal operator that is the equivalent of opt::noCapture
	 * @returns		A CaptureWrapper object for an opt::Option that does not accept any capture input.
	 */
	opt::CaptureWrapper operator""_nocap(const char* s, size_t)
	{
		return opt::noCapture(s);
	}
	/**
	 * @brief		String literal operator that is the equivalent of opt::reqCapture
	 * @returns		A CaptureWrapper object for an opt::Flag that requires capture input.
	 */
	opt::CaptureWrapper operator ""_req(char c)
	{
		return opt::reqCapture(c);
	}
	/**
	 * @brief		String literal operator that is the equivalent of opt::optCapture
	 * @returns		A CaptureWrapper object for an opt::Flag that does not require capture input.
	 */
	opt::CaptureWrapper operator ""_opt(char c)
	{
		return opt::optCapture(c);
	}
	/**
	 * @brief		String literal operator that is the equivalent of opt::noCapture
	 * @returns		A CaptureWrapper object for an opt::Flag that does not accept any capture input.
	 */
	opt::CaptureWrapper operator""_nocap(char c)
	{
		return opt::noCapture(c);
	}
}
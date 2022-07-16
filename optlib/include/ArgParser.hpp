#pragma once
#include <ArgContainer.hpp>
#include <str.hpp>
#include <charcompare.hpp>
#include <var.hpp>

#include <vector>
#include <concepts>

namespace opt {
	using StrVec = std::vector<std::string>;

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
		 * @brief	Default Constructor.
		 */
		ArgumentParsingRules() {}
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
		[[nodiscard]] std::pair<std::string, size_t> stripPrefix(const std::string& str, const size_t& max_delims) const
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
	 * @struct	CaptureList
	 * @brief	Contains a list of arguments that should be allowed to capture additional arguments. This is used by the parse function.
	 */
	struct CaptureList {
		/// @brief Vector of input wrapper strings that contains the argument names.
		const std::vector<InputWrapper> vec;
		/**
		 * @brief							Default Constructor.
		 * @tparam VT...					Variadic Templated Types.
		 * @param ...capturing_arguments	Arguments that should be allowed to capture additional arguments. Names should not contain prefix delimiters, but if they do, they are removed.
		 */
		template<ValidInputType... VT> constexpr CaptureList(const VT&... capturing_arguments) : vec{ var::variadic_accumulate<InputWrapper>(str::strip_line(InputWrapper{ capturing_arguments }, "", "-")...) } {}
		WINCONSTEXPR operator const std::vector<InputWrapper>() const { return vec; }
		/**
		 * @brief				Check if a given argument appears in the capture list. Names are case-sensitive.
		 * @tparam				Variadic Templated Type. (char)
		 * @param name			Input Argument Name.
		 * @param ...delims		Optional list of delimiters to remove from the given name.
		 * @returns				bool
		 */
		template<var::all_same<char>... DelimT>
		constexpr bool is_present(const std::string& name, const DelimT&... delims) const
		{
			constexpr const bool strip_delims{ var::at_least_one<DelimT...> }; // check if delimiters were included
			return std::any_of(vec.begin(), vec.end(), [&name, &strip_delims, &delims...](auto&& elem) {
				const auto elemname{ elem.operator const std::string() };
				return name == elemname // if names match directly, or if delimiters were included, if the stripped name matches any elements.
					|| strip_delims && str::strip_line(name, "", var::string_accumulate<std::string>(delims...)) == elemname;
			});
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
	inline ArgContainerType parse(StrVec&& args, const CaptureList& captures, const ArgumentParsingRules& parsingRules = {})
	{
		// remove empty arguments, which are possible when passing arguments from automated testing applications
		args.erase(std::remove_if(args.begin(), args.end(), [](auto&& s) { return s.empty(); }), args.end());

		ArgContainerType cont{};
		cont.reserve(args.size());

		for (StrVec::const_iterator it{ args.begin() }; it != args.end(); ++it) {
			auto [arg, d_count] { parsingRules.stripPrefix(*it, 2ull) };
			switch (d_count) {
			case 2ull: // Option
				if (const auto eqPos{ arg.find('=') }; eqPos != std::string::npos) {// argument contains an equals sign
					auto opt{ arg.substr(0ull, eqPos) }, cap{ arg.substr(eqPos + 1ull) };
					if (captures.is_present(opt))
						cont.emplace_back(opt::Option(std::make_pair(std::move(opt), std::move(cap))));
					else {
						cont.emplace_back(opt::Option(std::make_pair(std::move(opt), std::nullopt)));
						if (!cap.empty()) {
							arg = cap;
							goto JUMP_TO_PARAMETER; // skip flag case, add invalid capture as a parameter
						}
					}
				}
				else if (captures.is_present(arg) && parsingRules.canCaptureNext(it, args.end())) // argument can capture next arg
					cont.emplace_back(opt::Option(std::make_pair(arg, *++it)));
				else
					cont.emplace_back(opt::Option(std::make_pair(arg, std::nullopt)));
				break;
			case 1ull: // Flag
				if (!parsingRules.isNumber(arg)) { // single-dash prefix is not a number
					std::optional<opt::Flag> capt{ std::nullopt }; // this can contain a flag if there is a capturing flag at the end of a chain
					std::string invCap{}; //< for invalid captures that should be treated as parameters
					if (const auto eqPos{ arg.find('=') }; eqPos != std::string::npos) {
						invCap = arg.substr(eqPos + 1ull); // get string following '=', use invCap in case flag can't capture
						if (const auto flag{ arg.substr(eqPos - 1ull, 1ull) }; captures.is_present(flag)) {
							capt = opt::Flag(flag.front(), invCap); // push the capturing flag to capt, insert into vector once all other flags in this chain are parsed
							arg = arg.substr(0ull, eqPos - 1ull); // remove last flag, '=', and captured string from arg
							invCap.clear(); // flag can capture, clear invCap
						}
						else
							arg = arg.substr(0ull, eqPos); // remove everything from eqPos to arg.end()
					}
					// iterate through characters in arg
					for (auto fl{ arg.begin() }; fl != arg.end(); ++fl) {
						// If this is the last char, and it can capture
						if (fl == arg.end() - 1ll && captures.is_present(std::string(1ull, *fl)) && parsingRules.canCaptureNext(it, args.end()))
							cont.emplace_back(opt::Flag(std::make_pair(*fl, *++it)));
						else // not last char, or can't capture
							cont.emplace_back(opt::Flag(std::make_pair(*fl, std::nullopt)));
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
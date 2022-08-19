#pragma once
/**
 * @file	simpleINI.hpp
 * @author	radj307
 * @brief	Lightweight, header-only non-tokenizing parser for the INI config file format.
 */
#include <sysarch.h>
#include <make_exception.hpp>
#include <var.hpp>
#include <str.hpp>
#include <fileio.hpp>
#include <fileutil.hpp>

#include <compare>
#include <concepts>
#include <filesystem>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>

namespace ini {
	/// @brief	An inner section of an INI storage object.
	template<var::valid_char TChar, std::derived_from<std::char_traits<TChar>> TCharTraits, std::derived_from<std::allocator<TChar>> TAlloc>
	using ini_section = typename std::unordered_map<std::basic_string<TChar, TCharTraits, TAlloc>, std::basic_string<TChar, TCharTraits, TAlloc>>;
	using Section = ini_section<char, std::char_traits<char>, std::allocator<char>>;
	using wSection = ini_section<char, std::char_traits<char>, std::allocator<char>>;

	/// @brief	The outer container of an INI storage object.
	template<var::valid_char TChar, std::derived_from<std::char_traits<TChar>> TCharTraits, std::derived_from<std::allocator<TChar>> TAlloc>
	using ini_container = typename std::unordered_map<std::basic_string<TChar, TCharTraits, TAlloc>, ini_section<TChar, TCharTraits, TAlloc>>;
	using Container = ini_container<char, std::char_traits<char>, std::allocator<char>>;
	using wContainer = ini_container<char, std::char_traits<char>, std::allocator<char>>;

#pragma region Enum
	/**
	 * @enum	OverrideStyle
	 * @brief	Determines how to handle duplicate keys.
	 */
	enum class OverrideStyle : unsigned char {
		/// @brief	When a duplicate key is encountered, throw an `ini_key_exception`.
		Throw,
		/// @brief	When a duplicate key is encountered, override the existing key.
		Override,
		/// @brief	When a duplicate key is encountered, skip it.
		Skip,
	};
	/**
	 * @enum	SyntaxErrorStyle
	 * @brief	Determines how to handle syntax errors on a line.
	 */
	enum class SyntaxErrorStyle : unsigned char {
		/// @brief	When the syntax on a line is invalid, throw an `ini_syntax_exception`.
		Throw,
		/// @brief	When the syntax on a line is invalid, ignore it and continue. Header lines may have preceding or trailing non-whitespace/comment characters, but they are not parsed!
		Ignore,
	};
	/**
	 * @enum	CommentStyle
	 * @brief	Determines which characters, if any, are considered comments by the parser.
	 */
	$make_typed_bitfield(CommentStyle, char,
						 /// @brief		Comments are not allowed
						 None = 0,
						 /// @brief		'#' indicates a line comment.
						 Pound = 1,
						 /// @brief		';' indicates a line comment.
						 Semicolon = 2,
						 /// @brief		Either '#' or ';' indicate a line comment.
						 Both = Pound | Semicolon,
						 );
#pragma endregion Enum

	/**
	 * @struct	ini_parser_config
	 * @brief	Exposes properties that can be used to configure the INI parser's behaviour.
	 */
	template<var::valid_char TChar = char, std::derived_from<std::char_traits<TChar>> TCharTraits = std::char_traits<TChar>, std::derived_from<std::allocator<TChar>> TAlloc = std::allocator<TChar>>
	struct ini_parser_config {
	private:
		using string_t = typename std::basic_string<TChar, TCharTraits, TAlloc>;
		using string_view_t = typename std::basic_string_view<TChar, TCharTraits>;

	public:
		/// @brief	Determines how to handle duplicate key-value pairs.
		OverrideStyle overrideStyle{ OverrideStyle::Override };
		/// @brief	Determines how syntax errors are handled.
		SyntaxErrorStyle syntaxErrorStyle{ SyntaxErrorStyle::Ignore };
		/// @brief	Which characters to handle as line comments.
		CommentStyle commentStyle{ CommentStyle::Both };
		/// @brief	When true, enclosing quotation marks are not included as part of value strings; otherwise the quotes are included in the value string.
		bool stripEnclosingQuotes{ true };
		/// @brief	When true, preceding/trailing whitespace is removed from values. \n (You can enclose values with a pair of single (') or double (") quotes to preserve whitespace located within the quotes.)
		bool stripWhitespaceFromValue{ true };
		/// @brief	When true, all preceding/trailing whitespace within header names is removed.
		bool stripWhitespaceFromHeaders{ false };

	private:
		WINCONSTEXPR string_t commentChars() const noexcept
		{
			string_t chars{};
			if ((commentStyle & CommentStyle::Pound) != $c(CommentStyle, 0))
				chars += $c(TChar, '#');
			if ((commentStyle & CommentStyle::Semicolon) != $c(CommentStyle, 0))
				chars += $c(TChar, ';');
			return chars;
		}

	public:
		/// @brief	Uses the current value of `commentStyle` to remove comments from the given line.
		WINCONSTEXPR string_view_t strip_comments(string_t const& line) const noexcept
		{
			if (const auto& pos{ line.find_first_of(commentChars()) }; pos != std::string::npos) {
				if (pos == 0) return{};
				else return{ line.begin(), line.begin() + pos };
			}
			return{ line };
		}

		/// @brief	Uses the current value of `stripEnclosingQuotes` to remove one set of enclosing quotes from the given value.
		WINCONSTEXPR string_t format_value(string_t value) const noexcept
		{
			if (stripWhitespaceFromValue)
				value = str::trim(value);

			if (!stripEnclosingQuotes)
				return value;

			if (const size_t first{ value.find('\"') }, last{ value.rfind('\"') }; first != string_t::npos && last != string_t::npos) {
				return value.substr(first + 1, last - first - 1);
			}
			if (const size_t first{ value.find('\'') }, last{ value.rfind('\'') }; first != string_t::npos && last != string_t::npos) {
				return value.substr(first + 1, last - first - 1);
			}

			return value;
		}
	};

	/// @brief	INI syntax error exception.
	$DefineExcept(ini_syntax_exception);
	/// @brief	Duplicate INI key exception.
	$DefineExcept(ini_key_exception);

	/**
	 * @brief							Parses the given input stream into an INI container object.
	 * @param is						An input stream to parse data from.
	 * @param config					Optional configuration object that changes the behaviour of the parser.
	 * @returns							An ini_container type that contains all headers, keys, and values from the input stream.
	 * @throws ini_syntax_exception		Input stream contains invalid syntax, and the syntaxErrorStyle specified by the config was SyntaxErrorStyle::Throw
	 * @throws ini_key_exception		Input stream contains duplicate keys, and the overrideStyle specified by the config was OverrideStyle::Throw
	 */
	template<var::valid_char TChar = char, std::derived_from<std::char_traits<TChar>> TCharTraits = std::char_traits<TChar>, std::derived_from<std::allocator<TChar>> TAlloc = std::allocator<TChar>>
	INLINE ini_container<TChar, TCharTraits, TAlloc> parse(std::basic_istream<TChar, TCharTraits>& is, ini_parser_config<TChar, TCharTraits, TAlloc> const& config = {}) noexcept(false)
	{
		using string_t = typename std::basic_string<TChar, TCharTraits, TAlloc>;
		using string_view_t = typename std::basic_string_view<TChar, TCharTraits>;

		ini_container<TChar, TCharTraits, TAlloc> ini{};

		size_t ln{ 0 };

		string_t header{};

		for (string_t line; std::getline(is, line, '\n'); is.clear(), ++ln) {
			if (line.empty()) continue;

			string_view_t l{ config.strip_comments(line) };

			if (l.empty()) continue;

			// find headers
			if (const size_t iOpen{ l.find('[') }, iClose{ l.rfind(']') };
				iOpen != string_t::npos && iClose != string_t::npos) {
				string_t tmpHeader{ l.substr(iOpen + 1, iClose - iOpen - 1) };

				if (config.stripWhitespaceFromHeaders)
					tmpHeader = str::trim(tmpHeader);

				if (const size_t fstNonSpace{ l.find_first_not_of(" \t\v\r\n") }, lastNonSpace{ l.find_last_not_of(" \t\v\r\n") };
					(iOpen > fstNonSpace || iClose < lastNonSpace) && config.syntaxErrorStyle == SyntaxErrorStyle::Throw)
					throw ex::make_custom_exception<ini_syntax_exception>("Line ", ln, " contains a header with preceding or trailing non-whitespace characters! '", line, "'");

				header = tmpHeader;
			}
			// find key-value pairs
			else if (const size_t equals{ l.find('=') }; equals != string_t::npos) {
				if (const string_t key{ str::trim(l.substr(0ull, equals)) }; !key.empty()) {
					auto& section{ ini[header] };

					string_t value{ config.format_value(l.substr(equals + 1).data()) };

					if (section.contains(key)) {
						switch (config.overrideStyle) {
						case OverrideStyle::Override:
							section.at(key) = value;
							break;
						case OverrideStyle::Throw:
							throw ex::make_custom_exception<ini_key_exception>("Duplicate keys aren't allowed; '", header, (header.empty() ? "" : "::"), key, "' already has value '", section.at(key), "'! (Incoming value: '", value, "')");
						case OverrideStyle::Skip: [[fallthrough]];
						default:break;
						}
					}
					else section.insert(std::make_pair(key, value));
				}
				else if (config.syntaxErrorStyle == SyntaxErrorStyle::Throw)
					throw ex::make_custom_exception<ini_syntax_exception>("Invalid key specified on line ", ln, ": \"", line, '\"');
			}
		}

		return ini;
	}
	/**
	 * @brief							Parses the given input stream rvalue into an INI container object.
	 * @param is						The rvalue reference of an input stream to parse data from.
	 * @param config					Optional configuration object that changes the behaviour of the parser.
	 * @returns							An ini_container type that contains all headers, keys, and values from the input stream.
	 * @throws ini_syntax_exception		Input stream contains invalid syntax, and the syntaxErrorStyle specified by the config was SyntaxErrorStyle::Throw
	 * @throws ini_key_exception		Input stream contains duplicate keys, and the overrideStyle specified by the config was OverrideStyle::Throw
	 */
	template<var::valid_char TChar = char, std::derived_from<std::char_traits<TChar>> TCharTraits = std::char_traits<TChar>, std::derived_from<std::allocator<TChar>> TAlloc = std::allocator<TChar>>
	INLINE ini_container<TChar, TCharTraits, TAlloc> parse(std::basic_istream<TChar, TCharTraits>&& is, ini_parser_config<TChar, TCharTraits, TAlloc> const& config = {}) noexcept(false)
	{
		return parse<TChar, TCharTraits, TAlloc>(is, config);
	}

	namespace operators {
		/// @brief	Output stream insertion operator for the ini_section type.
		template<var::valid_char TChar, std::derived_from<std::char_traits<TChar>> TCharTraits, std::derived_from<std::allocator<TChar>> TAlloc>
		static std::basic_ostream<TChar, TCharTraits>& operator<<(std::basic_ostream<TChar, TCharTraits>& os, const ini_section<TChar, TCharTraits, TAlloc>& s)
		{
			for (const auto& [key, val] : s)
				os << key << $c(TChar, ' ') << $c(TChar, '=') << $c(TChar, ' ') << val << $c(TChar, '\n');
			return os;
		}
		/// @brief	Output stream insertion operator for the ini_container type.
		template<var::valid_char TChar, std::derived_from<std::char_traits<TChar>> TCharTraits, std::derived_from<std::allocator<TChar>> TAlloc>
		static std::basic_ostream<TChar, TCharTraits>& operator<<(std::basic_ostream<TChar, TCharTraits>& os, const ini_container<TChar, TCharTraits, TAlloc>& c)
		{
			for (const auto& [header, section] : c) {
				if (!header.empty())
					os << $c(TChar, '[') << header << $c(TChar, ']') << $c(TChar, '\n');
				os << section;
			}
			return os;
		}
	}

	/// @brief	INI file wasn't found at the specified path.
	$DefineExcept(ini_file_not_found_exception);

	/**
	 * @class	basic_ini
	 * @brief	Inherits directly from ini_container, and provides extra methods for reading/writing, handling nested keys, and type conversions.
	 */
	template<var::valid_char TChar = char, std::derived_from<std::char_traits<TChar>> TCharTraits = std::char_traits<TChar>, std::derived_from<std::allocator<TChar>> TAlloc = std::allocator<TChar>>
	class basic_ini : public ini_container<TChar, TCharTraits, TAlloc> {
		using this_t = basic_ini<TChar, TCharTraits, TAlloc>;
		using base_t = ini_container<TChar, TCharTraits, TAlloc>;
		using string_t = std::basic_string < TChar, TCharTraits, TAlloc>;
		using istream_t = std::basic_istream<TChar, TCharTraits>;
		using config_t = ini_parser_config<TChar, TCharTraits, TAlloc>;
		using section_t = ini_section<TChar, TCharTraits, TAlloc>;

		using iterator = typename base_t::iterator;
		using const_iterator = typename base_t::const_iterator;

		using inner_iterator = typename ini_section<TChar, TCharTraits, TAlloc>::iterator;
		using inner_const_iterator = typename ini_section<TChar, TCharTraits, TAlloc>::const_iterator;

	public:
		/// @brief	Default ctor
		basic_ini() {}
		/**
		 * @brief			Parsing ctor
		 * @param istream	Input stream ref that contains the input data.
		 * @param config	Parser configuration instance.
		 */
		basic_ini(istream_t& istream, config_t const& config = {})
			: base_t(parse<TChar, TCharTraits, TAlloc>(istream, config))
		{}
		/**
		 * @brief			Parsing ctor
		 * @param istream	Input stream rvalue that contains the input data.
		 * @param config	Parser configuration instance.
		 */
		basic_ini(istream_t&& istream, config_t const& config = {})
			: base_t(parse<TChar, TCharTraits, TAlloc>(std::forward<istream_t>(istream), config))
		{}
		/**
		 * @brief						Reading/Parsing ctor
		 * @param path					The location of an INI file in the local filesystem.
		 *\n							When the specified files exists, it is parsed into this instance.
		 *\n							If the specified file doesn't exist,
		 * @param config				Parser configuration instance.
		 * @param throwIfFileNotFound	When true & the specified file doesn't exist, an exception is thrown; otherwise when false, the object is initialized as an empty instance.
		 */
		basic_ini(std::filesystem::path const& path, config_t const& config = {}, const bool throwIfFileNotFound = false)
			: base_t([](auto&& path, auto&& config, auto&& throwIfFileNotFound) {
			if (file::exists(path)) return parse<TChar, TCharTraits, TAlloc>(file::read<TChar, TCharTraits, TAlloc>(path, std::ios_base::binary), config);
			else if (throwIfFileNotFound) throw make_custom_exception<ini_file_not_found_exception>("File Not Found:  ", path);
			else return base_t{};
					 }(path, config, throwIfFileNotFound))
		{}

					 /// @brief	Base Container Move Ctor
					 basic_ini(base_t&& o) noexcept : base_t(std::move(o)) {}
					 /// @brief	Base Container Copy Ctor
					 basic_ini(base_t const& o) : base_t(o) {}
					 /// @brief	Base Container Move-Set Operator
					 basic_ini& operator=(base_t&& o) noexcept
					 {
						 this->clear();
						 this->merge(std::move(o));
						 return *this;
					 }
					 /// @brief	Base Container Copy-Set Operator
					 basic_ini& operator=(base_t const& o)
					 {
						 this->clear();
						 this->merge(o);
						 return *this;
					 }

					 /// @brief	Default Move Ctor
					 basic_ini(basic_ini&&) noexcept = default;
					 /// @brief	Default Copy Ctor
					 basic_ini(basic_ini const&) = default;
					 /// @brief	Default Destructor
					 ~basic_ini() = default;
					 /// @brief	Default Move-Set Operator
					 basic_ini& operator=(basic_ini&&) noexcept = default;
					 /// @brief	Default Copy-Set Operator
					 basic_ini& operator=(basic_ini const&) = default;

					 template<var::same_or_convertible<std::pair<string_t, section_t>>... Ts>
					 basic_ini(Ts&&... sections) : base_t{ std::forward<Ts>(sections)... } {}

					 this_t& operator+=(std::pair<string_t, section_t>&& section_pr) { return *this; }

					 /// @brief	Default comparison operators.
					 auto operator<=>(basic_ini<TChar, TCharTraits, TAlloc> const&) const = default;

					 this_t& deep_merge(ini_container<TChar, TCharTraits, TAlloc> const& other, OverrideStyle const& overrideStyle = OverrideStyle::Override)
					 {
						 for (const auto& [header, section] : other) {
							 if (const auto& existing{ this->find(header) }; existing != this->end()) {
								 auto& [existingHeader, existingSection] { *existing };
								 for (const auto& [key, value] : section) {
									 if (existing->second.contains(key)) {
										 switch (overrideStyle) {
										 case OverrideStyle::Override:
											 existingSection.at(key) = value;
											 break;
										 case OverrideStyle::Throw:
											 throw ex::make_custom_exception<ini_key_exception>("Duplicate keys aren't allowed; '", header, (header.empty() ? "" : "::"), key, "' already has value '", section.at(key), "'! (Incoming value: '", value, "')");
										 case OverrideStyle::Skip: [[fallthrough]];
										 default:break;
										 }
									 }
									 else existingSection[key] = value;
								 }
							 }
							 else this->insert(std::make_pair(header, section));
						 }
						 return *this;
					 }

					 void read(std::filesystem::path const& path) noexcept(false)
					 {
						 this->deep_merge(parse<TChar, TCharTraits, TAlloc>(file::read<TChar, TCharTraits, TAlloc>(path)));
					 }
					 bool write(std::filesystem::path const& path) const noexcept(false)
					 {
						 using ::ini::operators::operator<<;
						 return file::write(path, *this);
					 }
	};
	/// @brief	basic_ini that uses narrow-width `char` types.
	using INI = basic_ini<char, std::char_traits<char>, std::allocator<char>>;
	/// @brief	basic_ini that uses wide-width `wchar_t` types.
	using wINI = basic_ini<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t>>;

	/**
	 * @brief			Creates a new instance of an INI section pair that can be passed to the constructor of basic_ini.
	 * @param header	The header name of the section.
	 * @param section	The contents of the section.
	 * @returns			A key-section pair.
	 */
	template<var::valid_char TChar, std::derived_from<std::char_traits<TChar>> TCharTraits = std::char_traits<TChar>, std::derived_from<std::allocator<TChar>> TAlloc = std::allocator<TChar>, var::same_or_convertible<std::basic_string<TChar, TCharTraits, TAlloc>> THeader>
	std::pair<std::basic_string<TChar, TCharTraits, TAlloc>, ini_section<TChar, TCharTraits, TAlloc>> make_section(THeader const& header, ini_section<TChar, TCharTraits, TAlloc> const& section)
	{
		return std::make_pair(header, section);
	}
}

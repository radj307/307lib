#pragma once
#include <TokenRedux.hpp>
#include <var.hpp>
#include <str.hpp>

#include <concepts>
#include <variant>
#include <stack>
#include <string>
#include <regex>

#if LANG_CPP >= 17
#include <filesystem>
#endif

#ifdef OS_WIN
#include <unordered_map>
#else
#include <map>
#endif

namespace xml {
	// templated types
	template<typename T> using container_t = std::vector<T>;
	template<typename TKey, typename TValue> using pair_t = std::pair<TKey, TValue>;
	template<typename TKey, typename TValue>
#	ifdef OS_WIN
	using dictionary_t = std::unordered_map<TKey, TValue>;
#	else
	using dictionary_t = std::map<TKey, TValue>;
#	endif

	// forward-declaration
	struct XMLElement;

	// value types
	using null_t = std::monostate;
	using string_t = std::string;
	using nodes_t = container_t<XMLElement>;
	using variant_t = std::variant<null_t, string_t, nodes_t>;

	// attribute types
	using attribute_pair_t = pair_t<string_t, string_t>;
	using attribute_container = dictionary_t<string_t, string_t>;

	/**
	 * @struct	ProcessingInstructions
	 * @brief	Represents XML Processing Instructions (Application Instructions), i.e. '<?xml version="1.0"?>'
	 */
	struct ProcessingInstructions {
		string_t target;
		attribute_container attributes;

		ProcessingInstructions() {}
		ProcessingInstructions(string_t&& target, attribute_container&& attributes = {}) : target{ std::move(target) }, attributes{ std::move(attributes) } {}
		ProcessingInstructions(string_t const& target, attribute_container const& attributes = {}) : target{ target }, attributes{ attributes } {}

		friend std::ostream& operator<<(std::ostream& os, const ProcessingInstructions& pi)
		{
			if (!pi.target.empty()) {
				os << "<?" << pi.target;
				for (const auto& [k, v] : pi.attributes)
					os << ' ' << k << "=\"" << v << '\"';
				os << "?>";
			}
			return os;
		}
	};

	/// @brief	Scope specifier characters for XML names.
	static const CONSTEXPR char* XML_SCOPE_DELIMITERS{ ":." };

	/**
	 * @struct	XMLElement
	 * @brief	Represents a single node in an XML document tree, which has a name, attributes, and a value whose type can be any of the following:
	 *			| Value Type | Description                                                              |
	 *			|------------|--------------------------------------------------------------------------|
	 *			| `null_t`   | Node does not contain a value, i.e. '<node/>'                            |
	 *			| `string_t` | Node contains a literal value, i.e. '<node>Hello World!</node>'          |
	 *			| `nodes_t`  | Node contains any number of subnodes, i.e. '<node><sub1/><sub2/></node>' |
	 */
	struct XMLElement {
	#pragma region Fields
		/// @brief	This XMLElement's name, i.e. '<NAME/>'
		string_t name;
		/// @brief	This XMLElement's value, i.e. '<n>VALUE</n>'
		variant_t value;
		/// @brief	This XMLElement's attributes, i.e. '<n ATTRIBUTE="value"/>'
		attribute_container attributes;
	#pragma endregion Fields

	#pragma region Constructors
		/**
		 * @brief	Blank XMLElement constructor.
		 *\n		Note that this creates an invalid node by default
		 */
		XMLElement() {}

		// No value ctors (null value)
		XMLElement(string_t&& name, attribute_container&& attributes = {}) : name{ std::move(name) }, attributes{ std::move(attributes) } {}
		XMLElement(const string_t& name, const attribute_container& attributes = {}) : name{ name }, attributes{ attributes } {}

		// Explicit value ctors
		template<var::any_same<null_t, string_t, nodes_t> T>
		XMLElement(string_t&& name, T&& value, attribute_container&& attributes = {}) : name{ std::move(name) }, value{ std::move(value) }, attributes{ std::move(attributes) } {}
		template<var::any_same<null_t, string_t, nodes_t> T>
		XMLElement(const string_t& name, const T& value, const attribute_container& attributes = {}) : name{ name }, value{ value }, attributes{ attributes } {}

		// Sub-Node value ctors
		XMLElement(string_t&& name, nodes_t&& subNodes, attribute_container&& attributes = {}) : name{ std::move(name) }, value{ std::move(subNodes) }, attributes{ std::move(attributes) } {}
		XMLElement(const string_t& name, const nodes_t& subNodes, const attribute_container& attributes = {}) : name{ name }, value{ subNodes }, attributes{ attributes } {}
	#pragma endregion Constructors

	#pragma region Methods
	#pragma region name
		/**
		 * @brief		Get this node's actual name after removing all scope-specifiers.
		 * @returns		This node's name, excluding all scope-specifiers.
		 */
		WINCONSTEXPR std::string_view getUnscopedName() const
		{
			if (const auto& lastSeperator{ name.find_last_of(XML_SCOPE_DELIMITERS) }; lastSeperator != std::string::npos)
				return { name.begin() + lastSeperator + 1, name.end() };
			else return name;
		}
		/**
		 * @brief		Get this node's name and namespace/scope as a vector of substrings.
		 * @returns		This node's full scoped name in sequential order. (Order of appearance)
		 */
		WINCONSTEXPR container_t<string_t> getScopedName() const { return str::split_all(name, XML_SCOPE_DELIMITERS); }
	#pragma endregion name

	#pragma region value
		/**
		 * @brief		Checks if the value field is not null.
		 * @returns		true when value is not null; otherwise false.
		 */
		CONSTEXPR bool has_value() const
		{
			return std::holds_alternative<null_t>(value);
		}
		/**
		 * @brief		Checks if the type of the value field is the same as type T.
		 * @tparam T	Type
		 * @returns		true when the value type is the same as type T; otherwise false.
		 */
		template<typename T>
		CONSTEXPR bool has_type() const
		{
			return std::holds_alternative<T>(value);
		}
	#pragma endregion value

	#pragma region attributes
		/**
		 * @brief			Get the value of the specified attribute on this XMLElement.
		 * @param attrName	The name of the target attribute.
		 * @returns			The value of the attrName attribute if it exists; otherwise std::nullopt.
		 */
		std::optional<string_t> getAttribute(string_t const& attrName) const
		{
			if (const auto& it{ attributes.find(attrName) }; it != attributes.end())
				return it->second;
			return std::nullopt;
		}
	#pragma endregion attributes
	#pragma endregion Methods
	};
	/// @brief	Slightly shorter alias for XMLElement.
	using XMLNode = XMLElement;

	/**
	 * @struct	XMLDocument
	 * @brief	Extends the XMLElement struct with processing instructions and other document-related methods.
	 */
	struct XMLDocument : XMLElement {
		using base = XMLElement;
		using base::base;

		/// @brief	Processing Instructions belonging to this document, i.e. '<?xml version="1.0"?>'
		ProcessingInstructions Instructions;

	#pragma region Methods
		/**
		 * @brief		Reads the target file and returns an XMLDocument instance.
		 * @param path	The location of the target file.
		 * @returns		XMLDocument object parsed from the specified file.
		 */
	#if LANG_CPP >= 17
		static XMLDocument read(const std::filesystem::path&);
	#else
		static XMLDocument read(const std::string&);
	#endif
		/**
		 * @brief		Writes the given XMLDocument to a specified file.
		 * @param path	The location of the target file.
		 * @param doc	The XMLDocument instance to write to the file.
		 * @returns		true when the file was written successfully; otherwise false.
		 */
	#if LANG_CPP >= 17
		static bool write(const std::filesystem::path&, XMLDocument&&);
	#else
		static bool write(const std::string&, XMLDocument&&);
	#endif
		/**
		 * @brief		Reads the specified file into this XMLDocument instance.
		 *\n			Any existing data within this instance is overwritten.
		 * @param path	The location of the target file.
		 */
	#if LANG_CPP >= 17
		void read_from(const std::filesystem::path&);
	#else
		void read_from(const std::string&);
	#endif
		/**
		 * @brief		Writes this XMLDocument instance to the specified file.
		 * @param path	The location of the target file.
		 * @returns		true when the file was successfully written; otherwise false.
		 */
	#if LANG_CPP >= 17
		bool write(const std::filesystem::path&);
	#else
		bool write(const std::string&);
	#endif
	#pragma endregion Methods
	};

	/**
	 * @struct		XMLPrinter
	 * @tparam T	Any type derived from XMLElement to print. Note that certain derived types have special handling, such as for XMLDocument::Instructions.
	 * @brief		Pretty-printer for XMLElement-derived types.
	 */
	template<var::derived_from_any<XMLElement> T>
	struct XMLPrinter {
		T const& node;

		// settings:
		bool useNewline{ false };
		bool useIndentation{ false };

		XMLPrinter(T const& node) : node{ node } {}
		XMLPrinter(T const& node, bool useNewline, bool useIndentation) : node{ node }, useNewline{ useNewline }, useIndentation{ useIndentation }{}
		template<var::any_same<XMLElement, XMLDocument> U>
		XMLPrinter(T const& node, XMLPrinter<U> const& p) : node{ node }, useNewline{ p.useNewline }, useIndentation{ p.useIndentation } {}

		/**
		 * @brief		Gets a 'newline' depending on the value of the useNewline field.
		 *\n			When useNewline is true, a newline '\n' character is returned; otherwise, a blank string is returned.
		 * @returns		string_t
		 */
		string_t getNewline() const noexcept { return useNewline ? "\n" : ""; }
	};

#	pragma region ostream-operators

	inline std::ostream& operator<<(std::ostream& os, const XMLPrinter<XMLElement>& p)
	{
		bool doCloseTag = true; //< determines whether the full closing tag should be printed or not (</...>)

		// Opening tag:
		os << '<' << p.node.name;
		// - Attributes:
		for (const auto& [attrib, value] : p.node.attributes)
			os << ' ' << attrib << "=\"" << value << '\"';
		//^ we don't close the tag yet since it is dependent on the value

		// Value:
		std::visit([&os, &p, &doCloseTag](auto&& value) {
			using U = std::decay_t<decltype(value)>;
			if constexpr (std::same_as<U, null_t>) {
				os << "/>";
				doCloseTag = false;
			}
			else if constexpr (std::same_as<U, string_t>)
				os << '>' << value;
			else if constexpr (std::same_as<U, nodes_t>) {
				os << '>';
				for (const auto& node : value)
					os << XMLPrinter(node, p); //< recurse
			}
			//	else static_assert(false, "XMLPrinter Value visitor does not handle all potential variant types!");
				   }, p.node.value);

		if (doCloseTag) // Closing Tag:
			os << "</" << p.node.name << '>';

		return os;
	}
	inline std::ostream& operator<<(std::ostream& os, const XMLPrinter<XMLDocument>& p)
	{
		return os << p.node.Instructions << XMLPrinter(static_cast<XMLElement>(p.node), p);
	}

	inline std::ostream& operator<<(std::ostream& os, const XMLElement& node) { return os << XMLPrinter(node); }
	inline std::ostream& operator<<(std::ostream& os, const XMLDocument& root) { return os << XMLPrinter(root); }

#	pragma endregion ostream-operators

	/**
	 * @namespace	parser
	 * @brief		XML Tokenization & Parsing Types & Classes
	 */
	namespace parser {
		/// @brief	Unsigned Char
		using uchar = unsigned char;

		/**
		 * @enum	Lexeme
		 * @brief	Defines lexical types that a single character may represent in the XML specification.
		 */
		enum class Lexeme : uchar {
			/// @brief	Null
			None,
			/// @brief	End-Of-File
			Eof,
			/// @brief	Whitespace or newlines.
			Whitespace,
			/// @brief	Opening angle-bracket '<'
			BracketOpen,
			/// @brief	Closing angle-bracket '>'
			BracketClose,
			/// @brief	Forward Slash
			Slash,
			/// @brief	Backward Slash
			Backslash,
			/// @brief	Equals sign
			Equals,
			/// @brief	Question mark
			Question,
			/// @brief	Exclamation mark
			Exclamation,
			/// @brief	Double-quotation mark.
			Quote,
			/// @brief	Case-insensitive alphabetic characters (a-z).
			Alpha,
			/// @brief	Digit characters (0-9).
			Digit,
		};
		/**
		 * @struct	LexemeDict
		 * @brief	Used to convert values from char to the equivalent Lexeme.
		 */
		struct LexemeDict : token::base::LexemeDictBase<Lexeme> {
			[[nodiscard]] LexemeT char_to_lexeme(const char& c) const noexcept override
			{
				if (str::stdpred::isspace(c))
					return Lexeme::Whitespace;
				else if (str::stdpred::isalpha(c))
					return Lexeme::Alpha;
				else if (str::stdpred::isdigit(c))
					return Lexeme::Digit;
				switch (c) {
				case '<':	return Lexeme::BracketOpen;
				case '>':	return Lexeme::BracketClose;
				case '/':	return Lexeme::Slash;
				case '\\':	return Lexeme::Backslash;
				case '"':	return Lexeme::Quote;
				case '=':	return Lexeme::Equals;
				case '?':	return Lexeme::Question;
				case '!':	return Lexeme::Exclamation;
					//case ':':	return Lexeme::Colon; //< unused
				case -1:	return Lexeme::Eof;
				default:	return Lexeme::None;
				}
			}
		};

		/**
		 * @enum	TokenType
		 * @brief	Defines the various components of an XML document for use in the XMLTokenizer & XMLParser objects.
		 */
		enum class TokenType : uchar {
			None,			//< null
			Eof,			//< -1
			Whitespace,		//< ' ', '\t', '\v', '\r', '\n'
			Tag,
			Value,
			ProcessingInstructions,
			Comment,
		};

		/**
		 * @class	XMLTokenizer
		 * @brief	Tokenizes raw data streams into a vector of tokens.
		 */
		class XMLTokenizer : public token::base::TokenizerBase<Lexeme, LexemeDict, TokenType> {
			using base = token::base::TokenizerBase<Lexeme, LexemeDict, TokenType>;

			TokenT getNextToken(const char& c) override
			{
				Lexeme lexeme = get_lexeme(c);

				switch (lexeme) {
				case Lexeme::Whitespace: [[fallthrough]];
				case Lexeme::Digit: [[fallthrough]];
				case Lexeme::Alpha: {
					string_t s{ c };
					s += getnotsimilar(Lexeme::BracketOpen, Lexeme::BracketClose);

					if (std::all_of(s.begin(), s.end(), str::stdpred::isspace))
						return{ TokenType::Whitespace, s };
					else
						return{ TokenType::Value, s };
				}
				case Lexeme::BracketOpen: {
					auto next{ peeklex() };

					string_t s{ c };
					s += getBrackets(Lexeme::BracketOpen, Lexeme::BracketClose, false);

					if (s.find('<', 1) != std::string::npos || s.rfind('>', 1) != std::string::npos)
						throw make_exception("XMLTokenizer::getNextToken() syntax error:  Illegal tag definition '", s, '\'');

					switch (next) {
					case Lexeme::Question: // processing instruction:
						return{ TokenType::ProcessingInstructions, s };
					case Lexeme::Exclamation: // comment:
						return{ TokenType::Comment, s };
					default: // tag:
						return{ TokenType::Tag, s };
					}
					break;
				}
				case Lexeme::BracketClose:
					throw make_exception("XMLTokenizer::getNextToken() syntax error:  Unexpected closing bracket '", c, "' at character ", ss.tellg());
				default:
					return{ TokenType::None, c };
				}
			}
		public:
			using base::base;
		};

		/**
		 * @enum	TagType
		 * @brief	Defines XML tag types; opening, closing, or self-closing.
		 */
		enum class TagType {
			/// @brief	XML Opening Tag, i.e. '<n>'
			Open,
			/// @brief	XML Closing Tag, i.e. '</n>'
			Close,
			/// @brief	XML Self-Closing Tag, i.e. '<n/>'
			SelfClose,
		};


		/**
		 * @brief					Parses the attributes portion of a
		 * @param attributesString	Any number of serialized XML attributes to parse.
		 * @returns					An attribute_container of all of the attributes present in attributesString
		 */
		inline attribute_container parseAttributes(string_t const& attributesString)
		{
			if (attributesString.empty())
				return{};

			static const std::regex regexParseAttribute{ "([:\\.\\w·-]+)=\\\"(.*?)\\\"", std::regex_constants::optimize };

			attribute_container map;

			// keep parsing the string until we've reached the end:
			for (std::sregex_iterator iter{ attributesString.begin(), attributesString.end(), regexParseAttribute }, end; iter != end; ++iter)
				if (const auto& [existing, added] {map.insert(std::make_pair((*iter)[1], (*iter)[2]))}; !added)
					throw make_exception("XMLParser::parseAttributes() error:  Duplicate attribute '", iter->str(), '\''); // if the attribute name is a duplicate, throw an exception

			return map;
		}
		/**
		 * @brief				Parses the given XML tag string using regular expressions.
		 *\n					Note that this string must contain ONLY the tag itself!
		 * @param tagString		The XML tag to parse, as a string.
		 * @returns				The std::smatch object resulting from the regular expression search operation.
		 */
		inline std::smatch parseTag(string_t const& tagString)
		{
			if (tagString.empty())
				return{};

			static const std::regex regexParseTagName{ "<[\\/\\?]{0,1}?([:\\.\\w·-]+)\\s*(.*?)[\\/\\?]{0,1}?>", std::regex_constants::optimize };

			std::smatch matches;

			if (!std::regex_search(tagString, matches, regexParseTagName))
				throw make_exception("XMLParser::parse() syntax error:  Malformed tag '", tagString, "'");

			return matches;
		}
		/**
		 * @brief		Parses any number of XML tags from the given string using regular expressions.
		 *\n			This is not used by the XMLParser, but is available as a static function.
		 * @param s		Input string.
		 * @returns		Vector of all tags located within the given string.
		 */
		inline std::vector<std::smatch> parseTags(string_t const& s)
		{
			if (s.empty())
				return{};

			static const std::regex regexParseTagName{ "<[\\/\\?]{0,1}?([:\\.\\w·-]+)\\s*(.*?)[\\/\\?]{0,1}?>", std::regex_constants::optimize };

			return{ std::sregex_iterator{ s.begin(), s.end(), regexParseTagName }, std::sregex_iterator{} };
		}

		/**
		 * @class		XMLParser
		 * @brief		Parses tokenized XML data into a usable object.
		 * @tparam T	The type to store the parsed XML data in.
		 *\n			Note that when using XMLElement instead of XMLDocument, any processing instructions are discarded!
		 */
		template<var::derived_from_any<XMLNode> T = XMLDocument>
		class XMLParser : public token::base::IteratingParserBase<T, TokenType> {
			using base = token::base::IteratingParserBase<T, TokenType>;

			template<var::Streamable... Ts>
			static void throwex(string_t const& fromMethod, string_t const& error_type, Ts const&... message)
			{
				throw make_exception("XMLParser::", fromMethod, (fromMethod.ends_with("()") ? "" : "()"), ' ', error_type, ":  ", message...);
			}

			/**
			 * @brief			Get the TagType that describes the given XML tag string.
			 * @param tag_str	A XML tag string.
			 * @returns			TagType
			 */
			TagType getTagTypeOf(string_t const& tag_str) const
			{
				if (tag_str.starts_with("</"))
					return TagType::Close;
				else if (tag_str.ends_with("/>"))
					return TagType::SelfClose;
				return TagType::Open;
			}

		protected:
			using base::base;
		public:
			XMLParser(std::stringstream&& ss) : base(std::move(XMLTokenizer(std::move(ss), Lexeme::Whitespace, Lexeme::Eof).tokenize())) {}

			/**
			 * @brief		Parses the entire stringstream supplied in the XMLParser constructor.
			 * @returns		An object of type T that represents the input XML data.
			 */
			T parse() override
			{
				T root{};

				std::stack<XMLElement*> nodeStack;

				bool breakLoop{ false };
				for (auto it{ this->tokens.begin() }; !breakLoop && it != this->tokens.end(); ++it) {
					const auto& tkn{ *it };
					const string_t s{ tkn.str };

					switch (tkn.type) {
					case TokenType::ProcessingInstructions: {
						if constexpr (std::same_as<T, XMLDocument>) {
							if (!root.Instructions.attributes.empty())
								throwex("parse", "syntax error", "Duplicate processing instruction definition '", tkn.str, '\'');

							const auto& matches{ parseTag(tkn.str) };

							root.Instructions.target = matches[1];
							root.Instructions.attributes = parseAttributes(matches[2]);
						} // else discard Instructions since there is nowhere to store them
						break;
					}
					case TokenType::Tag: {
						// parse name & attribute substrings:
						const auto& matches{ parseTag(s) };

						const string_t name{ matches[1].str() };
						attribute_container attributes;

						if (matches[2].matched)
							attributes = parseAttributes(matches[2].str());

						// Determine the type of this tag
						switch (getTagTypeOf(s)) {
						case TagType::Open: {
							if (nodeStack.size() == 0) { // this is the root node
								root.name = name;
								root.attributes = std::move(attributes);
								nodeStack.push(&root);
							}
							else { // this isn't the root node

								std::visit([this, &nodeStack, &name, &attributes](auto&& value) {
									using U = std::decay_t<decltype(value)>;

									if constexpr (std::same_as<U, null_t>)
										nodeStack.top()->value = nodes_t{ XMLElement{ name, std::move(attributes) } };
									else if constexpr (std::same_as<U, nodes_t>)
										value.emplace_back(XMLElement{ name, std::move(attributes) });
									else
										throwex("parse", "syntax error", "Elements with value types cannot contain sub-elements!");
										   }, nodeStack.top()->value);

								if (auto* p = std::get_if<nodes_t>(&nodeStack.top()->value))
									nodeStack.push(&p->back());
								else throwex("parse", "logic error", "Failed to resolve value type!");
							}
							break;
						}
						case TagType::Close: {
							auto top{ nodeStack.top() };
							nodeStack.pop();

							if (top->name != name)
								throwex("parse", "syntax error", "Missing closing tag for '", top->name, "' before '", name, '\'');
							else if (nodeStack.size() == 0)
								return root;
							break;
						}
						case TagType::SelfClose:
							if (nodeStack.size() == 0ull) {
								root.name = name;
								root.attributes = std::move(attributes);
								if (std::distance(it, this->tokens.end()) <= 1ull)
									throwex("parse", "syntax error", "Cannot parse multiple root elements!");
							}
							else {
								std::visit([&s, &name, &attributes, &nodeStack](auto&& top) {
									using U = std::decay_t<decltype(top)>;

									if constexpr (std::same_as<U, null_t>)
										nodeStack.top()->value = nodes_t{ XMLElement{ name, attributes } };
									else if constexpr (std::same_as<U, nodes_t>)
										top.emplace_back(XMLElement{ name, attributes });
									else throw make_exception("XMLParser::parse() syntax error:  Illegal characters '", s, '\'');
										   }, nodeStack.top()->value);
							}
							break;
						}
						break;
					}
					case TokenType::Value: {
						std::visit([&s, &nodeStack](auto&& top) {
							using U = std::decay_t<decltype(top)>;

							if constexpr (std::same_as<U, null_t>)
								nodeStack.top()->value = s;
							else if constexpr (std::same_as<U, string_t>)
								top += s;
							else throw make_exception("XMLParser::parse() syntax error:  Unexpected value '", s, "'");
								   }, nodeStack.top()->value);
						break;
					}
					case TokenType::Eof:
						breakLoop = true;
						break;
					case TokenType::Comment: [[fallthrough]];
					case TokenType::None: [[fallthrough]];
					default:
						break;
					}
				}

				if (!nodeStack.empty())
					throwex("parse", "syntax error", "Missing ", nodeStack.size(), " closing tag", (nodeStack.size() == 1 ? "" : "s"), '.');

				return root;
			}
		};
	}
}

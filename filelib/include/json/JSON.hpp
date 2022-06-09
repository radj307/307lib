#pragma once
#include "Node.h"
#include "NodeView.h"

#include <TokenRedux.hpp>

namespace json {
	/// @brief	Stream extraction operator for the Node type.\nThis uses the 
	std::istream& operator>>(std::istream&, Node&);

	template<json_container_type T> std::istream& operator>>(std::istream&, ContainerNode<T>&);

	namespace parser {
		using uchar = unsigned char;

		/**
		 * @enum	LEXEME
		 * @brief	Defines the most basic lexical types used by the JSON tokenizer.
		 */
		enum class LEXEME : uchar {
			/// @brief	Null
			NONE,
			/// @brief	End of file virtual lexeme.
			_EOF,
			/// @brief	Whitespace, including newlines.
			WHITESPACE,
			/// @brief	"
			QUOTE_DOUBLE,
			/// @brief	'
			QUOTE_SINGLE,
			///	@brief	{
			BRACKET_OPEN,
			/// @brief	}
			BRACKET_CLOSE,
			/// @brief	[
			ARRAY_OPEN,
			/// @brief	]
			ARRAY_CLOSE,
			/// @brief	:
			COLON,
			/// @brief	.
			PERIOD,
			/// @brief	,
			COMMA,
			/// @brief	Numeric characters from '0' to '9'
			DIGIT,
			/// @brief	Alphabetic (both uppercase and lowercase) characters from 'a' to 'z'
			ALPHA,
		};

		/**
		 * @struct	LexemeDictionary
		 * @brief	Implements the LexemeDictBase struct for the JSON file format.
		 */
		struct LexemeDictionary : token::base::LexemeDictBase<LEXEME> {
			LexemeT char_to_lexeme(const char& c) const noexcept
			{
				if (str::call_if_valid(isalpha, c))
					return LEXEME::ALPHA;
				else if (str::call_if_valid(isdigit, c))
					return LEXEME::DIGIT;
				else if (str::call_if_valid(isspace, c))
					return LEXEME::WHITESPACE;
				else switch (c) {
				case '.':
					return LEXEME::PERIOD;
				case '\'':
					return LEXEME::QUOTE_SINGLE;
				case '\"':
					return LEXEME::QUOTE_DOUBLE;
				case '{':
					return LEXEME::BRACKET_OPEN;
				case '}':
					return LEXEME::BRACKET_CLOSE;
				case '[':
					return LEXEME::ARRAY_OPEN;
				case ']':
					return LEXEME::ARRAY_CLOSE;
				case ',':
					return LEXEME::COMMA;
				case ':':
					return LEXEME::COLON;
				default:
					return LEXEME::NONE;
				}
			}
		};

		/**
		 * @enum	TOKEN
		 * @brief	Defines JSON token types.
		 */
		enum class TOKEN : uchar {
			/// @brief	Null Token.
			NONE,
			/// @brief	Comma Token; indicates that additional data may follow
			COMMA,
			/// @brief	Colon Token; JSON setter character.
			COLON,
			/// @brief	Alphabetic characters (or spaces) that aren't enclosed by quotes.
			KEYWORD,
			/// @brief	Alphabetic characters (or spaces) that aren't enclosed by quotes.
			STRING,
			/// @brief	A number; this can be an integral or a real number.
			NUMBER,
			/// @brief	An array containing any number of nodes of any type.
			ARRAY,
			/// @brief	An object containing any number of key-node pairs.
			OBJECT,
		};
		/// @brief	JSON Token.
		using Token = token::base::TokenBase<TOKEN>;

		/**
		 * @class	Tokenizer
		 * @brief	
		 */
		class Tokenizer : public token::base::TokenizerBase<LEXEME, LexemeDictionary, TOKEN, Token> {
		protected:
			/**
			 * @brief	Retrieve the next token by tokenizing the given character.
			 * @param c	The input character.
			 * @returns	TokenT
			 */
			TokenT getNextToken(const char& c) override
			{
				switch (get_lexeme(c)) {
				case LEXEME::BRACKET_OPEN:
					return{ TOKEN::OBJECT, getBrackets(LEXEME::BRACKET_OPEN, LEXEME::BRACKET_CLOSE) };
				case LEXEME::BRACKET_CLOSE:
					throw make_exception("Tokenizer::getNextToken() syntax error:  Unmatched closing bracket '", c, "'!  (", findCurrentPosString(true), ")");
				case LEXEME::ARRAY_OPEN:
					return{ TOKEN::ARRAY, getBrackets(LEXEME::ARRAY_OPEN, LEXEME::ARRAY_CLOSE) };
				case LEXEME::ARRAY_CLOSE:
					throw make_exception("Tokenizer::getNextToken() syntax error:  Unmatched closing bracket '", c, "'!  (", findCurrentPosString(true), ")");
				case LEXEME::COMMA:
					return{ TOKEN::COMMA, c };
				case LEXEME::PERIOD: [[fallthrough]];
				case LEXEME::DIGIT:
					return{ TOKEN::NUMBER, getsimilar(LEXEME::DIGIT, LEXEME::PERIOD) };
				case LEXEME::QUOTE_SINGLE:
					throw make_exception("Tokenizer::getNextToken() syntax error:  Single quotation marks aren't allowed!  (", findCurrentPosString(true), ")");
				case LEXEME::QUOTE_DOUBLE:
					return{ TOKEN::STRING, getuntil_unescaped(LEXEME::QUOTE_DOUBLE) };
				case LEXEME::ALPHA: {
					std::string s;
					s += c;
					s += getsimilar(LEXEME::ALPHA, LEXEME::DIGIT);
					s = str::trim(s);
					return{ TOKEN::KEYWORD, s };
				}
				case LEXEME::COLON:
					return{ TOKEN::COLON, c };
				case LEXEME::WHITESPACE:
				default:break;
				}
				return{ TOKEN::NONE, c };
			}
		public:
			/**
			 * @brief		Constructs a new JSON Tokenizer instance.
			 * @param ss	Stringstream rvalue reference.
			 *\n			This is used as the raw data stream to be tokenized.
			 */
			Tokenizer(std::stringstream&& ss) : TokenizerBase(std::forward<std::stringstream>(ss), LEXEME::WHITESPACE, LEXEME::NONE) {}
			/**
			 * @brief		Constructs a new JSON Tokenizer instance.
			 * @param s		A string containing valid JSON data.
			 */
			Tokenizer(std::string const& s) : TokenizerBase(std::stringstream{ s }, LEXEME::WHITESPACE, LEXEME::NONE) {}
		};

		/**
		 * @class	Parser
		 * @brief	Implements the IteratingParserBase class for the JSON file format.
		 */
		class Parser : public token::base::IteratingParserBase<Node, TOKEN> {
			NodeType node_type;

			/**
			 * @brief		Parses a JSON keyword, such as 'null', 'true', & 'false'.
			 * @param kywd	A string containing the keyword to parse.
			 * @returns		Node
			 */
			Node parseKeyword(std::string const& kywd)
			{
				if (str::equalsAny<true>(kywd, "null"))
					return{ null };
				else if (str::equalsAny<true>(kywd, "true"))
					return{ true };
				else if (str::equalsAny<true>(kywd, "false"))
					return{ false };
				else throw make_exception("Unrecognized keyword '", kywd, "'!");
			}

		public:
			/**
			 * @brief				Constructs a new JSON parser instance.
			 * @param tokens		Input vector of tokens.
			 * @param node_type		The type of root node to produce from the given tokens.
			 *\n					This is usually either NodeType::Object or NodeType::Array.
			 * @throws ex::except	node_type was NodeType::Undefined
			 */
			Parser(std::vector<Token>&& tokens, NodeType const& node_type = NodeType::Object) : ParserBase(std::forward<decltype(tokens)>(tokens)), node_type{ node_type }
			{
				if (node_type == NodeType::Undefined)
					throw make_exception("Parser::parse() invalid operation:  Node type '", NodeType::Undefined, "' is never a valid parser output type!");
				strip_tokens(TOKEN::NONE);
			}

			/**
			 * @brief	Parse all tokens into the specified node type.
			 * @returns	Node
			 */
			OutputT parse() override;
		};
	}
}


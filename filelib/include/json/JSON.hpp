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

		struct LexemeDictionary : token::base::LexemeDictBase<LEXEME> {
			LexemeT char_to_lexeme(const char& c) const noexcept
			{
				if (isalpha(c))
					return LEXEME::ALPHA;
				else if (isdigit(c))
					return LEXEME::DIGIT;
				else if (isspace(c))
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
			ARRAY,
			OBJECT,
		};

		using Token = token::base::TokenBase<TOKEN>;

		class Tokenizer : public token::base::TokenizerBase<LEXEME, LexemeDictionary, TOKEN, Token> {
			TokenT getNextToken(const char& c) override
			{
				LEXEME lexeme{ get_lexeme(c) };

				switch (lexeme) {
				case LEXEME::BRACKET_OPEN:
					return{ TOKEN::OBJECT, getBrackets(LEXEME::BRACKET_OPEN, LEXEME::BRACKET_CLOSE) };
				case LEXEME::BRACKET_CLOSE:
					throw make_exception("Tokenizer::getNextToken() syntax error:  Unmatched closing bracket '", c, "'!");
				case LEXEME::ARRAY_OPEN:
					return{ TOKEN::ARRAY, getBrackets(LEXEME::ARRAY_OPEN, LEXEME::ARRAY_CLOSE) };
				case LEXEME::ARRAY_CLOSE:
					throw make_exception("Tokenizer::getNextToken() syntax error:  Unmatched closing bracket '", c, "'!");
				case LEXEME::COMMA:
					return{ TOKEN::COMMA, c };
				case LEXEME::PERIOD: [[fallthrough]];
				case LEXEME::DIGIT:
					return{ TOKEN::NUMBER, getsimilar(LEXEME::DIGIT, LEXEME::PERIOD) };
				case LEXEME::QUOTE_SINGLE:
					throw make_exception("Tokenizer::getNextToken() syntax error:  Single quotation marks aren't allowed!");
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
				case LEXEME::WHITESPACE:// whitespace is never passed directly to this method since getNext(false) is the default call
				default:
					return{ TOKEN::NONE, c };
				}
			}
		public:
			Tokenizer(std::stringstream&& ss) : TokenizerBase(std::forward<std::stringstream>(ss), LEXEME::WHITESPACE, LEXEME::NONE) {}
			Tokenizer(std::string const& s) : TokenizerBase(std::stringstream{ s }, LEXEME::WHITESPACE, LEXEME::NONE) {}
		};

		class Parser : public token::base::IteratingParserBase<Node, TOKEN> {
			NodeType node_type;

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

			static std::string removeEnclosing(std::string s, char open, char close, bool removeWhitespace = true)
			{
				s = str::trim(s);
				if (s.empty())
					return s;
				if (s.front() == open) {
					s.erase(s.begin());
					if (s.back() == close)
						s.erase(s.end() - 1ull);
				}
				return s;
			}

		public:
			Parser(std::vector<Token>&& tokens, NodeType const& node_type = NodeType::Object) : ParserBase(std::forward<decltype(tokens)>(tokens)), node_type{ node_type }
			{
				if (node_type != NodeType::Array && node_type != NodeType::Object)
					throw make_exception("Parser() error:  Unexpected node_type! (Must be Array/Object)");
			}

			OutputT parse() override;
		};
	
	}
}


#pragma once
#include <sysarch.h>
#include <var.hpp>

namespace file {
	template<typename LexemeT>
	struct get_lexeme_base {
		LexemeT lexeme;

		/**
		 * @brief		Override this function to select a lexeme type given a single character.
		 * @param ch	Input Char
		 * @returns		LexemeT
		 */
		virtual LexemeT get(const char& ch) const = 0;

		get_lexeme_base(const char& ch) : lexeme{ get(ch) } {}
		virtual ~get_lexeme_base() noexcept = default;

		/**
		 * @brief	Allow implicit conversion to LexemeT
		 * @returns	LexemeT
		 */
		operator LexemeT() const { return lexeme; }

		bool operator==(const get_lexeme_base<LexemeT>& o) const { return lexeme == o.lexeme; }
		bool operator!=(auto&& o) const { return !operator==(std::forward<decltype(o)>(o)); }
	};

	template<typename TokenT, var::valid_string StrT = std::string>
	struct TokenBase {
		TokenT type;
		StrT str;

		TokenBase(const TokenT& type, const StrT& str = StrT{}) : type{ type }, str{ str } {}
		virtual ~TokenBase() noexcept = default;

		operator TokenT() const { return type; }
		operator StrT() const { return str; }

		bool operator==(const TokenBase<TokenT, StrT>& o) const { return type == o.type && str == o.str; }
		bool operator!=(auto&& o) const { return !operator==(std::forward<decltype(o)>(o)); }

		friend std::ostream& operator<<(std::ostream& os, const TokenBase<TokenT, StrT>& token)
		{
			return os << token.str;
		}
	};
}

// Default Implementations:
namespace file {
	enum class LEXEME : unsigned short {
		NULL_TYPE,						///< @brief Any unrecognized, invalid character.
		UNKNOWN,						///< @brief An unrecognized character that is not invalid.
		ESCAPE,							///< @brief \033
		EQUALS,							///< @brief '='
		STAR,							///< @brief '*'
		CARET,							///< @brief '^'
		SLASH,							///< @brief '/'
		BACKSLASH,						///< @brief '\'
		ADDITION,						///< @brief '+'
		SUBTRACT,						///< @brief '-'
		UNDERSCORE,						///< @brief '_'
		COLON,							///< @brief ':'
		SEMICOLON,						///< @brief ';'
		POUND,							///< @brief '#'
		PERCENT,						///< @brief '%'
		QUOTE_SINGLE,					///< @brief '''
		QUOTE_DOUBLE,					///< @brief '"'
		BRACKET_OPEN,					///< @brief '{'
		BRACKET_CLOSE,					///< @brief '}'
		PARENTHESIS_OPEN,				///< @brief '('
		PARENTHESIS_CLOSE,				///< @brief ')'
		SQUAREBRACKET_OPEN,				///< @brief '['
		SQUAREBRACKET_CLOSE,			///< @brief ']'
		ANGLEBRACKET_OPEN,				///< @brief '<'
		ANGLEBRACKET_CLOSE,				///< @brief '>'
		LETTER_UPPERCASE,				///< @brief "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		LETTER_LOWERCASE,				///< @brief "abcdefghijklmnopqrstuvwxyz"
		DIGIT,							///< @brief "0123456789"
		PERIOD,							///< @brief '.'
		COMMA,							///< @brief ','
		WHITESPACE,						///< @brief " \t\v\r"
		NEWLINE,						///< @brief '\n'
		_EOF,							///< @brief Character with the value -1, or an imaginary end-of-file character.
	};

	struct get_lexeme : public get_lexeme_base<LEXEME> {
		LEXEME get(const char& ch) const override
		{
			switch (ch) {
			case -1:				return LEXEME::_EOF;
			case '\033':			return LEXEME::ESCAPE;
			case '\\':				return LEXEME::BACKSLASH;
			case '=':				return LEXEME::EQUALS;
			case '*':				return LEXEME::STAR;
			case '^':				return LEXEME::CARET;
			case '/':				return LEXEME::SLASH;
			case '+':				return LEXEME::ADDITION;
			case '-':				return LEXEME::SUBTRACT;
			case '_':				return LEXEME::UNDERSCORE;
			case ':':				return LEXEME::COLON;
			case ';':				return LEXEME::SEMICOLON;
			case '#':				return LEXEME::POUND;
			case '%':				return LEXEME::PERCENT;
			case '\'':				return LEXEME::QUOTE_SINGLE;
			case '\"':				return LEXEME::QUOTE_DOUBLE;
			case '{':				return LEXEME::BRACKET_OPEN;
			case '}':				return LEXEME::BRACKET_CLOSE;
			case '(':				return LEXEME::PARENTHESIS_OPEN;
			case ')':				return LEXEME::PARENTHESIS_CLOSE;
			case '[':				return LEXEME::SQUAREBRACKET_OPEN;
			case ']':				return LEXEME::SQUAREBRACKET_CLOSE;
			case '<':				return LEXEME::ANGLEBRACKET_OPEN;
			case '>':				return LEXEME::ANGLEBRACKET_CLOSE;
			case '.':				return LEXEME::PERIOD;
			case ',':				return LEXEME::COMMA;
			case '\n':				return LEXEME::NEWLINE;
			case ' ': [[fallthrough]]; // Misc Whitespace
			case '\t': [[fallthrough]];
			case '\v': [[fallthrough]];
			case '\r':
				return LEXEME::WHITESPACE;
			case '0': [[fallthrough]]; // Digits
			case '1': [[fallthrough]];
			case '2': [[fallthrough]];
			case '3': [[fallthrough]];
			case '4': [[fallthrough]];
			case '5': [[fallthrough]];
			case '6': [[fallthrough]];
			case '7': [[fallthrough]];
			case '8': [[fallthrough]];
			case '9':
				return LEXEME::DIGIT;
			case 'a': [[fallthrough]]; // LOWERCASE Alpha
			case 'b': [[fallthrough]];
			case 'c': [[fallthrough]];
			case 'd': [[fallthrough]];
			case 'e': [[fallthrough]];
			case 'f': [[fallthrough]];
			case 'g': [[fallthrough]];
			case 'h': [[fallthrough]];
			case 'i': [[fallthrough]];
			case 'j': [[fallthrough]];
			case 'k': [[fallthrough]];
			case 'l': [[fallthrough]];
			case 'm': [[fallthrough]];
			case 'n': [[fallthrough]];
			case 'o': [[fallthrough]];
			case 'p': [[fallthrough]];
			case 'q': [[fallthrough]];
			case 'r': [[fallthrough]];
			case 's': [[fallthrough]];
			case 't': [[fallthrough]];
			case 'u': [[fallthrough]];
			case 'v': [[fallthrough]];
			case 'w': [[fallthrough]];
			case 'x': [[fallthrough]];
			case 'y': [[fallthrough]];
			case 'z':
				return LEXEME::LETTER_LOWERCASE;
			case 'A': [[fallthrough]]; // UPPERCASE Alpha
			case 'B': [[fallthrough]];
			case 'C': [[fallthrough]];
			case 'D': [[fallthrough]];
			case 'E': [[fallthrough]];
			case 'F': [[fallthrough]];
			case 'G': [[fallthrough]];
			case 'H': [[fallthrough]];
			case 'I': [[fallthrough]];
			case 'J': [[fallthrough]];
			case 'K': [[fallthrough]];
			case 'L': [[fallthrough]];
			case 'M': [[fallthrough]];
			case 'N': [[fallthrough]];
			case 'O': [[fallthrough]];
			case 'P': [[fallthrough]];
			case 'Q': [[fallthrough]];
			case 'R': [[fallthrough]];
			case 'S': [[fallthrough]];
			case 'T': [[fallthrough]];
			case 'U': [[fallthrough]];
			case 'V': [[fallthrough]];
			case 'W': [[fallthrough]];
			case 'X': [[fallthrough]];
			case 'Y': [[fallthrough]];
			case 'Z':
				return LEXEME::LETTER_UPPERCASE;
			default:
				return LEXEME::UNKNOWN;
			}
		}
	};

	enum class TokenType : char {
		NULL_TYPE,
	};

	using Token = TokenBase<TokenType, std::string>;
}
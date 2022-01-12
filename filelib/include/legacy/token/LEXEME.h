#pragma once
#include <concepts>
namespace token {
	/**
	 * @enum LEXEME
	 * @brief Represents the partially-abstracted type of a character. This acts as an intermediary between the raw character types and a Token Type, and LEXEMEs are not recognized by anything downstream from the _TokenizerBase_ class & derivatives, including the _Token_ struct.
	 *\n	  A lexeme is the smallest unit in lexical analysis, and represents a character or sequence of characters.
	 */
	enum class LEXEME : unsigned short {
		NULL_TYPE,						///< @brief Any unrecognized, invalid character.
		UNKNOWN,						///< @brief An unrecognized character that is not invalid.
		ESCAPE,							///< @brief An escape character, usually a backslash.
		EQUALS,							///< @brief '='
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

	inline constexpr const char ESCAPE_CHARACTER{ '\\' };

	inline constexpr bool ishexnum(const char& ch)
	{
		switch (ch) {
		case '0': [[fallthrough]];
		case '1': [[fallthrough]];
		case '2': [[fallthrough]];
		case '3': [[fallthrough]];
		case '4': [[fallthrough]];
		case '5': [[fallthrough]];
		case '6': [[fallthrough]];
		case '7': [[fallthrough]];
		case '8': [[fallthrough]];
		case '9': [[fallthrough]];
		case 'a': [[fallthrough]];
		case 'b': [[fallthrough]];
		case 'c': [[fallthrough]];
		case 'd': [[fallthrough]];
		case 'e': [[fallthrough]];
		case 'f': [[fallthrough]];
		case 'A': [[fallthrough]];
		case 'B': [[fallthrough]];
		case 'C': [[fallthrough]];
		case 'D': [[fallthrough]];
		case 'E': [[fallthrough]];
		case 'F':
			return true;
		default:
			return false;
		}
	}

	/**
	 * @brief Resolve a single given character to a LEXEME, used during the scanning stage.
	 * @tparam T	- Valid Character Type. (char, wchar_t, unsigned char)
	 * @param ch	- Input character
	 * @returns LEXEME
	 */
	template<typename T> requires std::is_same_v<T, char> || std::is_same_v<T, wchar_t> || std::is_same_v<T, unsigned char>
	inline static constexpr LEXEME get_lexeme(const T & ch)
	{
		switch (ch) {
		case -1:				return LEXEME::_EOF;
		case '\033': [[fallthrough]];
		case ESCAPE_CHARACTER:	return LEXEME::ESCAPE;
		case '=':				return LEXEME::EQUALS;
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

}
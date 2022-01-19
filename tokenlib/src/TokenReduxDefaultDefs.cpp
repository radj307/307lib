#include "../include/TokenReduxDefaultDefs.hpp"

using namespace token;
using namespace token::DefaultDefs;

LEXEME LexemeDict::char_to_lexeme(char const& ch) const noexcept
{
	switch (ch) {
	case -1:				return LEXEME::_EOF;
	case '\033': [[fallthrough]];
	case '\\':				return LEXEME::ESCAPE;
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
